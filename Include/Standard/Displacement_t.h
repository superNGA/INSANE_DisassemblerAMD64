//=========================================================================
//                      Displacement
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Displacement bytes.
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include "../Rules.h"



namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct Displacement_t
    {
        Displacement_t();

        void Clear();
        int  ByteCount() const;
        bool PushByte(Byte iByte);


        Byte m_iDispBytes[Rules::MAX_DISPLACEMENT_BYTES];
        int  m_nDispBytes = 0;
    };
}
