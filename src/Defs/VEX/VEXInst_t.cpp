//=========================================================================
//                      VEX Instruction
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Handles a single VEX encoded instruction and all its components.
//-------------------------------------------------------------------------
#include "../../../Include/VEX/VEXInst_t.h"
#include <assert.h>
#include "../../Math/SafeBitWiseOps.h"
#include "../../../Include/Masks.h"



using namespace InsaneDASM64;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
VEX::VEXInst_t::VEXInst_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void VEX::VEXInst_t::Clear()
{
    m_vexPrefix.Clear();
    m_opcode.Clear();
    m_bHasSIB       = false;
    m_disp.Clear();
    m_immediate.Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t VEX::VEXInst_t::GetImmRegister() const
{
    assert(m_immediate.ByteCount() == 1 && "Must have an immediate byte before requesting register stored in immediate.");
    if(m_immediate.ByteCount() != 1)
        return UINT64_MAX;

    return Maths::SafeAnd(m_immediate.m_immediateByte[0], Masks::IMM_REGISTER) >> 4llu;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t VEX::VEXInst_t::ModRM_Mod() const
{
    return m_modrm.ModValueAbs();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t VEX::VEXInst_t::ModRM_Reg() const
{
    return Maths::SafeOr(m_modrm.RegValueAbs(), (~m_vexPrefix.R() & 1llu) << 3llu);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t VEX::VEXInst_t::ModRM_RM () const
{
    if(m_bHasSIB == true)
    {
        assert(m_modrm.RMValueAbs() == 0b100 && "Invalid RM value set.");
        return m_modrm.RMValueAbs();
    }

    // In case we have 2 VEX bytes, we need to extend the RM using VEX.B
    if(m_vexPrefix.m_iPrefix == SpecialChars::VEX_PREFIX_C4)
        return Maths::SafeOr(m_modrm.RMValueAbs(), (~m_vexPrefix.B() & 1llu) << 3llu);

    return m_modrm.RMValueAbs();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t VEX::VEXInst_t::SIB_Scale() const
{
    assert(m_bHasSIB == true && "SIB byte must be stored before accessing SIB.scale.");
    if(m_bHasSIB == false)
        return UINT64_MAX;

    return m_SIB.ScaleValueAbs();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t VEX::VEXInst_t::SIB_Index() const
{
    assert(m_bHasSIB == true && "SIB byte must be stored before accessing SIB.index.");
    if(m_bHasSIB == false)
        return UINT64_MAX;

    // In case we have 2 VEX bytes, we need to extend the SIB.index using VEX.B
    if(m_vexPrefix.m_iPrefix == SpecialChars::VEX_PREFIX_C4)
        return Maths::SafeOr(m_SIB.IndexValueAbs(), (~m_vexPrefix.X() & 1llu) << 3llu);

    return m_SIB.IndexValueAbs();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t VEX::VEXInst_t::SIB_Base () const
{
    assert(m_bHasSIB == true && "SIB byte must be stored before accessing SIB.base.");
    if(m_bHasSIB == false)
        return UINT64_MAX;

    // In case we have 2 VEX bytes, we need to extend the SIB.Base using VEX.B
    if(m_vexPrefix.m_iPrefix == SpecialChars::VEX_PREFIX_C4)
        return Maths::SafeOr(m_SIB.BaseValueAbs(), (~m_vexPrefix.B() & 1llu) << 3llu);

    return m_SIB.BaseValueAbs();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int VEX::VEXInst_t::GetOperandSizeInBytes() const
{
    if(m_vexPrefix.m_iPrefix == SpecialChars::VEX_PREFIX_C4)
        if(m_vexPrefix.W() != false)
            return 8;

    return 4;
}
