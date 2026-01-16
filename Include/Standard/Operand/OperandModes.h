//=========================================================================
//                      Operand Modes ( Operand Addressing Methods )
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Operand addressing methods ( ref.x86asm.net + linux's x86 opcode map )
//-------------------------------------------------------------------------
#pragma once
#include "../../Aliases.h"
#include <cstdint>



namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE
{
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

        // Custom ones. Added.
        OperandMode_VG,  // The VEX.vvvv field of the VEX prefix selects a general purpose register.
        OperandMode_VXY, // The VEX.vvvv field of the VEX prefix selects a 128-bit XMM register or a 256-bit YMM register
        OperandMode_IXY, // The upper 4 bits of the 8-bit immediate selects a 128-bit XMM register or a 256-bit YMM register.


        OperandMode_Count,
    };
}
