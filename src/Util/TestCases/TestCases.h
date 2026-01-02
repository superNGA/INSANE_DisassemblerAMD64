//=========================================================================
//                      Test Cases
//=========================================================================
// by      : INSANE
// created : 02/01/2026
// 
// purpose : Byte streams for testing disassembler's accuracy.
//-------------------------------------------------------------------------
#pragma once
#include <vector>
#include "../../ObjectNomenclature.h"
#include "../../../Include/INSANE_DisassemblerAMD64.h"


namespace INSANE_DASM64_NAMESPACE
{
    namespace TestCases
    {
        // For testing Legacy encoding instructions.
        
        // One byte opcode instructions only...
        extern std::vector<Byte> g_vecOneByteOpCodes_001;
        extern std::vector<Byte> g_vecOneByteOpCodes_002;
        
        // Two byte opcode instructions only...
        extern std::vector<Byte> g_vecTwoByteOpCodes_001;
        
        // Three byte opcode instructions ( escaped via 0x38 & 0x3A respectively )
        extern std::vector<Byte> g_vecThreeByteOpCodes_38;
        extern std::vector<Byte> g_vecThreeByteOpCodes_3A;
    }
}