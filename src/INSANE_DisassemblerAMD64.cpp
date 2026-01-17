//=========================================================================
//                      INSANE Disassembler AMD64 ( 64-bit mode only )
//=========================================================================
// by      : INSANE
// created : 13/12/2025
// 
// purpose : Disassembler for 64 bit AMD64 assembly.
//-------------------------------------------------------------------------
#include "../Include/INSANE_DisassemblerAMD64.h"

#include <iomanip>
#include <vector>
#include <assert.h>
#include <sstream>

// Util
#include "Tables/Tables.h"
#include "Math/SafeBitWiseOps.h"
#include "../Include/Aliases.h"
#include "Util/Terminal/Terminal.h"

// Instruction containers
#include "../Include/Instruction_t.h"
#include "../Include/Legacy/LegacyInst_t.h"
#include "../Include/VEX/VEXInst_t.h"

#include "../Include/Standard/OpCodeDesc_t.h"
#include "../Include/DASMInst_t.h"


// NOTE : Mind this.
using namespace InsaneDASM64;



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace INSANE_DASM64_NAMESPACE
{
    // Local functions, for decoding different encodings. To be used only in the InsaneDASM64::Decode function.
    // NOTE : These functions update the iterator ( index in the byteStream ) according to the bytes they consume.
    static IDASMErrorCode_t DecodeLegacyEncoding     (const std::vector<Byte>& vecInput, Instruction_t* pInstOut, size_t& iIterator);
    static IDASMErrorCode_t DecodeVEXEncoding        (const std::vector<Byte>& vecInput, Instruction_t* pInstOut, size_t& iIterator);

    // Local functions, for disassembling different encodings. To be used only in the main InsaneDASM64::Disassemble function.
    static IDASMErrorCode_t DisassembleLegacyEncoding(const Legacy::LegacyInst_t* inst, DASMInst_t* pOutput);
    static IDASMErrorCode_t DisassembleVEXEncoding   (const VEX::VEXInst_t*       inst, DASMInst_t* pOutput);
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t INSANE_DASM64_NAMESPACE::Initialize()
{
    // Tables...
    IDASMErrorCode_t iErrorCode = G::g_tables.Initialize();
    if (iErrorCode != IDASMErrorCode_t::IDASMErrorCode_Success)
        return iErrorCode;


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t INSANE_DASM64_NAMESPACE::DecodeAndDisassemble(const std::vector<Byte>& vecInput, std::vector<DASMInst_t>& vecOutput)
{
    // Parse input...
    std::vector<Instruction_t> vecDecodedInst; vecDecodedInst.clear();

    IDASMErrorCode_t iDecoderErrorCode = Decode(vecInput, vecDecodedInst);
    if (iDecoderErrorCode != IDASMErrorCode_Success)
        return iDecoderErrorCode;


    vecOutput.clear();

    // Decoding didn't fail but its empty :(
    if (vecDecodedInst.empty() == true)
        return IDASMErrorCode_t::IDASMErrorCode_Success;


    // Disassemble decoded instruction.
    IDASMErrorCode_t iDASMErrorCode = Disassemble(vecDecodedInst, vecOutput);
    if (iDASMErrorCode != IDASMErrorCode_t::IDASMErrorCode_Success)
        return iDASMErrorCode;


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int OperandTypeToAddressSizeInByte(Standard::CEOperandTypes_t iCEOperandType, int iAddressSizeInByte)
{
    using namespace Standard;
    assert((iAddressSizeInByte == 2 || iAddressSizeInByte == 4 || iAddressSizeInByte == 8) && "Invalid operand size!!");

    switch (iCEOperandType)
    {
    case CEOperandType_8:            return 1;
    case CEOpearndType_16or32_twice: return iAddressSizeInByte == 2 ? 4 : 8;
    case CEOperandType_16_32:        return iAddressSizeInByte == 2 ? 2 : 4;
    case CEOperandType_16_32_64:     return iAddressSizeInByte;

    case CEOperandType_16:
    case CEOperandType_16int:        return 2;

    case CEOperandType_32:
    case CEOperandType_32real:
    case CEOperandType_32int:        return 4;

    case CEOperandType_32_64:        return iAddressSizeInByte == 8 ? 8 : 4;

    case CEOperandType_64mmx:
    case CEOperandType_64:
    case CEOperandType_64int:
    case CEOperandType_64real:       return 8;

    case CEOperandType_64_16:        return iAddressSizeInByte == 2 ? 2 : 8;


    case CEOperandType_128pf:
    case CEOperandType_80dec:
    case CEOperandType_128:
    case CEOperandType_14_28:
    case CEOperandType_80real:
    case CEOperandType_p:
    case CEOperandType_ptp:
    case CEOperandType_94_108:
    case CEOperandType_512:
        break;

    default: break;
    }

    // If no operand type is any "opinion" of what the address size should be,
    // we shall default to the address size ( 8 bytes in longmode, unless override by 0x67 address size override prefix, in which case it will be 4 bytes ) ?
    return iAddressSizeInByte;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int OperandTypeToAddressSizeInBits(Standard::CEOperandTypes_t iCEOperandType, int iAddressSizeInByte)
{
    int iSize = OperandTypeToAddressSizeInByte(iCEOperandType, iAddressSizeInByte);
    
    // Keep the invalid size invalid, and scale valid size.
    return iSize <= 0 ? -1 : iSize * 8;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int OperandTypeToOperandSizeInBytes(Standard::CEOperandTypes_t iCEOperandType, int iOperandSizeInBytes)
{
    // This function is used to determine width of register used when we have to use the modrm byte
    // to determine which register we have to use. Because the operand size is not always the correct
    // and some operand types don't give a fuck about what the operand size is.

    using namespace Standard;
    assert((iOperandSizeInBytes == 2 || iOperandSizeInBytes == 4 || iOperandSizeInBytes == 8) && "Invalid operand size!!");

    switch (iCEOperandType)
    {
    case CEOperandType_8:            return 1;
    case CEOpearndType_16or32_twice: return iOperandSizeInBytes == 2 ? 4 : 8;
    case CEOperandType_16_32:        return iOperandSizeInBytes == 2 ? 2 : 4;
    case CEOperandType_16_32_64:     return iOperandSizeInBytes;

    case CEOperandType_16:
    case CEOperandType_16int:        return 2;

    case CEOperandType_32:
    case CEOperandType_32real:
    case CEOperandType_32int:        return 4;

    case CEOperandType_32_64:        return iOperandSizeInBytes == 8 ? 8 : 4;

    case CEOperandType_64mmx:
    case CEOperandType_64:
    case CEOperandType_64int:
    case CEOperandType_64real:       return 8;

    case CEOperandType_64_16:        return iOperandSizeInBytes == 2 ? 2 : 8;

        
    case CEOperandType_128pf:
    case CEOperandType_80dec:
    case CEOperandType_128:
    case CEOperandType_14_28:
    case CEOperandType_80real:
    case CEOperandType_p:
    case CEOperandType_ptp:
    case CEOperandType_94_108:
    case CEOperandType_512:
        break;

    default: break;
    }

    return -1;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int OperandTypeToOperandSizeInBits(Standard::CEOperandTypes_t iCEOperandType, int iOperandSizeInBytes)
{
    int iSize = OperandTypeToOperandSizeInBytes(iCEOperandType, iOperandSizeInBytes);
    
    // Keep the invalid size invalid, and scale valid size.
    return iSize <= 0 ? -1 : iSize * 8;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t InsaneDASM64::Disassemble(const std::vector<Instruction_t>& vecInput, std::vector<DASMInst_t>& vecOutput)
{
    vecOutput.clear();
    vecOutput.resize(vecInput.size());

    for(const Instruction_t& inst : vecInput)
    {
        vecOutput.emplace_back();
        DASMInst_t* pInstOut = &vecOutput.back();

        switch (inst.m_iInstEncodingType) 
        {
            case Instruction_t::InstEncodingType_Legacy:
                DisassembleLegacyEncoding(reinterpret_cast<Legacy::LegacyInst_t*>(inst.m_pInst), pInstOut);
                break;

            case Instruction_t::InstEncodingType_VEX:
                DisassembleVEXEncoding(reinterpret_cast<VEX::VEXInst_t*>(inst.m_pInst), pInstOut);
                break;

            case Instruction_t::InstEncodingType_EVEX:
            case Instruction_t::InstEncodingType_XOP:
            default:
                FAIL_LOG("Incompatible encoding : %d", inst.m_iInstEncodingType);
                assert(false && "Incompatible encoding type");
                break;
        }
    }

    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static IDASMErrorCode_t InsaneDASM64::DisassembleLegacyEncoding(const Legacy::LegacyInst_t* inst, DASMInst_t* pOutput)
{
    using namespace Standard;
    const Standard::OpCodeDesc_t* pOpCodeDesc = inst->m_opCode.m_pOpCodeDesc;
    assert(pOpCodeDesc != nullptr && "OpCodeDescription pointer was null");


    // Purpose of this string is to help me construct memory address in SIB format easily.
    // so I can write it to output at once.
    std::stringstream szTemp;


    // Store Mnemonic
    strcpy_s(pOutput->m_szMnemonic, sizeof(pOutput->m_szMnemonic), pOpCodeDesc->m_szName);
    // printf("%s ", pOpCodeDesc->m_szName);


    // Iterate all operands and disassembler.
    for (int iOperandIndex = 0; iOperandIndex < pOpCodeDesc->m_nOperands; iOperandIndex++)
    {
        szTemp.clear();

        const Standard::Operand_t* pOperand = &pOpCodeDesc->m_operands[iOperandIndex];

        Standard::CEOperandModes_t iCEOperandMode = GeekToCoderOperandMode(pOperand->m_iOperandMode);
        Standard::CEOperandTypes_t iCEOperandType = GeekToCoderOperandType(pOperand->m_iOperandType);
        Standard::OperandModes_t   iOperandMode   = pOperand->m_iOperandMode;
        Standard::OperandTypes_t   iOperandType   = pOperand->m_iOperandType;


        switch (pOperand->m_iOperandCatagory)
        {
            // Simple and easy, just print a fucking int.
            case Operand_t::OperandCatagory_Literal:
                pOutput->PushLiteralOperand(pOperand->m_iOperandLiteral);
                // printf("%d", pOperand->m_iOperandLiteral);
                break;


                // Simple and easy, just print register name.
            case Operand_t::OperandCatagory_Register:
                pOutput->PushBackOperand(pOperand->m_iOperandRegister.ToString());
                // printf("%s", pOperand->m_iOperandRegister.ToString());
                break;


                // This will get nasty. There are many operand addressing methods, and we 
                // need to take care of all of them. 
            case Operand_t::OperandCatagory_Legacy:
                {

                    switch(iCEOperandMode)
                    {
                        case CEOperandMode_CRn:
                            pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_Control, inst->ModRM_Reg(), 0).ToString());
                            // printf("%s", Standard::Register_t(Standard::Register_t::RegisterClass_Control, inst->ModRM_Reg(), 0).ToString());
                            break;

                        case CEOperandMode_DRn:
                            pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_Debug, inst->ModRM_Reg(), 0).ToString());
                            // printf("%s", Standard::Register_t(Standard::Register_t::RegisterClass_Debug, inst->ModRM_Reg(), 0).ToString());
                            break;

                        case CEOperandMode_ptr:
                        case CEOperandMode_rel:
                        case CEOperandMode_imm:
                        case CEOperandMode_moffs:
                            {
                                pOutput->PushLiteralFromString(inst->m_immediate.m_immediateByte, inst->m_immediate.ByteCount(), true);
                                // printf("0x"); // Handling endian.
                                // for (int i = inst->m_immediate.ByteCount() - 1; i >= 0; i--)
                                // {
                                //     printf("%02X", inst->m_immediate.m_immediateByte[i]);
                                // }
                                break;
                            }

                        case CEOperandMode_m:
                        case CEOperandMode_rm:
                        case CEOperandMode_mmm64:
                        case CEOperandMode_xmmm:
                            {
                                Standard::Register_t::RegisterClass_t iRegisterClass = Standard::Register_t::RegisterClass_GPR;


                                // Overriding register class.
                                if (iCEOperandMode == CEOperandModes_t::CEOperandMode_xmmm)
                                {
                                    iRegisterClass = Standard::Register_t::RegisterClass_SSE;
                                }
                                else if (iCEOperandMode == CEOperandModes_t::CEOperandMode_STim || iCEOperandMode == CEOperandModes_t::CEOperandMode_STi)
                                {
                                    iRegisterClass = Standard::Register_t::RegisterClass_FPU;
                                }
                                else if (iCEOperandMode == CEOperandModes_t::CEOperandMode_mmm64)
                                {
                                    iRegisterClass = Standard::Register_t::RegisterClass_MMX;
                                }


                                Standard::Register_t reg(iRegisterClass, inst->ModRM_RM(),
                                        inst->ModRM_Mod() == 0b11 ? 
                                        OperandTypeToOperandSizeInBits(iCEOperandType, inst->GetOperandSizeInBytes(false)) :
                                        OperandTypeToAddressSizeInBits(iCEOperandType, inst->GetAddressSizeInBytes()));


                                if(inst->ModRM_Mod() == 0b11)
                                {
                                    pOutput->PushBackOperand(reg.ToString());
                                    // printf("%s", reg.ToString());
                                    break;
                                }
                                else if (inst->m_bHasSIB == false)
                                {
                                    szTemp << "[ " << reg.ToString() << ' ';
                                    // printf("[ %s ", reg.ToString());

                                    // Printing displacement, if any.
                                    if(inst->m_displacement.ByteCount() > 0)
                                    {
                                        szTemp << "+ 0x";
                                        // printf("+ 0x");
                                        bool bLeadingZeroEnded = false;
                                        for (int i = inst->m_displacement.ByteCount() - 1; i >= 0; i--)
                                        {
                                            int iByte = static_cast<int>(inst->m_displacement.m_iDispBytes[i]);
                                            if(iByte != 0)
                                                bLeadingZeroEnded = true;

                                            if(bLeadingZeroEnded == false)
                                                continue;

                                            // Save old state
                                            auto oldFlags = szTemp.flags();
                                            auto oldFill  = szTemp.fill();

                                            szTemp << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << iByte;

                                            // Restore state
                                            szTemp.flags(oldFlags);
                                            szTemp.fill(oldFill);

                                            // printf("%02X", inst->m_displacement.m_iDispBytes[i]);
                                        }
                                    }

                                    szTemp << "]";
                                    // printf("]");
                                }
                                else // Here are expecting memory address using SIB byte.
                                {
                                    szTemp << "[ ";
                                    // printf("[ ");

                                    bool bNoBaseReg = inst->ModRM_Mod() == 0 && inst->SIB_Base() == 0b101;
                                    if (bNoBaseReg == false)
                                    {
                                        Standard::Register_t iBaseReg(
                                                Standard::Register_t::RegisterClass_GPR, 
                                                inst->SIB_Base(), 
                                                OperandTypeToAddressSizeInBits(iCEOperandType, inst->GetAddressSizeInBytes())); // * 8 : Bytes to bits.
 
                                        szTemp << iBaseReg.ToString();
                                        // printf("%s", iBaseReg.ToString());
                                    }

                                    // Index register.
                                    uint64_t iSIBIndex = inst->SIB_Index();
                                    if (iSIBIndex != 0b100)
                                    {
                                        Standard::Register_t indexReg(Standard::Register_t::RegisterClass_GPR, iSIBIndex, 
                                                OperandTypeToAddressSizeInBits(iCEOperandType, inst->GetAddressSizeInBytes()));
                                        szTemp << " + " << indexReg.ToString();
                                        // printf(" + %s", reg.ToString()); // * 8 : Bytes to bits.

                                        // Scale.
                                        uint64_t iScale = 1llu << inst->SIB_Scale();
                                        if (iScale > 1)
                                        {
                                            szTemp << " * " << iScale;
                                            // printf(" * %llu", iScale);
                                        }
                                    }

                                    // Displacement ( if any )
                                    if(inst->m_displacement.ByteCount() > 0)
                                    {
                                        szTemp << " + 0x";
                                        // printf(" + 0x");
                                        bool bLeadingZeroEnded = false;
                                        for (int i = inst->m_displacement.ByteCount() - 1; i >= 0; i--)
                                        {
                                            int iByte = static_cast<int>(inst->m_displacement.m_iDispBytes[i]);
                                            if(iByte != 0)
                                                bLeadingZeroEnded = true;

                                            if(bLeadingZeroEnded == false)
                                                continue;

                                            // Save old state
                                            auto oldFlags = szTemp.flags();
                                            auto oldFill  = szTemp.fill();

                                            szTemp << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << iByte;

                                            // Restore state
                                            szTemp.flags(oldFlags);
                                            szTemp.fill(oldFill);
                                            // printf("%02X", inst->m_displacement.m_iDispBytes[i]);
                                        }
                                    }

                                    szTemp << " ]";
                                    // printf(" ]");
                                }

                                pOutput->PushBackOperand(szTemp.str().c_str());
                                break;
                            }

                        case CEOperandMode_STi:
                            {
                                Standard::Register_t reg(Standard::Register_t::RegisterClass_FPU, inst->ModRM_RM(), OperandTypeToOperandSizeInBits(iCEOperandType, inst->GetOperandSizeInBytes(false)));
                                pOutput->PushBackOperand(reg.ToString());
                                // printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_r:
                            {
                                Standard::Register_t reg(Standard::Register_t::RegisterClass_GPR, -1, OperandTypeToOperandSizeInBits(iCEOperandType, inst->GetOperandSizeInBytes(false)));

                                if (iOperandMode == OperandMode_G)
                                {
                                    reg.m_iRegisterIndex = inst->ModRM_Reg();
                                }
                                else if (iOperandMode == OperandModes_t::OperandMode_Z)
                                {
                                    // REX.B bit extends the "Lower 3 bits of the opcode". How bizzare is that?
                                    reg.m_iRegisterIndex = Maths::SafeOr(
                                            Maths::SafeAnd(inst->m_opCode.m_pOpCodeDesc->m_iByte, 0b111), 
                                            Maths::SafeAnd(inst->m_bHasREX == false ? 0llu : inst->m_iREX, Masks::REX_B) << 3llu);
                                }
                                else // Addressing mode H & R, fall under this case.
                                {
                                    reg.m_iRegisterIndex = inst->ModRM_RM();
                                }

                                pOutput->PushBackOperand(reg.ToString());
                                // printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_mm:
                            {
                                Standard::Register_t reg(Standard::Register_t::RegisterClass_MMX, -1, OperandTypeToOperandSizeInBits(iCEOperandType, inst->GetOperandSizeInBytes(false)));

                                assert((iOperandMode == OperandModes_t::OperandMode_N || iOperandMode == OperandModes_t::OperandMode_P) && "Invalid operand mode with Coder's edition (mm)");

                                if (iOperandMode == OperandModes_t::OperandMode_N)
                                {
                                    reg.m_iRegisterIndex = inst->ModRM_RM();
                                }
                                else if (iOperandMode == OperandModes_t::OperandMode_P)
                                {
                                    reg.m_iRegisterIndex = inst->ModRM_Reg();
                                }

                                pOutput->PushBackOperand(reg.ToString());
                                // printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_Sreg:
                            {
                                Standard::Register_t reg(Standard::Register_t::RegisterClass_Segment, inst->ModRM_Reg(), OperandTypeToOperandSizeInBits(iCEOperandType, inst->GetOperandSizeInBytes(false)));
                                pOutput->PushBackOperand(reg.ToString());
                                // printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_TRn:
                            {
                                Standard::Register_t reg(Standard::Register_t::RegisterClass_Test, inst->ModRM_Reg(), OperandTypeToOperandSizeInBits(iCEOperandType, inst->GetOperandSizeInBytes(false)));
                                pOutput->PushBackOperand(reg.ToString());
                                // printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_xmm:
                            {
                                assert((iOperandMode == OperandModes_t::OperandMode_U || iOperandMode == OperandModes_t::OperandMode_V) && "Invalid Operand Modes for Coder's edition operand type : xmm");

                                Standard::Register_t reg(
                                        Standard::Register_t::RegisterClass_SSE, inst->ModRM_RM(), 
                                        OperandTypeToOperandSizeInBits(iCEOperandType, inst->GetOperandSizeInBytes(false)));

                                if (iOperandMode == OperandModes_t::OperandMode_V)
                                    reg.m_iRegisterIndex = inst->ModRM_Reg();

                                pOutput->PushBackOperand(reg.ToString());
                                // printf("%s", reg.ToString());
                                break;
                            }

                            // Just a __placeholder__ for now.
                        default: printf(CONSOLE_RED "%d %d" CONSOLE_RESET, pOperand->m_iOperandMode, pOperand->m_iOperandType); break;
                    }
                }
                break;

            default: break;
        }
    }


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static IDASMErrorCode_t InsaneDASM64::DisassembleVEXEncoding(const VEX::VEXInst_t* inst, DASMInst_t* pOutput)
{

    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t InsaneDASM64::Decode(const std::vector<Byte>& vecInput, std::vector<Instruction_t>& vecOutput)
{
    assert(G::g_tables.IsInitialized() == true && "Tables are not initialized. Initialize tables before parsing!");
    assert(vecOutput.empty()           == true && "Why is output not empty");
    vecOutput.clear();


    // No input?
    if(vecInput.empty() == true)
        return IDASMErrorCode_t::IDASMErrorCode_Success;


    size_t nBytes = vecInput.size();
    for(size_t iByteIndex = 0llu; iByteIndex < nBytes; iByteIndex++)
    {
        Byte             iByte = vecInput[iByteIndex];
        Instruction_t    inst;

        IDASMErrorCode_t iErrorCode = IDASMErrorCode_t::IDASMErrorCode_Success;


        // First we determine which encoding does the instruction has.
        switch (iByte) 
        {
            case SpecialChars::VEX_PREFIX_C4:
            case SpecialChars::VEX_PREFIX_C5:
                inst.InitEncodingType(Instruction_t::InstEncodingTypes_t::InstEncodingType_VEX);
                iErrorCode = DecodeVEXEncoding(vecInput, &inst, iByteIndex);
                break;

            default: 
                inst.InitEncodingType(Instruction_t::InstEncodingTypes_t::InstEncodingType_Legacy);
                iErrorCode = DecodeLegacyEncoding(vecInput, &inst, iByteIndex);
                break;
        }


        // Did we fail the decode ?
        if(iErrorCode != IDASMErrorCode_t::IDASMErrorCode_Success)
            return iErrorCode;


        vecOutput.push_back(inst);
    }


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static IDASMErrorCode_t INSANE_DASM64_NAMESPACE::DecodeVEXEncoding(const std::vector<Byte>& vecInput, Instruction_t* pInstOut, size_t& iIterator)
{
    assert(G::g_tables.IsInitialized() == true && "Tables are not initialized. Initialize tables before parsing!");
    assert(pInstOut->m_iInstEncodingType == Instruction_t::InstEncodingType_VEX && "Instruction passed to DecodeVEXEncoding() is not initialized to correct type.");


    // We must have atleast 4 bytes left in input. Thats the minimum for a VEX encoded instruction.
    if(vecInput.empty() == true || vecInput.size() - iIterator < Rules::MIN_VEX_INST_BYTES)
    {
        FAIL_LOG("Not bytes left in byte stream");
        return IDASMErrorCode_InvalidVEXInst;
    }


    size_t          nBytes = vecInput.size();
    VEX::VEXInst_t* pInst  = reinterpret_cast<VEX::VEXInst_t*>(pInstOut->m_pInst);
    pInst->Clear();

    pInst->m_prefix = vecInput[iIterator]; iIterator++;

    size_t nVEXBytes = 0;

    // Determine VEX byte count.
    if     (pInst->m_prefix == SpecialChars::VEX_PREFIX_C4) nVEXBytes = 2llu;
    else if(pInst->m_prefix == SpecialChars::VEX_PREFIX_C5) nVEXBytes = 1llu;

    // Invalid VEX Prefix check.
    if(nVEXBytes == 0)
        return IDASMErrorCode_InvalidVEXPrefix;


    // Store VEX bytes & move iterator.
    for(size_t iVEXIndex = iIterator; iVEXIndex - iIterator < nVEXBytes; iVEXIndex++)
    {
        pInst->m_vex[iVEXIndex - iIterator] = vecInput[iVEXIndex];
    }
    pInst->m_nVEXBytes = nVEXBytes;
    iIterator += nVEXBytes;

    if(iIterator >= nBytes)
    {
        FAIL_LOG("No OpCode present");
        return IDASMErrorCode_InvalidVEXInst;
    }


    // Capture opcode.
    pInst->m_opcode.PushOpCode(vecInput[iIterator]);

    // Checking opcode validity.
    {
        Byte iPrefix = 0x0F;
        if(pInst->m_nVEXBytes == 2)
        {
            int m_mmmm = pInst->m_vex[0] & 0b11111;
            if(m_mmmm == 0 || m_mmmm > 3) return IDASMErrorCode_InvalidVEXInst;

            static Byte s_iEscapeArr[] = { 0x0F, 0x38, 0x3A };
            iPrefix = s_iEscapeArr[m_mmmm - 1];
        }
        Standard::OpCodeDesc_t* opCodeTable = G::g_tables.GetVEXOpCodeTable(iPrefix);
        assert(opCodeTable != nullptr && "Nullptr table received.");

        // OpCode doesn't repesent a valid VEX encodable instruction.
        Standard::OpCodeDesc_t* pOpCodeDesc = &opCodeTable[pInst->m_opcode.GetMostSignificantOpCode()];
        if(pOpCodeDesc->m_bIsValidCode == false)
        {
            FAIL_LOG("OpCode [ 0x%02X 0x%02X ] is marked as invalid in opcode map", iPrefix, pInst->m_opcode.GetMostSignificantOpCode());
            return IDASMErrorCode_t::IDASMErrorCode_InvalidVEXInst;
        }


        // Since the OpCode description pointer is valid, we can store it.
        pInst->m_opcode.StoreOperatorDesc(pOpCodeDesc);
    }


    // Determining prefix using VEX.pp
    Byte iLegacyPrefix = 0x00;
    {
        static Byte s_prefixMap[] = { 0x00, 0x66, 0xF2, 0xF3 };
        iLegacyPrefix = s_prefixMap[pInst->m_vex[pInst->m_nVEXBytes - 1] & 0b11];
    }

    // In case we have a prefix split, we can try to detrmine final varient without modrm byte.
    // so we know if we need to get the modrm byte or not.
    // so we can use the modrm to get the final varient?
    if(pInst->m_opcode.m_pRootOpCodeDesc->m_iVarientType == Standard::OpCodeDesc_t::VarientKey_LegacyPrefix)
        pInst->m_opcode.InitChildVarient(pInst->m_modrm.Get(), 1, &iLegacyPrefix);


    // Capture modrm, if we got any operands.
    if(pInst->m_opcode.m_pOpCodeDesc->m_nOperands > 0)
    {
        iIterator++;
        if(iIterator >= nBytes)
            return IDASMErrorCode_t::IDASMErrorCode_ModRMNotFound;

        pInst->m_modrm.Store(vecInput[iIterator]); 
    }

    
    // Using modrm and perfix, determine final varient.
    pInst->m_opcode.InitChildVarient(pInst->m_modrm.Get(), 1, &iLegacyPrefix);



    // We need SIB??
    pInst->m_bHasSIB = pInst->m_modrm.ModValueAbs() != 0b11 && pInst->m_modrm.RMValueAbs() == 0b100;
    if(pInst->m_bHasSIB == true)
    {
        iIterator++;
        if(iIterator >= nBytes)
            return IDASMErrorCode_t::IDASMErrorCode_SIBNotFound;

        pInst->m_SIB.Store(vecInput[iIterator]);
    }


    // Store displacement if required.
    // calc. size of displacement here.
    int      iDisplacementSize = 0;
    uint64_t iMod              = pInst->m_modrm.ModValueAbs();

    if (iMod == 0b01)
    {
        iDisplacementSize = 1;
    }
    else if (iMod == 0b10 || (iMod == 0b00 && pInst->m_modrm.RMValueAbs() == 0b101))
    {
        iDisplacementSize = 4;
    }
    else if (iMod == 0b00 && pInst->m_SIB.BaseValueAbs() == 0b101) // base == 101 ?
    {
        iDisplacementSize = 4;
    }

    // Invalid quantity of Displacement bytes.
    if (iDisplacementSize > Rules::MAX_DISPLACEMENT_BYTES)
        return IDASMErrorCode_t::IDASMErrorCode_InvalidDispSize;

    // We got enough bytes for displacement in byte stream.
    if(iIterator + iDisplacementSize >= nBytes)
        return IDASMErrorCode_t::IDASMErrorCode_DisplacementNotFound;

    // Capture Displacement bytes.
    for (size_t iDispIndex = iIterator + 1; iDispIndex < nBytes && iDispIndex - (iIterator + 1llu) < iDisplacementSize; iDispIndex++)
    {
        pInst->m_disp.PushByte(vecInput[iDispIndex]);
    }
    iIterator += pInst->m_disp.ByteCount();


    // Does this intruction need immediate.
    Standard::OpCodeDesc_t* pOpCodeDesc = pInst->m_opcode.m_pOpCodeDesc;
    assert(pOpCodeDesc != nullptr && "Invalid final opcode description");
    for(int iOperandIndex = 0; iOperandIndex < pOpCodeDesc->m_nOperands; iOperandIndex++)
    {
        Standard::Operand_t& operand = pOpCodeDesc->m_operands[iOperandIndex];
        if(operand.m_iOperandCatagory == Standard::Operand_t::OperandCatagory_Legacy)
        {
                Standard::OperandModes_t iOperandMode = operand.m_iOperandMode;
                if (iOperandMode == Standard::OperandModes_t::OperandMode_I || 
                    iOperandMode == Standard::OperandModes_t::OperandMode_J ||
                    iOperandMode == Standard::OperandModes_t::OperandMode_O ||
                    iOperandMode == Standard::OperandModes_t::OperandMode_A || 
                    iOperandMode == Standard::OperandModes_t::OperandMode_IXY)
                {
                    iIterator++;
                    pInst->m_immediate.PushByte(vecInput[iIterator]);
                    break;
                }
        }
    }


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static IDASMErrorCode_t INSANE_DASM64_NAMESPACE::DecodeLegacyEncoding(const std::vector<Byte>& vecInput, Instruction_t* pInstOut, size_t& iIterator)
{
    assert(G::g_tables.IsInitialized() == true && "Tables are not initialized. Initialize tables before parsing!");
    assert(pInstOut->m_iInstEncodingType == Instruction_t::InstEncodingType_Legacy && "Instruction passed to DecodeVEXEncoding() is not initialized to correct type.");


    // Valid Iterator??
    assert(iIterator >= 0llu && iIterator < vecInput.size() && "Out of bound interator.");

    
    // Empty input?
    if (vecInput.empty() == true)
        return IDASMErrorCode_t::IDASMErrorCode_Success;


    Legacy::LegacyInst_t* pInst = reinterpret_cast<Legacy::LegacyInst_t*>(pInstOut->m_pInst);
    pInst->Clear();


    size_t nBytes = vecInput.size();
    for (size_t iByteIndex = iIterator; iByteIndex < nBytes; iByteIndex++)
    {
        Byte        iByte     = vecInput[iByteIndex];
        InstTypes_t iInstType = static_cast<InstTypes_t>(G::g_tables.GetInstType(iByte));


        // Store Legacy Prefix...
        if ((iInstType & InstTypes_t::InstTypes_LegacyPrefixGrpAll) != false)
        {

            // We iterate forward ( upto Rules::MAX_LEGACY_PREFIX ( 4 ) bytes ) and collect all prefix.
            size_t iPrefixIndex = iByteIndex;
            for (iPrefixIndex = iByteIndex; iPrefixIndex < nBytes && iPrefixIndex - iByteIndex < Rules::MAX_LEGACY_PREFIX; iPrefixIndex++)
            {
                Byte iPrefixByte = vecInput[iPrefixIndex];


                // != Prefix? break.
                if ((G::g_tables.GetInstType(iPrefixByte) & InstTypes_t::InstTypes_LegacyPrefixGrpAll) == false)
                    break;


                // More than max prefix?
                if (pInst->m_legacyPrefix.PrefixCount() >= Rules::MAX_LEGACY_PREFIX)
                    return IDASMErrorCode_t::IDASMErrorCode_TooManyPefix; // already stored 'MAX_LEGACY_PREFIX' no. of prefixies.


                pInst->m_legacyPrefix.PushPrefix(iPrefixByte);
            }
            iByteIndex = iPrefixIndex - 1; // So we don't skip one byte.

        }
        else if ((iInstType & InstTypes_t::InstTypes_REX) != false)
        {
            for (size_t iREXIndex = iByteIndex; iREXIndex < nBytes - 1LLU; iREXIndex++)
            {
                Byte iCurByte  = vecInput[iREXIndex];
                Byte iNextByte = vecInput[iREXIndex + 1LLU];


                // Next byte != REX, and current Byte == REX. Store current & break.
                if ((G::g_tables.GetInstType(iNextByte) & InstTypes_t::InstTypes_REX) == false)
                {
                    pInst->m_bHasREX   = true;
                    pInst->m_iREX      = iCurByte;
                    pInst->m_iREXIndex = static_cast<int32_t>(iREXIndex);
                    iByteIndex       = iREXIndex;

                    break;
                }
            }
        }
        else // != REX && != LegacyPrefix
        {
            // OpCode Immediately preceeding REX?
            if (pInst->m_bHasREX == true && pInst->m_iREXIndex != iByteIndex - 1llu && pInst->m_opCode.OpByteCount() == 0)
                return IDASMErrorCode_t::IDASMErrorCode_REXNotPrecedingOpCode;


            // OpCodes....
            Byte iLastByte = 0x00;
            for (size_t iOpCodeIndex = iByteIndex; iOpCodeIndex < nBytes && iOpCodeIndex - iByteIndex < Rules::MAX_OPBYTES; iOpCodeIndex++)
            {

                Byte iOpCodeByte = vecInput[iOpCodeIndex];


                // NOTE : Last byte / escape byte is only used for 3 byte opcodes. so 0x00 is fine for first 2 iterations...
                int iTableIndex = static_cast<int>(iOpCodeIndex - iByteIndex + 1llu);
                Standard::OpCodeDesc_t* pOpCodeTable = G::g_tables.GetOpCodeTable(iTableIndex, iLastByte);
                if (pOpCodeTable == nullptr)
                {
                    FAIL_LOG("nullptr table\n");
                    return IDASMErrorCode_t::IDASMErrorCode_InvalidOpCode;
                }


                // Pull OpCode description from table.
                Standard::OpCodeDesc_t* pOpCodeDesc = &pOpCodeTable[iOpCodeByte];
                if (pOpCodeDesc == nullptr || pOpCodeDesc->m_bIsValidCode == false)
                {
                    FAIL_LOG("OpCode description is invalid or is nullptr.\n");
                    return IDASMErrorCode_t::IDASMErrorCode_InvalidOpCode;
                }


                pInst->m_opCode.PushOpCode(iOpCodeByte);


                // Escape??
                if(pOpCodeDesc->m_bIsEscapeCode == false)
                { 
                    pInst->m_opCode.StoreOperatorDesc(pOpCodeDesc);
                    break;
                }


                iLastByte = iOpCodeByte;
            }

            
            // Must store atleast 1 opcode byte.
            if (pInst->m_opCode.OpByteCount() <= 0)
                return IDASMErrorCode_t::IDASMErrorCode_NoOpByteFound;


            // Incrementus iteratus.
            iByteIndex += pInst->m_opCode.OpByteCount();


            // Store ModRM byte if required.
            if (pInst->m_opCode.ModRMRequired(&pInst->m_legacyPrefix) == true)
            {
                // Bytes left in byte stream?
                if (iByteIndex >= nBytes)
                    return IDASMErrorCode_t::IDASMErrorCode_ModRMNotFound;


                pInst->m_bHasModRM = true;
                pInst->m_modrm.Store(vecInput[iByteIndex]);
                iByteIndex++;
            }


            // At this point, we have modRM byte ( if any ), legacy prefix ( if any ) 
            // and root opcode description. Now we can and we must find the child opcode varient
            // that is refered in this instruction.
            pInst->m_opCode.InitChildVarient(&pInst->m_legacyPrefix, pInst->m_modrm.Get());



            // Store SIB if required. MOD == 11 && R/M == 100
            //bool bSIBRequired = inst->m_bHasModRM == true && (inst->m_modrm & 0b11000000) != 0b11000000 && (inst->m_modrm & 0b111) == 0b100;
            Byte modrm        = pInst->m_modrm.Get();
            bool bSIBRequired = pInst->m_bHasModRM == true && pInst->m_modrm.ModValueAbs() != 0b11 && pInst->m_modrm.RMValueAbs() == 0b100;
            if (bSIBRequired == true)
            {
                // Bytes left in byte stream?
                if (iByteIndex + 1llu >= nBytes)
                    return IDASMErrorCode_t::IDASMErrorCode_SIBNotFound;


                pInst->m_bHasSIB = true;
                pInst->m_SIB.Store(vecInput[iByteIndex]);
                iByteIndex++;
            }


            // Store displacement if required.
            // calc. size of displacement here.
            int iDisplacementSize = 0;
            if (pInst->m_bHasModRM == true)
            {
                uint64_t iMod = pInst->m_modrm.ModValueAbs();

                if (iMod == 0b01)
                {
                    iDisplacementSize = 1;
                }
                else if (iMod == 0b10 || (iMod == 0b00 && pInst->m_modrm.RMValueAbs() == 0b101))
                {
                    iDisplacementSize = 4;
                }
                else if (iMod == 0b00 && pInst->m_SIB.BaseValueAbs() == 0b101) // base == 101 ?
                {
                    iDisplacementSize = 4;
                }
            }
            
            if (iDisplacementSize > Rules::MAX_DISPLACEMENT_BYTES)
                return IDASMErrorCode_t::IDASMErrorCode_InvalidDispSize;

            
            for (size_t iDispIndex = iByteIndex; iDispIndex < nBytes && iDispIndex - iByteIndex < iDisplacementSize; iDispIndex++)
            {
                Byte iDispByte = vecInput[iDispIndex];

                pInst->m_displacement.PushByte(iDispByte);
            }
            iByteIndex += pInst->m_displacement.ByteCount();


            // Store immediate if required.
            Standard::CEOperandTypes_t iImmOperandType = Standard::CEOperandTypes_t::CEOperandType_Invalid;
            Standard::OperandModes_t   iImmOperandMode = Standard::OperandModes_t::OperandMode_Invalid;
            for (int iOperandIndex = 0; iOperandIndex < pInst->m_opCode.m_pOpCodeDesc->m_nOperands; iOperandIndex++)
            {
                const Standard::Operand_t* pOperand = &pInst->m_opCode.m_pOpCodeDesc->m_operands[iOperandIndex];

                // If we found an operand with addresing method that requires immediate, store its operand type, and 
                // break out.
                Standard::OperandModes_t iOperandMode = pOperand->m_iOperandMode;
                if (iOperandMode == Standard::OperandModes_t::OperandMode_I || 
                    iOperandMode == Standard::OperandModes_t::OperandMode_J ||
                    iOperandMode == Standard::OperandModes_t::OperandMode_O ||
                    iOperandMode == Standard::OperandModes_t::OperandMode_A)
                {
                    iImmOperandMode = iOperandMode;
                    iImmOperandType = GeekToCoderOperandType(pOperand->m_iOperandType);
                    break;
                }
            }


            // Store immediate bytes according to operand type
            if (iImmOperandType != Standard::CEOperandTypes_t::CEOperandType_Invalid)
            {
                int iImmediateSize = 0;
                if (iImmOperandMode == Standard::OperandMode_O)
                {
                    iImmediateSize = pInst->GetAddressSizeInBytes();
                }
                else
                {
                    switch (iImmOperandType)
                    {
                    case Standard::CEOperandType_8:        iImmediateSize = 1;            break;
                    case Standard::CEOperandType_16:       iImmediateSize = 2;            break;
                    case Standard::CEOperandType_32:       iImmediateSize = 4;            break;
                    case Standard::CEOperandType_64:       iImmediateSize = 8;            break;
                    case Standard::CEOperandType_16_32:    iImmediateSize = pInst->GetOperandSizeInBytes(true);  break;
                    case Standard::CEOperandType_16_32_64: iImmediateSize = pInst->GetOperandSizeInBytes(false); break; // Promoted to qword by REX.W ?

                    default: break;
                    }
                }


                if (iImmediateSize == 0 || iImmediateSize > Rules::MAX_IMMEDIATE_BYTES)
                    return IDASMErrorCode_t::IDASMErrorCode_InvalidImmediateSize;


                for (size_t iImmediateIndex = iByteIndex; iImmediateIndex < nBytes && iImmediateIndex - iByteIndex < iImmediateSize; iImmediateIndex++)
                {
                    pInst->m_immediate.PushByte(vecInput[iImmediateIndex]);
                }
                iByteIndex += pInst->m_immediate.ByteCount();


                // Check if we collected all immediate bytes.
                if (pInst->m_immediate.ByteCount() != iImmediateSize)
                    return IDASMErrorCode_t::IDASMErrorCode_InvalidImmediateSize;
            }


            // So we don't skip one byte when we go to next iteration.
            assert(iByteIndex > 0llu && "Byte index can't be 0 after storing a entire instruction. Alteast one byte must be stored!");
            iByteIndex--;

            iIterator = iByteIndex;
            return IDASMErrorCode_t::IDASMErrorCode_Success;
        }
    }


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
const char* InsaneDASM64::GetErrorMessage(IDASMErrorCode_t iErrorCode)
{
    switch (iErrorCode)
    {
    case InsaneDASM64::IDASMErrorCode_Success:               return "[ Insane Disassembler AMD64 ] No Error";
    case InsaneDASM64::IDASMErrorCode_FailedInit:            return "[ Insane Disassembler AMD64 ] Failed initialization!";
    case InsaneDASM64::IDASMErrorCode_TooManyPefix:          return "[ Insane Disassembler AMD64 ] Failed parsing!, more than MAX_PREFIX amount of prefix encountered consecutively";
    case InsaneDASM64::IDASMErrorCode_REXNotPrecedingOpCode: return "[ Insane Disassembler AMD64 ] REX exists in an instruction but doesn't immediately preced OpCode bytes.";
    case InsaneDASM64::IDASMErrorCode_InvalidOpCode:         return "[ Insane Disassembler AMD64 ] Invalid OpCode appeared in input.";
    case InsaneDASM64::IDASMErrorCode_NoOpByteFound:         return "[ Insane Disassembler AMD64 ] No OpByte after rex / legacy prefix. A OpByte was expected here.";
    case InsaneDASM64::IDASMErrorCode_ModRMNotFound:         return "[ Insane Disassembler AMD64 ] A ModR/M byte was expected but was not found.";
    case InsaneDASM64::IDASMErrorCode_SIBNotFound:           return "[ Insane Disassembler AMD64 ] A SID byte was expected but was not found.";
    case InsaneDASM64::IDASMErrorCode_InvalidDispSize:       return "[ Insane Disassembler AMD64 ] Determined displacement size is invalid.";
    case InsaneDASM64::IDASMErrorCode_DisplacementNotFound:  return "[ Insane Disassembler AMD64 ] Not enough displacement bytes in byte stream.";
    case InsaneDASM64::IDASMErrorCode_NoImmediateFound:      return "[ Insane Disassembler AMD64 ] An Immediate was expected, but was not found.";
    case InsaneDASM64::IDASMErrorCode_InvalidImmediateSize:  return "[ Insane Disassembler AMD64 ] Failed to determine immediate size for some instruction.";
    case InsaneDASM64::IDASMErrorCode_InvalidVEXPrefix:      return "[ Insane Disassembler AMD64 ] Invalid VEX prefix byte found. VEX prefix must be either 0xC4 or 0xC5";
    case InsaneDASM64::IDASMErrorCode_InvalidVEXInst:        return "[ Insane Disassembler AMD64 ] Provided VEX encoded instruction is invalid.";
    
    default: break;
    }

    static char s_invalidCodeBuffer[128] = "";
    sprintf_s(s_invalidCodeBuffer, "[ Insane Disassembler AMD64 ] Invalid Error Code { %d }", iErrorCode);
    return s_invalidCodeBuffer;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Standard::CEOperandModes_t INSANE_DASM64_NAMESPACE::GeekToCoderOperandMode(Standard::OperandModes_t iOperandMode)
{
    switch (iOperandMode)
    {
    case Standard::OperandMode_A:   return Standard::CEOperandMode_ptr;
    case Standard::OperandMode_BA:  return Standard::CEOperandMode_m;
    case Standard::OperandMode_BB:  return Standard::CEOperandMode_m;
    case Standard::OperandMode_BD:  return Standard::CEOperandMode_m;
    case Standard::OperandMode_C:   return Standard::CEOperandMode_CRn;
    case Standard::OperandMode_D:   return Standard::CEOperandMode_DRn;
    case Standard::OperandMode_E:   return Standard::CEOperandMode_rm;
    case Standard::OperandMode_ES:  return Standard::CEOperandMode_STim;
    case Standard::OperandMode_EST: return Standard::CEOperandMode_STi;
    // case OperandMode_F: return [ no equivalent operand mode in coder's edition ]
    case Standard::OperandMode_G: return Standard::CEOperandMode_r;
    case Standard::OperandMode_H: return Standard::CEOperandMode_r;
    case Standard::OperandMode_I: return Standard::CEOperandMode_imm;
    case Standard::OperandMode_J: return Standard::CEOperandMode_rel;
    case Standard::OperandMode_M: return Standard::CEOperandMode_m;
    case Standard::OperandMode_N: return Standard::CEOperandMode_mm;
    case Standard::OperandMode_O: return Standard::CEOperandMode_moffs;
    case Standard::OperandMode_P: return Standard::CEOperandMode_mm;
    case Standard::OperandMode_Q: return Standard::CEOperandMode_mmm64;
    case Standard::OperandMode_R: return Standard::CEOperandMode_r;
    case Standard::OperandMode_S: return Standard::CEOperandMode_Sreg;
    // case OperandMode_SC: return [ no equivalent operand mode in coder's edition ]
    case Standard::OperandMode_T: return Standard::CEOperandMode_TRn;
    case Standard::OperandMode_U: return Standard::CEOperandMode_xmm;
    case Standard::OperandMode_V: return Standard::CEOperandMode_xmm;
    case Standard::OperandMode_W: return Standard::CEOperandMode_xmmm;
    case Standard::OperandMode_X: return Standard::CEOperandMode_m;
    case Standard::OperandMode_Y: return Standard::CEOperandMode_m;
    case Standard::OperandMode_Z: return Standard::CEOperandMode_r;

    default: break;
    }

    return Standard::CEOperandModes_t::CEOperandMode_Invalid;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Standard::CEOperandTypes_t INSANE_DASM64_NAMESPACE::GeekToCoderOperandType(Standard::OperandTypes_t iOperandType)
{
    switch (iOperandType)
    {
    case Standard::OperandType_a:   return Standard::CEOpearndType_16or32_twice;
    case Standard::OperandType_b:   return Standard::CEOperandType_8;
    case Standard::OperandType_bs:  return Standard::CEOperandType_8;
    case Standard::OperandType_bss: return Standard::CEOperandType_8;
    case Standard::OperandType_bcd: return Standard::CEOperandType_80dec;
    // case OperandType_bsq: return [ no equivalent in coder's edition. ]
    // case OperandType_c: return [ no equivalent in coder's edition. ]
    case Standard::OperandType_d:   return Standard::CEOperandType_32;
    case Standard::OperandType_ds:  return Standard::CEOperandType_32;
    case Standard::OperandType_di:  return Standard::CEOperandType_32int;
    case Standard::OperandType_dq:  return Standard::CEOperandType_128;
    case Standard::OperandType_dqp: return Standard::CEOperandType_32_64;
    case Standard::OperandType_dr:  return Standard::CEOperandType_64real;
    case Standard::OperandType_e:   return Standard::CEOperandType_14_28;
    case Standard::OperandType_er:  return Standard::CEOperandType_80real;
    case Standard::OperandType_p:   return Standard::CEOperandType_p;
    case Standard::OperandType_pi:  return Standard::CEOperandType_64mmx;
    // case OperandType_pd: return [ no equivalent in coder's edition. ]
    case Standard::OperandType_ps:  return Standard::CEOperandType_128pf;
    case Standard::OperandType_psq: return Standard::CEOperandType_64;
    // case OperandType_pt: return [ no equivalent in coder's edition. ]
    case Standard::OperandType_ptp: return Standard::CEOperandType_ptp;
    case Standard::OperandType_q:   return Standard::CEOperandType_64;
    case Standard::OperandType_qi:  return Standard::CEOperandType_64int;
    case Standard::OperandType_qp:  return Standard::CEOperandType_64;
    // case OperandType_s: return [ no equivalent in coder's edition. ]
    // case OperandType_sd: return [ no equivalent in coder's edition. ]
    // case OperandType_si: return [ no equivalent in coder's edition. ]
    case Standard::OperandType_sr:  return Standard::CEOperandType_32real;
    // case OperandType_ss: return [ no equivalent in coder's edition. ]
    case Standard::OperandType_st:  return Standard::CEOperandType_94_108;
    case Standard::OperandType_stx: return Standard::CEOperandType_512;
    // case OperandType_t: return [ no equivalent in coder's edition. ]
    case Standard::OperandType_v:   return Standard::CEOperandType_16_32;
    case Standard::OperandType_vds: return Standard::CEOperandType_16_32;
    case Standard::OperandType_vq:  return Standard::CEOperandType_64_16;
    case Standard::OperandType_vqp: return Standard::CEOperandType_16_32_64;
    case Standard::OperandType_vs:  return Standard::CEOperandType_16_32;
    case Standard::OperandType_w:   return Standard::CEOperandType_16;
    case Standard::OperandType_wi:  return Standard::CEOperandType_16int;

    default: break;
    }

    return Standard::CEOperandType_Invalid;
}
