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

    inline const char* OPCODE_NAME_SENTINAL   = "xx_INVALID_xx";
    inline const char* REGISTER_NAME_SENTINAL = "xx_INVALID_REG_xx";
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace InsaneDASM64::SpecialChars
{
    // Acts as escape opcode character when appears as first byte.
    constexpr Byte ESCAPE_OPCODE_FIRST_INDEX     = 0X0F;

    // Acts as escape opcode character when appears as second byte.
    constexpr Byte ESCAPE_OPCODE_SECOND_INDEX_38 = 0X38;
    constexpr Byte ESCAPE_OPCODE_SECOND_INDEX_3A = 0X3A;


    // First byte for VEX encoded instructions.
    constexpr Byte VEX_PREFIX_C4                 = 0XC4;
    constexpr Byte VEX_PREFIX_C5                 = 0XC5;
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

    constexpr size_t REX_W     = 0b1000llu;
    constexpr size_t REX_R     = 0b100llu;
    constexpr size_t REX_X     = 0b10llu;
    constexpr size_t REX_B     = 0b1llu;
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
            RegisterClass_SSE, // xxm
            RegisterClass_MMX, // mmx
            RegisterClass_AVX, // ymm
            RegisterClass_AVX512,
            RegisterClass_Segment,
            RegisterClass_Control,
            RegisterClass_Debug,
            RegisterClass_Test,
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

        const char* ToString() const;

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
    enum OperandModes_t : int16_t // Geek's edition operand addressing methods.
    {
        //      This is an enum holding all operand addressing methods
        // according to the Geek's edition mazegen's .xml ( https://github.com/mazegen/x86reference/blob/master/x86reference.xml ), 
        // that is reference for this project.
        // 

        OperandMode_Invalid = -1,
        OperandMode_A,
        OperandMode_BA,
        OperandMode_BB,
        OperandMode_BD,
        OperandMode_C,
        OperandMode_D,
        OperandMode_E,
        OperandMode_ES,
        OperandMode_EST,
        OperandMode_F,
        OperandMode_G,
        OperandMode_H,
        OperandMode_I,
        OperandMode_J,
        OperandMode_M,
        OperandMode_N,
        OperandMode_O,
        OperandMode_P,
        OperandMode_Q,
        OperandMode_R,
        OperandMode_S,
        OperandMode_SC,
        OperandMode_T,
        OperandMode_U,
        OperandMode_V,
        OperandMode_W,
        OperandMode_X,
        OperandMode_Y,
        OperandMode_Z,

        OperandMode_Count,
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    enum CEOperandModes_t : int16_t // Coder's edition operand addressing methods.
    {
        //      This is an enum holding all relevant operand addressing methods
        // according to the coder's edition mazegen's .xml ( https://github.com/mazegen/x86reference/blob/master/x86reference.xml ), 
        // that is reference for this project.
        // 


        CEOperandMode_Invalid = -1,
        CEOperandMode_ptr = 0, // [ GEEK : A          ]
        CEOperandMode_CRn,     // [ GEEK : C          ]
        CEOperandMode_DRn,     // [ GEEK : D          ]
        CEOperandMode_rm,      // [ GEEK : E          ]
        CEOperandMode_STim,    // [ GEEK : ES         ]
        CEOperandMode_STi,     // [ GEEK : EST        ]
        CEOperandMode_r,       // [ GEEK : G, H, R, Z ]
        CEOperandMode_imm,     // [ GEEK : I          ]
        CEOperandMode_rel,     // [ GEEK : J          ]
        CEOperandMode_m,       // [ GEEK : M, X, Y, BA, BB, BD ]
        CEOperandMode_mm,      // [ GEEK : N, P       ]
        CEOperandMode_moffs,   // [ GEEK : O          ]
        CEOperandMode_mmm64,   // [ GEEK : Q          ]
        CEOperandMode_Sreg,    // [ GEEK : S          ]
        CEOperandMode_TRn,     // [ GEEK : T          ]
        CEOperandMode_xmm,     // [ GEEK : U, V       ]
        CEOperandMode_xmmm,    // [ GEEK : W          ]

        CEOperandMode_Count
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    enum OperandTypes_t : int16_t
    {
        // All different operand types from geek's edition of mazegen's .xml 
        // ( https://github.com/mazegen/x86reference/blob/master/x86reference.xml )
        // 
        // NOTE : Comment are copied from ref.x86asm.net
        //

        OperandType_Invalid = -1,
        OperandType_a = 0, // NOTE : Two one-word operands in memory or two double-word operands in memory, depending on operand-size attribute
        OperandType_b,     // NOTE : Byte, regardless of operand-size attribute.
        OperandType_bcd,   // NOTE : Packed-BCD. Only x87 FPU instructions
        OperandType_bs,    // NOTE : Byte, sign-extended to the size of the destination operand.
        OperandType_bsq,   // NOTE : (Byte, sign-extended to 64 bits.)
        OperandType_bss,   // NOTE : Byte, sign-extended to the size of the stack pointer
        OperandType_c,     // NOTE : Byte or word, depending on operand-size attribute.
        OperandType_d,     // NOTE : Doubleword, regardless of operand-size attribute.
        OperandType_di,    // NOTE : Doubleword-integer. Only x87 FPU instructions
        OperandType_dq,    // NOTE : Double-quadword, regardless of operand-size attribute
        OperandType_dqp,   // NOTE : Doubleword, or quadword, promoted by REX.W in 64-bit mode
        OperandType_dr,    // NOTE : Double-real. Only x87 FPU instructions
        OperandType_ds,    // NOTE : Doubleword, sign-extended to 64 bits
        OperandType_e,     // NOTE : x87 FPU environment
        OperandType_er,    // NOTE : Extended-real. Only x87 FPU instructions
        OperandType_p,     // NOTE : 32-bit or 48-bit pointer, depending on operand-size attribute
        OperandType_pi,    // NOTE : Quadword MMX technology data.
        OperandType_pd,    // NOTE : 128-bit packed double-precision floating-point data.
        OperandType_ps,    // NOTE : 128-bit packed single-precision floating-point data.
        OperandType_psq,   // NOTE : 64-bit packed single-precision floating-point data.
        OperandType_pt,    // NOTE : (80-bit far pointer.)
        OperandType_ptp,   // NOTE : 32-bit or 48-bit pointer, depending on operand-size attribute, or 80-bit far pointer, promoted by REX.W in 64-bit mode
        OperandType_q,     // NOTE : Quadword, regardless of operand-size attribute
        OperandType_qi,    // NOTE : Qword-integer. Only x87 FPU instructions
        OperandType_qp,    // NOTE : Quadword, promoted by REX.W
        OperandType_s,     // NOTE : 6-byte pseudo-descriptor, or 10-byte pseudo-descriptor in 64-bit mode
        OperandType_sd,    // NOTE : Scalar element of a 128-bit packed double-precision floating data.
        OperandType_si,    // NOTE : Doubleword integer register (e. g., eax).
        OperandType_sr,    // NOTE : Single-real. Only x87 FPU instructions
        OperandType_ss,    // NOTE : Scalar element of a 128-bit packed single-precision floating data.
        OperandType_st,    // NOTE : x87 FPU state
        OperandType_stx,   // NOTE : x87 FPU and SIMD state
        OperandType_t,     // NOTE : 10-byte far pointer.
        OperandType_v,     // NOTE : Word or doubleword, depending on operand-size attribute
        OperandType_vds,   // NOTE : Word or doubleword, depending on operand-size attribute, or doubleword, sign-extended to 64 bits for 64-bit operand size.
        OperandType_vq,    // NOTE : Quadword (default) or word if operand-size prefix is used
        OperandType_vqp,   // NOTE : Word or doubleword, depending on operand-size attribute, or quadword, promoted by REX.W in 64-bit mode.
        OperandType_vs,    // NOTE : Word or doubleword sign extended to the size of the stack pointer
        OperandType_w,     // NOTE : Word, regardless of operand-size attribute
        OperandType_wi,    // NOTE : Word-integer. Only x87 FPU instructions

        OperandType_Count,
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    enum CEOperandTypes_t : int16_t
    {
        // All different relevant operand types from coder's edition of mazegen's .xml 
        // ( https://github.com/mazegen/x86reference/blob/master/x86reference.xml )
        //


        CEOperandType_Invalid = -1,

        CEOperandType_8 = 0,        // [ GEEK : b, bs, bss ] Byte, regardless of operand-size attribute.
        CEOpearndType_16or32_twice, // [ GEEK : a          ] Two one-word operands in memory or two double-word operands in memory, depending on operand-size attribute (only BOUND).
        CEOperandType_16_32,        // [ GEEK : v, vds, vs ] Word or doubleword, depending on operand-size attribute
        CEOperandType_16_32_64,     // [ GEEK : vqp        ] Word or doubleword, depending on operand-size attribute, or quadword, promoted by REX.W in 64-bit mode.
        CEOperandType_16,           // [ GEEK : w          ] Word, regardless of operand-size attribute
        CEOperandType_16int,        // [ GEEK : wi         ] Word-integer. Only x87 FPU instructions
        CEOperandType_32,           // [ GEEK : d, ds      ] Doubleword, regardless of operand-size attribute.
        CEOperandType_32int,        // [ GEEK : di         ] Doubleword-integer. Only x87 FPU instructions
        CEOperandType_32_64,        // [ GEEK : dqp        ] Doubleword, or quadword, promoted by REX.W in 64-bit mode
        CEOperandType_32real,       // [ GEEK : sr         ] Single-real. Only x87 FPU instructions
        CEOperandType_64mmx,        // [ GEEK : pi         ] Quadword MMX technology data.
        CEOperandType_64,           // [ GEEK : q, qp, psq ] Quadword, regardless of operand-size attribute
        CEOperandType_64int,        // [ GEEK : qi         ] Qword-integer. Only x87 FPU instructions
        CEOperandType_64real,       // [ GEEK : dr         ] Double-real. Only x87 FPU instructions
        CEOperandType_64_16,        // [ GEEK : vq         ] Quadword (default) or word if operand-size prefix is used
        CEOperandType_128pf,        // [ GEEK : ps         ] 128-bit packed single-precision floating-point data.
        CEOperandType_80dec,        // [ GEEK : bcd        ] Packed-BCD. Only x87 FPU instructions
        CEOperandType_128,          // [ GEEK : dq         ] Double-quadword, regardless of operand-size attribute
        CEOperandType_14_28,        // [ GEEK : e          ] x87 FPU environment
        CEOperandType_80real,       // [ GEEK : er         ] Extended-real. Only x87 FPU instructions
        CEOperandType_p,            // [ GEEK : p          ] 32-bit or 48-bit pointer, depending on operand-size attribute ( most likely not used in 64 bit mode ?? )
        CEOperandType_ptp,          // [ GEEK : ptp        ] 32-bit or 48-bit pointer, depending on operand-size attribute, or 80-bit far pointer, promoted by REX.W in 64-bit mode (for example, CALLF (FF /3)).
        CEOperandType_94_108,       // [ GEEK : st         ] x87 FPU state
        CEOperandType_512,          // [ GEEK : stx        ] x87 FPU and SIMD state

        CEOperandType_Count
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct Operand_t
    {
        Operand_t() { Reset(); }
        Operand_t(int iLiteral);
        Operand_t(Register_t iRegister);
        Operand_t(OperandModes_t iOperandMode, OperandTypes_t iOperandType);

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
        OperandModes_t     m_iOperandMode     = OperandMode_Invalid;
        OperandTypes_t    m_iOperandType     = OperandType_Invalid;
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
        
        // REX ignored. Bits are shifted before returning
        uint64_t ModValueAbs() const;
        uint64_t RegValueAbs() const;
        uint64_t RMValueAbs() const;

        Byte m_modrm = 0x00;
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct SIB_t
    {
        inline void Store(Byte SIB) { m_SIB = SIB; }
        inline Byte Get() const { return m_SIB; }
        
        // REX ignored. Bits are shifted before returning. 
        uint64_t ScaleValueAbs() const;
        uint64_t IndexValueAbs() const;
        uint64_t BaseValueAbs() const;

        Byte m_SIB = 0x00;
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct Instruction_t
    {
        enum InstEncodingTypes_t
        {
            InstEncodingType_Invalid = -1,
            InstEncodingType_Legacy = 0,
            InstEncodingType_VEX,
            InstEncodingType_EVEX,
            InstEncodingType_XOP,

            InstEncodingType_Count
        };

        Instruction_t() {};
        Instruction_t(InstEncodingTypes_t iEncoding) { InitEncodingType(iEncoding); }
        bool InitEncodingType(InstEncodingTypes_t iEncoding);


        void*               m_pInst             = nullptr;
        InstEncodingTypes_t m_iInstEncodingType = InstEncodingTypes_t::InstEncodingType_Invalid;
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct LegacyInst_t
    {
        LegacyInst_t();

        void Clear();
        int GetOperandSizeInBytes(bool bIgnoreREXW) const;
        int GetAddressSizeInBytes() const;

        // NOTE : These functions account for the REX bits. 
        //        And the indivigual functions of ModRM_t or SIB_t don't account for that.
        //        So take precaution while using these functions.
        uint64_t ModRM_Reg() const;
        uint64_t ModRM_Mod() const;
        uint64_t ModRM_RM()  const;
        uint64_t SIB_Scale() const;
        uint64_t SIB_Index() const;
        uint64_t SIB_Base()  const;


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
    struct VEXInst_t
    {

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
    IDASMErrorCode_t DecodeAndDisassemble(const std::vector<Byte>&          vecInput, std::vector<std::string>&   vecOutput);
    IDASMErrorCode_t Disassemble         (const std::vector<Instruction_t>& vecInput, std::vector<std::string>&   vecOutput);
    IDASMErrorCode_t Decode              (const std::vector<Byte>&          vecInput, std::vector<Instruction_t>& vecOutput);

    const char*      GetErrorMessage       (IDASMErrorCode_t iErrorCode);
    CEOperandTypes_t GeekToCoderOperandType(OperandTypes_t iOperandType);
    CEOperandModes_t GeekToCoderOperandMode(OperandModes_t iOperandMode);
}
