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
#pragma once
#include "../../Include/Aliases.h"
#include "../../Include/INSANE_DisassemblerAMD64.h"



namespace INSANE_DASM64_NAMESPACE
{
    IDASMErrorCode_t Disassemble(const Instruction_t* pInst, DASMInst_t* pOutput);
}
