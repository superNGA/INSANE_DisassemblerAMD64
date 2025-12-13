//=========================================================================
//                      INSANE Disassembler AMD64 ( 64-bit mode only )
//=========================================================================
// by      : INSANE
// created : 13/12/2025
// 
// purpose : Disassembler for 64 bit AMD64 assembly.
//-------------------------------------------------------------------------
#include "../Include/INSANE_DisassemblerAMD64.h"

#include <vector>
#include "Tables/Tables.h"


// NOTE : Mind this.
using namespace InsaneDASM64;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
ErrorCode_t InsaneDASM64::Initialize()
{
    // Tables...
    {
        ErrorCode_t iErrorCode = G::g_tables.Initialize();
        if (iErrorCode != ErrorCode_t::ErrorCode_Success)
            return iErrorCode;
    }


    return ErrorCode_t::ErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
ErrorCode_t InsaneDASM64::Disassemble(const std::vector<Byte>& vecInput, std::vector<Instruction_t>& vecOutput)
{
    vecOutput.clear();


    // Parse input...
    std::vector<ParsedInst_t> vecParsedInst; vecParsedInst.clear();

    ErrorCode_t iParserErrorCode = Parse(vecInput, vecParsedInst);
    if (iParserErrorCode != ErrorCode_Success)
        return iParserErrorCode;


    // Parsing didn't fail but its empty :(
    if (vecParsedInst.empty() == true)
        return ErrorCode_t::ErrorCode_Success;


    // Decode...
    ErrorCode_t iDecodeErrorCode = Decode(vecParsedInst, vecOutput);
    if (iDecodeErrorCode != ErrorCode_t::ErrorCode_Success)
        return iDecodeErrorCode;


    return ErrorCode_t::ErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
ErrorCode_t InsaneDASM64::Parse(const std::vector<Byte>& vecInput, std::vector<ParsedInst_t>& vecOutput)
{
    return ErrorCode_t::ErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
ErrorCode_t InsaneDASM64::Decode(const std::vector<ParsedInst_t>& vecParsedInput, std::vector<Instruction_t>& vecOutput)
{
    return ErrorCode_t::ErrorCode_Success;
}
