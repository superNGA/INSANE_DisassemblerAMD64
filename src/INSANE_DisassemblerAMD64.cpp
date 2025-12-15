//=========================================================================
//                      INSANE Disassembler AMD64 ( 64-bit mode only )
//=========================================================================
// by      : INSANE
// created : 13/12/2025
// 
// purpose : Disassembler for 64 bit AMD64 assembly.
//-------------------------------------------------------------------------
#include "../Include/INSANE_DisassemblerAMD64.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <assert.h>
#include "Tables/Tables.h"


// NOTE : Mind this.
using namespace INSANE_DASM64_NAMESPACE;


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
    assert(G::g_tables.IsInitialized() == true && "Tables are not initialized. Initialize tables before parsing!.");

    // Temporary instruction.
    ParsedInst_t inst;


    size_t nBytes = vecInput.size();
    for (size_t iByteIndex = 0LLU; iByteIndex < nBytes; iByteIndex++)
    {
        Byte        iByte     = vecInput[iByteIndex];
        InstTypes_t iInstType = static_cast<InstTypes_t>(G::g_tables.GetInstType(iByte));


        // If this byte is a legacy prefix.
        if ((iInstType & InstTypes_t::InstTypes_LegacyPrefixGrpAll) != false)
        {
            // We iterate forward ( upto Rules::MAX_LEGACY_PREFIX ( 4 ) bytes ) and collect all prefix.
            for (size_t iPrefixIndex = iByteIndex; iPrefixIndex < nBytes && iPrefixIndex - iByteIndex < Rules::MAX_LEGACY_PREFIX; iPrefixIndex++)
            {
                Byte iPrefixByte = vecInput[iPrefixIndex];

                // if this byte ain't a prefix, break. We have already stored all consecutive prefix in this instruction.
                if ((G::g_tables.GetInstType(iPrefixByte) & InstTypes_t::InstTypes_LegacyPrefixGrpAll) == false)
                {
                    iByteIndex = iPrefixIndex;
                    break;
                }


                // Can we even store this prefix ? 
                if (inst.m_legacyPrefix.PrefixCount() >= Rules::MAX_LEGACY_PREFIX)
                    return ErrorCode_t::ErrorCode_TooManyPefix; // already stored 'MAX_LEGACY_PREFIX' no. of prefixies.


                inst.m_legacyPrefix.PushPrefix(iPrefixByte);
            }
        }
        // If this byte is a REX, skip till the last consecutive REX byte and store the last one.
        else if ((iInstType & InstTypes_t::InstTypes_REX) != false)
        {
            for (size_t iREXIndex = iByteIndex; iREXIndex < nBytes - 1LLU; iREXIndex++)
            {
                Byte iCurByte  = vecInput[iREXIndex];
                Byte iNextByte = vecInput[iREXIndex + 1LLU];


                // Next byte ain't REX, and this byte is. Store only this one.
                if ((G::g_tables.GetInstType(iNextByte) & InstTypes_t::InstTypes_REX) == false)
                {
                    inst.m_bHasREX   = true;
                    inst.m_iREX      = iCurByte;
                    inst.m_iREXIndex = iREXIndex;
                    iByteIndex       = iREXIndex;

                    break;
                }

            }
        }
        else // if neither REX nor LegacyPrefix.
        {
            // 1. is REX immediately preceding OpCode check...
            if (inst.m_bHasREX == true && inst.m_iREXIndex != iByteIndex - 1LLU && inst.m_opCode.OpByteCount() == 0)
                return ErrorCode_t::ErrorCode_REXNotPrecedingOpCode;


            // 2. Collect upto MAX_OPBYTES opcodes and gets its equivalent isntruction / operand properties.
            for (size_t iOpCodeByteIndex = iByteIndex; iOpCodeByteIndex < nBytes && iOpCodeByteIndex - iByteIndex < Rules::MAX_OPBYTES; iOpCodeByteIndex++)
            {
                Byte                  iOpByte       = vecInput[iOpCodeByteIndex];
                const OperatorInfo_t* pOperatorInfo = G::g_tables.GetOperatorInfo(iOpByte, inst.m_opCode.OpByteCount() + 1); // We wanna look at the next table.

                if (pOperatorInfo->m_bIsValidOpCode == false)
                    return ErrorCode_t::ErrorCode_InvalidOpCode;

                inst.m_opCode.PushOpCode(iOpByte);

                if (pOperatorInfo->m_bIsEscapeOpCode == false)
                {
                    inst.m_opCode.CopyOperandInfo(pOperatorInfo);
                    inst.m_opCode.m_szOperatorName = pOperatorInfo->m_szOperatorName;
                    break;
                }
            }

            printf("Stored OpCodes : ");
            for (int i = 0; i < inst.m_opCode.m_nOpBytes; i++)
                printf("0x%02X ", inst.m_opCode.m_opBytes[i]);
            printf("\n");


            // 3. Collect ModR/M if required.
            // 4. Collect SID if required.
            // 5. Collect Displacement if required.
            // 6. Collect Immediate if required.
            // 7. This instruction is complete, store it.
            

            // 8. Clear.
            inst.Clear();
        }
    }


    return ErrorCode_t::ErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
ErrorCode_t InsaneDASM64::Decode(const std::vector<ParsedInst_t>& vecParsedInput, std::vector<Instruction_t>& vecOutput)
{
    return ErrorCode_t::ErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
const char* InsaneDASM64::GetErrorMessage(ErrorCode_t iErrorCode)
{
    switch (iErrorCode)
    {
    case InsaneDASM64::ErrorCode_Success:               return "[ Insane Disassembler AMD64 ] No Error";
    case InsaneDASM64::ErrorCode_FailedInit:            return "[ Insane Disassembler AMD64 ] Failed initialization!";
    case InsaneDASM64::ErrorCode_TooManyPefix:          return "[ Insane Disassembler AMD64 ] Failed parsing!, more than MAX_PREFIX amount of prefix encountered consecutively";
    case InsaneDASM64::ErrorCode_REXNotPrecedingOpCode: return "[ Insane Disassembler AMD64 ] REX exists in an instruction but doesn't immediately preced OpCode bytes.";
    case InsaneDASM64::ErrorCode_InvalidOpCode:         return "[ Insane Disassembler AMD64 ] Invalid OpCode appeared in input.";
    
    default: break;
    }

    static char s_invalidCodeBuffer[128] = "";
    sprintf(s_invalidCodeBuffer, "[ Insane Disassembler AMD64 ] Invalid Error Code { %d }", iErrorCode);
    return s_invalidCodeBuffer;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
OpCodeOperand_t::OpCodeOperand_t()
{
    m_iAddressingMethod = OpCodeAddressingMethod_Invalid;
    m_iOperandType      = OpCodeOperandType_Invalid;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
OpCodeOperand_t::OpCodeOperand_t(OpCodeAddressingMethod_t iAddressingMethod, OpCodeOperandType_t iOperandType)
{
    m_iAddressingMethod = iAddressingMethod;
    m_iOperandType      = iOperandType;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void OpCodeOperand_t::operator=(const OpCodeOperand_t& other)
{
    m_iAddressingMethod = other.m_iAddressingMethod;
    m_iOperandType      = other.m_iOperandType;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool OpCodeOperand_t::IsValid() const
{
    bool bValidAddressingMethod = m_iAddressingMethod > OpCodeAddressingMethod_Invalid && m_iAddressingMethod < OpCodeAddressingMethod_Count;
    bool bValidOperandType      = m_iOperandType      > OpCodeOperandType_Invalid      && m_iOperandType      < OpCodeOperantType_Count;

    return bValidAddressingMethod == true && bValidOperandType == true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
OpCode_t::OpCode_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void OpCode_t::Clear()
{
    m_nOpBytes       = 0;
    m_nOperands      = 0;
    m_szOperatorName = nullptr;
    m_iOpCodeFlag    = OpCodeFlag_t::OpCodeFlag_None;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int OpCode_t::OpByteCount() const
{
    return m_nOpBytes;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int OpCode_t::OperandCount() const
{
    return m_nOperands;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool OpCode_t::PushOpCode(Byte iByte)
{
    assert(m_nOpBytes < Rules::MAX_OPBYTES && "MAX opbytes are already stored!");
    if(m_nOpBytes >= Rules::MAX_OPBYTES)
        return false;

    m_opBytes[m_nOpBytes] = iByte;
    m_nOpBytes++;
    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void OpCode_t::CopyOperandInfo(const OperatorInfo_t* pOperatorInfo)
{
    // NOTE : This functiond deliberatly does not copy any information other
    //        then operand information. Operand Name & byte are to be copied by caller seperately.

    m_nOperands = pOperatorInfo->m_nOperands;

    // Copy operands.
    for (int i = 0; i < Rules::MAX_OPERANDS; i++)
        m_operands[i] = pOperatorInfo->m_operands[i];

    m_iOpCodeFlag = pOperatorInfo->m_iOpCodeFlag;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
LegacyPrefix_t::LegacyPrefix_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void LegacyPrefix_t::Clear()
{
    m_nPrefix = 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool LegacyPrefix_t::PushPrefix(Byte iByte)
{
    assert(m_nPrefix < Rules::MAX_LEGACY_PREFIX && "Prefix list is already full!!!");
    
    if (m_nPrefix >= Rules::MAX_LEGACY_PREFIX)
        return false;

    m_legacyPrefix[m_nPrefix] = iByte;
    m_nPrefix++;

    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int LegacyPrefix_t::PrefixCount() const
{
    return m_nPrefix;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
ParsedInst_t::ParsedInst_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void ParsedInst_t::Clear()
{
    m_opCode.Clear();
    m_legacyPrefix.Clear();

    m_bHasREX   = false;
    m_iREX      = 0x00; // This is an invalid REX byte ( 0x40 to 0x4F ).
    m_iREXIndex = -1; // This is used to check if REX preseeds the OpBytes or not. ( they must.)
}
