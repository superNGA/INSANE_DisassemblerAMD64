//=========================================================================
//                      Operand Types
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Operand Types ( ref.x86asm.net + linux's x86 opcode map )
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include <cstdint>



namespace INSANE_DASM64_NAMESPACE
{
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

        // Custom Operand Types.
        OperandType_qq,    // NOTE : Quad-Quadword (256-bits), regardless of operand-size attribute.
        OperandType_x,     // NOTE : dq or qq based on the operand-size attribute.

        // These operand types are not present in the intel's architecture software developer manual
        // ( or maybe they IDGAF. ) but are present in the linux's x86 opcode map. I have been told that
        // these are very "niche" operand types and are used on rare occasions only. So I won't be using
        // much brain pow-a on disassembling these.
        OperandType_k,     // NOTE : AVX-512 mask register (k0-k7). Bitmask controlling conditional vector lane execution, merging, or zeroing in masked operations.
        OperandType_tr,    // NOTE : AMX tile register (tmm0-tmm7). Large dedicated matrix register for Intel Advanced Matrix Extensions, holding configurable tile data for AI/ML.
        OperandType_tc,    // NOTE : Tile configuration memory operand (64-byte block). Memory structure loaded/stored to define AMX tile palette, rows, and data formats.
        OperandType_sm,    // NOTE : Supervisor state memory operand. Privileged memory area for saving/restoring kernel/extended CPU state in XSAVE/XRSTOR supervisor variants.


        OperandType_Count,
    };
}
