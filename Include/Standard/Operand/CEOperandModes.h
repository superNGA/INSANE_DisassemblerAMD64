//=========================================================================
//                      Coder's Edition Operand Modes ( Operand addressing methods )
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Coder's edition operand addressing methods ( ref.x86asm.net )
//-------------------------------------------------------------------------
#pragma once
#include "../../Aliases.h"
#include <cstdint>



namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE
{
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
        CEOperandMode_MRn,     // [ GEEK : K          ]

        CEOperandMode_Count
    };
}
