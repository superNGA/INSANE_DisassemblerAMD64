//=========================================================================
//                      EVEX Instruction
//=========================================================================
// by      : INSANE
// created : 29/01/2026
//
// purpose : Holds various modular components of a single EVEX encoded instruction.
//-------------------------------------------------------------------------
#include "../../../Include/EVEX/EVEXInst_t.h"
#include <cassert>
#include "../../../Include/Masks.h"
#include "../../Math/SafeBitWiseOps.h"


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


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t EVEX::EVEXInst_t::ModRM_Mod() const
{
    return m_modrm.ModValueAbs();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t EVEX::EVEXInst_t::ModRM_Reg() const
{
    // Bits returned are stored in R' R ModRM.Reg format,  where R and R' are inverted.
    // NOTE : We don't need Maths::SafeOr here cause values here are of type unsigned long long.
    //        So, no unexpected type promotions should take place?
    return m_modrm.RegValueAbs() | ((~m_evexPrefix.R() & 1llu) << 3llu) | ((~m_evexPrefix.Rdash() & 1llu) << 4llu);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t EVEX::EVEXInst_t::ModRM_RM () const
{
    if(m_bHasSIB == true)
    {
        assert(m_modrm.RMValueAbs() == 0b100 && "Invalid RM value set.");
        return m_modrm.RMValueAbs();
    }

    return Maths::SafeOr(m_modrm.RMValueAbs(), (~m_evexPrefix.B() & 1llu) << 3llu);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t EVEX::EVEXInst_t::SIB_Scale() const
{
    assert(m_bHasSIB == true && "SIB byte must be stored before accessing SIB.scale.");
    if(m_bHasSIB == false)
        return UINT64_MAX;

    return m_SIB.ScaleValueAbs();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t EVEX::EVEXInst_t::SIB_Index() const
{
    assert(m_bHasSIB == true && "SIB byte must be stored before accessing SIB.index.");
    if(m_bHasSIB == false)
        return UINT64_MAX;

    return Maths::SafeOr(m_SIB.IndexValueAbs(), (~m_evexPrefix.X() & 1llu) << 3llu);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t EVEX::EVEXInst_t::SIB_Base () const
{
    assert(m_bHasSIB == true && "SIB byte must be stored before accessing SIB.base.");
    if(m_bHasSIB == false)
        return UINT64_MAX;

    return Maths::SafeOr(m_SIB.BaseValueAbs(), (~m_evexPrefix.B() & 1llu) << 3llu);
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int EVEX::EVEXInst_t::GetOperandSizeInBytes() const
{
    if(m_evexPrefix.m_iPrefix == SpecialChars::EVEX_PREFIX_62)
        if(m_evexPrefix.W() != false)
            return 8;

    return 4;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t EVEX::EVEXInst_t::GetImmRegister() const
{
    assert(m_immediate.ByteCount() == 1 && "Must have an immediate byte before requesting register stored in immediate.");
    if(m_immediate.ByteCount() != 1)
        return UINT64_MAX;

    // Get upper 4 bits of the immediate, shifted for ease of use.
    return Maths::SafeAnd(m_immediate.m_immediateByte[0], VEX::Masks::IMM_REGISTER) >> 4llu;
}
