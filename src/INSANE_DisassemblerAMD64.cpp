//=========================================================================
//                      INSANE Disassembler AMD64 ( 64-bit mode only )
//=========================================================================
// by      : INSANE
// created : 13/12/2025
// 
// purpose : Disassembler for 64 bit AMD64 assembly.
//-------------------------------------------------------------------------
#include "../Include/INSANE_DisassemblerAMD64.h"

#include <vector>
#include <assert.h>
#include "Tables/Tables.h"
#include "Tables/TableValidation/TableValidation.h"
#include "Math/SafeBitWiseOps.h"
#include "ObjectNomenclature.h"
#include "Util/Terminal/Terminal.h"


// NOTE : Mind this.
using namespace INSANE_DASM64_NAMESPACE;



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace INSANE_DASM64_NAMESPACE
{
    // Local functions, for decoding different encodings. To be used only in the main InsaneDASM64::Decode function.
    // NOTE : These functions update the iterator ( index in the byteStream ) according to the bytes they consume.
    static IDASMErrorCode_t DecodeLegacyEncoding     (const std::vector<Byte>& vecInput, Instruction_t* pInstOut, size_t& iIterator);
    static IDASMErrorCode_t DecodeVEXEncoding        (const std::vector<Byte>& vecInput, Instruction_t* pInstOut, size_t& iIterator);

    // Local functions, for disassembling different encodings. To be used only in the main InsaneDASM64::Disassemble function.
    static IDASMErrorCode_t DisassembleLegacyEncoding(const LegacyInst_t* inst, std::string& szOutput);
    static IDASMErrorCode_t DisassembleVEXEncoding   (const VEXInst_t* inst,    std::string& szOutput);
};



// Local Fn...
static inline void PrintInstBytes(const std::vector<Instruction_t>& vecInput)
{
    for (const Instruction_t& instO : vecInput)
    {
        const LegacyInst_t& inst = *reinterpret_cast<LegacyInst_t*>(instO.m_pInst);

        if (inst.m_opCode.OpByteCount() == 0)
        {
            printf("EMTPY INSTRUCTION!\n");
            continue;
        }


        printf("[ %12s ]", inst.m_opCode.m_pOpCodeDesc->m_szName);

        for (int i = 0; i < Rules::MAX_LEGACY_PREFIX; i++)
        {
            if (i < inst.m_legacyPrefix.m_nPrefix)
                printf("0x%02X ", inst.m_legacyPrefix.m_legacyPrefix[i]);
            else
                printf("~~~~ ");
        }
        printf(" . ");


        if(inst.m_bHasREX == true)
        {
            printf("0x%02X ", inst.m_iREX);
        }
        else
        {
            printf("~~~~ ");
        }
        printf(" . ");


        for (int i = 0; i < Rules::MAX_OPBYTES; i++)
        {
            if(i < inst.m_opCode.OpByteCount())
            {
                printf("0x%02X ", inst.m_opCode.m_opBytes[i]);
            }
            else
            {
                printf("~~~~ ");
            }
        }
        printf(" . ");


        if (inst.m_bHasModRM == true)
        {
            printf("0x%02X ", inst.m_modrm.Get());
        }
        else
        {
            printf("~~~~ ");
        }
        printf(" . ");


        if (inst.m_bHasSIB == true)
        {
            printf("0x%02X ", inst.m_SIB.Get());
        }
        else
        {
            printf("~~~~ ");
        }
        printf(" . ");


        for (int i = 0; i < Rules::MAX_DISPLACEMENT_BYTES; i++)
        {
            if(i < inst.m_displacement.ByteCount())
            {
                printf("0x%02X ", inst.m_displacement.m_iDispBytes[i]);
            }
            else
            {
                printf("~~~~ ");
            }
        }
        printf(" . ");


        for (int i = 0; i < Rules::MAX_IMMEDIATE_BYTES; i++)
        {
            if (i < inst.m_immediate.ByteCount())
            {
                printf("0x%02X ", inst.m_immediate.m_immediateByte[i]);
            }
            else
            {
                printf("~~~~ ");
            }
        }
        printf(" . ");


        printf("\n");
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t INSANE_DASM64_NAMESPACE::Initialize()
{
    // Tables...
    {
        IDASMErrorCode_t iErrorCode = G::g_tables.Initialize();
        if (iErrorCode != IDASMErrorCode_t::IDASMErrorCode_Success)
            return iErrorCode;
    }


    // Validate tables.
    {
        if(CheckTables() == false)
        {
            FAIL_LOG("Tables failed validation check");
            return IDASMErrorCode_FailedInit;
        }
    }


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t INSANE_DASM64_NAMESPACE::DecodeAndDisassemble(const std::vector<Byte>& vecInput, std::vector<std::string>& vecOutput)
{
    vecOutput.clear();


    // Parse input...
    std::vector<Instruction_t> vecParsedInst; vecParsedInst.clear();

    IDASMErrorCode_t iDecoderErrorCode = Decode(vecInput, vecParsedInst); /* Delete this */ PrintInstBytes(vecParsedInst);
    if (iDecoderErrorCode != IDASMErrorCode_Success)
        return iDecoderErrorCode;


    // Decoding didn't fail but its empty :(
    if (vecParsedInst.empty() == true)
        return IDASMErrorCode_t::IDASMErrorCode_Success;


    // Disassemble decoded instruction.
    IDASMErrorCode_t iDASMErrorCode = Disassemble(vecParsedInst, vecOutput);
    if (iDASMErrorCode != IDASMErrorCode_t::IDASMErrorCode_Success)
        return iDASMErrorCode;


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int OperandTypeToAddressSizeInByte(CEOperandTypes_t iCEOperandType, int iAddressSizeInByte)
{
    assert((iAddressSizeInByte == 2 || iAddressSizeInByte == 4 || iAddressSizeInByte == 8) && "Invalid operand size!!");

    switch (iCEOperandType)
    {
    case InsaneDASM64::CEOperandType_8:            return 1;
    case InsaneDASM64::CEOpearndType_16or32_twice: return iAddressSizeInByte == 2 ? 4 : 8;
    case InsaneDASM64::CEOperandType_16_32:        return iAddressSizeInByte == 2 ? 2 : 4;
    case InsaneDASM64::CEOperandType_16_32_64:     return iAddressSizeInByte;

    case InsaneDASM64::CEOperandType_16:
    case InsaneDASM64::CEOperandType_16int:        return 2;

    case InsaneDASM64::CEOperandType_32:
    case InsaneDASM64::CEOperandType_32real:
    case InsaneDASM64::CEOperandType_32int:        return 4;

    case InsaneDASM64::CEOperandType_32_64:        return iAddressSizeInByte == 8 ? 8 : 4;

    case InsaneDASM64::CEOperandType_64mmx:
    case InsaneDASM64::CEOperandType_64:
    case InsaneDASM64::CEOperandType_64int:
    case InsaneDASM64::CEOperandType_64real:       return 8;

    case InsaneDASM64::CEOperandType_64_16:        return iAddressSizeInByte == 2 ? 2 : 8;


    case InsaneDASM64::CEOperandType_128pf:
    case InsaneDASM64::CEOperandType_80dec:
    case InsaneDASM64::CEOperandType_128:
    case InsaneDASM64::CEOperandType_14_28:
    case InsaneDASM64::CEOperandType_80real:
    case InsaneDASM64::CEOperandType_p:
    case InsaneDASM64::CEOperandType_ptp:
    case InsaneDASM64::CEOperandType_94_108:
    case InsaneDASM64::CEOperandType_512:
        break;

    default: break;
    }

    // If no operand type is any "opinion" of what the address size should be,
    // we shall default to the address size ( 8 bytes in longmode, unless override by 0x67 address size override prefix, in which case it will be 4 bytes ) ?
    return iAddressSizeInByte;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int OperandTypeToAddressSizeInBits(CEOperandTypes_t iCEOperandType, int iAddressSizeInByte)
{
    int iSize = OperandTypeToAddressSizeInByte(iCEOperandType, iAddressSizeInByte);
    
    // Keep the invalid size invalid, and scale valid size.
    return iSize <= 0 ? -1 : iSize * 8;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int OperandTypeToOperandSizeInBytes(CEOperandTypes_t iCEOperandType, int iOperandSizeInBytes)
{
    // This function is used to determine width of register used when we have to use the modrm byte
    // to determine which register we have to use. Because the operand size is not always the correct
    // and some operand types don't give a fuck about what the operand size is.

    assert((iOperandSizeInBytes == 2 || iOperandSizeInBytes == 4 || iOperandSizeInBytes == 8) && "Invalid operand size!!");

    switch (iCEOperandType)
    {
    case InsaneDASM64::CEOperandType_8:            return 1;
    case InsaneDASM64::CEOpearndType_16or32_twice: return iOperandSizeInBytes == 2 ? 4 : 8;
    case InsaneDASM64::CEOperandType_16_32:        return iOperandSizeInBytes == 2 ? 2 : 4;
    case InsaneDASM64::CEOperandType_16_32_64:     return iOperandSizeInBytes;

    case InsaneDASM64::CEOperandType_16:
    case InsaneDASM64::CEOperandType_16int:        return 2;

    case InsaneDASM64::CEOperandType_32:
    case InsaneDASM64::CEOperandType_32real:
    case InsaneDASM64::CEOperandType_32int:        return 4;

    case InsaneDASM64::CEOperandType_32_64:        return iOperandSizeInBytes == 8 ? 8 : 4;

    case InsaneDASM64::CEOperandType_64mmx:
    case InsaneDASM64::CEOperandType_64:
    case InsaneDASM64::CEOperandType_64int:
    case InsaneDASM64::CEOperandType_64real:       return 8;

    case InsaneDASM64::CEOperandType_64_16:        return iOperandSizeInBytes == 2 ? 2 : 8;

        
    case InsaneDASM64::CEOperandType_128pf:
    case InsaneDASM64::CEOperandType_80dec:
    case InsaneDASM64::CEOperandType_128:
    case InsaneDASM64::CEOperandType_14_28:
    case InsaneDASM64::CEOperandType_80real:
    case InsaneDASM64::CEOperandType_p:
    case InsaneDASM64::CEOperandType_ptp:
    case InsaneDASM64::CEOperandType_94_108:
    case InsaneDASM64::CEOperandType_512:
        break;

    default: break;
    }

    return -1;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int OperandTypeToOperandSizeInBits(CEOperandTypes_t iCEOperandType, int iOperandSizeInBytes)
{
    int iSize = OperandTypeToOperandSizeInBytes(iCEOperandType, iOperandSizeInBytes);
    
    // Keep the invalid size invalid, and scale valid size.
    return iSize <= 0 ? -1 : iSize * 8;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t INSANE_DASM64_NAMESPACE::Disassemble(const std::vector<Instruction_t>& vecInput, std::vector<std::string>& vecOutput)
{
    for(const Instruction_t& inst : vecInput)
    {
        std::string szInst(Rules::OPCODE_NAME_SENTINAL);


        switch (inst.m_iInstEncodingType) 
        {
            case Instruction_t::InstEncodingType_Legacy:
                DisassembleLegacyEncoding(reinterpret_cast<LegacyInst_t*>(inst.m_pInst), szInst);
                break;

            case Instruction_t::InstEncodingType_VEX:
                DisassembleVEXEncoding(reinterpret_cast<VEXInst_t*>(inst.m_pInst), szInst);
                break;

            case Instruction_t::InstEncodingType_EVEX:
            case Instruction_t::InstEncodingType_XOP:
            default:
                FAIL_LOG("Incompatible encoding : %d", inst.m_iInstEncodingType);
                assert(false && "Incompatible encoding type");
                break;
        }


        vecOutput.push_back(szInst);
    }

    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static IDASMErrorCode_t INSANE_DASM64_NAMESPACE::DisassembleLegacyEncoding(const LegacyInst_t* inst, std::string& szOutput)
{
    const OpCodeDesc_t* pOpCodeDesc = inst->m_opCode.m_pOpCodeDesc;
    assert(pOpCodeDesc != nullptr && "OpCodeDescription pointer was null");


    // Dump our the operator name.
    printf("%s ", pOpCodeDesc->m_szName);


    // Iterate all operands and disassembler.
    for (int iOperandIndex = 0; iOperandIndex < pOpCodeDesc->m_nOperands; iOperandIndex++)
    {
        const Operand_t* pOperand = &pOpCodeDesc->m_operands[iOperandIndex];

        CEOperandModes_t iCEOperandMode = GeekToCoderOperandMode(pOperand->m_iOperandMode);
        CEOperandTypes_t iCEOperandType = GeekToCoderOperandType(pOperand->m_iOperandType);
        OperandModes_t   iOperandMode   = pOperand->m_iOperandMode;
        OperandTypes_t   iOperandType   = pOperand->m_iOperandType;


        switch (pOperand->m_iOperandCatagory)
        {
            // Simple and easy, just print a fucking int.
            case Operand_t::OperandCatagory_Literal:
                printf("%d", pOperand->m_iOperandLiteral);
                break;


                // Simple and easy, just print register name.
            case Operand_t::OperandCatagory_Register:
                printf("%s", pOperand->m_iOperandRegister.ToString());
                break;


                // This will get nasty. There are many operand addressing methods, and we 
                // need to take care of all of them. 
            case Operand_t::OperandCatagory_Legacy:
                {

                    switch(iCEOperandMode)
                    {
                        case CEOperandMode_CRn:
                            printf("%s", Register_t(Register_t::RegisterClass_Control, inst->ModRM_Reg(), 0).ToString());
                            break;

                        case CEOperandMode_DRn:
                            printf("%s", Register_t(Register_t::RegisterClass_Debug, inst->ModRM_Reg(), 0).ToString());
                            break;

                        case CEOperandMode_ptr:
                        case CEOperandMode_rel:
                        case CEOperandMode_imm:
                        case CEOperandMode_moffs:
                            {
                                printf("0x"); // Handling endian.
                                for (int i = inst->m_immediate.ByteCount() - 1; i >= 0; i--)
                                {
                                    printf("%02X", inst->m_immediate.m_immediateByte[i]);
                                }
                                break;
                            }

                        case CEOperandMode_m:
                        case CEOperandMode_rm:
                        case CEOperandMode_mmm64:
                        case CEOperandMode_xmmm:
                            {
                                Register_t::RegisterClass_t iRegisterClass = Register_t::RegisterClass_GPR;


                                // Overriding register class.
                                if (iCEOperandMode == CEOperandModes_t::CEOperandMode_xmmm)
                                {
                                    iRegisterClass = Register_t::RegisterClass_SSE;
                                }
                                else if (iCEOperandMode == CEOperandModes_t::CEOperandMode_STim || iCEOperandMode == CEOperandModes_t::CEOperandMode_STi)
                                {
                                    iRegisterClass = Register_t::RegisterClass_FPU;
                                }
                                else if (iCEOperandMode == CEOperandModes_t::CEOperandMode_mmm64)
                                {
                                    iRegisterClass = Register_t::RegisterClass_MMX;
                                }


                                Register_t reg(iRegisterClass, inst->ModRM_RM(),
                                        inst->ModRM_Mod() == 0b11 ? 
                                        OperandTypeToOperandSizeInBits(iCEOperandType, inst->GetOperandSizeInBytes(false)) :
                                        OperandTypeToAddressSizeInBits(iCEOperandType, inst->GetAddressSizeInBytes()));


                                if(inst->ModRM_Mod() == 0b11)
                                {
                                    printf("%s", reg.ToString());
                                    break;
                                }
                                else if (inst->m_bHasSIB == false)
                                {
                                    printf("[ %s ", reg.ToString());

                                    // Printing displacement, if any.
                                    if(inst->m_displacement.ByteCount() > 0)
                                    {
                                        printf("+ 0x");
                                        for (int i = inst->m_displacement.ByteCount() - 1; i >= 0; i--)
                                        {
                                            printf("%02X", inst->m_displacement.m_iDispBytes[i]);
                                        }
                                    }

                                    printf("]");
                                }
                                else // Here are expecting memory address using SIB byte.
                                {
                                    printf("[ ");

                                    bool bNoBaseReg = inst->ModRM_Mod() == 0 && inst->SIB_Base() == 0b101;
                                    if (bNoBaseReg == false)
                                    {
                                        Register_t iBaseReg(Register_t::RegisterClass_GPR, inst->SIB_Base(), OperandTypeToAddressSizeInBits(iCEOperandType, inst->GetAddressSizeInBytes())); // * 8 : Bytes to bits.
                                        printf("%s", iBaseReg.ToString());
                                    }

                                    // Index register.
                                    uint64_t iSIBIndex = inst->SIB_Index();
                                    if (iSIBIndex != 0b100)
                                    {
                                        printf(" + %s", Register_t(Register_t::RegisterClass_GPR, iSIBIndex, OperandTypeToAddressSizeInBits(iCEOperandType, inst->GetAddressSizeInBytes())).ToString()); // * 8 : Bytes to bits.

                                        // Scale.
                                        uint64_t iScale = 1llu << inst->SIB_Scale();
                                        if (iScale > 1)
                                        {
                                            printf(" * %llu", iScale);
                                        }
                                    }

                                    // Displacement ( if any )
                                    if(inst->m_displacement.ByteCount() > 0)
                                    {
                                        printf(" + 0x");
                                        for (int i = inst->m_displacement.ByteCount() - 1; i >= 0; i--)
                                        {
                                            printf("%02X", inst->m_displacement.m_iDispBytes[i]);
                                        }
                                    }

                                    printf(" ]");
                                }
                                break;
                            }

                        case CEOperandMode_STi:
                            {
                                Register_t reg(Register_t::RegisterClass_FPU, inst->ModRM_RM(), OperandTypeToOperandSizeInBits(iCEOperandType, inst->GetOperandSizeInBytes(false)));
                                printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_r:
                            {
                                Register_t reg(Register_t::RegisterClass_GPR, -1, OperandTypeToOperandSizeInBits(iCEOperandType, inst->GetOperandSizeInBytes(false)));

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

                                printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_mm:
                            {
                                Register_t reg(Register_t::RegisterClass_MMX, -1, OperandTypeToOperandSizeInBits(iCEOperandType, inst->GetOperandSizeInBytes(false)));

                                assert((iOperandMode == OperandModes_t::OperandMode_N || iOperandMode == OperandModes_t::OperandMode_P) && "Invalid operand mode with Coder's edition (mm)");

                                if (iOperandMode == OperandModes_t::OperandMode_N)
                                {
                                    reg.m_iRegisterIndex = inst->ModRM_RM();
                                }
                                else if (iOperandMode == OperandModes_t::OperandMode_P)
                                {
                                    reg.m_iRegisterIndex = inst->ModRM_Reg();
                                }

                                printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_Sreg:
                            {
                                Register_t reg(Register_t::RegisterClass_Segment, inst->ModRM_Reg(), OperandTypeToOperandSizeInBits(iCEOperandType, inst->GetOperandSizeInBytes(false)));
                                printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_TRn:
                            {
                                Register_t reg(Register_t::RegisterClass_Test, inst->ModRM_Reg(), OperandTypeToOperandSizeInBits(iCEOperandType, inst->GetOperandSizeInBytes(false)));
                                printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_xmm:
                            {
                                assert((iOperandMode == OperandModes_t::OperandMode_U || iOperandMode == OperandModes_t::OperandMode_V) && "Invalid Operand Modes for Coder's edition operand type : xmm");

                                Register_t reg(Register_t::RegisterClass_SSE, inst->ModRM_RM(), OperandTypeToOperandSizeInBits(iCEOperandType, inst->GetOperandSizeInBytes(false)));
                                if (iOperandMode == OperandModes_t::OperandMode_V)
                                    reg.m_iRegisterIndex = inst->ModRM_Reg();

                                printf("%s", reg.ToString());
                                break;
                            }

                            // Just a __placeholder__ for now.
                        default: printf(CONSOLE_RED "%d %d" CONSOLE_RESET, pOperand->m_iOperandMode, pOperand->m_iOperandType); break;
                    }
                }
                break;

            default: break;
        }

        printf(", ");
    }

    printf("\n");


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static IDASMErrorCode_t INSANE_DASM64_NAMESPACE::DisassembleVEXEncoding(const VEXInst_t* inst, std::string& szOutput)
{
    assert(false && "VEX disassembler is not implemted yet.");

    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t INSANE_DASM64_NAMESPACE::Decode(const std::vector<Byte>& vecInput, std::vector<Instruction_t>& vecOutput)
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
        return IDASMErrorCode_InvalidVEXInst;


    size_t     nBytes = vecInput.size();
    VEXInst_t* pInst  = reinterpret_cast<VEXInst_t*>(pInstOut->m_pInst);
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
    iIterator         += nVEXBytes;

    if(iIterator >= nBytes)
        return IDASMErrorCode_InvalidVEXInst;


    // Capture opcode.
    pInst->m_opcode = vecInput[iIterator]; iIterator++;

    if(iIterator >= nBytes)
        return IDASMErrorCode_InvalidVEXInst;


    // Capture modrm.
    pInst->m_modrm.Store(vecInput[iIterator]); iIterator++;

    if(iIterator >= nBytes)
        return IDASMErrorCode_InvalidVEXInst;


    // We need SIB??
    pInst->m_bHasSIB = pInst->m_modrm.ModValueAbs() != 0b11 && pInst->m_modrm.RMValueAbs() == 0b100;
    if(pInst->m_bHasSIB == true)
    {
        pInst->m_SIB.Store(vecInput[iIterator]); iIterator++;
    }

    if(iIterator >= nBytes)
        return IDASMErrorCode_InvalidVEXInst;


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


    // Capture Displacement bytes.
    for (size_t iDispIndex = iIterator; iDispIndex < nBytes && iDispIndex - iIterator < iDisplacementSize; iDispIndex++)
    {
        pInst->m_disp.PushByte(vecInput[iDispIndex]);
    }
    iIterator += pInst->m_disp.ByteCount();


    if(iIterator >= nBytes)
        return IDASMErrorCode_InvalidVEXInst;


    // Immediate?


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


    LegacyInst_t* pInst = reinterpret_cast<LegacyInst_t*>(pInstOut->m_pInst);
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
                OpCodeDesc_t* pOpCodeTable = G::g_tables.GetOpCodeTable(iTableIndex, iLastByte);
                if (pOpCodeTable == nullptr)
                {
                    FAIL_LOG("nullptr table\n");
                    return IDASMErrorCode_t::IDASMErrorCode_InvalidOpCode;
                }


                // Pull OpCode description from table.
                OpCodeDesc_t* pOpCodeDesc = &pOpCodeTable[iOpCodeByte];
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
            CEOperandTypes_t iImmOperandType = CEOperandTypes_t::CEOperandType_Invalid;
            OperandModes_t   iImmOperandMode = OperandModes_t::OperandMode_Invalid;
            for (int iOperandIndex = 0; iOperandIndex < pInst->m_opCode.m_pOpCodeDesc->m_nOperands; iOperandIndex++)
            {
                const Operand_t* pOperand = &pInst->m_opCode.m_pOpCodeDesc->m_operands[iOperandIndex];

                // If we found an operand with addresing method that requires immediate, store its operand type, and 
                // break out.
                OperandModes_t iOperandMode = pOperand->m_iOperandMode;
                if (iOperandMode == OperandModes_t::OperandMode_I || 
                    iOperandMode == OperandModes_t::OperandMode_J ||
                    iOperandMode == OperandModes_t::OperandMode_O ||
                    iOperandMode == OperandModes_t::OperandMode_A)
                {
                    iImmOperandMode = iOperandMode;
                    iImmOperandType = GeekToCoderOperandType(pOperand->m_iOperandType);
                    break;
                }
            }


            // Store immediate bytes according to operand type
            if (iImmOperandType != CEOperandTypes_t::CEOperandType_Invalid)
            {
                int iImmediateSize = 0;
                if (iImmOperandMode == OperandMode_O)
                {
                    iImmediateSize = pInst->GetAddressSizeInBytes();
                }
                else
                {
                    switch (iImmOperandType)
                    {
                    case CEOperandType_8:        iImmediateSize = 1;            break;
                    case CEOperandType_16:       iImmediateSize = 2;            break;
                    case CEOperandType_32:       iImmediateSize = 4;            break;
                    case CEOperandType_64:       iImmediateSize = 8;            break;
                    case CEOperandType_16_32:    iImmediateSize = pInst->GetOperandSizeInBytes(true);  break;
                    case CEOperandType_16_32_64: iImmediateSize = pInst->GetOperandSizeInBytes(false); break; // Promoted to qword by REX.W ?

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
CEOperandModes_t INSANE_DASM64_NAMESPACE::GeekToCoderOperandMode(OperandModes_t iOperandMode)
{
    switch (iOperandMode)
    {
    case OperandMode_A: return CEOperandMode_ptr;
    case OperandMode_BA: return CEOperandMode_m;
    case OperandMode_BB: return CEOperandMode_m;
    case OperandMode_BD: return CEOperandMode_m;
    case OperandMode_C: return CEOperandMode_CRn;
    case OperandMode_D: return CEOperandMode_DRn;
    case OperandMode_E: return CEOperandMode_rm;
    case OperandMode_ES: return CEOperandMode_STim;
    case OperandMode_EST: return CEOperandMode_STi;
    // case OperandMode_F: return [ no equivalent operand mode in coder's edition ]
    case OperandMode_G: return CEOperandMode_r;
    case OperandMode_H: return CEOperandMode_r;
    case OperandMode_I: return CEOperandMode_imm;
    case OperandMode_J: return CEOperandMode_rel;
    case OperandMode_M: return CEOperandMode_m;
    case OperandMode_N: return CEOperandMode_mm;
    case OperandMode_O: return CEOperandMode_moffs;
    case OperandMode_P: return CEOperandMode_mm;
    case OperandMode_Q: return CEOperandMode_mmm64;
    case OperandMode_R: return CEOperandMode_r;
    case OperandMode_S: return CEOperandMode_Sreg;
    // case OperandMode_SC: return [ no equivalent operand mode in coder's edition ]
    case OperandMode_T: return CEOperandMode_TRn;
    case OperandMode_U: return CEOperandMode_xmm;
    case OperandMode_V: return CEOperandMode_xmm;
    case OperandMode_W: return CEOperandMode_xmmm;
    case OperandMode_X: return CEOperandMode_m;
    case OperandMode_Y: return CEOperandMode_m;
    case OperandMode_Z: return CEOperandMode_r;

    default: break;
    }

    return CEOperandModes_t::CEOperandMode_Invalid;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
CEOperandTypes_t INSANE_DASM64_NAMESPACE::GeekToCoderOperandType(OperandTypes_t iOperandType)
{
    switch (iOperandType)
    {
    case OperandType_a:   return CEOpearndType_16or32_twice;
    case OperandType_b:   return CEOperandType_8;
    case OperandType_bs:  return CEOperandType_8;
    case OperandType_bss: return CEOperandType_8;
    case OperandType_bcd: return CEOperandType_80dec;
    // case OperandType_bsq: return [ no equivalent in coder's edition. ]
    // case OperandType_c: return [ no equivalent in coder's edition. ]
    case OperandType_d:   return CEOperandType_32;
    case OperandType_ds:  return CEOperandType_32;
    case OperandType_di:  return CEOperandType_32int;
    case OperandType_dq:  return CEOperandType_128;
    case OperandType_dqp: return CEOperandType_32_64;
    case OperandType_dr:  return CEOperandType_64real;
    case OperandType_e:   return CEOperandType_14_28;
    case OperandType_er:  return CEOperandType_80real;
    case OperandType_p:   return CEOperandType_p;
    case OperandType_pi:  return CEOperandType_64mmx;
    // case OperandType_pd: return [ no equivalent in coder's edition. ]
    case OperandType_ps:  return CEOperandType_128pf;
    case OperandType_psq: return CEOperandType_64;
    // case OperandType_pt: return [ no equivalent in coder's edition. ]
    case OperandType_ptp: return CEOperandType_ptp;
    case OperandType_q:   return CEOperandType_64;
    case OperandType_qi:  return CEOperandType_64int;
    case OperandType_qp:  return CEOperandType_64;
    // case OperandType_s: return [ no equivalent in coder's edition. ]
    // case OperandType_sd: return [ no equivalent in coder's edition. ]
    // case OperandType_si: return [ no equivalent in coder's edition. ]
    case OperandType_sr:  return CEOperandType_32real;
    // case OperandType_ss: return [ no equivalent in coder's edition. ]
    case OperandType_st:  return CEOperandType_94_108;
    case OperandType_stx: return CEOperandType_512;
    // case OperandType_t: return [ no equivalent in coder's edition. ]
    case OperandType_v:   return CEOperandType_16_32;
    case OperandType_vds: return CEOperandType_16_32;
    case OperandType_vq:  return CEOperandType_64_16;
    case OperandType_vqp: return CEOperandType_16_32_64;
    case OperandType_vs:  return CEOperandType_16_32;
    case OperandType_w:   return CEOperandType_16;
    case OperandType_wi:  return CEOperandType_16int;

    default: break;
    }

    return CEOperandType_Invalid;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
OpCode_t::OpCode_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void OpCode_t::Clear()
{
    m_nOpBytes        = 0;
    m_pRootOpCodeDesc = nullptr;
    m_pOpCodeDesc     = nullptr;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int OpCode_t::OpByteCount() const
{
    return m_nOpBytes;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int OpCode_t::OperandCount() const
{
    assert(m_pOpCodeDesc != nullptr && "No OpCode description is stored for this OpCode_t");
    return m_pOpCodeDesc == nullptr ? -1 : m_pOpCodeDesc->m_nOperands;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Byte OpCode_t::GetMostSignificantOpCode() const
{
    assert(m_nOpBytes > 0 && m_nOpBytes <= Rules::MAX_OPBYTES && "OpCode count is invalid!");
    if (m_nOpBytes <= 0 || m_nOpBytes > Rules::MAX_OPBYTES)
        return 0x00;

    return m_opBytes[m_nOpBytes - 1];
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool OpCode_t::PushOpCode(Byte iByte)
{
    assert(m_nOpBytes < Rules::MAX_OPBYTES && "MAX opbytes are already stored!");
    if(m_nOpBytes >= Rules::MAX_OPBYTES)
        return false;

    m_opBytes[m_nOpBytes] = iByte;
    m_nOpBytes++;
    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void OpCode_t::StoreOperatorDesc(OpCodeDesc_t* pOperatorInfo)
{
    // Storing root & final opcode description.
    m_pRootOpCodeDesc = pOperatorInfo;
    m_pOpCodeDesc     = nullptr; // To determine final child opcodeDesc, we will need modrm byte. which we might not have.
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool OpCode_t::ModRMRequired(const LegacyPrefix_t* pPrefix) const
{
    assert(m_pRootOpCodeDesc != nullptr && "No root OpCode description is stored for this OpCode_t");
    
    if (m_pRootOpCodeDesc == nullptr)
        return false;


    // In case split type is any of these. We will need modRM 100%
    if (m_pRootOpCodeDesc->m_iVarientType == OpCodeDesc_t::VarientKey_ModRM_MOD ||
        m_pRootOpCodeDesc->m_iVarientType == OpCodeDesc_t::VarientKey_ModRM_REG ||
        m_pRootOpCodeDesc->m_iVarientType == OpCodeDesc_t::VarientKey_ModRM_RM)
    {
        return true;
    }


    // In case we have a legacy prefix split, we need to check if we need a
    // modrm or not, according to child OpCodeDesc.
    if (m_pRootOpCodeDesc->m_iVarientType == OpCodeDesc_t::VarientKey_LegacyPrefix)
    {
        // Every legacy prefix split has default entry ( no prefix ) @ index 0.
        OpCodeDesc_t* pBestDesc = m_pRootOpCodeDesc->m_pVarients[0];
        assert(pBestDesc != nullptr && "Some opcode entry with legacy prefix split has default entry ( index 0 ) as default.");


        for (int i = 0; i < pPrefix->m_nPrefix; i++)
        {
            int iPrefixIndex = G::g_tables.GetLegacyPrefixIndex(pPrefix->m_legacyPrefix[i]);
            
            if (m_pRootOpCodeDesc->m_pVarients[iPrefixIndex] != nullptr)
            {
                pBestDesc = m_pRootOpCodeDesc->m_pVarients[iPrefixIndex];
                break;
            }
        }

        return pBestDesc->m_bModrmRequired;
    }


    // If we reach here, it means there is no split, so we use root.
    return m_pRootOpCodeDesc->m_bModrmRequired;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static OpCodeDesc_t* FindVarientRecurse(const LegacyPrefix_t* pPrefix, Byte iModRM, OpCodeDesc_t* pRootOpCodeDesc)
{
    if (pRootOpCodeDesc->m_iVarientType == OpCodeDesc_t::VarientKey_None)
        return pRootOpCodeDesc;

    assert(pRootOpCodeDesc->m_pVarients != nullptr && "Varient array is nullptr, when expected a varient array!.");


    switch (pRootOpCodeDesc->m_iVarientType)
    {
    case OpCodeDesc_t::VarientKey_ModRM_MOD:
        return FindVarientRecurse(pPrefix, iModRM, pRootOpCodeDesc->m_pVarients[(iModRM >> 6) & 0b11]);

    case OpCodeDesc_t::VarientKey_ModRM_REG:
        return FindVarientRecurse(pPrefix, iModRM, pRootOpCodeDesc->m_pVarients[(iModRM >> 3) & 0b111]);

    case OpCodeDesc_t::VarientKey_ModRM_RM:
        return FindVarientRecurse(pPrefix, iModRM, pRootOpCodeDesc->m_pVarients[iModRM & 0b111]);

    case OpCodeDesc_t::VarientKey_LegacyPrefix:
    {
        OpCodeDesc_t* pOpCodeDesc = pRootOpCodeDesc->m_pVarients[0];

        for (int i = 0; i < pPrefix->PrefixCount(); i++)
        {
            Byte          iPrefix            = pPrefix->m_legacyPrefix[i];
            int           iPrefixIndex       = G::g_tables.GetLegacyPrefixIndex(iPrefix);
            OpCodeDesc_t* pPrefixOpCodeDesc  = pRootOpCodeDesc->m_pVarients[iPrefixIndex];
                
            // Check if we have any prefix such that there exists a opcode varient for that
            // prefix. Else we can always use the default entry.
            if (pPrefixOpCodeDesc != nullptr)
            {
                pOpCodeDesc = pPrefixOpCodeDesc;
                break;
            }
        }

        return FindVarientRecurse(pPrefix, iModRM, pOpCodeDesc);

        break;
    }
    default: break;
    }


    assert(false && "Invalid opcode varient set!");
    return nullptr;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool OpCode_t::InitChildVarient(const LegacyPrefix_t* pPrefix, Byte iModRM)
{
    assert(m_pRootOpCodeDesc != nullptr && "We need a root opcode description pointer to init child varient!");
    if (m_pRootOpCodeDesc == nullptr)
        return false;


    m_pOpCodeDesc = FindVarientRecurse(pPrefix, iModRM, m_pRootOpCodeDesc);
    return m_pOpCodeDesc != nullptr;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
LegacyPrefix_t::LegacyPrefix_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void LegacyPrefix_t::Clear()
{
    m_nPrefix = 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool LegacyPrefix_t::PushPrefix(Byte iByte)
{
    assert(m_nPrefix < Rules::MAX_LEGACY_PREFIX && "Prefix list is already full!!!");
    
    if (m_nPrefix >= Rules::MAX_LEGACY_PREFIX)
        return false;

    m_legacyPrefix[m_nPrefix] = iByte;
    m_nPrefix++;

    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int LegacyPrefix_t::PrefixCount() const
{
    return m_nPrefix;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
LegacyInst_t::LegacyInst_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Displacement_t::Displacement_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Displacement_t::Clear()
{
    m_nDispBytes = 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int Displacement_t::ByteCount() const
{
    return m_nDispBytes;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool Displacement_t::PushByte(Byte iByte)
{
    assert(m_nDispBytes < Rules::MAX_DISPLACEMENT_BYTES && "Maximum displcament bytes already stored!");
    if (m_nDispBytes >= Rules::MAX_DISPLACEMENT_BYTES)
        return false;

    m_iDispBytes[m_nDispBytes] = iByte; 
    m_nDispBytes++;
    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Immediate_t::Immediate_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Immediate_t::Clear()
{
    m_nImmediateBytes = 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int Immediate_t::ByteCount() const
{
    return m_nImmediateBytes;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool Immediate_t::PushByte(Byte iByte)
{
    assert(m_nImmediateBytes < Rules::MAX_IMMEDIATE_BYTES && "Max immediate bytes are already stored!!!");
    if (m_nImmediateBytes >= Rules::MAX_IMMEDIATE_BYTES)
        return false;

    m_immediateByte[m_nImmediateBytes] = iByte;
    m_nImmediateBytes++;

    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void LegacyInst_t::Clear()
{
    m_opCode.Clear();
    m_legacyPrefix.Clear();

    m_bHasREX    = false;
    m_iREX       = 0x00; // This is an invalid REX byte ( 0x40 to 0x4F ).
    m_iREXIndex  = -1; // This is used to check if REX preseeds the OpBytes or not. ( they must.)

    m_bHasModRM  = false;

    m_bHasSIB    = false;

    m_displacement.Clear();
    m_immediate.Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int LegacyInst_t::GetOperandSizeInBytes(bool bIgnoreREXW) const
{
    // REX.W == true ?
    if (m_bHasREX == true && bIgnoreREXW == false)
    {
        if (Maths::SafeAnd(m_iREX, Masks::REX_W) != false)
        {
            return 8;
        }
    }


    // Operand-size override prefix present ?
    for (int i = 0; i < m_legacyPrefix.PrefixCount(); i++)
        if (m_legacyPrefix.m_legacyPrefix[i] == 0x66)
            return 2;


    // Default operand size or 64 bit mode.
    return 4;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int InsaneDASM64::LegacyInst_t::GetAddressSizeInBytes() const
{
    // Address-size override prefix present ?
    for (int i = 0; i < m_legacyPrefix.PrefixCount(); i++)
        if (m_legacyPrefix.m_legacyPrefix[i] == 0x67)
            return 4;

    return 8;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t LegacyInst_t::ModRM_Reg() const
{
    assert(m_bHasModRM == true && "Instruction doesn't have a modrm byte.");
    if (m_bHasModRM == false)
        return UINT64_MAX;


    uint64_t iReg = (Maths::SafeAnd(m_modrm.Get(), Masks::MODRM_REG) >> 3llu);
    uint64_t iREX_W = m_bHasREX == false ? 0llu : Maths::SafeAnd(m_iREX, Masks::REX_R);

    return Maths::SafeOr(iReg, (iREX_W << 1llu));
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t LegacyInst_t::ModRM_Mod() const
{
    assert(m_bHasModRM == true && "Instruction doesn't have a modrm byte.");
    if (m_bHasModRM == false)
        return UINT64_MAX;

    return m_modrm.ModValueAbs(); // Nothings effecting modrm.mod
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t LegacyInst_t::ModRM_RM() const
{
    assert(m_bHasModRM == true && "Instruction doesn't have a modrm byte.");
    if (m_bHasModRM == false)
        return UINT64_MAX;


    // if we have SIB byte, that means modrm.RM must be 0b100. No exceptions.
    if (m_bHasSIB == true)
    {
        assert(m_modrm.RMValueAbs() == 0b100 && "SIB exists for this instruction but modrm.RM != 0b100");
        return m_modrm.RMValueAbs();
    }


    uint64_t iREX_W = m_bHasREX == false ? 0llu : Maths::SafeAnd(m_iREX, Masks::REX_B);

    return Maths::SafeOr(Maths::SafeAnd(m_modrm.Get(), Masks::MODRM_RM), (iREX_W << 3llu));
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t LegacyInst_t::SIB_Scale() const
{
    assert(m_bHasSIB == true && "SIB byte is not present for this instruction.");
    if (m_bHasSIB == false)
        return UINT64_MAX;


    // Not REX bits affect this.
    return m_SIB.ScaleValueAbs();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t LegacyInst_t::SIB_Index() const
{
    assert(m_bHasSIB == true && "SIB byte is not present for this instruction.");
    if (m_bHasSIB == false)
        return UINT64_MAX;


    uint64_t iIndex = Maths::SafeAnd(m_SIB.Get(), Masks::SIB_INDEX) >> 3llu;
    uint64_t iREX_X = m_bHasREX == false ? 0llu : Maths::SafeAnd(m_iREX, Masks::REX_X);
    return Maths::SafeOr(iIndex, iREX_X << 2llu);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t LegacyInst_t::SIB_Base() const
{
    assert(m_bHasSIB == true && "SIB byte is not present for this instruction.");
    if (m_bHasSIB == false)
        return UINT64_MAX;


    uint64_t iREX_B = m_bHasREX == false ? 0llu : Maths::SafeAnd(m_iREX, Masks::REX_B);
    return Maths::SafeOr(Maths::SafeAnd(m_SIB.Get(), Masks::SIB_BASE), iREX_B << 2llu);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Operand_t::Operand_t(int iLiteral)
{
    m_iOperandCatagory = OperandCatagory_Literal;
    m_iOperandLiteral  = iLiteral;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Operand_t::Operand_t(Register_t iRegister)
{
    m_iOperandCatagory = OperandCatagory_Register;
    m_iOperandRegister = iRegister;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Operand_t::Operand_t(OperandModes_t iOperandMode, OperandTypes_t iOperandType)
{
    m_iOperandCatagory = OperandCatagory_Legacy;
    m_iOperandMode     = iOperandMode;
    m_iOperandType     = iOperandType;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Operand_t::Reset()
{
    m_iOperandCatagory = OperandCatagory_Undefined;
    m_iOperandRegister = Register_t(Register_t::RegisterClass_Invalid, -1, -1);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t ModRM_t::ModValueAbs() const
{
    return Maths::SafeAnd(m_modrm, Masks::MODRM_MOD) >> 6;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t ModRM_t::RegValueAbs() const
{
    return Maths::SafeAnd(m_modrm, Masks::MODRM_REG) >> 3;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t ModRM_t::RMValueAbs() const
{
    return Maths::SafeAnd(m_modrm, Masks::MODRM_RM);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t SIB_t::ScaleValueAbs() const
{
    return Maths::SafeAnd(m_SIB, Masks::SIB_SCALE) >> 6;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t SIB_t::IndexValueAbs() const
{
    return Maths::SafeAnd(m_SIB, Masks::SIB_INDEX) >> 3;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t SIB_t::BaseValueAbs() const
{
    return Maths::SafeAnd(m_SIB, Masks::SIB_BASE);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
const char* Register_t::ToString() const
{
    switch (m_iRegisterClass)
    {
    case Register_t::RegisterClass_GPR:
    {
        static const char* s_szGPR64[16] = { "rax","rcx","rdx","rbx","rsp","rbp","rsi","rdi","r8","r9","r10","r11","r12","r13","r14","r15" };
        static const char* s_szGPR32[16] = { "eax","ecx","edx","ebx","esp","ebp","esi","edi","r8d","r9d","r10d","r11d","r12d","r13d","r14d","r15d" };
        static const char* s_szGPR16[16] = { "ax","cx","dx","bx","sp","bp","si","di","r8w","r9w","r10w","r11w","r12w","r13w","r14w","r15w" };
        static const char* s_szGPR8 [16] = { "al","cl","dl","bl","spl","bpl","sil","dil","r8b","r9b","r10b","r11b","r12b","r13b","r14b","r15b" };
        assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szGPR64) / sizeof(char*)) && "Invalid register index.");

        switch (m_iRegisterSize)
        {
        case 8:  return s_szGPR8[m_iRegisterIndex];
        case 16: return s_szGPR16[m_iRegisterIndex];
        case 32: return s_szGPR32[m_iRegisterIndex];
        case 64: return s_szGPR64[m_iRegisterIndex];
        
        default: 
            FAIL_LOG("Invalid register size : %d\n", m_iRegisterSize);
            assert(false && "Invalid register size"); 
            break;
        }

        return Rules::REGISTER_NAME_SENTINAL;

        break; // Just in case, if I have missed something.
    }

    case Register_t::RegisterClass_FPU:
    {
        static const char* s_szFPU[8] = { "st0","st1","st2","st3","st4","st5","st6","st7" };
        assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szFPU) / sizeof(char*)) && "Invalid register index.");

        return s_szFPU[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_SSE: // xxm Registers.
    {
        static const char* s_szXXM[16] = { "xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7","xmm8","xmm9","xmm10","xmm11","xmm12","xmm13","xmm14","xmm15" };
        assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szXXM) / sizeof(char*)) && "Invalid register index.");

        return s_szXXM[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_MMX:
    {
        static const char* s_szMMX[8] = { "mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"};
        assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szMMX) / sizeof(char*)) && "Invalid register index.");

        return s_szMMX[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_AVX: // yml Registers.
    {
        static const char* s_szYMM[16] = { "ymm0","ymm1","ymm2","ymm3","ymm4","ymm5","ymm6","ymm7","ymm8","ymm9","ymm10","ymm11","ymm12","ymm13","ymm14","ymm15" };
        assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szYMM) / sizeof(char*)) && "Invalid register index.");

        return s_szYMM[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_AVX512:
        return Rules::REGISTER_NAME_SENTINAL;
        break;

    case Register_t::RegisterClass_Segment:
    {
        static const char* s_szSegmentReg[6] = { "es","cs","ss","ds","fs","gs" };
        assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szSegmentReg) / sizeof(char*)) && "Invalid register index.");

        return s_szSegmentReg[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_Control:
    {
        static const char* s_szControlReg[9] = { "cr0","cr1","cr2","cr3","cr4","cr5","cr6","cr7","cr8" };
        assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szControlReg) / sizeof(char*)) && "Invalid register index.");

        return s_szControlReg[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_Debug:
    {
        static const char* s_szDebugReg[8] = { "dr0","dr1","dr2","dr3","dr4","dr5","dr6","dr7" };
        assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szDebugReg) / sizeof(char*)) && "Invalid register index.");

        return s_szDebugReg[m_iRegisterIndex];
        break;
    }

    case Register_t::RegisterClass_Test:
    {
        // Although I have been told that only 2 test registers ( TR6 & TR7 ) are accessible,
        // but if I only define those, it might nuke the index. So I define all test registers here.
        static const char* s_szTestReg[8] = { "TR0", "TR1", "TR2", "TR3", "TR4", "TR5", "TR6", "TR7" };
        assert(m_iRegisterIndex >= 0 && m_iRegisterIndex < (sizeof(s_szTestReg) / sizeof(char*)) && "Invalid register index.");

        return s_szTestReg[m_iRegisterIndex];
        break;
    }

    default: break;
    }

    return Rules::REGISTER_NAME_SENTINAL;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool INSANE_DASM64_NAMESPACE::Instruction_t::InitEncodingType(InstEncodingTypes_t iEncoding)
{
    // Is the encoding type valid.
    bool bValidEncoding = iEncoding >= InstEncodingTypes_t::InstEncodingType_Legacy && iEncoding < InstEncodingTypes_t::InstEncodingType_Count;
    assert(bValidEncoding == true && "Invalid encoding type passed for initializing instruction.");
    if(bValidEncoding == false)
        return false;


    // Instruction encoding must not be set.
    assert(m_iInstEncodingType == InstEncodingType_Invalid && "Instruction is already initialized to some type");
    if (m_iInstEncodingType != InstEncodingType_Invalid)
        return false;


    // Instruction pointer must be NULL.
    assert(m_pInst == nullptr && "Instruction pointer is pointing to some memory address before initializing");
    if(m_pInst != nullptr)
        return false;


    // Get the instruction size.
    size_t iInstSize = 0llu;
    switch (iEncoding) 
    {
        case InstEncodingType_Legacy: iInstSize = sizeof(LegacyInst_t); break;
        case InstEncodingType_VEX:    iInstSize = sizeof(VEXInst_t);    break;

        default: assert(false && "Size of this instruction encoding type is not know. Not implemented yet!"); break;
    }
    assert(iInstSize > 0llu && iInstSize < 0x100 && "Invalid instruction size."); // Sanity checks. So we don't accidently blow up.


    // Boom!
    m_iInstEncodingType = iEncoding;
    m_pInst             = malloc(iInstSize);


    // TODO We will make a arena allocator soon, so will need to be rewritten.
    if(m_pInst == nullptr)
    {
        FAIL_LOG("Failed memory allocation for inst encoding [ %d ] and size [ %zu ]", iEncoding, iInstSize);
        return false;
    }

    // If we don't clear it, bad things will happen.
    memset(m_pInst, 0, iInstSize);
    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
INSANE_DASM64_NAMESPACE::VEXInst_t::VEXInst_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void INSANE_DASM64_NAMESPACE::VEXInst_t::Clear()
{
    m_prefix        = 0x00;
    m_nVEXBytes     = -1;
    m_opcode        = 0x00;
    m_bHasSIB       = false;
    m_disp.Clear();
    m_bHasImmediate = false;
}
