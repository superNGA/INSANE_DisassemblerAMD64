#include <iostream>
#include <vector>

#include "Include/INSANE_DisassemblerAMD64.h"


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int main(void)
{
    // Input & Output data for disassembler.
    //std::vector<InsaneDASM64::Byte> vecInput = { 0x48, 0x83, 0xEC, 0x28, 0xC7, 0x44, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0xE8, 0xEF, 0xCE, 0xFF, 0xFF, 0x24, 0x01 };
    std::vector<InsaneDASM64::Byte> vecInputSimple = {
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


    std::vector<InsaneDASM64::Byte> vecInputRipOff =
    {
        0x48, 0xB8,                   // mov rax, imm64
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08
    };


    std::vector<InsaneDASM64::Byte> vecInput = {
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



    std::vector<InsaneDASM64::Byte> vecInputOkOk = {
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




    std::vector<InsaneDASM64::Instruction_t> vecOutput;
    vecOutput.clear();


    // Initialize disassembler.
    {
        InsaneDASM64::ErrorCode_t iErrorCode = InsaneDASM64::Initialize();
        if (iErrorCode == InsaneDASM64::ErrorCode_FailedInit)
        {
            printf("%s\n", InsaneDASM64::GetErrorMessage(iErrorCode));
            return 1;
        }
    }


    // Disassemble...
    {
        InsaneDASM64::ErrorCode_t iErrorCode = InsaneDASM64::Disassemble(vecInput, vecOutput);

        if (iErrorCode != InsaneDASM64::ErrorCode_t::ErrorCode_Success)
        {
            printf("%s\n", InsaneDASM64::GetErrorMessage(iErrorCode));
            return 1;
        }
    }


    /*for (int i = 0; i < 10; i++)
        printf("%s\n", InsaneDASM64::GetErrorMessage(static_cast<InsaneDASM64::ErrorCode_t>(i)));*/


    return 0;
}