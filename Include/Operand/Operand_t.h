//=========================================================================
//                      Operand
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Holds operand information.
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include "OperandModes.h"
#include "OperandTypes.h"
#include "CEOperandModes.h"
#include "CEOperandTypes.h"
#include "../Standard/Register_t.h"



namespace INSANE_DASM64_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct Operand_t
    {
        Operand_t();
        Operand_t(int iLiteral);
        Operand_t(Standard::Register_t iRegister);
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


        // In case of a literal operand. This will hold the literal ( int ).
        int               m_iOperandLiteral  = 0;


        // In case of a fixed register operand. This will hold the register as enum.
        Standard::Register_t m_iOperandRegister;


        // In case of a OG / normal / legacy operand ( addressing mode + operand type ).
        // These will hold it.
        OperandModes_t    m_iOperandMode     = OperandMode_Invalid;
        OperandTypes_t    m_iOperandType     = OperandType_Invalid;
    };
}
