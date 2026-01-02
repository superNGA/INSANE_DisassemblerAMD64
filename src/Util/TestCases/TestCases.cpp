//=========================================================================
//                      Test Cases
//=========================================================================
// by      : INSANE
// created : 02/01/2026
// 
// purpose : Byte streams for testing disassembler's accuracy.
//-------------------------------------------------------------------------
#include "TestCases.h"


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::vector<InsaneDASM64::Byte> INSANE_DASM64_NAMESPACE::TestCases::g_vecOneByteOpCodes_001 = {
    0x55,
    0x48, 0x89, 0xE5,
    0x48, 0x83, 0xEC, 0x20,
    0xB8, 0x01, 0x00, 0x00, 0x00,
    0x89, 0x45, 0xFC,
    0x83, 0xC0, 0x02,
    0x83, 0xF8, 0x03,
    0x75, 0x05,
    0xB8, 0x00, 0x00, 0x00, 0x00,
    0xC9,
    0xC3
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::vector<InsaneDASM64::Byte> INSANE_DASM64_NAMESPACE::TestCases::g_vecOneByteOpCodes_002 = {
    // ---- simple, no ModRM ----
    0x90,                         // nop
    0xC3,                         // ret
    0x9C,                         // pushfq
    0x9D,                         // popfq
    0x98,                         // cdqe
    0x99,                         // cqo

    // ---- push/pop reg (opcode encoded reg) ----
    0x50,                         // push rax
    0x58,                         // pop rax
    0x41, 0x50,                   // push r8
    0x41, 0x58,                   // pop r8

    // ---- mov reg, imm ----
    0xB8, 0x78, 0x56, 0x34, 0x12, // mov eax, 0x12345678
    0x48, 0xB8,                   // mov rax, imm64
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08,

    // ---- ModRM: reg-reg ----
    0x48, 0x89, 0xD8,             // mov rax, rbx
    0x48, 0x01, 0xD8,             // add rax, rbx
    0x48, 0x29, 0xD8,             // sub rax, rbx

    // ---- ModRM: reg-mem ----
    0x48, 0x8B, 0x03,             // mov rax, [rbx]
    0x48, 0x89, 0x03,             // mov [rbx], rax

    // ---- ModRM + disp8 ----
    0x48, 0x8B, 0x43, 0x08,       // mov rax, [rbx+8]

    // ---- ModRM + disp32 ----
    0x48, 0x8B, 0x83,
    0x78, 0x56, 0x34, 0x12,   // mov rax, [rbx+0x12345678]

    // ---- SIB ----
    0x48, 0x8B, 0x04, 0x8B,       // mov rax, [rbx + rcx*4]

    // ---- immediate arithmetic ----
    0x48, 0x83, 0xC0, 0x10,       // add rax, 0x10 (imm8)
    0x48, 0x81, 0xC0,
    0x78, 0x56, 0x34, 0x12,   // add rax, 0x12345678

    // ---- control flow ----
    0xEB, 0x05,                   // jmp short +5
    0x74, 0x03,                   // je short +3
    0x75, 0x01,                   // jne short +1
    0xE8, 
    0x12, 0x34, 0x56, 0x78,   // call rel32
    0xE9,
    0x01, 0x02, 0x03, 0x04,   // jmp rel32

    // ---- stack frame ----
    0x55,                         // push rbp
    0x48, 0x89, 0xE5,             // mov rbp, rsp
    0x48, 0x83, 0xEC, 0x20,       // sub rsp, 0x20
    0x48, 0x83, 0xC4, 0x20,       // add rsp, 0x20
    0x5D,                         // pop rbp

    // ---- flags / misc ----
    0xF8,                         // clc
    0xF9,                         // stc
    0xFC,                         // cld
    0xFD                          // std
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::vector<InsaneDASM64::Byte> INSANE_DASM64_NAMESPACE::TestCases::g_vecTwoByteOpCodes_001 = {
    // ---- no ModR/M ----
    0x0F, 0x05,             // SYSCALL
    0x0F, 0x31,             // RDTSC
    0x0F, 0xA2,             // CPUID

    // ---- ModR/M required ----
    0x0F, 0x01, 0xD0,       // XGETBV   (ModR/M = D0)
    0x0F, 0xAF, 0xC1,       // IMUL r32, r/m32   (eax, ecx)
    0x0F, 0xB6, 0xC1,       // MOVZX r32, r/m8   (eax, cl)
    0x0F, 0xBE, 0xC1,       // MOVSX r32, r/m8   (eax, cl)

    // ---- conditional moves ----
    0x0F, 0x44, 0xC1,       // CMOVE r32, r/m32
    0x0F, 0x45, 0xC1,       // CMOVNE r32, r/m32

    // ---- setcc ----
    0x0F, 0x94, 0xC1,       // SETE r/m8
    0x0F, 0x95, 0xC1        // SETNE r/m8
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::vector<InsaneDASM64::Byte> INSANE_DASM64_NAMESPACE::TestCases::g_vecThreeByteOpCodes_38 = {
    // 66 0F 38 00 C1 -> pshufb xmm0, xmm1 (SSSE3, XMM)
    0x66, 0x0F, 0x38, 0x00, 0xC1,

    // 0F 38 00 C1 -> pshufb mm0, mm1 (SSSE3, MMX)
    0x0F, 0x38, 0x00, 0xC1,

    // 66 0F 38 0B C1 -> pmulhrsw xmm0, xmm1 (SSSE3)
    0x66, 0x0F, 0x38, 0x0B, 0xC1,

    // 66 0F 38 1C C1 -> pabsb xmm0, xmm1 (SSSE3)
    0x66, 0x0F, 0x38, 0x1C, 0xC1,

    // 66 0F 38 37 C1 -> pcmpgtq xmm0, xmm1 (SSE4.2)
    0x66, 0x0F, 0x38, 0x37, 0xC1,

    // F2 0F 38 F0 C1 -> crc32 eax, cl (SSE4.2, byte)
    0xF2, 0x0F, 0x38, 0xF0, 0xC1,

    // F2 0F 38 F1 C8 -> crc32 ecx, eax (SSE4.2, 32-bit)
    0xF2, 0x0F, 0x38, 0xF1, 0xC8
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::vector<InsaneDASM64::Byte> INSANE_DASM64_NAMESPACE::TestCases::g_vecThreeByteOpCodes_3A = {
    // 66 0F 3A 0F C1 00 -> palignr xmm0, xmm1, 0 (SSSE3, XMM)
    0x66, 0x0F, 0x3A, 0x0F, 0xC1, 0x00,

    // 0F 3A 0F C1 00 -> palignr mm0, mm1, 0 (SSSE3, MMX)
    0x0F, 0x3A, 0x0F, 0xC1, 0x00,

    // 66 0F 3A 08 C1 03 -> roundps xmm0, xmm1, 3 (SSE4.1)
    0x66, 0x0F, 0x3A, 0x08, 0xC1, 0x03,

    // 66 0F 3A 0C C1 0F -> blendps xmm0, xmm1, 0x0F (SSE4.1)
    0x66, 0x0F, 0x3A, 0x0C, 0xC1, 0x0F,

    // 66 0F 3A 0E C1 AA -> pblendw xmm0, xmm1, 0xAA (SSE4.1)
    0x66, 0x0F, 0x3A, 0x0E, 0xC1, 0xAA,

    // 66 0F 3A 41 C1 55 -> dppd xmm0, xmm1, 0x55 (SSE4.1)
    0x66, 0x0F, 0x3A, 0x41, 0xC1, 0x55,

    // 66 0F 3A 61 C1 10 -> pcmpestri xmm0, xmm1, 0x10 (SSE4.2)
    0x66, 0x0F, 0x3A, 0x61, 0xC1, 0x10
};