//=========================================================================
//                      Immediate
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Immedaite bytes
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include "../Rules.h"


namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct Immediate_t
    {
        Immediate_t();
        void Clear();

        int  ByteCount() const;
        bool PushByte(Byte iByte);

        Byte m_immediateByte[Rules::MAX_IMMEDIATE_BYTES];
        int  m_nImmediateBytes = 0;
    };
}
