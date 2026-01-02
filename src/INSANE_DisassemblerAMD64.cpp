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
#include "Math/SafeBitWiseOps.h"


// NOTE : Mind this.
using namespace INSANE_DASM64_NAMESPACE;


// Local Fn...
static inline void PrintInstBytes(const std::vector<ParsedInst_t>& vecInput)
{
    for (const ParsedInst_t& inst : vecInput)
    {
        if (inst.m_opCode.OpByteCount() == 0)
        {
            printf("EMTPY INSTRUCTION!\n");
            continue;
        }


        printf("[ %12s ]", inst.m_opCode.m_pOpCodeDesc->m_szName);

        for (int i = 0; i < Rules::MAX_LEGACY_PREFIX; i++)
        {
            if (i < inst.m_legacyPrefix.m_nPrefix)
                printf("0x%02X ", inst.m_legacyPrefix.m_legacyPrefix[i]);
            else
                printf("~~~~ ");
        }
        printf(" . ");


        if(inst.m_bHasREX == true)
        {
            printf("0x%02X ", inst.m_iREX);
        }
        else
        {
            printf("~~~~ ");
        }
        printf(" . ");


        for (int i = 0; i < Rules::MAX_OPBYTES; i++)
        {
            if(i < inst.m_opCode.OpByteCount())
            {
                printf("0x%02X ", inst.m_opCode.m_opBytes[i]);
            }
            else
            {
                printf("~~~~ ");
            }
        }
        printf(" . ");


        if (inst.m_bHasModRM == true)
        {
            printf("0x%02X ", inst.m_modrm.Get());
        }
        else
        {
            printf("~~~~ ");
        }
        printf(" . ");


        if (inst.m_bHasSIB == true)
        {
            printf("0x%02X ", inst.m_SIB.Get());
        }
        else
        {
            printf("~~~~ ");
        }
        printf(" . ");


        for (int i = 0; i < Rules::MAX_DISPLACEMENT_BYTES; i++)
        {
            if(i < inst.m_displacement.ByteCount())
            {
                printf("0x%02X ", inst.m_displacement.m_iDispBytes[i]);
            }
            else
            {
                printf("~~~~ ");
            }
        }
        printf(" . ");


        for (int i = 0; i < Rules::MAX_IMMEDIATE_BYTES; i++)
        {
            if (i < inst.m_immediate.ByteCount())
            {
                printf("0x%02X ", inst.m_immediate.m_immediateByte[i]);
            }
            else
            {
                printf("~~~~ ");
            }
        }
        printf(" . ");


        printf("\n");
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t InsaneDASM64::Initialize()
{
    // Tables...
    {
        IDASMErrorCode_t iErrorCode = G::g_tables.Initialize();
        if (iErrorCode != IDASMErrorCode_t::IDASMErrorCode_Success)
            return iErrorCode;
    }


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t DecodeAndDisassemble(const std::vector<Byte>& vecInput, std::vector<std::string>& vecOutput)
{
    vecOutput.clear();


    // Parse input...
    std::vector<ParsedInst_t> vecParsedInst; vecParsedInst.clear();

    IDASMErrorCode_t iParserErrorCode = Decode(vecInput, vecParsedInst); /* Delete this */ PrintInstBytes(vecParsedInst);
    if (iParserErrorCode != IDASMErrorCode_Success)
        return iParserErrorCode;


    // Parsing didn't fail but its empty :(
    if (vecParsedInst.empty() == true)
        return IDASMErrorCode_t::IDASMErrorCode_Success;


    // Disassemble decoded instruction.
    IDASMErrorCode_t iDASMErrorCode = Disassemble(vecParsedInst, vecOutput);
    if (iDASMErrorCode != IDASMErrorCode_t::IDASMErrorCode_Success)
        return iDASMErrorCode;


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t Disassemble(const std::vector<ParsedInst_t>& vecInput, std::vector<std::string>& vecOutput)
{
    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t InsaneDASM64::Decode(const std::vector<Byte>& vecInput, std::vector<ParsedInst_t>& vecOutput)
{
    assert(G::g_tables.IsInitialized() == true && "Tables are not initialized. Initialize tables before parsing!");

    
    vecOutput.clear();

    // Empty input?
    if (vecInput.empty() == true)
        return IDASMErrorCode_t::IDASMErrorCode_Success;


    ParsedInst_t inst;


    size_t nBytes = vecInput.size();
    for (size_t iByteIndex = 0LLU; iByteIndex < nBytes; iByteIndex++)
    {
        Byte        iByte     = vecInput[iByteIndex];
        InstTypes_t iInstType = static_cast<InstTypes_t>(G::g_tables.GetInstType(iByte));


        // Store Legacy Prefix...
        if ((iInstType & InstTypes_t::InstTypes_LegacyPrefixGrpAll) != false)
        {

            // We iterate forward ( upto Rules::MAX_LEGACY_PREFIX ( 4 ) bytes ) and collect all prefix.
            size_t iPrefixIndex = iByteIndex;
            for (iPrefixIndex = iByteIndex; iPrefixIndex < nBytes && iPrefixIndex - iByteIndex < Rules::MAX_LEGACY_PREFIX; iPrefixIndex++)
            {
                Byte iPrefixByte = vecInput[iPrefixIndex];


                // != Prefix? break.
                if ((G::g_tables.GetInstType(iPrefixByte) & InstTypes_t::InstTypes_LegacyPrefixGrpAll) == false)
                    break;


                // More than max prefix?
                if (inst.m_legacyPrefix.PrefixCount() >= Rules::MAX_LEGACY_PREFIX)
                    return IDASMErrorCode_t::IDASMErrorCode_TooManyPefix; // already stored 'MAX_LEGACY_PREFIX' no. of prefixies.


                inst.m_legacyPrefix.PushPrefix(iPrefixByte);
            }
            iByteIndex = iPrefixIndex - 1; // So we don't skip one byte.

        }
        else if ((iInstType & InstTypes_t::InstTypes_REX) != false)
        {
            for (size_t iREXIndex = iByteIndex; iREXIndex < nBytes - 1LLU; iREXIndex++)
            {
                Byte iCurByte  = vecInput[iREXIndex];
                Byte iNextByte = vecInput[iREXIndex + 1LLU];


                // Next byte != REX, and current Byte == REX. Store current & break.
                if ((G::g_tables.GetInstType(iNextByte) & InstTypes_t::InstTypes_REX) == false)
                {
                    inst.m_bHasREX   = true;
                    inst.m_iREX      = iCurByte;
                    inst.m_iREXIndex = static_cast<int32_t>(iREXIndex);
                    iByteIndex       = iREXIndex;

                    break;
                }
            }
        }
        else // != REX && != LegacyPrefix
        {
            // OpCode Immediately preceeding REX?
            if (inst.m_bHasREX == true && inst.m_iREXIndex != iByteIndex - 1llu && inst.m_opCode.OpByteCount() == 0)
                return IDASMErrorCode_t::IDASMErrorCode_REXNotPrecedingOpCode;


            // OpCodes....
            Byte iLastByte = 0x00;
            for (size_t iOpCodeIndex = iByteIndex; iOpCodeIndex < nBytes && iOpCodeIndex - iByteIndex < Rules::MAX_OPBYTES; iOpCodeIndex++)
            {

                Byte iOpCodeByte = vecInput[iOpCodeIndex];


                // NOTE : Last byte / escape byte is only used for 3 byte opcodes. so 0x00 is fine for first 2 iterations...
                int iTableIndex = static_cast<int>(iOpCodeIndex - iByteIndex + 1llu);
                OpCodeDesc_t* pOpCodeTable = G::g_tables.GetOpCodeTable(iTableIndex, iLastByte);
                if (pOpCodeTable == nullptr)
                {
                    printf("nullptr table\n");
                    return IDASMErrorCode_t::IDASMErrorCode_InvalidOpCode;
                }


                // Pull OpCode description from table.
                OpCodeDesc_t* pOpCodeDesc = &pOpCodeTable[iOpCodeByte];
                if (pOpCodeDesc == nullptr || pOpCodeDesc->m_bIsValidCode == false)
                {
                    printf("OpCode description is invalid or is nullptr.\n");
                    return IDASMErrorCode_t::IDASMErrorCode_InvalidOpCode;
                }


                inst.m_opCode.PushOpCode(iOpCodeByte);


                // Escape??
                if(pOpCodeDesc->m_bIsEscapeCode == false)
                { 
                    inst.m_opCode.StoreOperatorDesc(pOpCodeDesc);
                    break;
                }


                iLastByte = iOpCodeByte;
            }

            
            // Must store atleast 1 opcode byte.
            if (inst.m_opCode.OpByteCount() <= 0)
                return IDASMErrorCode_t::IDASMErrorCode_NoOpByteFound;


            // Incrementus iteratus.
            iByteIndex += inst.m_opCode.OpByteCount();


            // Store ModRM byte if required.
            if (inst.m_opCode.ModRMRequired(&inst.m_legacyPrefix) == true)
            {
                // Bytes left in byte stream?
                if (iByteIndex >= nBytes)
                    return IDASMErrorCode_t::IDASMErrorCode_ModRMNotFound;


                inst.m_bHasModRM = true;
                inst.m_modrm.Store(vecInput[iByteIndex]);
                iByteIndex++;
            }


            // At this point, we have modRM byte ( if any ), legacy prefix ( if any ) 
            // and root opcode description. Now we can and we must find the child opcode varient
            // that is refered in this instruction.
            inst.m_opCode.InitChildVarient(&inst.m_legacyPrefix, inst.m_modrm.Get());



            // Store SIB if required. MOD == 11 && R/M == 100
            //bool bSIBRequired = inst.m_bHasModRM == true && (inst.m_modrm & 0b11000000) != 0b11000000 && (inst.m_modrm & 0b111) == 0b100;
            Byte modrm        = inst.m_modrm.Get();
            bool bSIBRequired = inst.m_bHasModRM == true && inst.m_modrm.ModValue() != 0b11 && inst.m_modrm.RMValue() == 0b100;
            if (bSIBRequired == true)
            {
                // Bytes left in byte stream?
                if (iByteIndex + 1llu >= nBytes)
                    return IDASMErrorCode_t::IDASMErrorCode_SIBNotFound;


                inst.m_bHasSIB = true;
                inst.m_SIB.Store(vecInput[iByteIndex]);
                iByteIndex++;
            }


            // Store displacement if required.
            // calc. size of displacement here.
            int iDisplacementSize = 0;
            if (inst.m_bHasModRM == true)
            {
                uint64_t iMod = inst.m_modrm.ModValue();

                if (iMod == 0b01)
                {
                    iDisplacementSize = 1;
                }
                else if (iMod == 0b10 || (iMod == 0b00 && inst.m_modrm.RMValue() == 0b101))
                {
                    iDisplacementSize = 4;
                }
                else if (iMod == 0b00 && inst.m_SIB.BaseValue() == 0b101) // base == 101 ?
                {
                    iDisplacementSize = 4;
                }
            }
            
            if (iDisplacementSize > Rules::MAX_DISPLACEMENT_BYTES)
                return IDASMErrorCode_t::IDASMErrorCode_InvalidDispSize;

            
            for (size_t iDispIndex = iByteIndex; iDispIndex < nBytes && iDispIndex - iByteIndex < iDisplacementSize; iDispIndex++)
            {
                Byte iDispByte = vecInput[iDispIndex];

                inst.m_displacement.PushByte(iDispByte);
            }
            iByteIndex += inst.m_displacement.ByteCount();


            // Store immediate if required.
            OperandType_t iImmOperandType = OperandType_t::OperandType_Invalid;
            for (int iOperandIndex = 0; iOperandIndex < inst.m_opCode.m_pOpCodeDesc->m_nOperands; iOperandIndex++)
            {
                const Operand_t* pOperand = &inst.m_opCode.m_pOpCodeDesc->m_operands[iOperandIndex];
                
                // If we found an operand with addresing method as "Immediate" store its operand type, and 
                // break out.
                if (pOperand->m_iOperandMode == OperandMode_t::OperandMode_imm || pOperand->m_iOperandMode == OperandMode_t::OperandMode_rel)
                {
                    iImmOperandType = pOperand->m_iOperandType;
                    break;
                }
            }


            // Store immediate bytes according to operand type
            if (iImmOperandType != OperandType_t::OperandType_Invalid)
            {
                // default 4 bytes operand size in 64 bit mode.
                int iOperandSize = 4; 


                // Prefix changing operand size?
                for (int i = 0; i < inst.m_legacyPrefix.PrefixCount(); i++)
                {
                    if (inst.m_legacyPrefix.m_legacyPrefix[i] == 0x66)
                        iOperandSize = 2;
                }

                bool bREX_W = inst.m_iREX & 0b00001000;
                if (bREX_W == true) // REX.W == 1 ?
                    iOperandSize = 4;


                int iImmediateSize = 0;
                switch (iImmOperandType)
                {
                case OperandType_8:        iImmediateSize = 1;            break;
                case OperandType_16:       iImmediateSize = 2;            break;
                case OperandType_32:       iImmediateSize = 4;            break;
                case OperandType_64:       iImmediateSize = 6;            break; // TODO Why is operandtype_64 mean 6 byte immediate size ? Shouldn't it be 8 bytes ?
                case OperandType_16_32:    iImmediateSize = iOperandSize; break;
                case OperandType_16_32_64: iImmediateSize = bREX_W == true ? 8 : iOperandSize; break; // Promoted to qword by REX.W ?

                default: break;
                }


                if (iImmediateSize == 0 || iImmediateSize > Rules::MAX_IMMEDIATE_BYTES)
                    return IDASMErrorCode_t::IDASMErrorCode_InvalidImmediateSize;


                for (size_t iImmediateIndex = iByteIndex; iImmediateIndex < nBytes && iImmediateIndex - iByteIndex < iImmediateSize; iImmediateIndex++)
                {
                    inst.m_immediate.PushByte(vecInput[iImmediateIndex]);
                }
                iByteIndex += inst.m_immediate.ByteCount();


                // Check if we collected all immediate bytes.
                if (inst.m_immediate.ByteCount() != iImmediateSize)
                    return IDASMErrorCode_t::IDASMErrorCode_InvalidImmediateSize;
            }


            // So we don't skip one byte when we go to next iteration.
            assert(iByteIndex > 0llu && "Byte index can't be 0 after storing a entire instruction. Alteast one byte must be stored!");
            iByteIndex--;


            vecOutput.push_back(inst);
            inst.Clear();
        }
    }


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
const char* InsaneDASM64::GetErrorMessage(IDASMErrorCode_t iErrorCode)
{
    switch (iErrorCode)
    {
    case InsaneDASM64::IDASMErrorCode_Success:               return "[ Insane Disassembler AMD64 ] No Error";
    case InsaneDASM64::IDASMErrorCode_FailedInit:            return "[ Insane Disassembler AMD64 ] Failed initialization!";
    case InsaneDASM64::IDASMErrorCode_TooManyPefix:          return "[ Insane Disassembler AMD64 ] Failed parsing!, more than MAX_PREFIX amount of prefix encountered consecutively";
    case InsaneDASM64::IDASMErrorCode_REXNotPrecedingOpCode: return "[ Insane Disassembler AMD64 ] REX exists in an instruction but doesn't immediately preced OpCode bytes.";
    case InsaneDASM64::IDASMErrorCode_InvalidOpCode:         return "[ Insane Disassembler AMD64 ] Invalid OpCode appeared in input.";
    case InsaneDASM64::IDASMErrorCode_NoOpByteFound:         return "[ Insane Disassembler AMD64 ] No OpByte after rex / legacy prefix. A OpByte was expected here.";
    case InsaneDASM64::IDASMErrorCode_ModRMNotFound:         return "[ Insane Disassembler AMD64 ] A ModR/M byte was expected but was not found.";
    case InsaneDASM64::IDASMErrorCode_SIBNotFound:           return "[ Insane Disassembler AMD64 ] A SID byte was expected but was not found.";
    case InsaneDASM64::IDASMErrorCode_InvalidDispSize:       return "[ Insane Disassembler AMD64 ] Determined displacement size is invalid.";
    case InsaneDASM64::IDASMErrorCode_NoImmediateFound:      return "[ Insane Disassembler AMD64 ] An Immediate was expected, but was not found.";
    case InsaneDASM64::IDASMErrorCode_InvalidImmediateSize:  return "[ Insane Disassembler AMD64 ] Failed to determine immediate size for some instruction.";
    
    default: break;
    }

    static char s_invalidCodeBuffer[128] = "";
    sprintf_s(s_invalidCodeBuffer, "[ Insane Disassembler AMD64 ] Invalid Error Code { %d }", iErrorCode);
    return s_invalidCodeBuffer;
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
    m_nOpBytes        = 0;
    m_pRootOpCodeDesc = nullptr;
    m_pOpCodeDesc     = nullptr;
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
    assert(m_pOpCodeDesc != nullptr && "No OpCode description is stored for this OpCode_t");
    return m_pOpCodeDesc == nullptr ? -1 : m_pOpCodeDesc->m_nOperands;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Byte OpCode_t::GetMostSignificantOpCode() const
{
    assert(m_nOpBytes > 0 && m_nOpBytes <= Rules::MAX_OPBYTES && "OpCode count is invalid!");
    if (m_nOpBytes <= 0 || m_nOpBytes > Rules::MAX_OPBYTES)
        return 0x00;

    return m_opBytes[m_nOpBytes - 1];
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
void OpCode_t::StoreOperatorDesc(OpCodeDesc_t* pOperatorInfo)
{
    // Storing root & final opcode description.
    m_pRootOpCodeDesc = pOperatorInfo;
    m_pOpCodeDesc     = nullptr; // To determine final child opcodeDesc, we will need modrm byte. which we might not have.
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool OpCode_t::ModRMRequired(const LegacyPrefix_t* pPrefix) const
{
    assert(m_pRootOpCodeDesc != nullptr && "No root OpCode description is stored for this OpCode_t");
    
    if (m_pRootOpCodeDesc == nullptr)
        return false;


    // In case split type is any of these. We will need modRM 100%
    if (m_pRootOpCodeDesc->m_iVarientType == OpCodeDesc_t::VarientKey_ModRM_MOD ||
        m_pRootOpCodeDesc->m_iVarientType == OpCodeDesc_t::VarientKey_ModRM_REG ||
        m_pRootOpCodeDesc->m_iVarientType == OpCodeDesc_t::VarientKey_ModRM_RM)
    {
        return true;
    }


    // In case we have a legacy prefix split, we need to check if we need a
    // modrm or not, according to child OpCodeDesc.
    if (m_pRootOpCodeDesc->m_iVarientType == OpCodeDesc_t::VarientKey_LegacyPrefix)
    {
        // Every legacy prefix split has default entry ( no prefix ) @ index 0.
        OpCodeDesc_t* pBestDesc = m_pRootOpCodeDesc->m_pVarients[0];
        assert(pBestDesc != nullptr && "Some opcode entry with legacy prefix split has default entry ( index 0 ) as default.");


        for (int i = 0; i < pPrefix->m_nPrefix; i++)
        {
            int iPrefixIndex = G::g_tables.GetLegacyPrefixIndex(pPrefix->m_legacyPrefix[i]);
            
            if (m_pRootOpCodeDesc->m_pVarients[iPrefixIndex] != nullptr)
            {
                pBestDesc = m_pRootOpCodeDesc->m_pVarients[iPrefixIndex];
                break;
            }
        }

        return pBestDesc->m_bModrmRequired;
    }


    // If we reach here, it means there is no split, so we use root.
    return m_pRootOpCodeDesc->m_bModrmRequired;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static OpCodeDesc_t* FindVarientRecurse(const LegacyPrefix_t* pPrefix, Byte iModRM, OpCodeDesc_t* pRootOpCodeDesc)
{
    if (pRootOpCodeDesc->m_iVarientType == OpCodeDesc_t::VarientKey_None)
        return pRootOpCodeDesc;

    assert(pRootOpCodeDesc->m_pVarients != nullptr && "Varient array is nullptr, when expected a varient array!.");


    switch (pRootOpCodeDesc->m_iVarientType)
    {
    case OpCodeDesc_t::VarientKey_ModRM_MOD:
        return FindVarientRecurse(pPrefix, iModRM, pRootOpCodeDesc->m_pVarients[(iModRM >> 6) & 0b11]);

    case OpCodeDesc_t::VarientKey_ModRM_REG:
        return FindVarientRecurse(pPrefix, iModRM, pRootOpCodeDesc->m_pVarients[(iModRM >> 3) & 0b111]);

    case OpCodeDesc_t::VarientKey_ModRM_RM:
        return FindVarientRecurse(pPrefix, iModRM, pRootOpCodeDesc->m_pVarients[iModRM & 0b111]);

    case OpCodeDesc_t::VarientKey_LegacyPrefix:
    {
        OpCodeDesc_t* pOpCodeDesc = pRootOpCodeDesc->m_pVarients[0];

        for (int i = 0; i < pPrefix->PrefixCount(); i++)
        {
            Byte          iPrefix            = pPrefix->m_legacyPrefix[i];
            int           iPrefixIndex       = G::g_tables.GetLegacyPrefixIndex(iPrefix);
            OpCodeDesc_t* pPrefixOpCodeDesc  = pRootOpCodeDesc->m_pVarients[iPrefixIndex];
                
            // Check if we have any prefix such that there exists a opcode varient for that
            // prefix. Else we can always use the default entry.
            if (pPrefixOpCodeDesc != nullptr)
            {
                pOpCodeDesc = pPrefixOpCodeDesc;
                break;
            }
        }

        return FindVarientRecurse(pPrefix, iModRM, pOpCodeDesc);

        break;
    }
    default: break;
    }


    assert(false && "Invalid opcode varient set!");
    return nullptr;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool OpCode_t::InitChildVarient(const LegacyPrefix_t* pPrefix, Byte iModRM)
{
    assert(m_pRootOpCodeDesc != nullptr && "We need a root opcode description pointer to init child varient!");
    if (m_pRootOpCodeDesc == nullptr)
        return false;


    m_pOpCodeDesc = FindVarientRecurse(pPrefix, iModRM, m_pRootOpCodeDesc);
    return m_pOpCodeDesc != nullptr;
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

    m_bHasSIB    = false;

    m_displacement.Clear();
    m_immediate.Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Operand_t::Operand_t(int iLiteral)
{
    m_iOperandCatagory = OperandCatagory_Literal;
    m_iOperandLiteral  = iLiteral;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Operand_t::Operand_t(Register_t iRegister)
{
    m_iOperandCatagory = OperandCatagory_Register;
    m_iOperandRegister = iRegister;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Operand_t::Operand_t(OperandMode_t iOperandMode, OperandType_t iOperandType)
{
    m_iOperandCatagory = OperandCatagory_Legacy;
    m_iOperandMode     = iOperandMode;
    m_iOperandType     = iOperandType;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Operand_t::Reset()
{
    m_iOperandCatagory = OperandCatagory_Undefined;
    m_iOperandRegister = Register_t(Register_t::RegisterClass_Invalid, -1, -1);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t ModRM_t::ModValue() const
{
    return Maths::SafeAnd(m_modrm, Masks::MODRM_MOD) >> 6;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t ModRM_t::RegValue() const
{
    return Maths::SafeAnd(m_modrm, Masks::MODRM_REG) >> 3;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t ModRM_t::RMValue() const
{
    return Maths::SafeAnd(m_modrm, Masks::MODRM_RM);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t SIB_t::ScaleValue() const
{
    return Maths::SafeAnd(m_SIB, Masks::SIB_SCALE) >> 6;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t SIB_t::IndexValue() const
{
    return Maths::SafeAnd(m_SIB, Masks::SIB_INDEX) >> 3;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t SIB_t::BaseValue() const
{
    return Maths::SafeAnd(m_SIB, Masks::SIB_BASE);
}
