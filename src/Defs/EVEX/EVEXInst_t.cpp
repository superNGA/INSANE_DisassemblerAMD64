//=========================================================================
//                      EVEX Instruction
//=========================================================================
// by      : INSANE
// created : 29/01/2026
//
// purpose : Holds various modular components of a single EVEX encoded instruction.
//-------------------------------------------------------------------------
#include "../../../Include/EVEX/EVEXInst_t.h"


using namespace InsaneDASM64;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::EVEX::EVEXInst_t::EVEXInst_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void EVEX::EVEXInst_t::Clear()
{
    m_evexPrefix.Clear();
    m_opcode.Clear();
    m_disp.Clear();
    m_bHasSIB = false;
    m_immediate.Clear();
    m_iCompressedDisp = 0x00;
}
