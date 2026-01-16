//=========================================================================
//                      Displacement
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Displacement bytes.
//-------------------------------------------------------------------------
#include "../../../Include/Standard/Displacement_t.h"
#include <assert.h>



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::Standard::Displacement_t::Displacement_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::Standard::Displacement_t::Clear()
{
    m_nDispBytes = 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int InsaneDASM64::Standard::Displacement_t::ByteCount() const
{
    return m_nDispBytes;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool InsaneDASM64::Standard::Displacement_t::PushByte(Byte iByte)
{
    assert(m_nDispBytes < Rules::MAX_DISPLACEMENT_BYTES && "Maximum displcament bytes already stored!");
    if (m_nDispBytes >= Rules::MAX_DISPLACEMENT_BYTES)
        return false;

    m_iDispBytes[m_nDispBytes] = iByte; 
    m_nDispBytes++;
    return true;
}
