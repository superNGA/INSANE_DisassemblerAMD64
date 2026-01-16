//=========================================================================
//                      Operand
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Holds operand information.
//-------------------------------------------------------------------------
#include "../../../../Include/Standard/Operand/Operand_t.h"


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::Standard::Operand_t::Operand_t()
{
    Reset();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::Standard::Operand_t::Operand_t(int iLiteral)
{
    m_iOperandCatagory = OperandCatagory_Literal;
    m_iOperandLiteral  = iLiteral;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::Standard::Operand_t::Operand_t(Standard::Register_t iRegister)
{
    m_iOperandCatagory = OperandCatagory_Register;
    m_iOperandRegister = iRegister;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::Standard::Operand_t::Operand_t(OperandModes_t iOperandMode, OperandTypes_t iOperandType)
{
    m_iOperandCatagory = OperandCatagory_Legacy;
    m_iOperandMode     = iOperandMode;
    m_iOperandType     = iOperandType;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::Standard::Operand_t::Reset()
{
    m_iOperandCatagory = OperandCatagory_Undefined;
    m_iOperandRegister = Standard::Register_t(Standard::Register_t::RegisterClass_Invalid, -1, -1);
}
