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



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace InsaneDASM64
{
    // Data types...
    typedef unsigned char Byte;

    // Foward Decls...
    struct OperatorInfo_t;
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace InsaneDASM64::Rules
{
    constexpr size_t MAX_OPERANDS      = 4llu;
    constexpr size_t MAX_LEGACY_PREFIX = 4llu;
    constexpr size_t MAX_OPBYTES       = 3llu;
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace InsaneDASM64::SpecialChars
{
    // Acts as escape opcode character when appears as first byte.
    constexpr Byte ESCAPE_OPCODE_FIRST_INDEX    = 0X0F;

    // Acts as escape opcode character when appears as second byte.
    constexpr Byte ESCAPE_OPCODE_SECOND_INDEX_A = 0X38;
    constexpr Byte ESCAPE_OPCODE_SECOND_INDEX_B = 0X3A;
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace InsaneDASM64
{

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    enum OpCodeAddressingMethod_t : int16_t
    {
        OpCodeAddressingMethod_Invalid = -1,

        OpCodeAddressingMethod_A = 0,
        OpCodeAddressingMethod_B,
        OpCodeAddressingMethod_C,
        OpCodeAddressingMethod_D,
        OpCodeAddressingMethod_E,
        OpCodeAddressingMethod_F,
        OpCodeAddressingMethod_G,
        OpCodeAddressingMethod_H,
        OpCodeAddressingMethod_I,
        OpCodeAddressingMethod_J,
        // OpCodeAddressingMethod_K,
        OpCodeAddressingMethod_L,
        OpCodeAddressingMethod_M,
        OpCodeAddressingMethod_N,
        OpCodeAddressingMethod_O,
        OpCodeAddressingMethod_P,
        OpCodeAddressingMethod_Q,
        OpCodeAddressingMethod_R,
        OpCodeAddressingMethod_S,
        // OpCodeAddressingMethod_T,
        OpCodeAddressingMethod_U,
        OpCodeAddressingMethod_V,
        OpCodeAddressingMethod_W,
        OpCodeAddressingMethod_X,
        OpCodeAddressingMethod_Y,

        // Fixed register addressing methods...
        // RAX...
        OpCodeAddressingMethod_AL,
        OpCodeAddressingMethod_AX,
        OpCodeAddressingMethod_EAX,
        OpCodeAddressingMethod_RAX,

        // RBX...
        OpCodeAddressingMethod_BL,
        OpCodeAddressingMethod_BX,
        OpCodeAddressingMethod_EBX,
        OpCodeAddressingMethod_RBX,

        // RCX...
        OpCodeAddressingMethod_CL,
        OpCodeAddressingMethod_CX,
        OpCodeAddressingMethod_ECX,
        OpCodeAddressingMethod_RCX,

        // RDX...
        OpCodeAddressingMethod_DL,
        OpCodeAddressingMethod_DX,
        OpCodeAddressingMethod_EDX,
        OpCodeAddressingMethod_RDX,

        // RSP
        OpCodeAddressingMethod_SPL,
        OpCodeAddressingMethod_SP,
        OpCodeAddressingMethod_ESP,
        OpCodeAddressingMethod_RSP,

        // RDI...
        OpCodeAddressingMethod_DIL,
        OpCodeAddressingMethod_DI,
        OpCodeAddressingMethod_EDI,
        OpCodeAddressingMethod_RDI,

        // RSI...
        OpCodeAddressingMethod_SIL,
        OpCodeAddressingMethod_SI,
        OpCodeAddressingMethod_ESI,
        OpCodeAddressingMethod_RSI,

        // RBP
        OpCodeAddressingMethod_BPL,
        OpCodeAddressingMethod_BP,
        OpCodeAddressingMethod_EBP,
        OpCodeAddressingMethod_RBP,

        OpCodeAddressingMethod_Count
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    enum OpCodeOperandType_t : int16_t
    {
        OpCodeOperandType_Invalid = -1,

        OpCodeOperandType_a = 0,
        OpCodeOperandType_b,
        OpCodeOperandType_c,
        OpCodeOperandType_d,
        OpCodeOperandType_dq,
        OpCodeOperandType_p,
        OpCodeOperandType_pd,
        OpCodeOperandType_pi,
        OpCodeOperandType_ps,
        OpCodeOperandType_q,
        OpCodeOperandType_qq,
        OpCodeOperandType_s,
        OpCodeOperandType_sd,
        OpCodeOperandType_ss,
        OpCodeOperandType_si,
        OpCodeOperandType_v,
        OpCodeOperandType_w,
        OpCodeOperandType_x,
        OpCodeOperandType_y,
        OpCodeOperandType_z,

        OpCodeOperantType_Count
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct OpCodeOperand_t
    {
        OpCodeOperand_t();
        OpCodeOperand_t(OpCodeAddressingMethod_t iAddressingMethod, OpCodeOperandType_t iOperandType);

        void operator=(const OpCodeOperand_t& other);

        // Valid operand check...
        bool IsValid() const;


        OpCodeAddressingMethod_t m_iAddressingMethod;
        OpCodeOperandType_t      m_iOperandType;
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct OpCode_t
    {
        OpCode_t();
        void Clear();

        int OpByteCount () const;
        int OperandCount() const;

        bool PushOpCode     (Byte iByte);
        void CopyOperandInfo(const OperatorInfo_t* pOperatorInfo);

        // NOTE : One OpCode_t will hold all the OpCodes that are present in one
        //        instruction. That means, if multibyte opcode is present in one instruction,
        //        we will store all bytes ( 1 to 3 ) in a single OpCode_t struct.


        Byte            m_opBytes[Rules::MAX_OPBYTES];
        int32_t         m_nOpBytes;

        OpCodeOperand_t m_operands[Rules::MAX_OPERANDS];
        int32_t         m_nOperands = 0;

        const char*     m_szOperatorName = nullptr;
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct LegacyPrefix_t
    {
        LegacyPrefix_t();
        void Clear();

        bool PushPrefix(Byte iByte);
        int  PrefixCount() const;

        int32_t m_nPrefix = 0;
        Byte    m_legacyPrefix[Rules::MAX_LEGACY_PREFIX];
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct Instruction_t
    {

    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct ParsedInst_t
    {
        ParsedInst_t();

        void Clear();

        LegacyPrefix_t m_legacyPrefix;
        OpCode_t       m_opCode;

        bool           m_bHasREX   = false;
        Byte           m_iREX      = 0x00;
        int32_t        m_iREXIndex = -1;
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    enum ErrorCode_t : int
    {
        ErrorCode_Success    = 0, // All functions return ErrorCode_Success on success.
        ErrorCode_FailedInit = 1,
        ErrorCode_TooManyPefix,
        ErrorCode_REXNotPrecedingOpCode,
        ErrorCode_InvalidOpCode,
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