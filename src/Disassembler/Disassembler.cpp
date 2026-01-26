//=========================================================================
//                      Disassembler
//=========================================================================
// by      : INSANE
// created : 19/01/2026
//
// purpose : Disassemblers all different instruction encoding types. 
//
// Input  : Decoded instruction in Instruction_t* form.
// Output : Disassembled instrution comprised of many strings in DASMInst_t* form.
//-------------------------------------------------------------------------
#include "Disassembler.h"
#include <assert.h>
#include "../Util/Terminal/Terminal.h"
#include "../Math/SafeBitWiseOps.h"

#include "../../Include/Standard/OpCodeDesc_t.h"
#include "../../Include/Legacy/LegacyInst_t.h"
#include "../../Include/VEX/VEXInst_t.h"
#include "../../Include/Masks.h"



using namespace InsaneDASM64;


namespace InsaneDASM64
{
    static inline void HandleOperandMode_A  (DASMInst_t* pOutput, const Standard::Immediate_t* pImmediate);
    static inline void HandleOperandMode_BA (DASMInst_t* pOutput);
    static inline void HandleOperandMode_BB (DASMInst_t* pOutput);
    static inline void HandleOperandMode_BD (DASMInst_t* pOutput);
    static inline void HandleOperandMode_C  (DASMInst_t* pOutput, uint64_t iModRM_Reg);
    static inline void HandleOperandMode_D  (DASMInst_t* pOutput, uint64_t iModRM_Reg);
    static inline void HandleOperandMode_E  (DASMInst_t* pOutput);
    static inline void HandleOperandMode_ES (DASMInst_t* pOutput);
    static inline void HandleOperandMode_EST(DASMInst_t* pOutput, uint64_t iModRM_RM);
    static inline void HandleOperandMode_F  (DASMInst_t* pOutput);
    static inline void HandleOperandMode_G  (DASMInst_t* pOutput, uint64_t iModRM_Reg, Standard::CEOperandTypes_t iCEOperandType, int iOperandSize);
    static inline void HandleOperandMode_H  (DASMInst_t* pOutput, uint64_t iModRM_RM, Standard::CEOperandTypes_t iCEOperandType, int iOperandSize);
    static inline void HandleOperandMode_I  (DASMInst_t* pOutput, const Standard::Immediate_t* pImmediate);
    static inline void HandleOperandMode_J  (DASMInst_t* pOutput, const Standard::Immediate_t* pImmediate);
    static inline void HandleOperandMode_M  (DASMInst_t* pOutput);
    static inline void HandleOperandMode_N  (DASMInst_t* pOutput, uint64_t iModRM_RM);
    static inline void HandleOperandMode_O  (DASMInst_t* pOutput, const Standard::Immediate_t* pImmediate);
    static inline void HandleOperandMode_P  (DASMInst_t* pOutput, uint64_t iModRM_Reg);
    static inline void HandleOperandMode_Q  (DASMInst_t* pOutput);
    static inline void HandleOperandMode_R  (DASMInst_t* pOutput, uint64_t iModRM_RM, Standard::CEOperandTypes_t iCEOperandType, int iOperandSize);
    static inline void HandleOperandMode_S  (DASMInst_t* pOutput, uint64_t iModRM_Reg);
    static inline void HandleOperandMode_SC (DASMInst_t* pOutput);
    static inline void HandleOperandMode_T  (DASMInst_t* pOutput, uint64_t iModRM_Reg);
    static inline void HandleOperandMode_U  (DASMInst_t* pOutput, uint64_t iModRM_RM, const VEX::VEXPrefix_t* pVEXPrefix);
    static inline void HandleOperandMode_V  (DASMInst_t* pOutput, uint64_t iModRM_Reg, const VEX::VEXPrefix_t* pVEXPrefix);
    static inline void HandleOperandMode_W  (DASMInst_t* pOutput);
    static inline void HandleOperandMode_X  (DASMInst_t* pOutput);
    static inline void HandleOperandMode_Y  (DASMInst_t* pOutput);
    static inline void HandleOperandMode_Z  (DASMInst_t* pOutput, Byte iOpCodeByte, uint64_t iREX_B, Standard::CEOperandTypes_t iCEOpeandType, int iOperandSize);
    static inline void HandleOperandMode_VG (DASMInst_t* pOutput, Standard::CEOperandTypes_t iCEOperandType, int iOperandSize, const VEX::VEXPrefix_t* pVEXPrefix);
    static inline void HandleOperandMode_VXY(DASMInst_t* pOutput, const VEX::VEXPrefix_t* pVEXPrefix);
    static inline void HandleOperandMode_IXY(DASMInst_t* pOutput, uint64_t iImmRegisterIndex, const Standard::Immediate_t* pImmediate, const VEX::VEXPrefix_t* pVEXPrefix);


    static int CEOperandTypeToSizeInBytes(Standard::CEOperandTypes_t iCEOperandType, int iOperandSizeInBytes);
    static int CEOperandTypeToSizeInBits (Standard::CEOperandTypes_t iCEOperandType, int iOperandSizeInBytes);
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t InsaneDASM64::Disassemble(const Instruction_t* pInst, DASMInst_t* pOutput)
{
    // All important components of an instruction that we need to disassemble the instrution.
    const Standard::OpCode_t*       pOpCode       = nullptr;
    const Standard::ModRM_t*        pModRM        = nullptr; bool bHasModRM = false;
    const Standard::SIB_t*          pSIB          = nullptr; bool bHasSIB   = false;
    const VEX::VEXPrefix_t*         pVEXPrefix    = nullptr;
    const Standard::Immediate_t*    pImmediate    = nullptr;
    const Standard::Displacement_t* pDisplacement = nullptr;

    // ModRM & SIB components, with bit extension.
    uint64_t iModRM_Mod = 0llu, iModRM_Reg = 0llu, iModRM_RM = 0llu;
    uint64_t iSIB_Scale = 0llu, iSIB_Index = 0llu, iSIB_Base = 0llu;

    int iOperandSize = 0, iAddressSize = 0;

    // Only for Operand types IXY
    uint64_t iImmRegisterIndex = 0llu;

    // Used for Operand Addressing Method Z.
    uint64_t iREX_B = 0llu; 


    // Extracting all required components from instruction according to the instruction encoding type. 
    switch(pInst->m_iInstEncodingType)
    {
        case Instruction_t::InstEncodingType_Legacy:
            {
                const Legacy::LegacyInst_t* pLegacyInst = reinterpret_cast<const Legacy::LegacyInst_t*>(pInst->m_pInst);
                pOpCode       = &pLegacyInst->m_opCode; // NOTE : pLegacyInst->m_opCode is of type LegacyOpCode_t which inherits from opCode_t. They are pretty much the same.
                pModRM        = &pLegacyInst->m_modrm; bHasModRM = pLegacyInst->m_bHasModRM;
                pSIB          = &pLegacyInst->m_SIB;   bHasSIB   = pLegacyInst->m_bHasSIB;
                pVEXPrefix    = nullptr;
                pDisplacement = &pLegacyInst->m_displacement;
                pImmediate    = &pLegacyInst->m_immediate;

                if(bHasModRM == true)
                {
                    iModRM_Mod = pLegacyInst->ModRM_Mod(); 
                    iModRM_Reg = pLegacyInst->ModRM_Reg(); 
                    iModRM_RM  = pLegacyInst->ModRM_RM();
                }
                iSIB_Scale = pLegacyInst->SIB_Scale(); iSIB_Index = pLegacyInst->SIB_Index(); iSIB_Base = pLegacyInst->SIB_Base();

                // Operand size & Address size for this instruction in bytes.
                iOperandSize = pLegacyInst->GetOperandSizeInBytes(false);
                iAddressSize = pLegacyInst->GetAddressSizeInBytes();

                iImmRegisterIndex = 0llu;

                iREX_B = pLegacyInst->m_bHasREX == false ? 0llu : Maths::SafeAnd(pLegacyInst->m_iREX, Legacy::Masks::REX_B);
            }
            break;

        case Instruction_t::InstEncodingType_VEX:
            {
                const VEX::VEXInst_t* pVEXInst = reinterpret_cast<const VEX::VEXInst_t*>(pInst->m_pInst);
                pOpCode       = &pVEXInst->m_opcode; // NOTE : This is just a standard::opCode_t object.
                pModRM        = &pVEXInst->m_modrm; bHasModRM = true;
                pSIB          = &pVEXInst->m_SIB;   bHasSIB   = pVEXInst->m_bHasSIB;
                pVEXPrefix    = &pVEXInst->m_vexPrefix;
                pDisplacement = &pVEXInst->m_disp;
                pImmediate    = &pVEXInst->m_immediate;

                iModRM_Mod = pVEXInst->ModRM_Mod(); iModRM_Reg = pVEXInst->ModRM_Reg(); iModRM_RM = pVEXInst->ModRM_RM();
                iSIB_Scale = pVEXInst->SIB_Scale(); iSIB_Index = pVEXInst->SIB_Index(); iSIB_Base = pVEXInst->SIB_Base();

                iOperandSize = pVEXInst->GetOperandSizeInBytes();
                iAddressSize = 8llu;

                iImmRegisterIndex = pVEXInst->m_immediate.ByteCount() == 1 ? pVEXInst->GetImmRegister() : 0llu;

                iREX_B = pVEXInst->m_vexPrefix.B();
            }
            break;

        case Instruction_t::InstEncodingType_EVEX:
        case Instruction_t::InstEncodingType_XOP:
        default:
        FAIL_LOG("Invalid Instruction Encoding Type [ %d ]", pInst->m_iInstEncodingType);
        assert(false && "Invalid / unsupported instrution encoding type.");
        break;
    }


    // Did we successfully acquire all required objects??
    bool bInstComponentAcquired = pOpCode != nullptr && pModRM != nullptr && pSIB != nullptr &&
        pDisplacement != nullptr && pImmediate != nullptr;

    if(bInstComponentAcquired == false)
    {
        FAIL_LOG("Failed to acquire necessary instruction compnents.");
        assert(bInstComponentAcquired == true && "Failed to acquire instruction components.");
        return IDASMErrorCode_t::IDASMErrorCode_InstComponentsNotFound;
    }


    const Standard::OpCodeDesc_t* pOpCodeDesc = pOpCode->m_pOpCodeDesc;


    // OpCode initialized completely.
    if(pOpCodeDesc == nullptr)
    {
        FAIL_LOG("OpCode's final varient not initialized.");
        assert(pOpCode->m_pOpCodeDesc != nullptr && "OpCode's final varient not initialized.");
        return IDASMErrorCode_t::IDASMErrorCode_OpCodeNotInitialized;
    }


    // Copy the name upfront.
    strcpy(pOutput->m_szMnemonic, pOpCode->m_pOpCodeDesc->m_szName);


    // We shall journey through the operands all, refining them such that common man may readeth and know their truth.
    for(uint64_t iOperandIndex = 0llu; iOperandIndex < pOpCodeDesc->m_nOperands; iOperandIndex++)
    {
        const Standard::Operand_t* pOperand       = &pOpCodeDesc->m_operands[iOperandIndex];
        Standard::OperandModes_t   iOperandMode   = pOperand->m_iOperandMode;
        Standard::OperandTypes_t   iOperandType   = pOperand->m_iOperandType;
        Standard::CEOperandModes_t iCEOperandMode = GeekToCoderOperandMode(iOperandMode);
        Standard::CEOperandTypes_t iCEOperandType = GeekToCoderOperandType(iOperandType);


        switch (pOperand->m_iOperandCatagory) 
        {
            case Standard::Operand_t::OperandCatagory_Literal:
                switch (iOperandMode) 
                {
                    case Standard::OperandMode_A:   HandleOperandMode_A  (pOutput, pImmediate); break;
                    case Standard::OperandMode_BA:  HandleOperandMode_BA (pOutput); break;
                    case Standard::OperandMode_BB:  HandleOperandMode_BB (pOutput); break;
                    case Standard::OperandMode_BD:  HandleOperandMode_BD (pOutput); break;
                    case Standard::OperandMode_C:   HandleOperandMode_C  (pOutput, iModRM_Reg); break;
                    case Standard::OperandMode_D:   HandleOperandMode_D  (pOutput, iModRM_Reg); break;
                    case Standard::OperandMode_E:   HandleOperandMode_E  (pOutput); break;
                    case Standard::OperandMode_ES:  HandleOperandMode_ES (pOutput); break;
                    case Standard::OperandMode_EST: HandleOperandMode_EST(pOutput, iModRM_RM); break;
                    case Standard::OperandMode_F:   HandleOperandMode_F  (pOutput); break;
                    case Standard::OperandMode_G:   HandleOperandMode_G  (pOutput, iModRM_Reg, iCEOperandType, iOperandSize); break;
                    case Standard::OperandMode_H:   HandleOperandMode_H  (pOutput, iModRM_RM,  iCEOperandType, iOperandSize); break;
                    case Standard::OperandMode_I:   HandleOperandMode_I  (pOutput, pImmediate); break;
                    case Standard::OperandMode_J:   HandleOperandMode_J  (pOutput, pImmediate); break;
                    case Standard::OperandMode_M:   HandleOperandMode_M  (pOutput); break;
                    case Standard::OperandMode_N:   HandleOperandMode_N  (pOutput, iModRM_RM); break;
                    case Standard::OperandMode_O:   HandleOperandMode_O  (pOutput, pImmediate); break;
                    case Standard::OperandMode_P:   HandleOperandMode_P  (pOutput, iModRM_Reg); break;
                    case Standard::OperandMode_Q:   HandleOperandMode_Q  (pOutput); break;
                    case Standard::OperandMode_R:   HandleOperandMode_R  (pOutput, iModRM_RM, iCEOperandType, iOperandSize); break;
                    case Standard::OperandMode_S:   HandleOperandMode_S  (pOutput, iModRM_Reg); break;
                    case Standard::OperandMode_SC:  HandleOperandMode_SC (pOutput); break;
                    case Standard::OperandMode_T:   HandleOperandMode_T  (pOutput, iModRM_Reg); break;
                    case Standard::OperandMode_U:   HandleOperandMode_U  (pOutput, iModRM_RM, pVEXPrefix); break;
                    case Standard::OperandMode_V:   HandleOperandMode_V  (pOutput, iModRM_Reg, pVEXPrefix); break;
                    case Standard::OperandMode_W:   HandleOperandMode_W  (pOutput); break;
                    case Standard::OperandMode_X:   HandleOperandMode_X  (pOutput); break;
                    case Standard::OperandMode_Y:   HandleOperandMode_Y  (pOutput); break;
                    case Standard::OperandMode_Z:   HandleOperandMode_Z  (pOutput, pOpCodeDesc->m_iByte, iREX_B, iCEOperandType, iOperandSize); break;
                    case Standard::OperandMode_VG:  HandleOperandMode_VG (pOutput, iCEOperandType, iOperandSize, pVEXPrefix); break;
                    case Standard::OperandMode_VXY: HandleOperandMode_VXY(pOutput, pVEXPrefix); break;
                    case Standard::OperandMode_IXY: HandleOperandMode_IXY(pOutput, iImmRegisterIndex, pImmediate, pVEXPrefix); break;

                    default:
                        FAIL_LOG("Invalid Operand Addressing Mode!");
                        assert("Invalid Operand Addressing Mode!");
                        break;
                }
                break;

            case Standard::Operand_t::OperandCatagory_Register:
                pOutput->PushBackOperand(pOperand->m_iOperandRegister.ToString());
                break;

            case Standard::Operand_t::OperandCatagory_Legacy:
                pOutput->PushLiteralOperand(pOperand->m_iOperandLiteral);
                break;

            default:
                FAIL_LOG("Invalid Operand Catagory!");
                assert("Invalid Operand Catagory!");
                break;
        }

    } // for(uint64_t iOperandIndex = 0llu; iOperandIndex < pOpCodeDesc->m_nOperands; iOperandIndex++)


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_A(DASMInst_t* pOutput, const Standard::Immediate_t* pImmediate)
{
    pOutput->PushLiteralFromString(pImmediate->m_immediateByte, pImmediate->ByteCount(), true);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_BA(DASMInst_t* pOutput)
{
    // Brief : Memory addressed by DS:EAX, or by rAX in 64-bit mode (only 0F01C8 MONITOR).
    return;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_BB(DASMInst_t* pOutput)
{
    // Brief : Memory addressed by DS:eBX+AL, or by rBX+AL in 64-bit mode (only XLAT 0xD7).
    return;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_BD(DASMInst_t* pOutput)
{
    // Brief : Memory addressed by DS:eDI or by RDI (only 0FF7 MASKMOVQ and 660FF7 MASKMOVDQU).
    return;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_C(DASMInst_t* pOutput, uint64_t iModRM_Reg)
{
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_Control, iModRM_Reg, 0).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_D(DASMInst_t* pOutput, uint64_t iModRM_Reg)
{
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_Control, iModRM_Reg, 0).ToString());
}

static inline void InsaneDASM64::HandleOperandMode_E(DASMInst_t* pOutput) { return; }
static inline void InsaneDASM64::HandleOperandMode_ES(DASMInst_t* pOutput) { return; }


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_EST(DASMInst_t* pOutput, uint64_t iModRM_RM)
{
    // Brief : (Implies original E). A ModR/M byte follows the opcode and specifies the x87 FPU stack register.

    // Register width is constant for all x87 FPU stack registers. so size doesn't matter here.
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_FPU, iModRM_RM, 64).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_F(DASMInst_t* pOutput)
{
    // Brief : rFLAGS register.
    return;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_G(DASMInst_t* pOutput, uint64_t iModRM_Reg, Standard::CEOperandTypes_t iCEOperandType, int iOperandSize)
{
    // Brief : The reg field of the ModR/M byte selects a general register.
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_GPR, iModRM_Reg, CEOperandTypeToSizeInBits(iCEOperandType, iOperandSize)).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_H(DASMInst_t* pOutput, uint64_t iModRM_RM, Standard::CEOperandTypes_t iCEOperandType, int iOperandSize)
{
    // Brief : The r/m field of the ModR/M byte always selects a general register, regardless of the mod field.
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_GPR, iModRM_RM, CEOperandTypeToSizeInBits(iCEOperandType, iOperandSize)).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_I(DASMInst_t* pOutput, const Standard::Immediate_t* pImmediate)
{
    // Brief : Immediate data. The operand value is encoded in subsequent bytes of the instruction.
    pOutput->PushLiteralFromString(pImmediate->m_immediateByte, pImmediate->ByteCount(), true);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_J(DASMInst_t* pOutput, const Standard::Immediate_t* pImmediate)
{
    // Brief : The instruction contains a relative offset to be added to the instruction pointer register.
    pOutput->PushLiteralFromString(pImmediate->m_immediateByte, pImmediate->ByteCount(), true);
}


static inline void InsaneDASM64::HandleOperandMode_M(DASMInst_t* pOutput) { return; }


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_N(DASMInst_t* pOutput, uint64_t iModRM_RM)
{
    // Brief : The R/M field of the ModR/M byte selects a packed quadword MMX technology register.
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_MMX, iModRM_RM, 64).ToString()); // MMX reigsters have fixed width.
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_O(DASMInst_t* pOutput, const Standard::Immediate_t* pImmediate)
{
    pOutput->PushLiteralFromString(pImmediate->m_immediateByte, pImmediate->ByteCount(), true);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_P(DASMInst_t* pOutput, uint64_t iModRM_Reg)
{
    // Brief : The reg field of the ModR/M byte selects a packed quadword MMX technology register.
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_MMX, iModRM_Reg, 64).ToString()); // MMX reigsters have fixed width.
}


static inline void InsaneDASM64::HandleOperandMode_Q(DASMInst_t* pOutput) { return; }


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_R(DASMInst_t* pOutput, uint64_t iModRM_RM, Standard::CEOperandTypes_t iCEOperandType, int iOperandSize)
{
    // Brief : The mod field of the ModR/M byte may refer only to a general register
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_GPR, iModRM_RM, CEOperandTypeToSizeInBits(iCEOperandType, iOperandSize)).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_S(DASMInst_t* pOutput, uint64_t iModRM_Reg)
{
    // Brief : The reg field of the ModR/M byte selects a segment register.
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_Segment, iModRM_Reg, 64).ToString()); // Segment registers have fixed width.
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_SC(DASMInst_t* pOutput)
{
    // Brief : Stack operand, used by instructions which either push an operand to the stack or pop an operand from the stack.
    return;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_T(DASMInst_t* pOutput, uint64_t iModRM_Reg)
{
    // Brief : The reg field of the ModR/M byte selects a test register
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_Test, iModRM_Reg, 64).ToString()); // Test registers have fixed width.
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_U(DASMInst_t* pOutput, uint64_t iModRM_RM, const VEX::VEXPrefix_t* pVEXPrefix)
{
    // Brief : The R/M field of the ModR/M byte selects a 128-bit XMM register.
    
    // Some encodings like Legacy encoding won't have VEXPrefix, so they will pass nullptr.
    // But they also should never have a operand mode that requires a VEX prefix.
    assert(pVEXPrefix != nullptr && "NULL VEXPrefix received for handling operand mode U");
    if(pVEXPrefix == nullptr)
    {
        FAIL_LOG("NULL VEXPrefix received for handling operand mode U");
        return;
    }

    // Register width.
    int16_t iRegisterWidth = pVEXPrefix->L() == false ? 128 : 256;

    // Register class.
    Standard::Register_t::RegisterClass_t iRegisterClass = pVEXPrefix->L() == false ? 
        Standard::Register_t::RegisterClass_SSE : Standard::Register_t::RegisterClass_AVX;

    pOutput->PushBackOperand(Standard::Register_t(iRegisterClass, iModRM_RM, iRegisterWidth).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_V(DASMInst_t* pOutput, uint64_t iModRM_Reg, const VEX::VEXPrefix_t* pVEXPrefix)
{
    // Brief : The reg field of the ModR/M byte selects a 128-bit XMM register.
    
    // Some encodings like Legacy encoding won't have VEXPrefix, so they will pass nullptr.
    // But they also should never have a operand mode that requires a VEX prefix.
    assert(pVEXPrefix != nullptr && "NULL VEXPrefix received for handling operand mode U");
    if(pVEXPrefix == nullptr)
    {
        FAIL_LOG("NULL VEXPrefix received for handling operand mode U");
        return;
    }

    // Register width.
    int16_t iRegisterWidth = pVEXPrefix->L() == false ? 128 : 256;

    // Register class.
    Standard::Register_t::RegisterClass_t iRegisterClass = pVEXPrefix->L() == false ? 
        Standard::Register_t::RegisterClass_SSE : Standard::Register_t::RegisterClass_AVX;

    pOutput->PushBackOperand(Standard::Register_t(iRegisterClass, iModRM_Reg, iRegisterWidth).ToString());
}


static inline void InsaneDASM64::HandleOperandMode_W(DASMInst_t* pOutput) { return; }
static inline void InsaneDASM64::HandleOperandMode_X(DASMInst_t* pOutput) { return; }
static inline void InsaneDASM64::HandleOperandMode_Y(DASMInst_t* pOutput) { return; }


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_Z(DASMInst_t* pOutput, Byte iOpCodeByte, uint64_t iREX_B, Standard::CEOperandTypes_t iCEOpeandType, int iOperandSize)
{
    // REX.B bit extends the "Lower 3 bits of the opcode". How bizzare is that?
    int iRegisterIndex = Maths::SafeOr(Maths::SafeAnd(iOpCodeByte, 0b111), iREX_B << 3llu);

    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_GPR, iRegisterIndex, CEOperandTypeToSizeInBytes(iCEOpeandType, iOperandSize)).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_VG(DASMInst_t* pOutput, Standard::CEOperandTypes_t iCEOperandType, int iOperandSize, const VEX::VEXPrefix_t* pVEXPrefix)
{
    uint64_t iVVVV = pVEXPrefix->vvvv();

    // register index is one's compliment of vex.vvvv,
    iVVVV = ~iVVVV & VEX::Masks::VVVV;

    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_GPR, iVVVV, CEOperandTypeToSizeInBits(iCEOperandType, iOperandSize)).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_VXY(DASMInst_t* pOutput, const VEX::VEXPrefix_t* pVEXPrefix)
{
    assert(pVEXPrefix != nullptr && "NULL VEXPrefix received for handling operand mode U");
    if(pVEXPrefix == nullptr)
    {
        FAIL_LOG("NULL VEXPrefix received for handling operand mode U");
        return;
    }

    // Register index, size & class
    uint64_t iVVVV         = pVEXPrefix->vvvv();
    int      iRegisterSize = pVEXPrefix->L() == 0llu ? 128 : 256;
    Standard::Register_t::RegisterClass_t iRegClass = pVEXPrefix->L() == 0llu ? 
        Standard::Register_t::RegisterClass_SSE : // XXM
        Standard::Register_t::RegisterClass_AVX;  // YMM

    //  register index is one's complimed of vex.vvvv,
    iVVVV = ~iVVVV & (VEX::Masks::VVVV >> 3llu);

    pOutput->PushBackOperand(Standard::Register_t(iRegClass, iVVVV, iRegisterSize).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_IXY(DASMInst_t* pOutput, uint64_t iImmRegisterIndex, const Standard::Immediate_t* pImmediate, const VEX::VEXPrefix_t* pVEXPrefix)
{
    // Oprand Addressing Method IXY must only be used by VEX encoding instructions
    // to extract register index from upper 4 bits of the immediate byte.
    assert(pImmediate->ByteCount() == 1 && "Invalid byte count for operand type IXY");
    if(pImmediate->ByteCount() != 1)
    {
        FAIL_LOG("Invalid byte count for operand type IXY");
        return;
    }

    // Register index, size & class
    int      iRegisterSize = pVEXPrefix->L() == 0llu ? 128 : 256;
    Standard::Register_t::RegisterClass_t iRegClass = pVEXPrefix->L() == 0llu ? 
        Standard::Register_t::RegisterClass_SSE : // XXM
        Standard::Register_t::RegisterClass_AVX;  // YMM


    pOutput->PushBackOperand(Standard::Register_t(iRegClass, iImmRegisterIndex, iRegisterSize).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int InsaneDASM64::CEOperandTypeToSizeInBytes(Standard::CEOperandTypes_t iCEOperandType, int iOperandSizeInBytes)
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
static int InsaneDASM64::CEOperandTypeToSizeInBits(Standard::CEOperandTypes_t iCEOperandType, int iOperandSizeInBytes)
{
    int iSize = CEOperandTypeToSizeInBytes(iCEOperandType, iOperandSizeInBytes);

    return iSize <= 0 ? -1 : iSize * 8;
}
