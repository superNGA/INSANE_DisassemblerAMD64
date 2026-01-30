//=========================================================================
//                      Decoder
//=========================================================================
// by      : INSANE
// created : 19/01/2026
//
// purpose : Handles decoding for all instruction encoding types.
//-------------------------------------------------------------------------
#pragma once
#include "../../Include/Aliases.h"
#include "../../Include/INSANE_DisassemblerAMD64.h"



namespace INSANE_DASM64_NAMESPACE
{
    IDASMErrorCode_t DecodeLegacyEncoding(const std::vector<Byte>& vecInput, Instruction_t* pInstOut, size_t& iIterator);
    IDASMErrorCode_t DecodeVEXEncoding   (const std::vector<Byte>& vecInput, Instruction_t* pInstOut, size_t& iIterator);
    IDASMErrorCode_t DecodeEVEXEncoding  (const std::vector<Byte>& vecInput, Instruction_t* pInstOut, size_t& iIterator);
}
