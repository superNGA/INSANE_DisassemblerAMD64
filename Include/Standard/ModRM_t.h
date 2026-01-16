//=========================================================================
//                      ModRM
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : ModRM.
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include <cstdint>


namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct ModRM_t
    {
        void Store(Byte modrm);
        Byte Get() const;
        
        // REX ignored. Bits are shifted before returning
        uint64_t ModValueAbs() const;
        uint64_t RegValueAbs() const;
        uint64_t RMValueAbs() const;

        Byte m_modrm = 0x00;
    };
}
