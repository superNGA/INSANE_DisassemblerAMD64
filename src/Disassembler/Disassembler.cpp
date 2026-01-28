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
#include <iomanip>
#include <sstream> // :(
#include "../Util/Terminal/Terminal.h"
#include "../Math/SafeBitWiseOps.h"

#include "../../Include/Standard/OpCodeDesc_t.h"
#include "../../Include/Legacy/LegacyInst_t.h"
#include "../../Include/VEX/VEXInst_t.h"
#include "../../Include/Masks.h"
#include "InstSummary/InstSummary_t.h"



using namespace InsaneDASM64;


namespace InsaneDASM64
{
    static inline void HandleOperandMode_A  (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_BA (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_BB (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_BD (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_C  (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_D  (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_E  (DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType);
    static inline void HandleOperandMode_ES (DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType);
    static inline void HandleOperandMode_EST(DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_F  (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_G  (DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType);
    static inline void HandleOperandMode_H  (DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType);
    static inline void HandleOperandMode_I  (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_J  (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_M  (DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType);
    static inline void HandleOperandMode_N  (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_O  (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_P  (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_Q  (DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType);
    static inline void HandleOperandMode_R  (DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType);
    static inline void HandleOperandMode_S  (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_SC (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_T  (DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_U  (DASMInst_t* pOutput, InstSummary_t* pInst, Instruction_t::InstEncodingTypes_t iEncodingType);
    static inline void HandleOperandMode_V  (DASMInst_t* pOutput, InstSummary_t* pInst, Instruction_t::InstEncodingTypes_t iEncodingType);
    static inline void HandleOperandMode_W  (DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType);
    static inline void HandleOperandMode_X  (DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType);
    static inline void HandleOperandMode_Y  (DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType);
    static inline void HandleOperandMode_Z  (DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType);
    static inline void HandleOperandMode_VG (DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType);
    static inline void HandleOperandMode_VXY(DASMInst_t* pOutput, InstSummary_t* pInst);
    static inline void HandleOperandMode_IXY(DASMInst_t* pOutput, InstSummary_t* pInst);


    // Since Operand Addressing Methods E, ES, M, Q, W, X and Y use the ModRM byte to determine either
    // a register or memory address, they share same logic, and the only difference is the register class.
    // So while handling above mentioned Operand Addressing Methods, we use this fuction with different 
    // register classes.
    static void RegOrMemoryUsingModRM(DASMInst_t* pOutput, InstSummary_t* pInst, 
            Standard::CEOperandTypes_t iCEOperandType, Standard::Register_t::RegisterClass_t iRegisterClass);


    static int CEOperandTypeToOperandSizeInBytes(Standard::CEOperandTypes_t iCEOperandType, int iOperandSizeInBytes);
    static int CEOperandTypeToOperandSizeInBits (Standard::CEOperandTypes_t iCEOperandType, int iOperandSizeInBytes);
    static int CEOperandTypeToAdrsSizeInBytes   (Standard::CEOperandTypes_t iCEOperandType, int iAddressSizeInByte);
    static int CEOperandTypeToAdrsSizeInBits    (Standard::CEOperandTypes_t iCEOperandType, int iAddressSizeInByte);
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t InsaneDASM64::Disassemble(const Instruction_t* pInst, DASMInst_t* pOutput)
{
    // NOTE : Ran once for each instruction.
    InstSummary_t inst;

    // Extracting all required components from instruction according to the instruction encoding type. 
    switch(pInst->m_iInstEncodingType)
    {
        case Instruction_t::InstEncodingType_Legacy:
            inst.Initialize(reinterpret_cast<const Legacy::LegacyInst_t*>(pInst->m_pInst));
            break;

        case Instruction_t::InstEncodingType_VEX:
            inst.Initialize(reinterpret_cast<const VEX::VEXInst_t*>(pInst->m_pInst));
            break;

        case Instruction_t::InstEncodingType_EVEX:
        case Instruction_t::InstEncodingType_XOP:
        default:
        FAIL_LOG("Invalid Instruction Encoding Type [ %d ]", pInst->m_iInstEncodingType);
        assert(false && "Invalid / unsupported instrution encoding type.");
        break;
    }


    // Did we successfully acquire all required objects??
    bool bInstComponentAcquired = inst.IsValid();

    if(bInstComponentAcquired == false)
    {
        FAIL_LOG("Failed to acquire necessary instruction components.");
        assert(bInstComponentAcquired == true && "Failed to acquire instruction components.");
        return IDASMErrorCode_t::IDASMErrorCode_InstComponentsNotFound;
    }


    // Description to the final opcode varient.
    const Standard::OpCodeDesc_t* pOpCodeDesc = inst.m_pOpCode->m_pOpCodeDesc;


    // OpCode initialized completely.
    if(pOpCodeDesc == nullptr)
    {
        FAIL_LOG("OpCode's final varient not initialized.");
        assert(pOpCodeDesc != nullptr && "OpCode's final varient not initialized.");
        return IDASMErrorCode_t::IDASMErrorCode_OpCodeNotInitialized;
    }


    // Copy the name upfront.
    strcpy(pOutput->m_szMnemonic, pOpCodeDesc->m_szName);


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
            case Standard::Operand_t::OperandCatagory_Legacy:
                switch (iOperandMode) 
                {
                    case Standard::OperandMode_A:   HandleOperandMode_A  (pOutput, &inst); break;
                    case Standard::OperandMode_BA:  HandleOperandMode_BA (pOutput, &inst); break;
                    case Standard::OperandMode_BB:  HandleOperandMode_BB (pOutput, &inst); break;
                    case Standard::OperandMode_BD:  HandleOperandMode_BD (pOutput, &inst); break;
                    case Standard::OperandMode_C:   HandleOperandMode_C  (pOutput, &inst); break;
                    case Standard::OperandMode_D:   HandleOperandMode_D  (pOutput, &inst); break;
                    case Standard::OperandMode_E:   HandleOperandMode_E  (pOutput, &inst, iCEOperandType); break;
                    case Standard::OperandMode_ES:  HandleOperandMode_ES (pOutput, &inst, iCEOperandType); break;
                    case Standard::OperandMode_EST: HandleOperandMode_EST(pOutput, &inst); break;
                    case Standard::OperandMode_F:   HandleOperandMode_F  (pOutput, &inst); break;
                    case Standard::OperandMode_G:   HandleOperandMode_G  (pOutput, &inst, iCEOperandType); break;
                    case Standard::OperandMode_H:   HandleOperandMode_H  (pOutput, &inst, iCEOperandType); break;
                    case Standard::OperandMode_I:   HandleOperandMode_I  (pOutput, &inst); break;
                    case Standard::OperandMode_J:   HandleOperandMode_J  (pOutput, &inst); break;
                    case Standard::OperandMode_M:   HandleOperandMode_M  (pOutput, &inst, iCEOperandType); break;
                    case Standard::OperandMode_N:   HandleOperandMode_N  (pOutput, &inst); break;
                    case Standard::OperandMode_O:   HandleOperandMode_O  (pOutput, &inst); break;
                    case Standard::OperandMode_P:   HandleOperandMode_P  (pOutput, &inst); break;
                    case Standard::OperandMode_Q:   HandleOperandMode_Q  (pOutput, &inst, iCEOperandType); break;
                    case Standard::OperandMode_R:   HandleOperandMode_R  (pOutput, &inst, iCEOperandType); break;
                    case Standard::OperandMode_S:   HandleOperandMode_S  (pOutput, &inst); break;
                    case Standard::OperandMode_SC:  HandleOperandMode_SC (pOutput, &inst); break;
                    case Standard::OperandMode_T:   HandleOperandMode_T  (pOutput, &inst); break;
                    case Standard::OperandMode_U:   HandleOperandMode_U  (pOutput, &inst, pInst->m_iInstEncodingType); break;
                    case Standard::OperandMode_V:   HandleOperandMode_V  (pOutput, &inst, pInst->m_iInstEncodingType); break;
                    case Standard::OperandMode_W:   HandleOperandMode_W  (pOutput, &inst, iCEOperandType); break;
                    case Standard::OperandMode_X:   HandleOperandMode_X  (pOutput, &inst, iCEOperandType); break;
                    case Standard::OperandMode_Y:   HandleOperandMode_Y  (pOutput, &inst, iCEOperandType); break;
                    case Standard::OperandMode_Z:   HandleOperandMode_Z  (pOutput, &inst, iCEOperandType); break;
                    case Standard::OperandMode_VG:  HandleOperandMode_VG (pOutput, &inst, iCEOperandType); break;
                    case Standard::OperandMode_VXY: HandleOperandMode_VXY(pOutput, &inst); break;
                    case Standard::OperandMode_IXY: HandleOperandMode_IXY(pOutput, &inst); break;

                    default:
                        FAIL_LOG("Invalid Operand Addressing Mode!");
                        assert("Invalid Operand Addressing Mode!");
                        break;
                }
                break;

            case Standard::Operand_t::OperandCatagory_Register:
                pOutput->PushBackOperand(pOperand->m_iOperandRegister.ToString());
                break;

            case Standard::Operand_t::OperandCatagory_Literal:
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
static inline void InsaneDASM64::HandleOperandMode_A(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    pOutput->PushLiteralFromString(pInst->m_pImmediate->m_immediateByte, pInst->m_pImmediate->ByteCount(), true);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_BA(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    // Brief : Memory addressed by DS:EAX, or by rAX in 64-bit mode (only 0F01C8 MONITOR).
    return;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_BB(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    // Brief : Memory addressed by DS:eBX+AL, or by rBX+AL in 64-bit mode (only XLAT 0xD7).
    return;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_BD(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    // Brief : Memory addressed by DS:eDI or by RDI (only 0FF7 MASKMOVQ and 660FF7 MASKMOVDQU).
    return;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_C(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    assert(pInst->m_bHasModRM == true && "Instruction doesn't have a ModRM byte but has operand addressing mode C");

    // Brief : The reg field of the ModR/M byte selects a control register.
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_Control, pInst->m_iModRM_Reg, 0).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_D(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    // Brief : The reg field of the ModR/M byte selects a debug register.
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_Debug, pInst->m_iModRM_Reg, 0).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_E(DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType)
{
    // Brief : The operand is either a general-purpose register or a memory address.
    RegOrMemoryUsingModRM(pOutput, pInst, iCEOperandType, Standard::Register_t::RegisterClass_GPR);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_ES(DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType)
{
    // Brief : The operand is either a x87 FPU stack register or a memory address.
    RegOrMemoryUsingModRM(pOutput, pInst, iCEOperandType, Standard::Register_t::RegisterClass_FPU);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_EST(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    // Brief : (Implies original E). A ModR/M byte follows the opcode and specifies the x87 FPU stack register.

    // Register width is constant for all x87 FPU stack registers. so size doesn't matter here.
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_FPU, pInst->m_iModRM_RM, 64).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_F(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    // Brief : rFLAGS register.
    return;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_G(DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType)
{
    // Brief : The reg field of the ModR/M byte selects a general register.
    pOutput->PushBackOperand(Standard::Register_t(
                Standard::Register_t::RegisterClass_GPR, 
                pInst->m_iModRM_Reg, 
                CEOperandTypeToOperandSizeInBits(iCEOperandType, pInst->m_iOperandSizeInByte)).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_H(DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType)
{
    // Brief : The r/m field of the ModR/M byte always selects a general register, regardless of the mod field.
    pOutput->PushBackOperand(Standard::Register_t(
                Standard::Register_t::RegisterClass_GPR, 
                pInst->m_iModRM_RM,
                CEOperandTypeToOperandSizeInBits(iCEOperandType, pInst->m_iOperandSizeInByte)).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_I(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    // Brief : Immediate data. The operand value is encoded in subsequent bytes of the instruction.
    pOutput->PushLiteralFromString(pInst->m_pImmediate->m_immediateByte, pInst->m_pImmediate->ByteCount(), true);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_J(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    // Brief : The instruction contains a relative offset to be added to the instruction pointer register.
    pOutput->PushLiteralFromString(pInst->m_pImmediate->m_immediateByte, pInst->m_pImmediate->ByteCount(), true);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_M(DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType)
{
    // Brief : The ModR/M byte may refer only to memory: mod != 11bin.
    RegOrMemoryUsingModRM(pOutput, pInst, iCEOperandType, Standard::Register_t::RegisterClass_GPR);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_N(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    // Brief : The R/M field of the ModR/M byte selects a packed quadword MMX technology register.
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_MMX, pInst->m_iModRM_RM, 64).ToString()); // MMX reigsters have fixed width.
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_O(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    assert(pInst->m_bHasModRM == false && "Instruction with Operand Addressing Method must not have ModRM byte.");

    // Brief : The instruction has no ModR/M byte; the offset of the operand is coded as a word, double word or quad word.
    pOutput->PushLiteralFromString(pInst->m_pImmediate->m_immediateByte, pInst->m_pImmediate->ByteCount(), true);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_P(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    // Brief : The reg field of the ModR/M byte selects a packed quadword MMX technology register.
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_MMX, pInst->m_iModRM_Reg, 64).ToString()); // MMX reigsters have fixed width.
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_Q(DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType)
{
    // Brief : The operand is either an MMX technology register or a memory address.
    RegOrMemoryUsingModRM(pOutput, pInst, iCEOperandType, Standard::Register_t::RegisterClass_MMX);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_R(DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType)
{
    // Brief : The mod field of the ModR/M byte may refer only to a general register
    pOutput->PushBackOperand(Standard::Register_t(
                Standard::Register_t::RegisterClass_GPR, 
                pInst->m_iModRM_RM, 
                CEOperandTypeToOperandSizeInBits(iCEOperandType, pInst->m_iOperandSizeInByte)).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_S(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    // Brief : The reg field of the ModR/M byte selects a segment register.
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_Segment, pInst->m_iModRM_Reg, 64).ToString()); // Segment registers have fixed width.
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_SC(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    // Brief : Stack operand, used by instructions which either push an operand to the stack or pop an operand from the stack.
    return;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_T(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    // Brief : The reg field of the ModR/M byte selects a test register
    pOutput->PushBackOperand(Standard::Register_t(Standard::Register_t::RegisterClass_Test, pInst->m_iModRM_Reg, 64).ToString()); // Test registers have fixed width.
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_U(DASMInst_t* pOutput, InstSummary_t* pInst, Instruction_t::InstEncodingTypes_t iEncodingType)
{
    // Brief : The R/M field of the ModR/M byte selects a 128-bit XMM register.

    // Register width.
    int16_t iRegisterWidth = 128;

    if(iEncodingType == Instruction_t::InstEncodingType_VEX)
        iRegisterWidth = pInst->m_pVEXPrefix->L() == false ? 128 : 256;

    // Register class.
    Standard::Register_t::RegisterClass_t iRegisterClass = Standard::Register_t::RegisterClass_SSE;

    if(iEncodingType == Instruction_t::InstEncodingType_VEX)
        iRegisterClass = pInst->m_pVEXPrefix->L() == false ? Standard::Register_t::RegisterClass_SSE : Standard::Register_t::RegisterClass_AVX;

    pOutput->PushBackOperand(Standard::Register_t(iRegisterClass, pInst->m_iModRM_RM, iRegisterWidth).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_V(DASMInst_t* pOutput, InstSummary_t* pInst, Instruction_t::InstEncodingTypes_t iEncodingType)
{
    // Brief : The reg field of the ModR/M byte selects a 128-bit XMM register.
    
    // Register width.
    int16_t iRegisterWidth = 128;

    if(iEncodingType == Instruction_t::InstEncodingType_VEX)
        iRegisterWidth = pInst->m_pVEXPrefix->L() == false ? 128 : 256;

    // Register class.
    Standard::Register_t::RegisterClass_t iRegisterClass = Standard::Register_t::RegisterClass_SSE;

    if(iEncodingType == Instruction_t::InstEncodingType_VEX)
        iRegisterClass = pInst->m_pVEXPrefix->L() == false ? Standard::Register_t::RegisterClass_SSE : Standard::Register_t::RegisterClass_AVX;

    pOutput->PushBackOperand(Standard::Register_t(iRegisterClass, pInst->m_iModRM_Reg, iRegisterWidth).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_W(DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType)
{
    // Brief : The operand is either a 128-bit XMM register or a memory address.
    RegOrMemoryUsingModRM(pOutput, pInst, iCEOperandType, Standard::Register_t::RegisterClass_SSE);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_X(DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType)
{
    // Brief : Memory addressed by the DS:eSI or by RSI.
    assert((pInst->m_iAddressSizeInByte == 4 || pInst->m_iAddressSizeInByte == 8) && "Invalid address size");


    if(pInst->m_iAddressSizeInByte == 4)
    {
        pOutput->PushBackOperand("DS:eSI");
    }
    else if(pInst->m_iAddressSizeInByte == 8)
    {
        pOutput->PushBackOperand("DS:rSI");
    }
    else
    {
        FAIL_LOG("Invlaid address size.");
        return;
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_Y(DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType)
{
    // Brief : Memory addressed by the ES:eDI or by RDI.
    assert((pInst->m_iAddressSizeInByte == 4 || pInst->m_iAddressSizeInByte == 8) && "Invalid address size");


    if(pInst->m_iAddressSizeInByte == 4)
    {
        pOutput->PushBackOperand("ES:eSI");
    }
    else if(pInst->m_iAddressSizeInByte == 8)
    {
        pOutput->PushBackOperand("ES:rSI");
    }
    else
    {
        FAIL_LOG("Invlaid address size.");
        return;
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_Z(DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType)
{
    // REX.B bit extends the "Lower 3 bits of the opcode". How bizzare is that?
    uint64_t iRegisterIndex = Maths::SafeOr(Maths::SafeAnd(pInst->m_pOpCode->m_pOpCodeDesc->m_iByte, 0b111), pInst->m_iREX_B << 3llu);
    

    pOutput->PushBackOperand(Standard::Register_t(
                Standard::Register_t::RegisterClass_GPR, 
                iRegisterIndex, 
                CEOperandTypeToOperandSizeInBits(iCEOperandType, pInst->m_iOperandSizeInByte)).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_VG(DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType)
{
    uint64_t iVVVV = pInst->m_pVEXPrefix->vvvv();

    // register index is one's compliment of vex.vvvv,
    iVVVV = ~iVVVV & VEX::Masks::VVVV;

    pOutput->PushBackOperand(Standard::Register_t(
                Standard::Register_t::RegisterClass_GPR, iVVVV, CEOperandTypeToOperandSizeInBits(iCEOperandType, pInst->m_iOperandSizeInByte)).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_VXY(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    assert(pInst->m_pVEXPrefix != nullptr && "NULL VEXPrefix received for handling operand mode U");
    if(pInst->m_pVEXPrefix == nullptr)
    {
        FAIL_LOG("NULL VEXPrefix received for handling operand mode U");
        return;
    }

    // Register index, size & class
    uint64_t iVVVV         = pInst->m_pVEXPrefix->vvvv();
    int      iRegisterSize = pInst->m_pVEXPrefix->L() == 0llu ? 128 : 256;
    Standard::Register_t::RegisterClass_t iRegClass = pInst->m_pVEXPrefix->L() == 0llu ? 
        Standard::Register_t::RegisterClass_SSE : // XXM
        Standard::Register_t::RegisterClass_AVX;  // YMM

    //  register index is one's complimed of vex.vvvv,
    iVVVV = ~iVVVV & (VEX::Masks::VVVV >> 3llu);

    pOutput->PushBackOperand(Standard::Register_t(iRegClass, iVVVV, iRegisterSize).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void InsaneDASM64::HandleOperandMode_IXY(DASMInst_t* pOutput, InstSummary_t* pInst)
{
    // Oprand Addressing Method IXY must only be used by VEX encoding instructions
    // to extract register index from upper 4 bits of the immediate byte.
    assert(pInst->m_pImmediate->ByteCount() == 1 && "Invalid byte count for operand type IXY");
    if(pInst->m_pImmediate->ByteCount() != 1)
    {
        FAIL_LOG("Invalid byte count for operand type IXY");
        return;
    }

    // Register index, size & class
    int      iRegisterSize = pInst->m_pVEXPrefix->L() == 0llu ? 128 : 256;
    Standard::Register_t::RegisterClass_t iRegClass = pInst->m_pVEXPrefix->L() == 0llu ? 
        Standard::Register_t::RegisterClass_SSE : // XXM
        Standard::Register_t::RegisterClass_AVX;  // YMM


    pOutput->PushBackOperand(Standard::Register_t(iRegClass, pInst->m_iImmRegisterIndex, iRegisterSize).ToString());
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void InsaneDASM64::RegOrMemoryUsingModRM(DASMInst_t* pOutput, InstSummary_t* pInst, Standard::CEOperandTypes_t iCEOperandType, Standard::Register_t::RegisterClass_t iRegisterClass)
{
    // Must have a ModRM byte.
    assert(pInst->m_bHasModRM == true && "Instruction doesn't have a ModRM byte.");
    if(pInst->m_bHasModRM == false)
    {
        FAIL_LOG("Instruction doesn't have a ModRM byte.");
        return;
    }

    
    // Register.
    if(pInst->m_iModRM_Mod == 0b11)
    {
        pOutput->PushBackOperand(Standard::Register_t(
                    iRegisterClass, 
                    pInst->m_iModRM_RM,
                    CEOperandTypeToOperandSizeInBits(iCEOperandType, pInst->m_iOperandSizeInByte)).ToString());
    }
    else if(pInst->m_bHasSIB == false)
    {
        std::stringstream ssTemp;

        // for modrm.rm == 101 && modrm.mod == 00, we only have to store displacement, and no register.
        bool bDispOnly          = pInst->m_iModRM_RM == 0b101 && pInst->m_iModRM_Mod == 0;
        int  iAddressSizeInBits = CEOperandTypeToOperandSizeInBits(iCEOperandType, pInst->m_iOperandSizeInByte);
        if(iAddressSizeInBits < 0)
            iAddressSizeInBits = pInst->m_iOperandSizeInByte * 8;

        // Adding address size out side bracket.
        switch (iAddressSizeInBits) 
        {
            case 8:  ssTemp << "byte ptr";  break;
            case 16: ssTemp << "word ptr";  break;
            case 32: ssTemp << "dword ptr"; break;
            case 64: ssTemp << "qword ptr"; break;

            default: assert(false && "Invalid address size"); break;
        }

        ssTemp << "[";
        if(bDispOnly == false)
        { 
            // Since we will be writing operand's width seperately, we will hard set base reg size to 64 bits.
            ssTemp << Standard::Register_t(Standard::Register_t::RegisterClass_GPR, pInst->m_iModRM_RM, 
                    64/*iAddressSizeInBits*/).ToString();
        }

        // Printing displacement, if any.
        if(pInst->m_pDisplacement->ByteCount() > 0)
        {
            // if we have stored a register in front of it. store + sign
            if(bDispOnly == false)
                ssTemp << " + ";
            else // RIP relative addess.
                ssTemp << "rIP + ";

            ssTemp << "0x";

            bool bLeadingZeroEnded = false;
            for (int i = pInst->m_pDisplacement->ByteCount() - 1; i >= 0; i--)
            {
                int iByte = static_cast<int>(pInst->m_pDisplacement->m_iDispBytes[i]);
                if(iByte != 0)
                    bLeadingZeroEnded = true;

                if(bLeadingZeroEnded == false)
                    continue;

                // Save old state
                auto oldFlags = ssTemp.flags();
                auto oldFill  = ssTemp.fill();

                ssTemp << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << iByte;

                // Restore state
                ssTemp.flags(oldFlags);
                ssTemp.fill(oldFill);
            }
        }

        ssTemp << "]";

        pOutput->PushBackOperand(ssTemp.str().c_str());
    }
    else 
    {
        std::stringstream ssTemp; 
        
        // Adding address size out side bracket.
        int iAddressSizeInBits = CEOperandTypeToOperandSizeInBits(iCEOperandType, pInst->m_iOperandSizeInByte);
        if(iAddressSizeInBits < 0)
            iAddressSizeInBits = pInst->m_iOperandSizeInByte * 8;

        switch (iAddressSizeInBits) 
        {
            case 8:  ssTemp << "byte ptr";  break;
            case 16: ssTemp << "word ptr";  break;
            case 32: ssTemp << "dword ptr"; break;
            case 64: ssTemp << "qword ptr"; break;

            default: assert(false && "Invalid address size"); break;
        }

        ssTemp << "[";

        // Base register...
        bool bNoBaseReg = pInst->m_iModRM_Mod == 0 && pInst->m_iSIB_Base == 0b101;
        if (bNoBaseReg == false)
        {
            Standard::Register_t iBaseReg(
                    Standard::Register_t::RegisterClass_GPR, 
                    pInst->m_iSIB_Base, 
                    64); // We represent address size using seperate strings ( "size" ptr format ), so we set base reg to 64 bits wide.

            ssTemp << iBaseReg.ToString();
        }


        // Index register.
        bool bIndexRegPresent = pInst->m_iSIB_Index != 0b100;
        if (bIndexRegPresent == true)
        {
            Standard::Register_t indexReg(Standard::Register_t::RegisterClass_GPR, pInst->m_iSIB_Index, 
                    CEOperandTypeToAdrsSizeInBits(iCEOperandType, pInst->m_iAddressSizeInByte));

            if(bNoBaseReg == false)
                ssTemp << " + ";

            ssTemp << indexReg.ToString();

            // Scale...
            uint64_t iScale = 1llu << pInst->m_iSIB_Scale;
            if (iScale > 1)
                ssTemp << "*" << iScale;
        }

        // Displacement ( if any )
        if(pInst->m_pDisplacement->ByteCount() > 0)
        {
            if(bIndexRegPresent == true || bNoBaseReg == false)
                ssTemp << " + ";
            else // RIP relative adrs.
                ssTemp << "rIP + ";

            ssTemp << "0x";

            bool bLeadingZeroEnded = false;
            for (int i = pInst->m_pDisplacement->ByteCount() - 1; i >= 0; i--)
            {
                int iByte = static_cast<int>(pInst->m_pDisplacement->m_iDispBytes[i]);
                if(iByte != 0)
                    bLeadingZeroEnded = true;

                if(bLeadingZeroEnded == false)
                    continue;

                // Save old state
                auto oldFlags = ssTemp.flags();
                auto oldFill  = ssTemp.fill();

                ssTemp << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << iByte;

                // Restore state
                ssTemp.flags(oldFlags);
                ssTemp.fill(oldFill);
            }
        }

        ssTemp << "]";
        pOutput->PushBackOperand(ssTemp.str().c_str());
    }
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int InsaneDASM64::CEOperandTypeToOperandSizeInBytes(Standard::CEOperandTypes_t iCEOperandType, int iOperandSizeInBytes)
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
static int InsaneDASM64::CEOperandTypeToOperandSizeInBits(Standard::CEOperandTypes_t iCEOperandType, int iOperandSizeInBytes)
{
    int iSize = CEOperandTypeToOperandSizeInBytes(iCEOperandType, iOperandSizeInBytes);

    return iSize <= 0 ? -1 : iSize * 8;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int InsaneDASM64::CEOperandTypeToAdrsSizeInBytes(Standard::CEOperandTypes_t iCEOperandType, int iAddressSizeInByte)
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

    // If no operand type can override the address size
    // we shall default to the address size ( 8 bytes in longmode, unless override by 0x67 address size override prefix, in which case it will be 4 bytes ) ?
    return iAddressSizeInByte;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int InsaneDASM64::CEOperandTypeToAdrsSizeInBits(Standard::CEOperandTypes_t iCEOperandType, int iAddressSizeInByte)
{
    int iSize = CEOperandTypeToAdrsSizeInBytes(iCEOperandType, iAddressSizeInByte);
    
    // Keep the invalid size invalid, and scale valid size.
    return iSize <= 0 ? -1 : iSize * 8;
}
