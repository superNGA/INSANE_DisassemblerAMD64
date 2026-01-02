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
    struct OpCodeDesc_t;
    struct LegacyPrefix_t;
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

    inline const char* OPCODE_NAME_SENTINAL = "xx_INVALID_xx";
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace InsaneDASM64::SpecialChars
{
    // Acts as escape opcode character when appears as first byte.
    constexpr Byte ESCAPE_OPCODE_FIRST_INDEX    = 0X0F;

    // Acts as escape opcode character when appears as second byte.
    constexpr Byte ESCAPE_OPCODE_SECOND_INDEX_38 = 0X38;
    constexpr Byte ESCAPE_OPCODE_SECOND_INDEX_3A = 0X3A;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace InsaneDASM64::Masks
{
    constexpr size_t MODRM_MOD = 0b11000000llu;
    constexpr size_t MODRM_REG = 0b111000llu;
    constexpr size_t MODRM_RM  = 0b111llu;

    constexpr size_t SIB_SCALE = 0b11000000llu;
    constexpr size_t SIB_INDEX = 0b111000llu;
    constexpr size_t SIB_BASE  = 0b111llu;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace InsaneDASM64
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct Register_t
    {
        enum RegisterClass_t : int16_t
        {
            RegisterClass_Invalid = -1,
            RegisterClass_GPR,
            RegisterClass_FPU,
            RegisterClass_SSE,
            RegisterClass_AVX,
            RegisterClass_AVX512,
            RegisterClass_Segment,
            RegisterClass_Control,
            RegisterClass_Debug,
        };

        Register_t()
        {
            m_iRegisterClass = RegisterClass_Invalid;
            m_iRegisterIndex = -1;
            m_iRegisterSize  = -1;
        }

        Register_t(RegisterClass_t iRegisterClass, int16_t iRegisterIndex, int16_t iRegisterSize)
        {
            m_iRegisterClass = iRegisterClass;
            m_iRegisterIndex = iRegisterIndex;
            m_iRegisterSize  = iRegisterSize;
        }

        RegisterClass_t m_iRegisterClass = RegisterClass_Invalid;
        int16_t         m_iRegisterIndex = -1;
        int16_t         m_iRegisterSize  = -1;
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
    enum OperandMode_t : int16_t
    {
        //      This is an enum holding all relevant operand addressing methods
        // according to the coder's edition mazegen's .xml ( https://github.com/mazegen/x86reference/blob/master/x86reference.xml ), 
        // that is reference for this project.
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
        Operand_t(int iLiteral);
        Operand_t(Register_t iRegister);
        Operand_t(OperandMode_t iOperandMode, OperandType_t iOperandType);

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
        // bool              m_bHidden          = false; // Some operands are hidden.

        
        // In case of a literal operand. This will hold the literal ( int ).
        int               m_iOperandLiteral  = 0;


        // In case of a fixed register operand. This will hold the register as enum.
        Register_t        m_iOperandRegister;


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
        void StoreOperatorDesc(OpCodeDesc_t* pOperatorInfo);

        bool ModRMRequired(const LegacyPrefix_t* pPrefix) const;
        bool InitChildVarient(const LegacyPrefix_t* pPrefix, Byte iModRM);


        // NOTE : One OpCode_t will hold all the OpCodes that are present in one
        //        instruction. That means, if multibyte opcode is present in one instruction,
        //        we will store all bytes ( 1 to 3 ) in a single OpCode_t struct.
        Byte            m_opBytes[Rules::MAX_OPBYTES];
        int32_t         m_nOpBytes        = 0;

        OpCodeDesc_t*   m_pRootOpCodeDesc = nullptr;
        OpCodeDesc_t*   m_pOpCodeDesc     = nullptr;
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
    struct ModRM_t
    {
        inline void Store(Byte modrm) { m_modrm = modrm; }
        inline Byte Get() const { return m_modrm; }
        
        // Bits are shifted before returning. 
        uint64_t ModValue() const;
        uint64_t RegValue() const;
        uint64_t RMValue() const;

        Byte m_modrm = 0x00;
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct SIB_t
    {
        inline void Store(Byte SIB) { m_SIB = SIB; }
        inline Byte Get() const { return m_SIB; }
        
        // Bits are shifted before returning. 
        uint64_t ScaleValue() const;
        uint64_t IndexValue() const;
        uint64_t BaseValue() const;

        Byte m_SIB = 0x00;
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
        ModRM_t        m_modrm;

        bool           m_bHasSIB   = false;
        SIB_t          m_SIB;

        Displacement_t m_displacement;
        Immediate_t    m_immediate;
    };


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
        IDASMErrorCode_NoImmediateFound,
        IDASMErrorCode_InvalidImmediateSize,
        IDASMErrorCode_Count
    };

    
    // Functions...
    // NOTE : Each function will return 0 ( ErrorCode_Success ) on success, and a non-zero IDASMErrorCode_t on fail.
    IDASMErrorCode_t Initialize ();
    IDASMErrorCode_t DecodeAndDisassemble(const std::vector<Byte>&         vecInput, std::vector<std::string>&  vecOutput);
    IDASMErrorCode_t Disassemble         (const std::vector<ParsedInst_t>& vecInput, std::vector<std::string>&  vecOutput);
    IDASMErrorCode_t Decode              (const std::vector<Byte>&         vecInput, std::vector<ParsedInst_t>& vecOutput);
    // TODO ToString function.

    const char* GetErrorMessage(IDASMErrorCode_t iErrorCode);
}