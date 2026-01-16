//=========================================================================
//                      Coder Edition Operand Types
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Coder's edition operand types ( ref.x86asm.net )
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include <cstdint>



namespace INSANE_DASM64_NAMESPACE
{
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
}
