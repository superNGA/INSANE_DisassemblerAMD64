//=========================================================================
//                      Tables
//=========================================================================
// by      : INSANE
// created : 13/12/2025
// 
// purpose : Holds Look-up-tables of instruction information.
//-------------------------------------------------------------------------
#pragma once
#include <cstdint>

#include "../../Include/INSANE_DisassemblerAMD64.h"
#include "../ObjectNomenclature.h"


namespace INSANE_DASM64_NAMESPACE
{

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    enum InstTypes_t : uint16_t
    {
        InstTypes_LegacyPrefixGrp1 = (1 << 0),
        InstTypes_LegacyPrefixGrp2 = (1 << 1),
        InstTypes_LegacyPrefixGrp3 = (1 << 2),
        InstTypes_LegacyPrefixGrp4 = (1 << 3),

        // All legacy prefixies.
        InstTypes_LegacyPrefixGrpAll = 
            InstTypes_LegacyPrefixGrp1 | InstTypes_LegacyPrefixGrp2 | InstTypes_LegacyPrefixGrp3 | InstTypes_LegacyPrefixGrp4,

        InstTypes_REX              = (1 << 4),

        InstTypes_OpCode           = (1 << 5)
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct OpCodeDesc_t
    {
        OpCodeDesc_t()
        {
            m_iVarientKey   = VarientKey_None;
            m_nVarients     = 0;
            m_pVarients     = nullptr;
            m_szName[0]     = '\0';
            m_bIsValidCode  = false;
            m_bIsEscapeCode = false;
            m_iByte         = 0x00;
            m_nOperands     = 0;
        }


        // NOTE :
        //      VarientKey tells us, if this operands has others varients or not ( imagine nested opcodes ).
        // And how to access the other varients. Pointer to all varients are stored inside the m_pVarients ( array ).
        // 
        //      Ex. VarientKey_ModRM_REG, means m_pVarients[modrm.REG] will gives us all different 
        // varients of this instruction.
        enum VarientKey_t : int
        {
            VarientKey_None = -1,
            VarientKey_ModRM_REG = 0,
            VarientKey_ModRM_RM,
            VarientKey_ModRM_MOD,
            VarientKey_LegacyPrefix, // Run legacy prefix through the look-up-table before using as index in m_pVarients.
        };


        // Braching info, to reach varients of this opcode.
        VarientKey_t  m_iVarientKey   = VarientKey_None;
        int           m_nVarients     = 0;
        OpCodeDesc_t* m_pVarients     = nullptr;

        // OpCode's info...
        char          m_szName[0x10];
        bool          m_bIsValidCode  = false;
        bool          m_bIsEscapeCode = false;
        Byte          m_iByte         = 0x00;

        // Operands...
        Operand_t     m_operands[Rules::MAX_OPERANDS];
        int           m_nOperands     = 0;
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct OperatorInfo_t
    {
        OperatorInfo_t()
        {
            m_szOperatorName  = nullptr;
            m_nOperands       = 0;
            m_bIsEscapeOpCode = false;
            m_bIsValidOpCode  = false;
            m_iOpCodeFlag     = OpCodeFlag_t::OpCodeFlag_None;
        }


        inline void SetOperatorInfo(
            const char* szOperatorName, Byte iByte, int nOperands, bool bValidOpCode, bool bEscapeCode,
            OpCodeOperand_t operand1,
            OpCodeOperand_t operand2,
            OpCodeOperand_t operand3,
            OpCodeOperand_t operand4)
        {
            m_szOperatorName  = szOperatorName;
            iByte             = m_iByte;
            m_bIsValidOpCode  = bValidOpCode;
            m_bIsEscapeOpCode = bEscapeCode;
            m_nOperands       = nOperands;
            m_nOperands       = nOperands;
            m_operands[0]     = operand1;
            m_operands[1]     = operand2;
            m_operands[2]     = operand3;
            m_operands[3]     = operand4;
        }


        // Operator Properties...
        const char*     m_szOperatorName  = nullptr;
        bool            m_bIsValidOpCode  = false;
        bool            m_bIsEscapeOpCode = false;
        Byte            m_iByte           = 0x00;


        // Operands...
        OpCodeOperand_t m_operands[Rules::MAX_OPERANDS];
        int32_t         m_nOperands       = 0;
        OpCodeFlag_t    m_iOpCodeFlag     = OpCodeFlag_t::OpCodeFlag_None;
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    class Tables_t
    {
    public:
        Tables_t();

        InsaneDASM64::ErrorCode_t Initialize();
        bool                      IsInitialized() const;

        uint16_t                  GetInstType    (Byte iOpCode) const;
        const OperatorInfo_t*     GetOperatorInfo(Byte iOpCode, int iTableIndex) const;


    private:
        InsaneDASM64::ErrorCode_t _InitializeInstTypeLUT();
        uint16_t                  m_instTypeLUT[0xFFLLU + 1LLU]; // 256 entries...
        bool                      m_bInstTypeLUTInit = false;


        InsaneDASM64::ErrorCode_t _InitializeOpCodeTable();
        void                      InitOneByteOpCodeTable();
        void                      InitTwoByteOpCodeTable();
        OperatorInfo_t            m_opCodeTable1[0xFFLLU + 1LLU]; // 256 entries...
        OperatorInfo_t            m_opCodeTable2[0xFFLLU + 1LLU]; // 256 entries...
        OperatorInfo_t            m_opCodeTable3[0xFFLLU + 1LLU]; // 256 entries...
        bool                      m_bOpCodeTableInit = false;
    };
    DEFINE_GLOBAL_OBJECT(g_tables, Tables_t)

}