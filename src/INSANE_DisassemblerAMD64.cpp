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

// Standard stuff...
#include "../Include/Standard/OpCodeDesc_t.h"
#include "../Include/DASMInst_t.h"
#include "../Include/Masks.h"

// Decoder -n- Disassembler.
#include "Decoder/Decoder.h"
#include "Disassembler/Disassembler.h"


// NOTE : Mind this.
using namespace InsaneDASM64;



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace INSANE_DASM64_NAMESPACE
{
    // Local functions, for disassembling different encodings. To be used only in the main InsaneDASM64::Disassemble function.
    static IDASMErrorCode_t DisassembleLegacyEncoding(const Legacy::LegacyInst_t* pInst, DASMInst_t* pOutput);
    static IDASMErrorCode_t DisassembleVEXEncoding   (const VEX::VEXInst_t*       pInst, DASMInst_t* pOutput);
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
    vecOutput.reserve(vecInput.size());

    for(const Instruction_t& inst : vecInput)
    {
        vecOutput.emplace_back();
        DASMInst_t* pInstOut = &vecOutput.back();

        Disassemble(&inst, pInstOut);
        // switch (inst.m_iInstEncodingType) 
        // {
        //     case Instruction_t::InstEncodingType_Legacy:
        //         DisassembleLegacyEncoding(reinterpret_cast<Legacy::LegacyInst_t*>(inst.m_pInst), pInstOut);
        //         break;
        //
        //     case Instruction_t::InstEncodingType_VEX:
        //         DisassembleVEXEncoding(reinterpret_cast<VEX::VEXInst_t*>(inst.m_pInst), pInstOut);
        //         break;
        //
        //     case Instruction_t::InstEncodingType_EVEX:
        //     case Instruction_t::InstEncodingType_XOP:
        //     default:
        //         FAIL_LOG("Incompatible encoding : %d", inst.m_iInstEncodingType);
        //         assert(false && "Incompatible encoding type");
        //         break;
        // }
    }

    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static IDASMErrorCode_t InsaneDASM64::DisassembleLegacyEncoding(const Legacy::LegacyInst_t* pInst, DASMInst_t* pOutput)
{
    using namespace Standard;
    const Standard::OpCodeDesc_t* pOpCodeDesc = pInst->m_opCode.m_pOpCodeDesc;
    assert(pOpCodeDesc != nullptr && "OpCodeDescription pointer was null");
    assert(pOpCodeDesc->m_iVarientType == Standard::OpCodeDesc_t::VarientKey_None && "A varient with valid split key arrived. Expected a final varient.");


    // Purpose of this string stream is to help me construct memory address in SIB format easily.
    // so I can write it to output at once. ( C style string would have been exceptionally painful. )
    std::stringstream szTemp;


    // Store Mnemonic
    strcpy(pOutput->m_szMnemonic, pOpCodeDesc->m_szName);
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
            case Operand_t::OperandCatagory_Literal:
                pOutput->PushLiteralOperand(pOperand->m_iOperandLiteral);
                break;

            case Operand_t::OperandCatagory_Register:
                pOutput->PushBackOperand(pOperand->m_iOperandRegister.ToString());
                break;

                // This will get nasty. There are many operand addressing methods, and we 
                // need to take care of all of them. 
            case Operand_t::OperandCatagory_Legacy:
                {

                    switch(iCEOperandMode)
                    {
                        case CEOperandMode_CRn:
                            pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_Control, pInst->ModRM_Reg(), 0).ToString());
                            break;

                        case CEOperandMode_DRn:
                            pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_Debug, pInst->ModRM_Reg(), 0).ToString());
                            break;

                        case CEOperandMode_ptr:
                        case CEOperandMode_rel:
                        case CEOperandMode_imm:
                        case CEOperandMode_moffs:
                                pOutput->PushLiteralFromString(pInst->m_immediate.m_immediateByte, pInst->m_immediate.ByteCount(), true);
                                break;

                        case CEOperandMode_m:
                        case CEOperandMode_rm:
                        case CEOperandMode_mmm64:
                        case CEOperandMode_xmmm:
                        // case CEOperandMode_STi:
                            {
                                Standard::Register_t::RegisterClass_t iRegisterClass = Standard::Register_t::RegisterClass_GPR;


                                // Overriding register class.
                                if(pInst->ModRM_Mod() == 0b11)
                                {
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
                                }


                                Standard::Register_t reg(iRegisterClass, pInst->ModRM_RM(),
                                        pInst->ModRM_Mod() == 0b11 ? 
                                        OperandTypeToOperandSizeInBits(iCEOperandType, pInst->GetOperandSizeInBytes(false)) :
                                        OperandTypeToAddressSizeInBits(iCEOperandType, pInst->GetAddressSizeInBytes()));


                                if(pInst->ModRM_Mod() == 0b11)
                                {
                                    pOutput->PushBackOperand(reg.ToString());
                                    // printf("%s", reg.ToString());
                                    break;
                                }
                                else if (pInst->m_bHasSIB == false)
                                {
                                    // for modrm.rm == 101, we only have to store displacement, and no register.
                                    bool bDispOnly = pInst->ModRM_RM() == 0b101;

                                    szTemp << "[ ";
                                    if(bDispOnly == false)
                                    {
                                        // Here, the modrm byte is used to represent a memory adrs.
                                        // So we will use the full width general purpose register instead of whatever this was holding.
                                        auto iOldRegSize = reg.m_iRegisterSize;
                                        reg.m_iRegisterSize = 64;

                                        szTemp << reg.ToString() << ' ';

                                        reg.m_iRegisterSize = iOldRegSize;
                                    }

                                    // Printing displacement, if any.
                                    if(pInst->m_displacement.ByteCount() > 0)
                                    {
                                        // if we have stored a register in front of it. store + sign
                                        if(bDispOnly == false)
                                            szTemp << " + ";

                                        szTemp << "0x";

                                        bool bLeadingZeroEnded = false;
                                        for (int i = pInst->m_displacement.ByteCount() - 1; i >= 0; i--)
                                        {
                                            int iByte = static_cast<int>(pInst->m_displacement.m_iDispBytes[i]);
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

                                    bool bNoBaseReg = pInst->ModRM_Mod() == 0 && pInst->SIB_Base() == 0b101;
                                    if (bNoBaseReg == false)
                                    {
                                        Standard::Register_t iBaseReg(
                                                Standard::Register_t::RegisterClass_GPR, 
                                                pInst->SIB_Base(), 
                                                OperandTypeToAddressSizeInBits(iCEOperandType, pInst->GetAddressSizeInBytes())); // * 8 : Bytes to bits.
 
                                        szTemp << iBaseReg.ToString();
                                        // printf("%s", iBaseReg.ToString());
                                    }

                                    // Index register.
                                    uint64_t iSIBIndex = pInst->SIB_Index();
                                    if (iSIBIndex != 0b100)
                                    {
                                        Standard::Register_t indexReg(Standard::Register_t::RegisterClass_GPR, iSIBIndex, 
                                                OperandTypeToAddressSizeInBits(iCEOperandType, pInst->GetAddressSizeInBytes()));
                                        szTemp << " + " << indexReg.ToString();
                                        // printf(" + %s", reg.ToString()); // * 8 : Bytes to bits.

                                        // Scale.
                                        uint64_t iScale = 1llu << pInst->SIB_Scale();
                                        if (iScale > 1)
                                        {
                                            szTemp << " * " << iScale;
                                            // printf(" * %llu", iScale);
                                        }
                                    }

                                    // Displacement ( if any )
                                    if(pInst->m_displacement.ByteCount() > 0)
                                    {
                                        szTemp << " + 0x";
                                        // printf(" + 0x");
                                        bool bLeadingZeroEnded = false;
                                        for (int i = pInst->m_displacement.ByteCount() - 1; i >= 0; i--)
                                        {
                                            int iByte = static_cast<int>(pInst->m_displacement.m_iDispBytes[i]);
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
                                Standard::Register_t reg(Standard::Register_t::RegisterClass_FPU, pInst->ModRM_RM(), OperandTypeToOperandSizeInBits(iCEOperandType, pInst->GetOperandSizeInBytes(false)));
                                pOutput->PushBackOperand(reg.ToString());
                                // printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_r:
                            {
                                Standard::Register_t reg(Standard::Register_t::RegisterClass_GPR, -1, OperandTypeToOperandSizeInBits(iCEOperandType, pInst->GetOperandSizeInBytes(false)));

                                if (iOperandMode == OperandMode_G)
                                {
                                    reg.m_iRegisterIndex = pInst->ModRM_Reg();
                                }
                                else if (iOperandMode == OperandModes_t::OperandMode_Z)
                                {
                                    // REX.B bit extends the "Lower 3 bits of the opcode". How bizzare is that?
                                    reg.m_iRegisterIndex = Maths::SafeOr(
                                            Maths::SafeAnd(pInst->m_opCode.m_pOpCodeDesc->m_iByte, 0b111), 
                                            Maths::SafeAnd(pInst->m_bHasREX == false ? 0llu : pInst->m_iREX, Legacy::Masks::REX_B) << 3llu);
                                }
                                else // Addressing mode H & R, fall under this case.
                                {
                                    reg.m_iRegisterIndex = pInst->ModRM_RM();
                                }

                                pOutput->PushBackOperand(reg.ToString());
                                // printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_mm:
                            {
                                Standard::Register_t reg(Standard::Register_t::RegisterClass_MMX, -1, OperandTypeToOperandSizeInBits(iCEOperandType, pInst->GetOperandSizeInBytes(false)));

                                assert((iOperandMode == OperandModes_t::OperandMode_N || iOperandMode == OperandModes_t::OperandMode_P) && "Invalid operand mode with Coder's edition (mm)");

                                if (iOperandMode == OperandModes_t::OperandMode_N)
                                {
                                    reg.m_iRegisterIndex = pInst->ModRM_RM();
                                }
                                else if (iOperandMode == OperandModes_t::OperandMode_P)
                                {
                                    reg.m_iRegisterIndex = pInst->ModRM_Reg();
                                }

                                pOutput->PushBackOperand(reg.ToString());
                                // printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_Sreg:
                            {
                                Standard::Register_t reg(Standard::Register_t::RegisterClass_Segment, pInst->ModRM_Reg(), OperandTypeToOperandSizeInBits(iCEOperandType, pInst->GetOperandSizeInBytes(false)));
                                pOutput->PushBackOperand(reg.ToString());
                                // printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_TRn:
                            {
                                Standard::Register_t reg(Standard::Register_t::RegisterClass_Test, pInst->ModRM_Reg(), OperandTypeToOperandSizeInBits(iCEOperandType, pInst->GetOperandSizeInBytes(false)));
                                pOutput->PushBackOperand(reg.ToString());
                                // printf("%s", reg.ToString());
                                break;
                            }

                        case CEOperandMode_xmm:
                            {
                                assert((iOperandMode == OperandModes_t::OperandMode_U || iOperandMode == OperandModes_t::OperandMode_V) && "Invalid Operand Modes for Coder's edition operand type : xmm");

                                Standard::Register_t reg(
                                        Standard::Register_t::RegisterClass_SSE, pInst->ModRM_RM(), 
                                        OperandTypeToOperandSizeInBits(iCEOperandType, pInst->GetOperandSizeInBytes(false)));

                                if (iOperandMode == OperandModes_t::OperandMode_V)
                                    reg.m_iRegisterIndex = pInst->ModRM_Reg();

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
static IDASMErrorCode_t InsaneDASM64::DisassembleVEXEncoding(const VEX::VEXInst_t* pInst, DASMInst_t* pOutput)
{
    /*
    -> Operand Addresing Method distribution among all the valid VEX encodable instructions.
    ( Data generated from the Linux kernel repository's x86-opcode-map.txt. Data might be off due
    to my lack of basic programming abilities. )
    
    OperandMode_B : 25
    OperandMode_E : 17
    OperandMode_G : 37
    OperandMode_H : 265
    OperandMode_I : 55
    OperandMode_L : 3
    OperandMode_M : 47
    OperandMode_R : 8
    OperandMode_U : 52
    OperandMode_V : 395
    OperandMode_W : 310


    -> Operand Type distribution among all the valid VEX encodable instrutions.

    OperandType_y  : 93
    OperandType_q  : 29
    OperandType_ps : 67
    OperandType_pd : 62
    OperandType_x  : 607
    OperandType_ss : 45
    OperandType_sd : 39
    OperandType_dq : 53
    OperandType_k  : 74
    OperandType_b  : 57
    OperandType_v  : 8
    OperandType_d  : 8
    OperandType_qq : 32
    OperandType_tc : 2
    OperandType_t  : 28
    OperandType_sm : 3
    OperandType_w  : 5

    */
    

    pOutput->Clear();

    const Standard::OpCodeDesc_t* pOpCodeDesc = pInst->m_opcode.m_pOpCodeDesc;
    assert(pOpCodeDesc != nullptr && "OpCodeDescription pointer was null");
    assert(pOpCodeDesc->m_iVarientType == Standard::OpCodeDesc_t::VarientKey_None && "A varient with valid split key arrived. Expected a final varient.");


    // Purpose of this string stream is to help me construct memory address in SIB format easily.
    // so I can write it to output at once. ( C style string would have been exceptionally painful. )
    std::stringstream szTemp;


    // Store Mnemoic name.
    strcpy(pOutput->m_szMnemonic, pOpCodeDesc->m_szName);


    // Iterate all operands for this opcode description and turm em to string.
    for(size_t iOperandIndex = 0; iOperandIndex < pOpCodeDesc->m_nOperands; iOperandIndex++)
    {
        szTemp.clear();

        const Standard::Operand_t* pOperand        = &pOpCodeDesc->m_operands[iOperandIndex];
        Standard::OperandModes_t   iOperandMode    = pOperand->m_iOperandMode;
        Standard::CEOperandModes_t iCEOpearendMode = GeekToCoderOperandMode(iOperandMode);
        Standard::CEOperandTypes_t iCEOperandTypes = GeekToCoderOperandType(pOperand->m_iOperandType);

        switch(pOperand->m_iOperandCatagory)
        {
            case Standard::Operand_t::OperandCatagory_Legacy:
                switch(iOperandMode)
                {
                    case Standard::OperandMode_VG:
                        {
                            uint64_t iVVVV = pInst->m_vexPrefix.vvvv();
                            
                            // register index is one's compliment of vex.vvvv,
                            iVVVV = ~iVVVV & VEX::Masks::VVVV;
                            Standard::Register_t iRegister(
                                    Standard::Register_t::RegisterClass_GPR, iVVVV,
                                    OperandTypeToOperandSizeInBits(iCEOperandTypes, pInst->GetOperandSizeInBytes()));
                            pOutput->PushBackOperand(iRegister.ToString());
                        }
                        break;

                    case Standard::OperandMode_G:
                        pOutput->PushBackOperand(Standard::Register_t(
                                    Standard::Register_t::RegisterClass_GPR, pInst->ModRM_Reg(), 
                                    OperandTypeToOperandSizeInBits(iCEOperandTypes, pInst->GetOperandSizeInBytes())).ToString());
                        break;

                    case Standard::OperandMode_VXY:
                        {
                            // Register index, size & class
                            uint64_t iVVVV         = pInst->m_vexPrefix.vvvv();
                            int      iRegisterSize = pInst->m_vexPrefix.L() == 0llu ? 128 : 256;
                            Standard::Register_t::RegisterClass_t iRegClass = pInst->m_vexPrefix.L() == 0llu ? 
                                Standard::Register_t::RegisterClass_SSE : // XXM
                                Standard::Register_t::RegisterClass_AVX;  // YMM

                            //  register index is one's complimed of vex.vvvv,
                            iVVVV = ~iVVVV & (VEX::Masks::VVVV >> 3llu);
                            Standard::Register_t iRegister(iRegClass, iVVVV, iRegisterSize);

                            pOutput->PushBackOperand(iRegister.ToString());
                        }
                        break;

                    case Standard::OperandMode_I:
                        pOutput->PushLiteralFromString(pInst->m_immediate.m_immediateByte, pInst->m_immediate.ByteCount(), true);
                        break;

                    case Standard::OperandMode_IXY:
                        {
                            assert(pInst->m_immediate.ByteCount() == 1 && "Invalid byte count for instruction with operand IXY. != 1");
                            if(pInst->m_immediate.ByteCount() != 1)
                                return IDASMErrorCode_VEXDisassemblyFailed;

                            // Register index, size & class
                            uint64_t iRegisterIndex = pInst->GetImmRegister();
                            int      iRegisterSize = pInst->m_vexPrefix.L() == 0llu ? 128 : 256;
                            Standard::Register_t::RegisterClass_t iRegClass = pInst->m_vexPrefix.L() == 0llu ? 
                                Standard::Register_t::RegisterClass_SSE : // XXM
                                Standard::Register_t::RegisterClass_AVX;  // YMM


                            Standard::Register_t iRegister(iRegClass, iRegisterIndex, iRegisterSize);

                            pOutput->PushBackOperand(iRegister.ToString());
                        }
                        break;

                    case Standard::OperandMode_R:
                        pOutput->PushBackOperand(Standard::Register_t(
                                    Standard::Register_t::RegisterClass_GPR, pInst->ModRM_RM(), 
                                    OperandTypeToOperandSizeInBits(iCEOperandTypes, pInst->GetOperandSizeInBytes())).ToString());
                        break;

                    case Standard::OperandMode_U:
                        {
                            // Register width.
                            int16_t iRegisterWidth = pInst->m_vexPrefix.L() == false ? 128 : 256;

                            // Register class.
                            Standard::Register_t::RegisterClass_t iRegisterClass = pInst->m_vexPrefix.L() == false ? 
                                Standard::Register_t::RegisterClass_SSE : Standard::Register_t::RegisterClass_AVX;

                            pOutput->PushBackOperand(Standard::Register_t(iRegisterClass, pInst->ModRM_RM(), iRegisterWidth).ToString());
                        }
                        break;

                    case Standard::OperandMode_V:
                        {
                            // Register width.
                            int16_t iRegisterWidth = pInst->m_vexPrefix.L() == false ? 128 : 256;

                            // Register class.
                            Standard::Register_t::RegisterClass_t iRegisterClass = pInst->m_vexPrefix.L() == false ? 
                                Standard::Register_t::RegisterClass_SSE : Standard::Register_t::RegisterClass_AVX;

                            pOutput->PushBackOperand(Standard::Register_t(iRegisterClass, pInst->ModRM_Reg(), iRegisterWidth).ToString());
                        }
                        break;

                    case Standard::OperandMode_E:
                    case Standard::OperandMode_M:
                    case Standard::OperandMode_W:
                        {
                            Standard::Register_t::RegisterClass_t iRegisterClass = Standard::Register_t::RegisterClass_GPR;

                            // Overriding register class.
                            if (pInst->ModRM_Mod() == 0b11 && iOperandMode == Standard::OperandMode_W) // Make sure we only use GPR when modrm represents memory.
                            {
                                iRegisterClass = pInst->m_vexPrefix.L() == false ? 
                                    Standard::Register_t::RegisterClass_SSE : Standard::Register_t::RegisterClass_AVX;
                            }

                            Standard::Register_t reg(iRegisterClass, pInst->ModRM_RM(),
                                    pInst->ModRM_Mod() == 0b11 ? 
                                    OperandTypeToOperandSizeInBits(iCEOperandTypes, pInst->GetOperandSizeInBytes()) :
                                    64); // Memory representing register's width 64 bits at all times?


                            if(pInst->ModRM_Mod() == 0b11)
                            {
                                pOutput->PushBackOperand(reg.ToString());
                                // printf("%s", reg.ToString());
                                break;
                            }
                            else if (pInst->m_bHasSIB == false)
                            {
                                // for modrm.rm == 101, we only have to store displacement, and no register.
                                bool bDispOnly = pInst->ModRM_RM() == 0b101;

                                szTemp << "[ ";
                                if(bDispOnly == false)
                                {
                                    szTemp << reg.ToString() << ' ';
                                }

                                // Printing displacement, if any.
                                if(pInst->m_disp.ByteCount() > 0)
                                {
                                    // if we have stored a register in front of it. store + sign
                                    if(bDispOnly == false)
                                        szTemp << " + ";

                                    szTemp << "0x";

                                    bool bLeadingZeroEnded = false;
                                    for (int i = pInst->m_disp.ByteCount() - 1; i >= 0; i--)
                                    {
                                        int iByte = static_cast<int>(pInst->m_disp.m_iDispBytes[i]);
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
                                // printf("]");
                            }
                            else // Here we are expecting memory address using SIB byte.
                            {
                                szTemp << "[ ";
                                // printf("[ ");

                                bool bNoBaseReg = pInst->ModRM_Mod() == 0 && pInst->SIB_Base() == 0b101;
                                if (bNoBaseReg == false)
                                {
                                    Standard::Register_t iBaseReg(Standard::Register_t::RegisterClass_GPR, pInst->SIB_Base(), 64); 

                                    szTemp << iBaseReg.ToString();
                                    // printf("%s", iBaseReg.ToString());
                                }

                                // Index register.
                                uint64_t iSIBIndex = pInst->SIB_Index();
                                if (iSIBIndex != 0b100)
                                {
                                    Standard::Register_t indexReg(Standard::Register_t::RegisterClass_GPR, iSIBIndex, 64);
                                    szTemp << " + " << indexReg.ToString();
                                    // printf(" + %s", reg.ToString()); // * 8 : Bytes to bits.

                                    // Scale.
                                    uint64_t iScale = 1llu << pInst->SIB_Scale();
                                    if (iScale > 1)
                                    {
                                        szTemp << " * " << iScale;
                                        // printf(" * %llu", iScale);
                                    }
                                }

                                // Displacement ( if any )
                                if(pInst->m_disp.ByteCount() > 0)
                                {
                                    szTemp << " + 0x";
                                    // printf(" + 0x");
                                    bool bLeadingZeroEnded = false;
                                    for (int i = pInst->m_disp.ByteCount() - 1; i >= 0; i--)
                                    {
                                        int iByte = static_cast<int>(pInst->m_disp.m_iDispBytes[i]);
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
                        }
                        break;


                    default: 
                        FAIL_LOG("Unexpected operand addressing method appeared in VEX disassembler.");
                        // TODO : Uncomment this after disassembler VEX is done.
                        // assert(false && "Unexpected operand addressing method appeared in VEX disassembler."); 
                        break;
                }
                break;

            case Standard::Operand_t::OperandCatagory_Literal:
                pOutput->PushLiteralOperand(pOperand->m_iOperandLiteral);
                break;

            case Standard::Operand_t::OperandCatagory_Register:
                pOutput->PushBackOperand(pOperand->m_iOperandRegister.ToString());
                break;

            default: 
                FAIL_LOG("Some instruction with BullShit varient key appeared! [ Varient Key : %d ]", pOperand->m_iOperandCatagory); 
                assert(false && "Some instruction with BullShit varient key appeared! [ Varient Key : %d ]");
                break;
        }
    }


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
    case InsaneDASM64::IDASMErrorCode_VEXDisassemblyFailed:  return "[ Insane Disassembler AMD64 ] VEX disassembly failed.";
    case InsaneDASM64::IDASMErrorCode_InstComponentsNotFound:return "[ Insane Disassembler AMD64 ] Instruction components were found to be null while disassembling.";
    case InsaneDASM64::IDASMErrorCode_OpCodeNotInitialized:  return "[ Insane Disassembler AMD64 ] OpCode's final varient was not initialized.";
    
    default: break;
    }

    static char s_invalidCodeBuffer[128] = "";
    sprintf(s_invalidCodeBuffer, "[ Insane Disassembler AMD64 ] Invalid Error Code { %d }", iErrorCode);
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
    case Standard::OperandMode_G:   return Standard::CEOperandMode_r;
    case Standard::OperandMode_H:   return Standard::CEOperandMode_r;
    case Standard::OperandMode_I:   return Standard::CEOperandMode_imm;
    case Standard::OperandMode_J:   return Standard::CEOperandMode_rel;
    case Standard::OperandMode_M:   return Standard::CEOperandMode_m;
    case Standard::OperandMode_N:   return Standard::CEOperandMode_mm;
    case Standard::OperandMode_O:   return Standard::CEOperandMode_moffs;
    case Standard::OperandMode_P:   return Standard::CEOperandMode_mm;
    case Standard::OperandMode_Q:   return Standard::CEOperandMode_mmm64;
    case Standard::OperandMode_R:   return Standard::CEOperandMode_r;
    case Standard::OperandMode_S:   return Standard::CEOperandMode_Sreg;
    // case OperandMode_SC: return [ no equivalent operand mode in coder's edition ]
    case Standard::OperandMode_T:   return Standard::CEOperandMode_TRn;
    case Standard::OperandMode_U:   return Standard::CEOperandMode_xmm;
    case Standard::OperandMode_V:   return Standard::CEOperandMode_xmm;
    case Standard::OperandMode_W:   return Standard::CEOperandMode_xmmm;
    case Standard::OperandMode_X:   return Standard::CEOperandMode_m;
    case Standard::OperandMode_Y:   return Standard::CEOperandMode_m;
    case Standard::OperandMode_Z:   return Standard::CEOperandMode_r;

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
    // case OperandType_c: return   [ no equivalent in coder's edition. ]
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
    // case OperandType_pd: return  [ no equivalent in coder's edition. ]
    case Standard::OperandType_ps:  return Standard::CEOperandType_128pf;
    case Standard::OperandType_psq: return Standard::CEOperandType_64;
    // case OperandType_pt: return  [ no equivalent in coder's edition. ]
    case Standard::OperandType_ptp: return Standard::CEOperandType_ptp;
    case Standard::OperandType_q:   return Standard::CEOperandType_64;
    case Standard::OperandType_qi:  return Standard::CEOperandType_64int;
    case Standard::OperandType_qp:  return Standard::CEOperandType_64;
    // case OperandType_s: return   [ no equivalent in coder's edition. ]
    // case OperandType_sd: return  [ no equivalent in coder's edition. ]
    // case OperandType_si: return  [ no equivalent in coder's edition. ]
    case Standard::OperandType_sr:  return Standard::CEOperandType_32real;
    // case OperandType_ss: return  [ no equivalent in coder's edition. ]
    case Standard::OperandType_st:  return Standard::CEOperandType_94_108;
    case Standard::OperandType_stx: return Standard::CEOperandType_512;
    // case OperandType_t: return   [ no equivalent in coder's edition. ]
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
