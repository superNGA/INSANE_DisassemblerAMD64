//=========================================================================
//                      Legacy Prefix
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Stores / Handles legacy prefixies for a single instruction.
//-------------------------------------------------------------------------
#include "../../../Include/Legacy/LegacyPrefix.h"
#include <assert.h>


using namespace InsaneDASM64;



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::Legacy::LegacyPrefix_t::LegacyPrefix_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::Legacy::LegacyPrefix_t::Clear()
{
    m_nPrefix = 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool InsaneDASM64::Legacy::LegacyPrefix_t::PushPrefix(Byte iByte)
{
    assert(m_nPrefix < Rules::MAX_LEGACY_PREFIX && "Prefix list is already full!!!");
    
    if (m_nPrefix >= Rules::MAX_LEGACY_PREFIX)
        return false;

    m_legacyPrefix[m_nPrefix] = iByte;
    m_nPrefix++;

    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int InsaneDASM64::Legacy::LegacyPrefix_t::PrefixCount() const
{
    return m_nPrefix;
}
