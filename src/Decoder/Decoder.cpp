//=========================================================================
//                      Decoder
//=========================================================================
// by      : INSANE
// created : 19/01/2026
//
// purpose : Handles decoding for all instruction encoding types.
//-------------------------------------------------------------------------
#include "Decoder.h"
#include <assert.h>

#include "../../Include/Standard/OpCodeDesc_t.h"
#include "../../Include/Legacy/LegacyInst_t.h"
#include "../../Include/VEX/VEXPrefix_t.h"
#include "../../Include/VEX/VEXInst_t.h"
#include "../../Include/EVEX/EVEXInst_t.h"
#include "../../Include/EVEX/EVEXPrefix_t.h"

// Util
#include "../Tables/Tables.h"
#include "../Util/Terminal/Terminal.h"
#include "../Math/SafeBitWiseOps.h"



using namespace InsaneDASM64;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t INSANE_DASM64_NAMESPACE::DecodeLegacyEncoding(const std::vector<Byte>& vecInput, Instruction_t* pInstOut, size_t& iIterator)
{
    assert(G::g_tables.IsInitialized() == true && "Tables are not initialized. Initialize tables before parsing!");
    assert(pInstOut->m_iInstEncodingType == Instruction_t::InstEncodingType_Legacy && "Instruction passed to DecodeVEXEncoding() is not initialized to correct type.");


    // Valid Iterator??
    assert(iIterator >= 0llu && iIterator < vecInput.size() && "Out of bound interator.");

    
    // Empty input?
    if (vecInput.empty() == true)
        return IDASMErrorCode_t::IDASMErrorCode_Success;


    Legacy::LegacyInst_t* pInst = reinterpret_cast<Legacy::LegacyInst_t*>(pInstOut->m_pInst);
    pInst->Clear();


    size_t nBytes = vecInput.size();
    for (size_t iByteIndex = iIterator; iByteIndex < nBytes; iByteIndex++)
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
                if (pInst->m_legacyPrefix.PrefixCount() >= Rules::MAX_LEGACY_PREFIX)
                    return IDASMErrorCode_t::IDASMErrorCode_TooManyPefix; // already stored 'MAX_LEGACY_PREFIX' no. of prefixies.


                pInst->m_legacyPrefix.PushPrefix(iPrefixByte);
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
                    pInst->m_bHasREX   = true;
                    pInst->m_iREX      = iCurByte;
                    pInst->m_iREXIndex = static_cast<int32_t>(iREXIndex);
                    iByteIndex       = iREXIndex;

                    break;
                }
            }
        }
        else // != REX && != LegacyPrefix
        {
            // OpCode Immediately preceeding REX?
            if (pInst->m_bHasREX == true && pInst->m_iREXIndex != iByteIndex - 1llu && pInst->m_opCode.OpByteCount() == 0)
                return IDASMErrorCode_t::IDASMErrorCode_REXNotPrecedingOpCode;


            // OpCodes....
            Byte iLastByte = 0x00;
            for (size_t iOpCodeIndex = iByteIndex; iOpCodeIndex < nBytes && iOpCodeIndex - iByteIndex < Rules::MAX_OPBYTES; iOpCodeIndex++)
            {

                Byte iOpCodeByte = vecInput[iOpCodeIndex];


                // NOTE : Last byte / escape byte is only used for 3 byte opcodes. so 0x00 is fine for first 2 iterations...
                int iTableIndex = static_cast<int>(iOpCodeIndex - iByteIndex + 1llu);
                Standard::OpCodeDesc_t* pOpCodeTable = G::g_tables.GetOpCodeTable(iTableIndex, iLastByte);
                if (pOpCodeTable == nullptr)
                {
                    FAIL_LOG("nullptr table\n");
                    return IDASMErrorCode_t::IDASMErrorCode_InvalidOpCode;
                }


                // Pull OpCode description from table.
                Standard::OpCodeDesc_t* pOpCodeDesc = &pOpCodeTable[iOpCodeByte];
                if (pOpCodeDesc == nullptr || pOpCodeDesc->m_bIsValidCode == false)
                {
                    FAIL_LOG("OpCode description is invalid or is nullptr.\n");
                    return IDASMErrorCode_t::IDASMErrorCode_InvalidOpCode;
                }


                pInst->m_opCode.PushOpCode(iOpCodeByte);


                // Escape??
                if(pOpCodeDesc->m_bIsEscapeCode == false)
                { 
                    pInst->m_opCode.StoreOperatorDesc(pOpCodeDesc);
                    break;
                }


                iLastByte = iOpCodeByte;
            }

            
            // Must store atleast 1 opcode byte.
            if (pInst->m_opCode.OpByteCount() <= 0)
                return IDASMErrorCode_t::IDASMErrorCode_NoOpByteFound;


            // Incrementus iteratus.
            iByteIndex += pInst->m_opCode.OpByteCount();


            // Store ModRM byte if required.
            if (pInst->m_opCode.ModRMRequired(&pInst->m_legacyPrefix) == true)
            {
                // Bytes left in byte stream?
                if (iByteIndex >= nBytes)
                    return IDASMErrorCode_t::IDASMErrorCode_ModRMNotFound;


                pInst->m_bHasModRM = true;
                pInst->m_modrm.Store(vecInput[iByteIndex]);
                iByteIndex++;
            }


            // At this point, we have modRM byte ( if any ), legacy prefix ( if any ) 
            // and root opcode description. Now we can and we must find the child opcode varient
            // that is refered in this instruction.
            pInst->m_opCode.InitChildVarient(&pInst->m_legacyPrefix, pInst->m_modrm.Get());
            if(pInst->m_opCode.m_pOpCodeDesc == nullptr)
                return IDASMErrorCode_t::IDASMErrorCode_InvalidOpCode;


            // Store SIB if required. MOD == 11 && R/M == 100
            //bool bSIBRequired = inst->m_bHasModRM == true && (inst->m_modrm & 0b11000000) != 0b11000000 && (inst->m_modrm & 0b111) == 0b100;
            Byte modrm        = pInst->m_modrm.Get();
            bool bSIBRequired = pInst->m_bHasModRM == true && pInst->m_modrm.ModValueAbs() != 0b11 && pInst->m_modrm.RMValueAbs() == 0b100;
            if (bSIBRequired == true)
            {
                // Bytes left in byte stream?
                if (iByteIndex + 1llu >= nBytes)
                    return IDASMErrorCode_t::IDASMErrorCode_SIBNotFound;


                pInst->m_bHasSIB = true;
                pInst->m_SIB.Store(vecInput[iByteIndex]);
                iByteIndex++;
            }


            // Store displacement if required.
            // calc. size of displacement here.
            int iDisplacementSize = 0;
            if (pInst->m_bHasModRM == true)
            {
                uint64_t iMod = pInst->m_modrm.ModValueAbs();

                if (iMod == 0b01)
                {
                    iDisplacementSize = 1;
                }
                else if (iMod == 0b10 || (iMod == 0b00 && pInst->m_modrm.RMValueAbs() == 0b101))
                {
                    iDisplacementSize = 4;
                }
                else if (iMod == 0b00 && pInst->m_SIB.BaseValueAbs() == 0b101) // base == 101 ?
                {
                    iDisplacementSize = 4;
                }
            }
            
            if (iDisplacementSize > Rules::MAX_DISPLACEMENT_BYTES)
                return IDASMErrorCode_t::IDASMErrorCode_InvalidDispSize;

            
            for (size_t iDispIndex = iByteIndex; iDispIndex < nBytes && iDispIndex - iByteIndex < iDisplacementSize; iDispIndex++)
            {
                Byte iDispByte = vecInput[iDispIndex];

                pInst->m_displacement.PushByte(iDispByte);
            }
            iByteIndex += pInst->m_displacement.ByteCount();


            // Store immediate if required.
            Standard::CEOperandTypes_t iImmOperandType = Standard::CEOperandTypes_t::CEOperandType_Invalid;
            Standard::OperandModes_t   iImmOperandMode = Standard::OperandModes_t::OperandMode_Invalid;
            for (int iOperandIndex = 0; iOperandIndex < pInst->m_opCode.m_pOpCodeDesc->m_nOperands; iOperandIndex++)
            {
                const Standard::Operand_t* pOperand = &pInst->m_opCode.m_pOpCodeDesc->m_operands[iOperandIndex];

                // If we found an operand with addresing method that requires immediate, store its operand type, and 
                // break out.
                Standard::OperandModes_t iOperandMode = pOperand->m_iOperandMode;
                if (iOperandMode == Standard::OperandModes_t::OperandMode_I || 
                    iOperandMode == Standard::OperandModes_t::OperandMode_J ||
                    iOperandMode == Standard::OperandModes_t::OperandMode_O ||
                    iOperandMode == Standard::OperandModes_t::OperandMode_A)
                {
                    iImmOperandMode = iOperandMode;
                    iImmOperandType = GeekToCoderOperandType(pOperand->m_iOperandType);
                    break;
                }
            }


            // Store immediate bytes according to operand type
            if (iImmOperandType != Standard::CEOperandTypes_t::CEOperandType_Invalid)
            {
                int iImmediateSize = 0;
                if (iImmOperandMode == Standard::OperandMode_O)
                {
                    iImmediateSize = pInst->GetAddressSizeInBytes();
                }
                else
                {
                    switch (iImmOperandType)
                    {
                    case Standard::CEOperandType_8:        iImmediateSize = 1;            break;
                    case Standard::CEOperandType_16:       iImmediateSize = 2;            break;
                    case Standard::CEOperandType_32:       iImmediateSize = 4;            break;
                    case Standard::CEOperandType_64:       iImmediateSize = 8;            break;
                    case Standard::CEOperandType_16_32:    iImmediateSize = pInst->GetOperandSizeInBytes(true);  break;
                    case Standard::CEOperandType_16_32_64: iImmediateSize = pInst->GetOperandSizeInBytes(false); break; // Promoted to qword by REX.W ?

                    default: break;
                    }
                }


                if (iImmediateSize == 0 || iImmediateSize > Rules::MAX_IMMEDIATE_BYTES)
                    return IDASMErrorCode_t::IDASMErrorCode_InvalidImmediateSize;


                for (size_t iImmediateIndex = iByteIndex; iImmediateIndex < nBytes && iImmediateIndex - iByteIndex < iImmediateSize; iImmediateIndex++)
                {
                    pInst->m_immediate.PushByte(vecInput[iImmediateIndex]);
                }
                iByteIndex += pInst->m_immediate.ByteCount();


                // Check if we collected all immediate bytes.
                if (pInst->m_immediate.ByteCount() != iImmediateSize)
                    return IDASMErrorCode_t::IDASMErrorCode_InvalidImmediateSize;
            }


            // So we don't skip one byte when we go to next iteration.
            assert(iByteIndex > 0llu && "Byte index can't be 0 after storing a entire instruction. Alteast one byte must be stored!");
            iByteIndex--;

            iIterator = iByteIndex;
            return IDASMErrorCode_t::IDASMErrorCode_Success;
        }
    }


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t INSANE_DASM64_NAMESPACE::DecodeVEXEncoding(const std::vector<Byte>& vecInput, Instruction_t* pInstOut, size_t& iIterator)
{
    assert(G::g_tables.IsInitialized() == true && "Tables are not initialized. Initialize tables before parsing!");
    assert(pInstOut->m_iInstEncodingType == Instruction_t::InstEncodingType_VEX && "Instruction passed to DecodeVEXEncoding() is not initialized to correct type.");


    // We must have atleast 4 bytes left in input. Thats the minimum for a VEX encoded instruction.
    if(vecInput.empty() == true || vecInput.size() - iIterator < Rules::MIN_VEX_INST_BYTES)
    {
        FAIL_LOG("Not bytes left in byte stream");
        return IDASMErrorCode_InvalidVEXInst;
    }


    size_t          nBytes = vecInput.size();
    VEX::VEXInst_t* pInst  = reinterpret_cast<VEX::VEXInst_t*>(pInstOut->m_pInst);
    pInst->Clear();

    pInst->m_vexPrefix.StorePrefix(vecInput[iIterator]); iIterator++;

    // Store one byte VEX regardless.
    pInst->m_vexPrefix.PushVEXBytes(vecInput[iIterator]); iIterator++;

    // Store one more VEX byte if required.
    if(pInst->m_vexPrefix.GetPrefix() == SpecialChars::VEX_PREFIX_C4)
    {
        pInst->m_vexPrefix.PushVEXBytes(vecInput[iIterator]); 
        iIterator++;
    }


    // We must have more bytes, if not, raise error.
    if(iIterator >= nBytes)
    {
        FAIL_LOG("No OpCode present");
        return IDASMErrorCode_t::IDASMErrorCode_NoOpByteFound;
    }


    // Capture opcode.
    pInst->m_opcode.PushOpCode(vecInput[iIterator]);

    // Checking opcode validity.
    {
        Byte iPrefix = 0x0F;
        if(pInst->m_vexPrefix.GetPrefix() == SpecialChars::VEX_PREFIX_C4)
        {
            // int m_mmmm = pInst->m_vex[0] & 0b11111;
            uint64_t m_mmmm = pInst->m_vexPrefix.m_mmmm();
            if(m_mmmm == 0 || m_mmmm > 3) return IDASMErrorCode_InvalidVEXInst;

            static Byte s_iEscapeArr[] = { 0x0F, 0x38, 0x3A };
            iPrefix = s_iEscapeArr[m_mmmm - 1];
        }
        Standard::OpCodeDesc_t* opCodeTable = G::g_tables.GetVEXOpCodeTable(iPrefix);
        assert(opCodeTable != nullptr && "Nullptr table received.");

        // OpCode doesn't repesent a valid VEX encodable instruction.
        Standard::OpCodeDesc_t* pOpCodeDesc = &opCodeTable[pInst->m_opcode.GetMostSignificantOpCode()];
        if(pOpCodeDesc->m_bIsValidCode == false)
        {
            FAIL_LOG("OpCode [ 0x%02X 0x%02X ] is marked as invalid in opcode map", iPrefix, pInst->m_opcode.GetMostSignificantOpCode());
            return IDASMErrorCode_t::IDASMErrorCode_InvalidVEXInst;
        }


        // Since the OpCode description pointer is valid, we can store it.
        pInst->m_opcode.StoreOperatorDesc(pOpCodeDesc);
    }


    // Determining prefix using VEX.pp
    Byte iLegacyPrefix = 0x00;
    {
        static Byte s_prefixMap[] = { 0x00, 0x66, 0xF3, 0xF2 };
        iLegacyPrefix = s_prefixMap[pInst->m_vexPrefix.pp()];
    }

    // In case we have a prefix split, we can try to detrmine final varient without modrm byte.
    // so we know if we need to get the modrm byte or not.
    // so we can use the modrm to get the final varient?
    if(pInst->m_opcode.m_pRootOpCodeDesc->m_iVarientType == Standard::OpCodeDesc_t::VarientKey_LegacyPrefix)
        pInst->m_opcode.InitChildVarient(0x00, 1, &iLegacyPrefix);


    // Capture modrm, if we got any operands.
    if(pInst->m_opcode.m_pOpCodeDesc->m_nOperands > 0)
    {
        iIterator++;
        if(iIterator >= nBytes)
            return IDASMErrorCode_t::IDASMErrorCode_ModRMNotFound;

        pInst->m_modrm.Store(vecInput[iIterator]); 
    }

    
    // Using modrm and perfix, determine final varient.
    pInst->m_opcode.InitChildVarient(pInst->m_modrm.Get(), 1, &iLegacyPrefix);
    if(pInst->m_opcode.m_pOpCodeDesc == nullptr)
        return IDASMErrorCode_t::IDASMErrorCode_InvalidOpCode;


    // We need SIB??
    pInst->m_bHasSIB = pInst->m_modrm.ModValueAbs() != 0b11 && pInst->m_modrm.RMValueAbs() == 0b100;
    if(pInst->m_bHasSIB == true)
    {
        iIterator++;
        if(iIterator >= nBytes)
            return IDASMErrorCode_t::IDASMErrorCode_SIBNotFound;

        pInst->m_SIB.Store(vecInput[iIterator]);
    }


    // Store displacement if required.
    // calc. size of displacement here.
    int      iDisplacementSize = 0;
    uint64_t iMod              = pInst->m_modrm.ModValueAbs();

    if (iMod == 0b01)
    {
        iDisplacementSize = 1;
    }
    else if (iMod == 0b10 || (iMod == 0b00 && pInst->m_modrm.RMValueAbs() == 0b101))
    {
        iDisplacementSize = 4;
    }
    else if (iMod == 0b00 && pInst->m_SIB.BaseValueAbs() == 0b101) // base == 101 ?
    {
        iDisplacementSize = 4;
    }

    // Invalid quantity of Displacement bytes.
    if (iDisplacementSize > Rules::MAX_DISPLACEMENT_BYTES)
        return IDASMErrorCode_t::IDASMErrorCode_InvalidDispSize;

    // We got enough bytes for displacement in byte stream.
    if(iIterator + iDisplacementSize >= nBytes)
        return IDASMErrorCode_t::IDASMErrorCode_DisplacementNotFound;

    // Capture Displacement bytes.
    for (size_t iDispIndex = iIterator + 1; iDispIndex < nBytes && iDispIndex - (iIterator + 1llu) < iDisplacementSize; iDispIndex++)
    {
        pInst->m_disp.PushByte(vecInput[iDispIndex]);
    }
    iIterator += pInst->m_disp.ByteCount();


    // Does this intruction need immediate.
    Standard::OpCodeDesc_t* pOpCodeDesc = pInst->m_opcode.m_pOpCodeDesc;
    assert(pOpCodeDesc != nullptr && "Invalid final opcode description");
    for(int iOperandIndex = 0; iOperandIndex < pOpCodeDesc->m_nOperands; iOperandIndex++)
    {
        Standard::Operand_t& operand = pOpCodeDesc->m_operands[iOperandIndex];
        if(operand.m_iOperandCatagory == Standard::Operand_t::OperandCatagory_Legacy)
        {
                Standard::OperandModes_t iOperandMode = operand.m_iOperandMode;
                if (iOperandMode == Standard::OperandModes_t::OperandMode_I || 
                    iOperandMode == Standard::OperandModes_t::OperandMode_J ||
                    iOperandMode == Standard::OperandModes_t::OperandMode_O ||
                    iOperandMode == Standard::OperandModes_t::OperandMode_A || 
                    iOperandMode == Standard::OperandModes_t::OperandMode_IXY)
                {
                    iIterator++;
                    pInst->m_immediate.PushByte(vecInput[iIterator]);
                    break;
                }
        }
    }


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t InsaneDASM64::DecodeEVEXEncoding(const std::vector<Byte>& vecInput, Instruction_t* pInstOut, size_t& iIterator)
{
    /*
       README :
    
       I have proper and distinct EVEX instruction table. But there are some VEX instructions that can also be encoded as EVEX.
       But not all VEX instructions are EVEX encodable. There are no signs of what instructions are EVEX encodable in 
       the linux kernel's x86 opcode map. Now my table is some what incomplete.

       I can assume that all VEX instructions are EVEX encodable, but that would break while @ error checking for EVEX 
       instructions.

       Due to nature of how the intel's software developer's manual for IA-32 is written. I think assuming all VEX instructions 
       as valid EVEX is somewhat of a valid options? I will still be able to error check against legacy only instructions.

    */


    assert(G::g_tables.IsInitialized() == true && "Tables are not initialized. Initialize tables before parsing!");
    assert(pInstOut->m_iInstEncodingType == Instruction_t::InstEncodingType_EVEX && "Instruction passed to DecodeEVEXEncoding() is not initialized to correct type.");


    // We must have atleast 4 bytes left in input. Thats the minimum for a VEX encoded instruction.
    if(vecInput.empty() == true || vecInput.size() - iIterator < Rules::MIN_EVEX_INST_BYTES)
    {
        FAIL_LOG("Not bytes left in byte stream");
        return IDASMErrorCode_InvalidEVEXInst;
    }


    // Invalid ?
    if(vecInput[iIterator] != SpecialChars::EVEX_PREFIX_62)
        return IDASMErrorCode_InvalidEVEXInst;


    size_t            nBytes = vecInput.size();
    EVEX::EVEXInst_t* pInst  = reinterpret_cast<EVEX::EVEXInst_t*>(pInstOut->m_pInst);
    pInst->Clear();


    // Store prefix.
    pInst->m_evexPrefix.StorePrefix(vecInput[iIterator]); iIterator++;


    // Store payloads.
    pInst->m_evexPrefix.StorePayload(vecInput[iIterator], 0); iIterator++;
    pInst->m_evexPrefix.StorePayload(vecInput[iIterator], 1); iIterator++;
    pInst->m_evexPrefix.StorePayload(vecInput[iIterator], 2); iIterator++;


    // Store OpCode.
    Byte iOpCodeByte = vecInput[iIterator]; iIterator++;
    pInst->m_opcode.PushOpCode(iOpCodeByte);


    // Store ModRM...
    pInst->m_modrm.Store(vecInput[iIterator]);


    // Finding escape opcode byte.
    Byte iEscapeByte = Maths::SafeAnd(pInst->m_evexPrefix.mmm(), 0b11);

    bool bValidEscapeByte = (iEscapeByte >= 1 && iEscapeByte <= 3); 
    assert(bValidEscapeByte == true && "Invalid Implied Escape OpCode Byte.");
    if(bValidEscapeByte == false)
        return InsaneDASM64::IDASMErrorCode_InvalidEVEXEscapeOpcdByte;


    // Getting OpCode Table(s).
    static Byte s_iEscapeOpcdTable[] = { 0x0F, 0x38, 0x3A };
    Standard::OpCodeDesc_t* pOpcdTable    = G::g_tables.GetEVEXOpCodeTable(s_iEscapeOpcdTable[iEscapeByte - 1]);
    Standard::OpCodeDesc_t* pOpcdTableAlt = G::g_tables.GetVEXOpCodeTable (s_iEscapeOpcdTable[iEscapeByte - 1]);


    assert(pOpcdTable != nullptr && pOpcdTableAlt != nullptr && "Failed to get OpCode table.");
    if(pOpcdTable == nullptr || pOpcdTableAlt == nullptr) 
        return IDASMErrorCode_t::IDASMErrorCode_FaildToGetOpcdTable;


    // Store OpCode description.
    Standard::OpCodeDesc_t* pOpCodeDesc    = &pOpcdTable   [iOpCodeByte];
    Standard::OpCodeDesc_t* pOpCodeDescAlt = &pOpcdTableAlt[iOpCodeByte];
    if(pOpCodeDesc->m_bIsValidCode == false && pOpCodeDescAlt->m_bIsValidCode == false)
    {
        // Both of the tables have marked this instruction as invalid, hence this must be invalid AF.
        FAIL_LOG("Invalid varient for byte 0x%02x 0x%02X", s_iEscapeOpcdTable[iEscapeByte - 1], iOpCodeByte);
        return IDASMErrorCode_t::IDASMErrorCode_InvalidOpCode;
    }


    // Attempting to initialize opcode description using the first table.
    bool bFinalVarientFound = false;
    static Byte s_iLegacyPrefixTable[] = { 0x00, 0x66, 0xF3, 0xF2 };
    if(pOpCodeDesc->m_bIsValidCode == true)
    {
        pInst->m_opcode.StoreOperatorDesc(pOpCodeDesc);

        // Using modrm and perfix, determine final varient.
        Byte iLegacyPrefix = s_iLegacyPrefixTable[pInst->m_evexPrefix.pp()];

        bFinalVarientFound = pInst->m_opcode.InitChildVarient(pInst->m_modrm.Get(), 1, &iLegacyPrefix);
    }

    // If this instruction is not valid according to EVEX only talbe OR
    // we failed to find the final varient using the Opcd Description from EVEX table.
    // Try to initialize the final varient using VEX table.
    if(bFinalVarientFound == false && pOpCodeDescAlt->m_bIsValidCode == true)
    {
        pInst->m_opcode.StoreOperatorDesc(pOpCodeDescAlt);
        pInst->m_opcode.m_pOpCodeDesc = nullptr; // Make sure final varient is nulled out.

        // Using modrm and perfix, determine final varient.
        Byte iLegacyPrefix = s_iLegacyPrefixTable[pInst->m_evexPrefix.pp()];

        bFinalVarientFound = pInst->m_opcode.InitChildVarient(pInst->m_modrm.Get(), 1, &iLegacyPrefix);
    }


    // we failed to find final varient from both tables?
    if(pInst->m_opcode.m_pOpCodeDesc == nullptr || pInst->m_opcode.m_pRootOpCodeDesc == nullptr)
    {
        FAIL_LOG("Final varient for opcd 0x%02x 0x%02x couldn't be determined. Root : [ %p ], Final varient : [ %p ]", 
                s_iEscapeOpcdTable[iEscapeByte - 1], iOpCodeByte, pInst->m_opcode.m_pRootOpCodeDesc, pInst->m_opcode.m_pOpCodeDesc);

        return IDASMErrorCode_t::IDASMErrorCode_InvalidOpCode;
    }


    // store SIB...
    pInst->m_bHasSIB = pInst->m_modrm.ModValueAbs() != 0b11 && pInst->m_modrm.RMValueAbs() == 0b100;
    if(pInst->m_bHasSIB == true)
    {
        if(iIterator >= nBytes)
            return IDASMErrorCode_t::IDASMErrorCode_SIBNotFound;

        iIterator++;
        pInst->m_SIB.Store(vecInput[iIterator]);
    }


    // Final varient valid ?
    if(pInst->m_opcode.m_pOpCodeDesc->m_bIsValidCode == false)
    {
        FAIL_LOG("Invalid final varient for byte 0x%02X", iOpCodeByte);
        return IDASMErrorCode_t::IDASMErrorCode_InvalidOpCode;
    }


    // Store displacement if required.
    // calc. size of displacement here.
    int      iDisplacementSize = 0;
    uint64_t iMod              = pInst->m_modrm.ModValueAbs();

    if (iMod == 0b01)
    {
        iDisplacementSize = 1;
    }
    else if (iMod == 0b10 || (iMod == 0b00 && pInst->m_modrm.RMValueAbs() == 0b101))
    {
        iDisplacementSize = 4;
    }
    else if (iMod == 0b00 && pInst->m_SIB.BaseValueAbs() == 0b101) // base == 101 ?
    {
        iDisplacementSize = 4;
    }

    // Invalid quantity of Displacement bytes.
    if (iDisplacementSize > Rules::MAX_DISPLACEMENT_BYTES)
        return IDASMErrorCode_t::IDASMErrorCode_InvalidDispSize;


    // Got enough bytes for displacement in byte stream.
    if(iIterator >= nBytes)
        return IDASMErrorCode_t::IDASMErrorCode_DisplacementNotFound;

    if(nBytes - (iIterator + 1) < iDisplacementSize)
        return IDASMErrorCode_t::IDASMErrorCode_DisplacementNotFound;


    // Capture Displacement bytes.
    for (size_t iDispIndex = iIterator + 1; iDispIndex < nBytes && iDispIndex - (iIterator + 1) < iDisplacementSize; iDispIndex++)
    {
        pInst->m_disp.PushByte(vecInput[iDispIndex]);
    }
    iIterator += pInst->m_disp.ByteCount();


    // Store Immediate byte if required.
    Standard::OpCodeDesc_t* pFinalOpCodeDesc = pInst->m_opcode.m_pOpCodeDesc;
    for(int iOperandIndex = 0; iOperandIndex < pFinalOpCodeDesc->m_nOperands; iOperandIndex++)
    {
        Standard::Operand_t& operand = pFinalOpCodeDesc->m_operands[iOperandIndex];
        if(operand.m_iOperandCatagory == Standard::Operand_t::OperandCatagory_Legacy)
        {
                Standard::OperandModes_t iOperandMode = operand.m_iOperandMode;
                if (iOperandMode == Standard::OperandModes_t::OperandMode_I || 
                    iOperandMode == Standard::OperandModes_t::OperandMode_J ||
                    iOperandMode == Standard::OperandModes_t::OperandMode_O ||
                    iOperandMode == Standard::OperandModes_t::OperandMode_A || 
                    iOperandMode == Standard::OperandModes_t::OperandMode_IXY)
                {
                    iIterator++;
                    pInst->m_immediate.PushByte(vecInput[iIterator]);
                    break;
                }
        }
    }


    // Store displacement compression byte if required.
    // if(pInst->m_modrm.ModValueAbs() == 0b01) // Compressed displacement byte, only for disp8 addressing.
    // {
    //     if(iIterator >= nBytes)
    //         return IDASMErrorCode_t::IDASMErrorCode_NoDispCompressionByte;
    //
    //     FAIL_LOG("Storing compression byte");
    //     iIterator++;
    //     pInst->m_iCompressedDisp = vecInput[iIterator];
    // }


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}

