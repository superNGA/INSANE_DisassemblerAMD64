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
#include <assert.h>

// Util
#include "Tables/Tables.h"
#include "../Include/Aliases.h"

// Instruction containers
#include "../Include/Instruction_t.h"

// Standard stuff...
#include "../Include/DASMInst_t.h"

// Decoder -n- Disassembler.
#include "Decoder/Decoder.h"
#include "Disassembler/Disassembler.h"


// NOTE : Mind this.
using namespace InsaneDASM64;



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t INSANE_DASM64_NAMESPACE::Initialize()
{
    // Tables...
    IDASMErrorCode_t iErrorCode = G::g_tables.Initialize();
    if (iErrorCode != IDASMErrorCode_t::IDASMErrorCode_Success)
        return iErrorCode;


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t INSANE_DASM64_NAMESPACE::DecodeAndDisassemble(const std::vector<Byte>& vecInput, std::vector<DASMInst_t>& vecOutput)
{
    // Parse input...
    std::vector<Instruction_t> vecDecodedInst; vecDecodedInst.clear();

    IDASMErrorCode_t iDecoderErrorCode = Decode(vecInput, vecDecodedInst);
    if (iDecoderErrorCode != IDASMErrorCode_Success)
        return iDecoderErrorCode;


    vecOutput.clear();

    // Decoding didn't fail but its empty :(
    if (vecDecodedInst.empty() == true)
        return IDASMErrorCode_t::IDASMErrorCode_Success;


    // Disassemble decoded instruction.
    IDASMErrorCode_t iDASMErrorCode = Disassemble(vecDecodedInst, vecOutput);
    if (iDASMErrorCode != IDASMErrorCode_t::IDASMErrorCode_Success)
        return iDASMErrorCode;


    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int OperandTypeToAddressSizeInByte(Standard::CEOperandTypes_t iCEOperandType, int iAddressSizeInByte)
{
    using namespace Standard;
    assert((iAddressSizeInByte == 2 || iAddressSizeInByte == 4 || iAddressSizeInByte == 8) && "Invalid operand size!!");

    switch (iCEOperandType)
    {
    case CEOperandType_8:            return 1;
    case CEOpearndType_16or32_twice: return iAddressSizeInByte == 2 ? 4 : 8;
    case CEOperandType_16_32:        return iAddressSizeInByte == 2 ? 2 : 4;
    case CEOperandType_16_32_64:     return iAddressSizeInByte;

    case CEOperandType_16:
    case CEOperandType_16int:        return 2;

    case CEOperandType_32:
    case CEOperandType_32real:
    case CEOperandType_32int:        return 4;

    case CEOperandType_32_64:        return iAddressSizeInByte == 8 ? 8 : 4;

    case CEOperandType_64mmx:
    case CEOperandType_64:
    case CEOperandType_64int:
    case CEOperandType_64real:       return 8;

    case CEOperandType_64_16:        return iAddressSizeInByte == 2 ? 2 : 8;


    case CEOperandType_128pf:
    case CEOperandType_80dec:
    case CEOperandType_128:
    case CEOperandType_14_28:
    case CEOperandType_80real:
    case CEOperandType_p:
    case CEOperandType_ptp:
    case CEOperandType_94_108:
    case CEOperandType_512:
        break;

    default: break;
    }

    // If no operand type is any "opinion" of what the address size should be,
    // we shall default to the address size ( 8 bytes in longmode, unless override by 0x67 address size override prefix, in which case it will be 4 bytes ) ?
    return iAddressSizeInByte;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int OperandTypeToAddressSizeInBits(Standard::CEOperandTypes_t iCEOperandType, int iAddressSizeInByte)
{
    int iSize = OperandTypeToAddressSizeInByte(iCEOperandType, iAddressSizeInByte);
    
    // Keep the invalid size invalid, and scale valid size.
    return iSize <= 0 ? -1 : iSize * 8;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int OperandTypeToOperandSizeInBytes(Standard::CEOperandTypes_t iCEOperandType, int iOperandSizeInBytes)
{
    // This function is used to determine width of register used when we have to use the modrm byte
    // to determine which register we have to use. Because the operand size is not always the correct
    // and some operand types don't give a fuck about what the operand size is.

    using namespace Standard;
    assert((iOperandSizeInBytes == 2 || iOperandSizeInBytes == 4 || iOperandSizeInBytes == 8) && "Invalid operand size!!");

    switch (iCEOperandType)
    {
    case CEOperandType_8:            return 1;
    case CEOpearndType_16or32_twice: return iOperandSizeInBytes == 2 ? 4 : 8;
    case CEOperandType_16_32:        return iOperandSizeInBytes == 2 ? 2 : 4;
    case CEOperandType_16_32_64:     return iOperandSizeInBytes;

    case CEOperandType_16:
    case CEOperandType_16int:        return 2;

    case CEOperandType_32:
    case CEOperandType_32real:
    case CEOperandType_32int:        return 4;

    case CEOperandType_32_64:        return iOperandSizeInBytes == 8 ? 8 : 4;

    case CEOperandType_64mmx:
    case CEOperandType_64:
    case CEOperandType_64int:
    case CEOperandType_64real:       return 8;

    case CEOperandType_64_16:        return iOperandSizeInBytes == 2 ? 2 : 8;

        
    case CEOperandType_128pf:
    case CEOperandType_80dec:
    case CEOperandType_128:
    case CEOperandType_14_28:
    case CEOperandType_80real:
    case CEOperandType_p:
    case CEOperandType_ptp:
    case CEOperandType_94_108:
    case CEOperandType_512:
        break;

    default: break;
    }

    return -1;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int OperandTypeToOperandSizeInBits(Standard::CEOperandTypes_t iCEOperandType, int iOperandSizeInBytes)
{
    int iSize = OperandTypeToOperandSizeInBytes(iCEOperandType, iOperandSizeInBytes);
    
    // Keep the invalid size invalid, and scale valid size.
    return iSize <= 0 ? -1 : iSize * 8;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t InsaneDASM64::Disassemble(const std::vector<Instruction_t>& vecInput, std::vector<DASMInst_t>& vecOutput)
{
    vecOutput.clear();
    vecOutput.reserve(vecInput.size());

    for(const Instruction_t& inst : vecInput)
    {
        vecOutput.emplace_back();
        DASMInst_t* pInstOut = &vecOutput.back();

        Disassemble(&inst, pInstOut);
    }

    return IDASMErrorCode_t::IDASMErrorCode_Success;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IDASMErrorCode_t InsaneDASM64::Decode(const std::vector<Byte>& vecInput, std::vector<Instruction_t>& vecOutput)
{
    assert(G::g_tables.IsInitialized() == true && "Tables are not initialized. Initialize tables before parsing!");
    assert(vecOutput.empty()           == true && "Why is output not empty");
    vecOutput.clear();


    // No input?
    if(vecInput.empty() == true)
        return IDASMErrorCode_t::IDASMErrorCode_Success;


    size_t nBytes = vecInput.size();
    for(size_t iByteIndex = 0llu; iByteIndex < nBytes; iByteIndex++)
    {
        Byte             iByte = vecInput[iByteIndex];
        Instruction_t    inst;

        IDASMErrorCode_t iErrorCode = IDASMErrorCode_t::IDASMErrorCode_Success;


        // First we determine which encoding does the instruction has.
        switch (iByte) 
        {
            case SpecialChars::VEX_PREFIX_C4:
            case SpecialChars::VEX_PREFIX_C5:
                inst.InitEncodingType(Instruction_t::InstEncodingTypes_t::InstEncodingType_VEX);
                iErrorCode = DecodeVEXEncoding(vecInput, &inst, iByteIndex);
                break;

            default: 
                inst.InitEncodingType(Instruction_t::InstEncodingTypes_t::InstEncodingType_Legacy);
                iErrorCode = DecodeLegacyEncoding(vecInput, &inst, iByteIndex);
                break;
        }


        // Did we fail the decode ?
        if(iErrorCode != IDASMErrorCode_t::IDASMErrorCode_Success)
            return iErrorCode;


        vecOutput.push_back(inst);
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
    case InsaneDASM64::IDASMErrorCode_DisplacementNotFound:  return "[ Insane Disassembler AMD64 ] Not enough displacement bytes in byte stream.";
    case InsaneDASM64::IDASMErrorCode_NoImmediateFound:      return "[ Insane Disassembler AMD64 ] An Immediate was expected, but was not found.";
    case InsaneDASM64::IDASMErrorCode_InvalidImmediateSize:  return "[ Insane Disassembler AMD64 ] Failed to determine immediate size for some instruction.";
    case InsaneDASM64::IDASMErrorCode_InvalidVEXPrefix:      return "[ Insane Disassembler AMD64 ] Invalid VEX prefix byte found. VEX prefix must be either 0xC4 or 0xC5";
    case InsaneDASM64::IDASMErrorCode_InvalidVEXInst:        return "[ Insane Disassembler AMD64 ] Provided VEX encoded instruction is invalid.";
    case InsaneDASM64::IDASMErrorCode_VEXDisassemblyFailed:  return "[ Insane Disassembler AMD64 ] VEX disassembly failed.";
    case InsaneDASM64::IDASMErrorCode_InstComponentsNotFound:return "[ Insane Disassembler AMD64 ] Instruction components were found to be null while disassembling.";
    case InsaneDASM64::IDASMErrorCode_OpCodeNotInitialized:  return "[ Insane Disassembler AMD64 ] OpCode's final varient was not initialized.";
    
    default: break;
    }

    static char s_invalidCodeBuffer[128] = "";
    sprintf(s_invalidCodeBuffer, "[ Insane Disassembler AMD64 ] Invalid Error Code { %d }", iErrorCode);
    return s_invalidCodeBuffer;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Standard::CEOperandModes_t INSANE_DASM64_NAMESPACE::GeekToCoderOperandMode(Standard::OperandModes_t iOperandMode)
{
    switch (iOperandMode)
    {
    case Standard::OperandMode_A:   return Standard::CEOperandMode_ptr;
    case Standard::OperandMode_BA:  return Standard::CEOperandMode_m;
    case Standard::OperandMode_BB:  return Standard::CEOperandMode_m;
    case Standard::OperandMode_BD:  return Standard::CEOperandMode_m;
    case Standard::OperandMode_C:   return Standard::CEOperandMode_CRn;
    case Standard::OperandMode_D:   return Standard::CEOperandMode_DRn;
    case Standard::OperandMode_E:   return Standard::CEOperandMode_rm;
    case Standard::OperandMode_ES:  return Standard::CEOperandMode_STim;
    case Standard::OperandMode_EST: return Standard::CEOperandMode_STi;
    // case OperandMode_F: return [ no equivalent operand mode in coder's edition ]
    case Standard::OperandMode_G:   return Standard::CEOperandMode_r;
    case Standard::OperandMode_H:   return Standard::CEOperandMode_r;
    case Standard::OperandMode_I:   return Standard::CEOperandMode_imm;
    case Standard::OperandMode_J:   return Standard::CEOperandMode_rel;
    case Standard::OperandMode_M:   return Standard::CEOperandMode_m;
    case Standard::OperandMode_N:   return Standard::CEOperandMode_mm;
    case Standard::OperandMode_O:   return Standard::CEOperandMode_moffs;
    case Standard::OperandMode_P:   return Standard::CEOperandMode_mm;
    case Standard::OperandMode_Q:   return Standard::CEOperandMode_mmm64;
    case Standard::OperandMode_R:   return Standard::CEOperandMode_r;
    case Standard::OperandMode_S:   return Standard::CEOperandMode_Sreg;
    // case OperandMode_SC: return [ no equivalent operand mode in coder's edition ]
    case Standard::OperandMode_T:   return Standard::CEOperandMode_TRn;
    case Standard::OperandMode_U:   return Standard::CEOperandMode_xmm;
    case Standard::OperandMode_V:   return Standard::CEOperandMode_xmm;
    case Standard::OperandMode_W:   return Standard::CEOperandMode_xmmm;
    case Standard::OperandMode_X:   return Standard::CEOperandMode_m;
    case Standard::OperandMode_Y:   return Standard::CEOperandMode_m;
    case Standard::OperandMode_Z:   return Standard::CEOperandMode_r;

    default: break;
    }

    return Standard::CEOperandModes_t::CEOperandMode_Invalid;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Standard::CEOperandTypes_t INSANE_DASM64_NAMESPACE::GeekToCoderOperandType(Standard::OperandTypes_t iOperandType)
{
    switch (iOperandType)
    {
    case Standard::OperandType_a:   return Standard::CEOpearndType_16or32_twice;
    case Standard::OperandType_b:   return Standard::CEOperandType_8;
    case Standard::OperandType_bs:  return Standard::CEOperandType_8;
    case Standard::OperandType_bss: return Standard::CEOperandType_8;
    case Standard::OperandType_bcd: return Standard::CEOperandType_80dec;
    // case OperandType_bsq: return [ no equivalent in coder's edition. ]
    // case OperandType_c: return   [ no equivalent in coder's edition. ]
    case Standard::OperandType_d:   return Standard::CEOperandType_32;
    case Standard::OperandType_ds:  return Standard::CEOperandType_32;
    case Standard::OperandType_di:  return Standard::CEOperandType_32int;
    case Standard::OperandType_dq:  return Standard::CEOperandType_128;
    case Standard::OperandType_dqp: return Standard::CEOperandType_32_64;
    case Standard::OperandType_dr:  return Standard::CEOperandType_64real;
    case Standard::OperandType_e:   return Standard::CEOperandType_14_28;
    case Standard::OperandType_er:  return Standard::CEOperandType_80real;
    case Standard::OperandType_p:   return Standard::CEOperandType_p;
    case Standard::OperandType_pi:  return Standard::CEOperandType_64mmx;
    // case OperandType_pd: return  [ no equivalent in coder's edition. ]
    case Standard::OperandType_ps:  return Standard::CEOperandType_128pf;
    case Standard::OperandType_psq: return Standard::CEOperandType_64;
    // case OperandType_pt: return  [ no equivalent in coder's edition. ]
    case Standard::OperandType_ptp: return Standard::CEOperandType_ptp;
    case Standard::OperandType_q:   return Standard::CEOperandType_64;
    case Standard::OperandType_qi:  return Standard::CEOperandType_64int;
    case Standard::OperandType_qp:  return Standard::CEOperandType_64;
    // case OperandType_s: return   [ no equivalent in coder's edition. ]
    // case OperandType_sd: return  [ no equivalent in coder's edition. ]
    // case OperandType_si: return  [ no equivalent in coder's edition. ]
    case Standard::OperandType_sr:  return Standard::CEOperandType_32real;
    // case OperandType_ss: return  [ no equivalent in coder's edition. ]
    case Standard::OperandType_st:  return Standard::CEOperandType_94_108;
    case Standard::OperandType_stx: return Standard::CEOperandType_512;
    // case OperandType_t: return   [ no equivalent in coder's edition. ]
    case Standard::OperandType_v:   return Standard::CEOperandType_16_32;
    case Standard::OperandType_vds: return Standard::CEOperandType_16_32;
    case Standard::OperandType_vq:  return Standard::CEOperandType_64_16;
    case Standard::OperandType_vqp: return Standard::CEOperandType_16_32_64;
    case Standard::OperandType_vs:  return Standard::CEOperandType_16_32;
    case Standard::OperandType_w:   return Standard::CEOperandType_16;
    case Standard::OperandType_wi:  return Standard::CEOperandType_16int;

    default: break;
    }

    return Standard::CEOperandType_Invalid;
}
