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
    constexpr size_t MAX_OPERANDS           = 4llu;
    constexpr size_t MAX_LEGACY_PREFIX      = 4llu;
    constexpr size_t MAX_OPBYTES            = 3llu;
    constexpr size_t MAX_DISPLACEMENT_BYTES = 4llu;
    constexpr size_t MAX_IMMEDIATE_BYTES    = 8llu;
    constexpr size_t MAX_INST_NAME_SIZE     = 0x10llu;
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
    enum Register_t : int16_t
    {
        Register_Invalid = -1,
        Register_RAX     =  0,
        Register_RCX     =  1,
        Register_RDX     =  2,
        Register_RBX     =  3,
        Register_RSP     =  4,
        Register_RBP     =  5,
        Register_RSI     =  6,
        Register_RDI     =  7
    };


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
        OpCodeAddressingMethod_AH,
        OpCodeAddressingMethod_AX,
        OpCodeAddressingMethod_EAX,
        OpCodeAddressingMethod_RAX,

        // RBX...
        OpCodeAddressingMethod_BL,
        OpCodeAddressingMethod_BH,
        OpCodeAddressingMethod_BX,
        OpCodeAddressingMethod_EBX,
        OpCodeAddressingMethod_RBX,

        // RCX...
        OpCodeAddressingMethod_CL,
        OpCodeAddressingMethod_CH,
        OpCodeAddressingMethod_CX,
        OpCodeAddressingMethod_ECX,
        OpCodeAddressingMethod_RCX,

        // RDX...
        OpCodeAddressingMethod_DL,
        OpCodeAddressingMethod_DH,
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

        OpCodeAddressingMethod_R8,
        OpCodeAddressingMethod_R9,
        OpCodeAddressingMethod_R10,
        OpCodeAddressingMethod_R11,
        OpCodeAddressingMethod_R12,
        OpCodeAddressingMethod_R13,
        OpCodeAddressingMethod_R14,
        OpCodeAddressingMethod_R15,


        // GS & FS register used by some opcodes in 2 byte opcode table.
        OpCodeAddressingMethod_GS,
        OpCodeAddressingMethod_FS,

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

        OpCodeOperandType_rel8, // expect a 8 bit data next to OpBytes, to be used in RIP + offset format.

        OpCodeOperantType_Count
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    enum OpCodeFlag_t : int16_t
    {
        OpCodeFlag_None = -1,

        // Bits 5, 4, and 3 of ModR/M byte used as an opcode extension 
        // (refer to Section A.4, “Opcode Extensions For One-Byte And Two-byte Opcodes”).
        OpCodeFlag_1A   =  0, 

        // Use the 0F0B opcode (UD2 instruction), the 0FB9H opcode (UD1 instruction), 
        // the 0FFFH opcode (UD0 instruction), or the D6 opcode (UDB instruction) 
        // when deliberately trying to generate an invalid opcode exception (#UD).
        OpCodeFlag_1B,
        
        // Some instructions use the same two-byte opcode. 
        // If the instruction has variations, or the opcode represents 
        // different instructions, the ModR/M byte will be used to 
        // differentiate the instruction. For the value of the ModR/M 
        // byte needed to decode the instruction, see Table A-6. 
        OpCodeFlag_1C, 

        // The instruction is invalid or not encodable in 64-bit mode. 
        // 40 through 4F (single-byte INC and DEC) are REX prefix combinations
        // when in 64-bit mode (use FE/FF Grp 4 and 5 for INC and DEC).
        OpCodeFlag_i64, 
        
        // Instruction is only available when in 64-bit mode.
        OpCodeFlag_o64,

        // When in 64-bit mode, instruction defaults to 64-bit operand size and 
        // cannot encode 32-bit operand size.
        OpCodeFlag_d64,

        // The operand size is forced to a 64-bit operand size when in 
        // 64-bit mode (prefixes that change operand size are 
        // ignored for this instruction in 64-bit mode).
        OpCodeFlag_f64,

        // VEX form only exists. There is no legacy SSE form of the instruction. 
        // For Integer GPR instructions it VEX prefix required.
        OpCodeFlag_v,

        // VEX128 & SSE forms only exist (no VEX256), when can’t be inferred from the data size.
        OpCodeFlag_v1,

        // Yes, these comments are copied form the manual :).
        OpCodeFlag_Count
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
    enum OperandMode_t : int16_t
    {
        //      This is an enum holding all relevant operand addressing methods
        // according to the coder's edition mazegen's .xml ( https://github.com/mazegen/x86reference/blob/master/x86reference.xml ), 
        // that is reference for this project.
        // 
        //      Geek's edition was giving me too much trouble.
        // 


        OperandMode_Invalid = -1,
        OperandMode_ptr = 0,
        OperandMode_CRn,
        OperandMode_DRn,
        OperandMode_rm,
        OperandMode_STim,
        OperandMode_STi,
        OperandMode_r,
        OperandMode_imm,
        OperandMode_rel,
        OperandMode_m,
        OperandMode_mm,
        OperandMode_moffs,
        OperandMode_mmm64,
        OperandMode_Sreg,
        OperandMode_TRn,
        OperandMode_xmm,
        OperandMode_xmmm,

        OperandMode_Count
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    enum OperandType_t : int16_t
    {
        // All different relevant operand types from coder's edition of mazegen's .xml 
        // ( https://github.com/mazegen/x86reference/blob/master/x86reference.xml )
        //


        OperandType_Invalid = -1,

        OperandType_8 = 0,        // [ GEEK : b          ] Byte, regardless of operand-size attribute.
        OpearndType_16or32_twice, // [ GEEK : a          ] Two one-word operands in memory or two double-word operands in memory, depending on operand-size attribute (only BOUND).
        OperandType_16_32,        // [ GEEK : v, vds, vs ] Word or doubleword, depending on operand-size attribute
        OperandType_16_32_64,     // [ GEEK : vqp        ] Word or doubleword, depending on operand-size attribute, or quadword, promoted by REX.W in 64-bit mode.
        OperandType_16,           // [ GEEK : w          ] Word, regardless of operand-size attribute
        OperandType_16int,        // [ GEEK : wi         ] Word-integer. Only x87 FPU instructions
        OperandType_32,           // [ GEEK : d, ds      ] Doubleword, regardless of operand-size attribute.
        OperandType_32int,        // [ GEEK : di         ] Doubleword-integer. Only x87 FPU instructions
        OperandType_32_64,        // [ GEEK : dqp        ] Doubleword, or quadword, promoted by REX.W in 64-bit mode
        OperandType_32real,       // [ GEEK : sr         ] Single-real. Only x87 FPU instructions
        OperandType_64mmx,        // [ GEEK : pi         ] Quadword MMX technology data.
        OperandType_64,           // [ GEEK : q, qp, psq ] Quadword, regardless of operand-size attribute
        OperandType_64int,        // [ GEEK : qi         ] Qword-integer. Only x87 FPU instructions
        OperandType_64real,       // [ GEEK : dr         ] Double-real. Only x87 FPU instructions
        OperandType_64_16,        // [ GEEK : vq         ] Quadword (default) or word if operand-size prefix is used
        OperandType_128pf,        // [ GEEK : ps         ] 128-bit packed single-precision floating-point data.
        OperandType_80dec,        // [ GEEK : bcd        ] Packed-BCD. Only x87 FPU instructions
        OperandType_128,          // [ GEEK : dq         ] Double-quadword, regardless of operand-size attribute
        OperandType_14_28,        // [ GEEK : e          ] x87 FPU environment
        OperandType_80real,       // [ GEEK : er         ] Extended-real. Only x87 FPU instructions
        OperandType_p,            // [ GEEK : p          ] 32-bit or 48-bit pointer, depending on operand-size attribute ( most likely not used in 64 bit mode ?? )
        OperandType_ptp,          // [ GEEK : ptp        ] 32-bit or 48-bit pointer, depending on operand-size attribute, or 80-bit far pointer, promoted by REX.W in 64-bit mode (for example, CALLF (FF /3)).
        OperandType_94_108,       // [ GEEK : st         ] x87 FPU state
        OperandType_512,          // [ GEEK : stx        ] x87 FPU and SIMD state

        OperandType_Count
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct Operand_t
    {
        Operand_t() { Reset(); }
        void Reset();

        // NOTE : Since it is observed that operands can be literals, registers and 
        //        (Operand addressing method + operand type) combos. In order to acces the operand.

        enum OperandCatagory_t : int16_t
        {
            OperandCatagory_Undefined = -1,
            OperandCatagory_Literal, 
            OperandCatagory_Register, 
            OperandCatagory_Legacy // Legacy operands are operands with a addressing method & operand type
        };


        // Catagory & visibility of the operand.
        OperandCatagory_t m_iOperandCatagory = OperandCatagory_Undefined;
        bool              m_bHidden          = false; // Some operands are hidden.

        
        // In case of a literal operand. This will hold the literal ( int ).
        int               m_iOperandLiteral  = 0;


        // In case of a fixed register operand. This will hold the register as enum.
        Register_t        m_iOperandRegister = Register_Invalid;


        // In case of a OG / normal / legacy operand ( addressing mode + operand type ).
        // These will hold it.
        OperandMode_t     m_iOperandMode     = OperandMode_Invalid;
        OperandType_t     m_iOperandType     = OperandType_Invalid;
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct OpCode_t
    {
        OpCode_t();
        void Clear();

        int OpByteCount () const;
        int OperandCount() const;

        Byte GetMostSignificantOpCode() const;
        bool PushOpCode     (Byte iByte);
        void CopyOperandInfo(const OperatorInfo_t* pOperatorInfo);

        // NOTE : One OpCode_t will hold all the OpCodes that are present in one
        //        instruction. That means, if multibyte opcode is present in one instruction,
        //        we will store all bytes ( 1 to 3 ) in a single OpCode_t struct.


        Byte            m_opBytes[Rules::MAX_OPBYTES];
        int32_t         m_nOpBytes       = 0;

        OpCodeFlag_t    m_iOpCodeFlag    = OpCodeFlag_t::OpCodeFlag_None;

        OpCodeOperand_t m_operands[Rules::MAX_OPERANDS];
        int32_t         m_nOperands      = 0;

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
    struct Displacement_t
    {
        Displacement_t();
        void Clear();

        int  ByteCount() const;
        bool PushByte(Byte iByte);

        Byte m_iDispBytes[Rules::MAX_DISPLACEMENT_BYTES];
        int  m_nDispBytes = 0;
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct Immediate_t
    {
        Immediate_t();
        void Clear();

        int  ByteCount() const;
        bool PushByte(Byte iByte);

        Byte m_immediateByte[Rules::MAX_IMMEDIATE_BYTES];
        int  m_nImmediateBytes = 0;
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

        bool           m_bHasModRM = false;
        Byte           m_iModRM    = 0x00;

        bool           m_bHasSID   = false;
        Byte           m_iSIB      = 0x00;

        Displacement_t m_displacement;
        Immediate_t    m_immediate;
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
        ErrorCode_NoOpByteFound,
        ErrorCode_ModRMNotFound,
        ErrorCode_SIDNotFound,
        ErrorCode_NoImmediateFound,
        ErrorCode_InvalidImmediateSize,
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