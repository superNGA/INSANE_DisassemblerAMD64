//=========================================================================
//                      OpCode
//=========================================================================
// by      : INSANE
// created : 13/12/2025
// 
// purpose : Holds information about OpCode bytes.
//-------------------------------------------------------------------------
#pragma once
#include <cstdint>
#include "../ObjectNomenclature.h"
#include "../DASMRules.h"


namespace INSANE_DASM64_NAMESPACE
{

    ///////////////////////////////////////////////////////////////////////////
    enum OpCodeAddressingMethod_t : int16_t
    {
        OpCodeAddressingMethod_Invalid = -1,

        OpCodeAddressingMethod_A = 0,
        OpCodeAddressingMethod_B,
        OpCodeAddressingMethod_C,
        OpCodeAddressingMethod_D,
        OpCodeAddressingMethod_E,
        OpCodeAddressingMethod_F,
        OpCodeAddressingMethod_G,
        OpCodeAddressingMethod_H,
        OpCodeAddressingMethod_I,
        OpCodeAddressingMethod_J,
        // OpCodeAddressingMethod_K,
        OpCodeAddressingMethod_L,
        OpCodeAddressingMethod_M,
        OpCodeAddressingMethod_N,
        OpCodeAddressingMethod_O,
        OpCodeAddressingMethod_P,
        OpCodeAddressingMethod_Q,
        OpCodeAddressingMethod_R,
        OpCodeAddressingMethod_S,
        // OpCodeAddressingMethod_T,
        OpCodeAddressingMethod_U,
        OpCodeAddressingMethod_V,
        OpCodeAddressingMethod_W,
        OpCodeAddressingMethod_X,
        OpCodeAddressingMethod_Y,

        // Fixed register addressing methods...
        // RAX...
        OpCodeAddressingMethod_AL,
        OpCodeAddressingMethod_AX,
        OpCodeAddressingMethod_EAX,
        OpCodeAddressingMethod_RAX,

        // RBX...
        OpCodeAddressingMethod_BL,
        OpCodeAddressingMethod_BX,
        OpCodeAddressingMethod_EBX,
        OpCodeAddressingMethod_RBX,

        // RCX...
        OpCodeAddressingMethod_CL,
        OpCodeAddressingMethod_CX,
        OpCodeAddressingMethod_ECX,
        OpCodeAddressingMethod_RCX,

        // RDX...
        OpCodeAddressingMethod_DL,
        OpCodeAddressingMethod_DX,
        OpCodeAddressingMethod_EDX,
        OpCodeAddressingMethod_RDX,

        // RSP
        OpCodeAddressingMethod_SPL,
        OpCodeAddressingMethod_SP,
        OpCodeAddressingMethod_ESP,
        OpCodeAddressingMethod_RSP,

        // RDI...
        OpCodeAddressingMethod_DIL,
        OpCodeAddressingMethod_DI,
        OpCodeAddressingMethod_EDI,
        OpCodeAddressingMethod_RDI,

        // RSI...
        OpCodeAddressingMethod_SIL,
        OpCodeAddressingMethod_SI,
        OpCodeAddressingMethod_ESI,
        OpCodeAddressingMethod_RSI,

        // RBP
        OpCodeAddressingMethod_BPL,
        OpCodeAddressingMethod_BP,
        OpCodeAddressingMethod_EBP,
        OpCodeAddressingMethod_RBP,

        OpCodeAddressingMethod_Count
    };
    ///////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////
    enum OpCodeOperandType_t : int16_t
    {
        OpCodeOperandType_Invalid = -1,

        OpCodeOperandType_a = 0,
        OpCodeOperandType_b,
        OpCodeOperandType_c,
        OpCodeOperandType_d,
        OpCodeOperandType_dq,
        OpCodeOperandType_p,
        OpCodeOperandType_pd,
        OpCodeOperandType_pi,
        OpCodeOperandType_ps,
        OpCodeOperandType_q,
        OpCodeOperandType_qq,
        OpCodeOperandType_s,
        OpCodeOperandType_sd,
        OpCodeOperandType_ss,
        OpCodeOperandType_si,
        OpCodeOperandType_v,
        OpCodeOperandType_w,
        OpCodeOperandType_x,
        OpCodeOperandType_y,
        OpCodeOperandType_z,

        OpCodeOperantType_Count
    };
    ///////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////
    struct OpCodeOperand_t
    {
        OpCodeOperand_t(
            OpCodeAddressingMethod_t iAddressingMethod = OpCodeAddressingMethod_Invalid,
            OpCodeOperandType_t      iOperandType      = OpCodeOperandType_Invalid)
        {
            m_iAddressingMethod = iAddressingMethod;
            m_iOperandType      = iOperandType;
        }


        void operator=(const OpCodeOperand_t& other)
        {
            m_iAddressingMethod = other.m_iAddressingMethod;
            m_iOperandType      = other.m_iOperandType;
        }


        // Valid operand check...
        inline bool IsValid() const
        {
            bool bValidAddressingMethod = m_iAddressingMethod > OpCodeAddressingMethod_Invalid && m_iAddressingMethod < OpCodeAddressingMethod_Count;
            bool bValidOperandType      = m_iOperandType      > OpCodeOperandType_Invalid      && m_iOperandType      < OpCodeOperantType_Count;
            
            return bValidAddressingMethod == true && bValidOperandType == true;
        }


        OpCodeAddressingMethod_t m_iAddressingMethod;
        OpCodeOperandType_t      m_iOperandType;
    };
    ///////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////
    struct OpCode_t
    {
        // Default constructor...
        OpCode_t()
        {
            m_nOperands   = 0;
            m_operands[0] = OpCodeOperand_t();
            m_operands[1] = OpCodeOperand_t();
            m_operands[2] = OpCodeOperand_t();
            m_operands[3] = OpCodeOperand_t();
        }


        // Constructor...
        OpCode_t(int nOperands,
            OpCodeOperand_t iOperand1 = OpCodeOperand_t(),
            OpCodeOperand_t iOperand2 = OpCodeOperand_t(),
            OpCodeOperand_t iOperand3 = OpCodeOperand_t(),
            OpCodeOperand_t iOperand4 = OpCodeOperand_t())
        {
            m_nOperands   = nOperands;

            m_operands[0] = iOperand1; 
            m_operands[1] = iOperand2;
            m_operands[2] = iOperand3; 
            m_operands[3] = iOperand4;
        }

        // NOTE : One OpCode_t will hold all the OpCodes that are present in one
        //        instruction. That means, if multibyte opcode is present in one instruction,
        //        we will store all bytes ( 1 to 3 ) in a single OpCode_t struct.

        OpCodeOperand_t m_operands[MAX_OPERANDS];
        int32_t         m_nOperands = 0;
    };
    ///////////////////////////////////////////////////////////////////////////

}