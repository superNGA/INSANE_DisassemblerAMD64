//=========================================================================
//                      VEX Prefix
//=========================================================================
// by      : INSANE
// created : 17/01/2026
//
// purpose : Holds VEX prefix and VEX bytes, and provides easy access to bit-field
//           components.
//-------------------------------------------------------------------------
#include "../../../Include/VEX/VEXPrefix_t.h"
#include <assert.h>
#include "../../Math/SafeBitWiseOps.h"
#include "../../../Include/Masks.h"


using namespace InsaneDASM64;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::VEX::VEXPrefix_t::VEXPrefix_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::VEX::VEXPrefix_t::Clear()
{
    m_nVEXBytes = 0;
    m_iPrefix   = 0x00;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::VEX::VEXPrefix_t::StorePrefix(Byte iVEXPrefix)
{
    m_iPrefix = iVEXPrefix;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::VEX::VEXPrefix_t::PushVEXBytes(Byte iVEX)
{
    // VEX prefix must be valid.
    assert((m_iPrefix == SpecialChars::VEX_PREFIX_C4 || m_iPrefix == SpecialChars::VEX_PREFIX_C5) && "Can't push VEX bytes without initializing VEX prefix.");

    // Can we store this VEX byte, respexting our VEX prefix?
    bool bOverFlow = 
        (m_iPrefix == SpecialChars::VEX_PREFIX_C5 && m_nVEXBytes >= 1) || 
        (m_iPrefix == SpecialChars::VEX_PREFIX_C4 && m_nVEXBytes >= Rules::MAX_VEX_PREFIX_BYTES);

    assert(bOverFlow == false && "Can't store anymore VEX bytes.");
    if(bOverFlow == true)
        return;

    m_iVEX[m_nVEXBytes] = iVEX;
    m_nVEXBytes++;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int InsaneDASM64::VEX::VEXPrefix_t::VEXByteCount()
{
    assert(m_nVEXBytes >= 0 && m_nVEXBytes <= 2 && "Impossible VEX Byte count. Something went wrong.");
    return m_nVEXBytes;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Byte InsaneDASM64::VEX::VEXPrefix_t::GetPrefix()
{
    assert((m_iPrefix == SpecialChars::VEX_PREFIX_C4 || m_iPrefix == SpecialChars::VEX_PREFIX_C5) && "VEX prefix is not set, or set to a invalid value.");
    return m_iPrefix;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t InsaneDASM64::VEX::VEXPrefix_t::m_mmmm() const
{
    assert(m_iPrefix   == SpecialChars::VEX_PREFIX_C4 && "This VEX prefix does not have the correct VEX prefix.");
    assert(m_nVEXBytes == Rules::MAX_VEX_PREFIX_BYTES && "This instrution does not have any VEX bytes.");
    if(m_iPrefix != SpecialChars::VEX_PREFIX_C4 || m_nVEXBytes != Rules::MAX_VEX_PREFIX_BYTES)
        return UINT64_MAX;

    return Maths::SafeAnd(m_iVEX[0], Masks::M_MMMM);
}  


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t InsaneDASM64::VEX::VEXPrefix_t::vvvv() const
{
    assert((m_iPrefix == SpecialChars::VEX_PREFIX_C4 && m_nVEXBytes < Rules::MAX_VEX_PREFIX_BYTES) == false && "This instruction does not have valid VEX bytes.");
    assert((m_iPrefix == SpecialChars::VEX_PREFIX_C5 && m_nVEXBytes <= 0                         ) == false && "This instruction does not have valid VEX bytes.");

    if(m_iPrefix == SpecialChars::VEX_PREFIX_C4)
    {
        if(m_nVEXBytes < Rules::MAX_VEX_PREFIX_BYTES)
        {
            return UINT64_MAX;
        }
        else 
        {
            return Maths::SafeAnd(m_iVEX[1], Masks::VVVV) >> 3llu;
        }
    }
    else if(m_iPrefix == SpecialChars::VEX_PREFIX_C5)
    {
        if(m_nVEXBytes <= 0 )
        {
            return UINT64_MAX;
        }
        else 
        {
            return Maths::SafeAnd(m_iVEX[0], Masks::VVVV) >> 3llu;
        }
    }

    return UINT64_MAX;
}  


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t InsaneDASM64::VEX::VEXPrefix_t::L() const
{
    assert((m_iPrefix == SpecialChars::VEX_PREFIX_C4 && m_nVEXBytes < Rules::MAX_VEX_PREFIX_BYTES) == false && "This instruction does not have valid VEX bytes.");
    assert((m_iPrefix == SpecialChars::VEX_PREFIX_C5 && m_nVEXBytes <= 0                         ) == false && "This instruction does not have valid VEX bytes.");

    if(m_iPrefix == SpecialChars::VEX_PREFIX_C4)
    {
        if(m_nVEXBytes < Rules::MAX_VEX_PREFIX_BYTES)
        {
            return UINT64_MAX;
        }
        else 
        {
            return Maths::SafeAnd(m_iVEX[1], Masks::L) >> 2llu;
        }
    }
    else if(m_iPrefix == SpecialChars::VEX_PREFIX_C5)
    {
        if(m_nVEXBytes <= 0 )
        {
            return UINT64_MAX;
        }
        else 
        {
            return Maths::SafeAnd(m_iVEX[0], Masks::L) >> 2llu;
        }
    }

    return UINT64_MAX;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t InsaneDASM64::VEX::VEXPrefix_t::pp() const
{
    assert((m_iPrefix == SpecialChars::VEX_PREFIX_C4 && m_nVEXBytes < Rules::MAX_VEX_PREFIX_BYTES) == false && "This instruction does not have valid VEX bytes.");
    assert((m_iPrefix == SpecialChars::VEX_PREFIX_C5 && m_nVEXBytes <= 0                         ) == false && "This instruction does not have valid VEX bytes.");

    if(m_iPrefix == SpecialChars::VEX_PREFIX_C4)
    {
        if(m_nVEXBytes != Rules::MAX_VEX_PREFIX_BYTES)
        {
            return UINT64_MAX;
        }
        else 
        {
            return Maths::SafeAnd(m_iVEX[1], Masks::PP);
        }
    }
    else if(m_iPrefix == SpecialChars::VEX_PREFIX_C5)
    {
        if(m_nVEXBytes <= 0 )
        {
            return UINT64_MAX;
        }
        else 
        {
            return Maths::SafeAnd(m_iVEX[0], Masks::PP);
        }
    }

    return UINT64_MAX;
}  


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t InsaneDASM64::VEX::VEXPrefix_t::R() const
{
    assert(m_nVEXBytes >= 1 && "This intruction does not have sufficient VEX bytes.");
    if(m_nVEXBytes <= 0)
        return UINT64_MAX;

    return Maths::SafeAnd(m_iVEX[0], Masks::R) >> 7llu;
}  


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t InsaneDASM64::VEX::VEXPrefix_t::X() const
{
    assert(m_iPrefix   == SpecialChars::VEX_PREFIX_C4 && "This VEX prefix does not have the correct VEX prefix.");
    assert(m_nVEXBytes == Rules::MAX_VEX_PREFIX_BYTES && "This instrution does not have any VEX bytes.");
    if(m_iPrefix != SpecialChars::VEX_PREFIX_C4 || m_nVEXBytes != Rules::MAX_VEX_PREFIX_BYTES)
        return UINT64_MAX;

    return Maths::SafeAnd(m_iVEX[0], Masks::X) >> 6llu;
}  


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t InsaneDASM64::VEX::VEXPrefix_t::B() const
{
    assert(m_iPrefix   == SpecialChars::VEX_PREFIX_C4 && "This VEX prefix does not have the correct VEX prefix.");
    assert(m_nVEXBytes == Rules::MAX_VEX_PREFIX_BYTES && "This instrution does not have any VEX bytes.");
    if(m_iPrefix != SpecialChars::VEX_PREFIX_C4 || m_nVEXBytes != Rules::MAX_VEX_PREFIX_BYTES)
        return UINT64_MAX;

    return Maths::SafeAnd(m_iVEX[0], Masks::B) >> 5llu;
}  


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint64_t InsaneDASM64::VEX::VEXPrefix_t::W() const
{
    assert(m_iPrefix   == SpecialChars::VEX_PREFIX_C4 && "This VEXPrefix_t does not have the correct VEX prefix set.");
    assert(m_nVEXBytes == Rules::MAX_VEX_PREFIX_BYTES && "This instrution does not have any VEX bytes.");
    if(m_iPrefix != SpecialChars::VEX_PREFIX_C4 || m_nVEXBytes != Rules::MAX_VEX_PREFIX_BYTES)
        return UINT64_MAX;

    return Maths::SafeAnd(m_iVEX[1], Masks::W) >> 7llu;
}  
