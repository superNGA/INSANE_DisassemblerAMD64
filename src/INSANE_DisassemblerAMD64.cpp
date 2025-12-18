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

    vecOutput.clear();

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
            size_t iPrefixIndex = iByteIndex;
            for (iPrefixIndex = iByteIndex; iPrefixIndex < nBytes && iPrefixIndex - iByteIndex < Rules::MAX_LEGACY_PREFIX; iPrefixIndex++)
            {
                Byte iPrefixByte = vecInput[iPrefixIndex];

                // if this byte ain't a prefix, break. We have already stored all consecutive prefix in this instruction.
                if ((G::g_tables.GetInstType(iPrefixByte) & InstTypes_t::InstTypes_LegacyPrefixGrpAll) == false)
                    break;


                // Can we even store this prefix ? 
                if (inst.m_legacyPrefix.PrefixCount() >= Rules::MAX_LEGACY_PREFIX)
                    return ErrorCode_t::ErrorCode_TooManyPefix; // already stored 'MAX_LEGACY_PREFIX' no. of prefixies.


                inst.m_legacyPrefix.PushPrefix(iPrefixByte);
            }
            iByteIndex = iPrefixIndex;
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

                // NOTE: +1 is cause, when 0 opbytes are stored we want to check in table1 ( one byte opcodes )
                //        when opbytes are 1, we wanna look in table 2 ( two byte opcode ), and so on.
                int                   iTableIndex = inst.m_opCode.OpByteCount() + 1;
                Byte                  iOpByte = vecInput[iOpCodeByteIndex];
                const OperatorInfo_t* pOperatorInfo = G::g_tables.GetOperatorInfo(iOpByte, iTableIndex); // We wanna look at the next table.

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


            // Did we store any opbytes from the loop above ?
            if (inst.m_opCode.OpByteCount() <= 0)
                return ErrorCode_t::ErrorCode_NoOpByteFound;


            int iByteCaptured = inst.m_opCode.OpByteCount();
            assert(iByteCaptured >= 0 && "Negative amount of bytes captured. This is unacceptable!!!");
            iByteIndex += static_cast<size_t>(iByteCaptured);


            printf("Stored OpCodes : ");
            for (int i = 0; i < inst.m_opCode.m_nOpBytes; i++)
                printf("0x%02X ", inst.m_opCode.m_opBytes[i]);
            printf("\n");


            // 3. Collect ModR/M if required.
            for (int iOperandIndex = 0; iOperandIndex < inst.m_opCode.m_nOperands; iOperandIndex++)
            {
                OpCodeOperand_t* pOperand = &inst.m_opCode.m_operands[iOperandIndex];
                if (pOperand->m_iAddressingMethod == OpCodeAddressingMethod_E ||
                    pOperand->m_iAddressingMethod == OpCodeAddressingMethod_W ||
                    pOperand->m_iAddressingMethod == OpCodeAddressingMethod_Q)
                {

                    // Is out of bound?
                    if (iByteIndex >= nBytes)
                        return ErrorCode_t::ErrorCode_ModRMNotFound;

                    // Store the ModRM and move the interator forward by one.
                    inst.m_bHasModRM = true;
                    inst.m_iModRM    = vecInput[iByteIndex];
                    iByteIndex++;

                    printf("opcode [ 0x%02X ] -> modR/M [ 0x%02X ]\n", inst.m_opCode.m_opBytes[inst.m_opCode.m_nOpBytes - 1], inst.m_iModRM);

                    break;
                }
            }


            // 4. Collect SID if required.
            bool bSIDRequired = inst.m_bHasModRM == true && (inst.m_iModRM & 0b11000000) != 0b11000000 && (inst.m_iModRM & 0b111) == 0b100;

            if (bSIDRequired == true)
            {
                if (iByteIndex >= nBytes)
                    return ErrorCode_t::ErrorCode_SIDNotFound;

                inst.m_bHasSID = true;
                inst.m_iSIB = vecInput[iByteIndex];
                iByteIndex++;

                printf("SID Byte captured : 0x%02X, for ModR/M : 0x%02X\n", inst.m_iSIB, inst.m_iModRM);
            }


            // 5. Collect Displacement if required.
            int iDisplacementSize = 0;
            if (inst.m_bHasModRM == true)
            {
                Byte iMod = (inst.m_iModRM & 0b11000000);

                if (iMod == 0b01000000)
                {
                    iDisplacementSize = 1;
                }
                else if (iMod == 0b10000000 || (iMod == 0 && (inst.m_iModRM & 0b111) == 0b101))
                {
                    iDisplacementSize = 4;
                }
                else if (iMod == 0 && (inst.m_iSIB & 0b111) == 0b101) // index == 101 ?
                {
                    iDisplacementSize = 4;
                }
            }


            for (int iDispByteIndex = 0; iDispByteIndex < iDisplacementSize && iDispByteIndex + iByteIndex < nBytes; iDispByteIndex++)
            {
                Byte iDispByte = vecInput[iDispByteIndex + iByteIndex];
                
                inst.m_displacement.PushByte(iDispByte);
            }
            iByteIndex += inst.m_displacement.ByteCount(); // Move iterator by whatever we have collected.


            // 6. Collect Immediate if required.
            bool bRequireImmediate = false;
            for (int i = 0; i < inst.m_opCode.OperandCount(); i++)
            {
                OpCodeOperand_t* pOperand = &inst.m_opCode.m_operands[i];
                if (pOperand->m_iAddressingMethod == OpCodeAddressingMethod_I)
                {
                    bRequireImmediate = true; break;
                }
            }


            if (bRequireImmediate == true)
            {
                if (iByteIndex >= nBytes)
                    return ErrorCode_t::ErrorCode_NoImmediateFound;

                int iOperandSize = 4; // default 4 bytes operand size in 64 bit mode.
                for (int i = 0; i < inst.m_legacyPrefix.PrefixCount(); i++)
                    if (inst.m_legacyPrefix.m_legacyPrefix[i] == 0x66)
                        iOperandSize = 2;

                if (inst.m_iREX & 0b00001000)
                    iOperandSize = 8;


                for (int iImmediateIndex = 0; iImmediateIndex < iOperandSize && iImmediateIndex + iByteIndex < nBytes; iImmediateIndex++)
                {
                    Byte iImmediateByte = vecInput[iImmediateIndex + iByteIndex];
                    inst.m_immediate.PushByte(iImmediateByte);
                }
                iByteIndex += inst.m_immediate.ByteCount();

                // We tried to store immediate byte we were unable to store any immediate somehow.
                if (inst.m_immediate.ByteCount() <= 0)
                    return ErrorCode_t::ErrorCode_NoImmediateFound;
            }
            

            // 7. Collect Rel8 if required.
            // 8. This instruction is complete, store it.
            vecOutput.push_back(inst);


            // 8. Clear.
            inst.Clear();
        }
    }



    // Print stored instructions
    for (ParsedInst_t& inst : vecOutput)
    {
        if (inst.m_opCode.OpByteCount() == 0)
        {
            printf("EMPTY!\n");
            continue;
        }


        printf("%s", inst.m_opCode.m_szOperatorName);

        if(inst.m_displacement.ByteCount() > 0)
        {
            printf(", DISP : ");
            for (int i = 0; i < inst.m_displacement.ByteCount(); i++)
                printf("0x%02X ", inst.m_displacement.m_iDispBytes[i]);
        }

        if (inst.m_immediate.ByteCount() > 0)
        {
            printf(", IMM : ");
            for (int i = 0; i < inst.m_immediate.ByteCount(); i++)
                printf("0x%02X ", inst.m_immediate.m_immediateByte[i]);
        }

        printf("\n");
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
    case InsaneDASM64::ErrorCode_NoOpByteFound:         return "[ Insane Disassembler AMD64 ] No OpByte after rex / legacy prefix. A OpByte was expected here.";
    case InsaneDASM64::ErrorCode_ModRMNotFound:         return "[ Insane Disassembler AMD64 ] A ModR/M byte was expected but was not found.";
    case InsaneDASM64::ErrorCode_SIDNotFound:           return "[ Insane Disassembler AMD64 ] A SID byte was expected but was not found.";
    case InsaneDASM64::ErrorCode_NoImmediateFound:      return "[ Insane Disassembler AMD64 ] An Immediate was expected, but was not found.";
    
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
Displacement_t::Displacement_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Displacement_t::Clear()
{
    m_nDispBytes = 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int Displacement_t::ByteCount() const
{
    return m_nDispBytes;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool Displacement_t::PushByte(Byte iByte)
{
    assert(m_nDispBytes < Rules::MAX_DISPLACEMENT_BYTES && "Maximum displcament bytes already stored!");
    if (m_nDispBytes >= Rules::MAX_DISPLACEMENT_BYTES)
        return false;

    m_iDispBytes[m_nDispBytes] = iByte; 
    m_nDispBytes++;
    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Immediate_t::Immediate_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Immediate_t::Clear()
{
    m_nImmediateBytes = 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int Immediate_t::ByteCount() const
{
    return m_nImmediateBytes;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool Immediate_t::PushByte(Byte iByte)
{
    assert(m_nImmediateBytes < Rules::MAX_IMMEDIATE_BYTES && "Max immediate bytes are already stored!!!");
    if (m_nImmediateBytes >= Rules::MAX_IMMEDIATE_BYTES)
        return false;

    m_immediateByte[m_nImmediateBytes] = iByte;
    m_nImmediateBytes++;

    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void ParsedInst_t::Clear()
{
    m_opCode.Clear();
    m_legacyPrefix.Clear();

    m_bHasREX    = false;
    m_iREX       = 0x00; // This is an invalid REX byte ( 0x40 to 0x4F ).
    m_iREXIndex  = -1; // This is used to check if REX preseeds the OpBytes or not. ( they must.)

    m_bHasModRM  = false;
    m_iModRM     = 0x00;

    m_bHasSID    = false;
    m_iSIB       = 0x00;

    m_displacement.Clear();
    m_immediate.Clear();
}
