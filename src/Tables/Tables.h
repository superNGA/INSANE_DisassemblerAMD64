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


        inline void SetOperatorInfo(const char* szOperatorName, int nOperands, OpCodeFlag_t iOpCodeFlag = OpCodeFlag_None,
            OpCodeOperand_t operand1 = OpCodeOperand_t(),
            OpCodeOperand_t operand2 = OpCodeOperand_t(),
            OpCodeOperand_t operand3 = OpCodeOperand_t(),
            OpCodeOperand_t operand4 = OpCodeOperand_t())
        {
            m_szOperatorName = szOperatorName;

            m_iOpCodeFlag    = iOpCodeFlag;

            m_nOperands      = nOperands;
            m_operands[0]    = operand1;
            m_operands[1]    = operand2;
            m_operands[2]    = operand3;
            m_operands[3]    = operand4;

            m_bIsValidOpCode = true;
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
        OperatorInfo_t            m_opCodeTable1[0xFFLLU + 1LLU]; // 256 entries...
        OperatorInfo_t            m_opCodeTable2[0xFFLLU + 1LLU]; // 256 entries...
        OperatorInfo_t            m_opCodeTable3[0xFFLLU + 1LLU]; // 256 entries...
        bool                      m_bOpCodeTableInit = false;
    };
    DEFINE_GLOBAL_OBJECT(g_tables, Tables_t)

}