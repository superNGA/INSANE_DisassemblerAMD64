//=========================================================================
//                      DASMInst ( Disassembled Instruction )
//=========================================================================
// by      : INSANE
// created : 17/01/2026
//
// purpose : Holds disassembled instruction, as null-terminated strings.
//           Output format for disassembler.
//-------------------------------------------------------------------------
#pragma once
#include "Aliases.h"
#include "Rules.h"



namespace INSANE_DASM64_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct DASMInst_t
    {
        // Sets arg count to 0 and clears all buffers.
        DASMInst_t();
        void Clear();

        // Convert numbers as hexadecimal string.
        void PushLiteralOperand(uint64_t iLiteralOperand);
        void PushLiteralOperand(uint32_t iLiteralOperand);
        void PushLiteralOperand(uint16_t iLiteralOperand);
        void PushLiteralOperand(uint8_t  iLiteralOperand);
        void PushLiteralOperand(int iLiteralOperand);

        // Push a string representing representing a hex number as an operand ( with flipped endian if desired ).
        void PushLiteralFromString(const Byte* szOperand, int iSize, bool bFlipEndian);

        // string copy.
        void PushBackOperand(const char* szOperand);

        char m_szMnemonic[Rules::DASMINST_ARG_BUFFER_SIZE];

        int m_nOperands;
        char m_szOperands[Rules::MAX_OPERANDS][Rules::DASMINST_ARG_BUFFER_SIZE];
    };
}
