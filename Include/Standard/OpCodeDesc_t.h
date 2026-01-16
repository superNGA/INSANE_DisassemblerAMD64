//=========================================================================
//                      OpCode Description
//=========================================================================
// by      : INSANE
// created : 16/01/2026
//
// purpose : OpCode description, holds all operands, Modrm information, OpCode validity 
//          other information for an opcode, along with pointers to any varients and 
//          split method.
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include "../Rules.h"
#include "Operand/Operand_t.h"



namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct OpCodeDesc_t
    {
        OpCodeDesc_t();

        void Reset();


        // NOTE :
        //      VarientKey tells us, if this operands has others varients or not ( as in the case with 0x0f 0x00, 0x0f 0x01, etc... ).
        // And how to access the other varients. Pointer to all varients are stored inside the m_pVarients ( array ).
        // 
        //      Ex. VarientKey_ModRM_REG, means m_pVarients[modrm.REG] will gives us all different 
        // varients of this instruction.
        enum VarientType_t
        {
            VarientKey_None = -1,
            VarientKey_ModRM_REG = 0,
            VarientKey_ModRM_RM,
            VarientKey_ModRM_MOD,
            VarientKey_LegacyPrefix, // NOTE : Run legacy prefix through "the" look-up-table before using as index in m_pVarients.
        };


        // Max varients that can be stored in each of the varient types...
        static const size_t MAX_REG_VARIENTS           = 8llu;
        static const size_t MAX_MOD_VARIENTS           = 4llu;
        static const size_t MAX_RM_VARIENTS            = 8llu;
        static const size_t MAX_LEGACY_PREFIX_VARIENTS = 12llu;

        size_t GetMaxVarients(VarientType_t iVarientType);


        void Init(
                const char* szName, bool bValidOpcd, bool bEscapeOpcd, bool bModrmRequired, Byte iByte, 
                int nOperands, Operand_t operand1, Operand_t operand2, Operand_t operand3, Operand_t operand4);

        bool InsertVarient(int iIndex);

        // Initialize struct for one varient type ( now it doesn't hold any instruction info by itself, just varient array. )
        bool InitVarientType(VarientType_t iVarientType);


        // Braching info, to reach varients of this opcode.
        VarientType_t  m_iVarientType   = VarientKey_None;
        int            m_nVarients      = 0;
        OpCodeDesc_t** m_pVarients      = nullptr;

        // OpCode's info...
        char           m_szName[Rules::MAX_INST_NAME_SIZE];
        bool           m_bIsValidCode   = false;
        bool           m_bIsEscapeCode  = false;
        bool           m_bModrmRequired = false;
        Byte           m_iByte          = 0x00;

        // Operands...
        Operand_t      m_operands[Rules::MAX_OPERANDS];
        int            m_nOperands      = 0;
    };
}
