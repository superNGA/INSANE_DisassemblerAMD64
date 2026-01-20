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
#include <string>

#include "Aliases.h"
#include "Rules.h"
#include "Standard/Operand/CEOperandModes.h"
#include "Standard/Operand/CEOperandTypes.h"
#include "Standard/Operand/OperandModes.h"
#include "Standard/Operand/OperandTypes.h"

// Standard instructon structure, decoder returns in this format.
#include "Instruction_t.h"

// Output format for disassembler
#include "DASMInst_t.h"



namespace INSANE_DASM64_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    enum IDASMErrorCode_t : int
    {
        IDASMErrorCode_Success    = 0, // All functions return ErrorCode_Success on success.
        IDASMErrorCode_FailedInit = 1,
        IDASMErrorCode_TooManyPefix,
        IDASMErrorCode_REXNotPrecedingOpCode,
        IDASMErrorCode_InvalidOpCode,
        IDASMErrorCode_NoOpByteFound,
        IDASMErrorCode_ModRMNotFound,
        IDASMErrorCode_SIBNotFound,
        IDASMErrorCode_InvalidDispSize,
        IDASMErrorCode_DisplacementNotFound,
        IDASMErrorCode_NoImmediateFound,
        IDASMErrorCode_InvalidImmediateSize,
        IDASMErrorCode_InvalidVEXPrefix,
        IDASMErrorCode_InvalidVEXInst,
        IDASMErrorCode_VEXDisassemblyFailed, // Delete this.

        // Disassembler specific.
        IDASMErrorCode_InstComponentsNotFound,
        IDASMErrorCode_OpCodeNotInitialized,


        IDASMErrorCode_Count
    };

    
    // Functions...
    // NOTE : Each function will return 0 ( ErrorCode_Success ) on success, and a non-zero IDASMErrorCode_t on fail.
    IDASMErrorCode_t Initialize();
    IDASMErrorCode_t DecodeAndDisassemble(const std::vector<Byte>&          vecInput, std::vector<DASMInst_t>&    vecOutput);
    IDASMErrorCode_t Disassemble         (const std::vector<Instruction_t>& vecInput, std::vector<DASMInst_t>&    vecOutput);
    IDASMErrorCode_t Decode              (const std::vector<Byte>&          vecInput, std::vector<Instruction_t>& vecOutput);

    const char* GetErrorMessage(IDASMErrorCode_t iErrorCode);
    Standard::CEOperandTypes_t GeekToCoderOperandType(Standard::OperandTypes_t iOperandType);
    Standard::CEOperandModes_t GeekToCoderOperandMode(Standard::OperandModes_t iOperandMode);
}
