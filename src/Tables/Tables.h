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
            m_szOperatorName = nullptr;
            m_nOperands      = 0;

            m_bIsEscapeOpCode = false; m_bIsValidOpCode = false;
        }


        // Operator Properties...
        const char*     m_szOperatorName  = nullptr;
        bool            m_bIsValidOpCode  = false;
        bool            m_bIsEscapeOpCode = false;
        Byte            m_iByte           = 0x00;


        // Operands...
        OpCodeOperand_t m_operands[Rules::MAX_OPERANDS];
        int32_t         m_nOperands       = 0;
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
        OperatorInfo_t            m_OpCodeTable1[0xFFLLU + 1LLU]; // 256 entries...
        OperatorInfo_t            m_OpCodeTable2[0xFFLLU + 1LLU]; // 256 entries...
        OperatorInfo_t            m_OpCodeTable3[0xFFLLU + 1LLU]; // 256 entries...
        bool                      m_bOpCodeTableInit = false;
    };
    DEFINE_GLOBAL_OBJECT(g_tables, Tables_t)

}