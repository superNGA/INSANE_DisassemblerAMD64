//=========================================================================
//                      Legacy Instruction
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Handles a single legacy encoded instruction and all of its components.
//-------------------------------------------------------------------------
#include "../../../Include/Legacy/LegacyInst_t.h"
#include "../../../Include/Masks.h"
#include "../../Math/SafeBitWiseOps.h"
#include <assert.h>


using namespace InsaneDASM64;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::Legacy::LegacyInst_t::LegacyInst_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::Legacy::LegacyInst_t::Clear()
{
    m_opCode.Clear();
    m_legacyPrefix.Clear();

    m_bHasREX    = false;
    m_iREX       = 0x00; // This is an invalid REX byte ( 0x40 to 0x4F ).
    m_iREXIndex  = -1; // This is used to check if REX preseeds the OpBytes or not. ( they must.)

    m_bHasModRM  = false;

    m_bHasSIB    = false;

    m_displacement.Clear();
    m_immediate.Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int InsaneDASM64::Legacy::LegacyInst_t::GetOperandSizeInBytes(bool bIgnoreREXW) const
{
    // REX.W == true ?
    if (m_bHasREX == true && bIgnoreREXW == false)
    {
        if (Maths::SafeAnd(m_iREX, Masks::REX_W) != false)
        {
            return 8;
        }
    }


    // Operand-size override prefix present ?
    for (int i = 0; i < m_legacyPrefix.PrefixCount(); i++)
        if (m_legacyPrefix.m_legacyPrefix[i] == 0x66)
            return 2;


    // Default operand size or 64 bit mode.
    return 4;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int InsaneDASM64::Legacy::LegacyInst_t::GetAddressSizeInBytes() const
{
    // Address-size override prefix present ?
    for (int i = 0; i < m_legacyPrefix.PrefixCount(); i++)
        if (m_legacyPrefix.m_legacyPrefix[i] == 0x67)
            return 4;

    return 8;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t InsaneDASM64::Legacy::LegacyInst_t::ModRM_Reg() const
{
    assert(m_bHasModRM == true && "Instruction doesn't have a modrm byte.");
    if (m_bHasModRM == false)
        return UINT64_MAX;


    uint64_t iReg = (Maths::SafeAnd(m_modrm.Get(), Masks::MODRM_REG) >> 3llu);
    uint64_t iREX_R = m_bHasREX == false ? 0llu : Maths::SafeAnd(m_iREX, Masks::REX_R);

    return Maths::SafeOr(iReg, (iREX_R << 1llu));
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t InsaneDASM64::Legacy::LegacyInst_t::ModRM_Mod() const
{
    assert(m_bHasModRM == true && "Instruction doesn't have a modrm byte.");
    if (m_bHasModRM == false)
        return UINT64_MAX;

    return m_modrm.ModValueAbs(); // Nothings effecting modrm.mod
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t InsaneDASM64::Legacy::LegacyInst_t::ModRM_RM() const
{
    assert(m_bHasModRM == true && "Instruction doesn't have a modrm byte.");
    if (m_bHasModRM == false)
        return UINT64_MAX;


    // if we have SIB byte, that means modrm.RM must be 0b100. No exceptions.
    if (m_bHasSIB == true)
    {
        assert(m_modrm.RMValueAbs() == 0b100 && "SIB exists for this instruction but modrm.RM != 0b100");
        return m_modrm.RMValueAbs();
    }


    uint64_t iREX_B = m_bHasREX == false ? 0llu : Maths::SafeAnd(m_iREX, Masks::REX_B);

    return Maths::SafeOr(Maths::SafeAnd(m_modrm.Get(), Masks::MODRM_RM), (iREX_B << 3llu));
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t InsaneDASM64::Legacy::LegacyInst_t::SIB_Scale() const
{
    assert(m_bHasSIB == true && "SIB byte is not present for this instruction.");
    if (m_bHasSIB == false)
        return UINT64_MAX;


    // Not REX bits affect this.
    return m_SIB.ScaleValueAbs();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t InsaneDASM64::Legacy::LegacyInst_t::SIB_Index() const
{
    assert(m_bHasSIB == true && "SIB byte is not present for this instruction.");
    if (m_bHasSIB == false)
        return UINT64_MAX;


    uint64_t iIndex = Maths::SafeAnd(m_SIB.Get(), Masks::SIB_INDEX) >> 3llu;
    uint64_t iREX_X = m_bHasREX == false ? 0llu : Maths::SafeAnd(m_iREX, Masks::REX_X);
    return Maths::SafeOr(iIndex, iREX_X << 2llu);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t InsaneDASM64::Legacy::LegacyInst_t::SIB_Base() const
{
    assert(m_bHasSIB == true && "SIB byte is not present for this instruction.");
    if (m_bHasSIB == false)
        return UINT64_MAX;


    uint64_t iREX_B = m_bHasREX == false ? 0llu : Maths::SafeAnd(m_iREX, Masks::REX_B);
    return Maths::SafeOr(Maths::SafeAnd(m_SIB.Get(), Masks::SIB_BASE), iREX_B << 2llu);
}

