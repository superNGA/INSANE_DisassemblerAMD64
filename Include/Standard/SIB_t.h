//=========================================================================
//                      SIB
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : SIB
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include <cstdint>



namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct SIB_t
    {
        void Store(Byte SIB);
        Byte Get() const;
        
        // REX ignored. Bits are shifted before returning. 
        uint64_t ScaleValueAbs() const;
        uint64_t IndexValueAbs() const;
        uint64_t BaseValueAbs() const;

        Byte m_SIB = 0x00;
    };
}
