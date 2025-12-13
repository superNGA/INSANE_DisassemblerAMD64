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
#include "../Defs/OpCode_t.h"


namespace INSANE_DASM64_NAMESPACE
{

    ///////////////////////////////////////////////////////////////////////////
    enum InstTypes_t : uint16_t
    {
        InstTypes_LegacyPrefixGrp1 = (1 << 0),
        InstTypes_LegacyPrefixGrp2 = (1 << 1),
        InstTypes_LegacyPrefixGrp3 = (1 << 2),
        InstTypes_LegacyPrefixGrp4 = (1 << 3),

        InstTypes_REX              = (1 << 4),

        InstTypes_OpCode           = (1 << 5)
    };
    ///////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////
    class Tables_t
    {
    public:
        Tables_t();

        InsaneDASM64::ErrorCode_t Initialize();

        uint16_t GetInstType(Byte iOpCode, InstTypes_t iTypes) const;


    private:
        InsaneDASM64::ErrorCode_t _InitializeInstTypeLUT();
        uint16_t                  m_instTypeLUT[0xFFLLU + 1LLU]; // 256 entries...
        bool                      m_bInstTypeLUTInit = false;


        InsaneDASM64::ErrorCode_t _InitializeOpCodeTable();
        OpCode_t                  m_OpCodeTable[0xFFLLU + 1LLU]; // 256 entries...
        bool                      m_bOpCodeTableInit = false;
    };
    DEFINE_GLOBAL_OBJECT(g_tables, Tables_t)
    ///////////////////////////////////////////////////////////////////////////

}