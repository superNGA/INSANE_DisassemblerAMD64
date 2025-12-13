//=========================================================================
//                      INSANE Disassembler AMD64 ( 64-bit mode only )
//=========================================================================
// by      : INSANE
// created : 13/12/2025
// 
// purpose : Disassembler for 64 bit AMD64 assembly.
//-------------------------------------------------------------------------
#pragma once
#include <vector>


namespace InsaneDASM64
{
    // Data types...
    typedef unsigned char Byte;


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct Instruction_t
    {

    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct ParsedInst_t
    {

    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    enum ErrorCode_t : int
    {
        ErrorCode_Success    = 0, // All functions return ErrorCode_Success on success.
        ErrorCode_FailedInit = 1,
        ErrorCode_Count
    };

    
    // Functions...
    // NOTE : Each function will return 0 ( ErrorCode_Success ) on success, and a non-zero ErrorCode_t on fail.
    ErrorCode_t Initialize ();
    ErrorCode_t Disassemble(const std::vector<Byte>&         vecInput,       std::vector<Instruction_t>& vecOutput);
    ErrorCode_t Parse      (const std::vector<Byte>&         vecInput,       std::vector<ParsedInst_t>&  vecOutput);
    ErrorCode_t Decode     (const std::vector<ParsedInst_t>& vecParsedInput, std::vector<Instruction_t>& vecOutput);
    // TODO : ToString function.

    const char* GetErrorMessage(ErrorCode_t iErrorCode);
}