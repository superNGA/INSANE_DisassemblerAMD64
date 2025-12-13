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


///////////////////////////////////////////////////////////////////////////
class Tables_t
{
public:
    InsaneDASM64::ErrorCode_t Initialize();

private:
    InsaneDASM64::ErrorCode_t _InitializeInstTypeLUT();
    uint16_t m_instTypeLUT[0xFFLLU + 1LLU]; // 256 entries

};
DEFINE_GLOBAL_DISASSEMBLER_OBJECT(g_tables, Tables_t)
///////////////////////////////////////////////////////////////////////////