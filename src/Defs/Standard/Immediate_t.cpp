//=========================================================================
//                      Immediate
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Immedaite bytes
//-------------------------------------------------------------------------
#include "../../../Include/Standard/Immediate_t.h"
#include <assert.h>



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
InsaneDASM64::Standard::Immediate_t::Immediate_t()
{
    Clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InsaneDASM64::Standard::Immediate_t::Clear()
{
    m_nImmediateBytes = 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int InsaneDASM64::Standard::Immediate_t::ByteCount() const
{
    return m_nImmediateBytes;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool InsaneDASM64::Standard::Immediate_t::PushByte(Byte iByte)
{
    assert(m_nImmediateBytes < Rules::MAX_IMMEDIATE_BYTES && "Max immediate bytes are already stored!!!");
    if (m_nImmediateBytes >= Rules::MAX_IMMEDIATE_BYTES)
        return false;

    m_immediateByte[m_nImmediateBytes] = iByte;
    m_nImmediateBytes++;

    return true;
}
