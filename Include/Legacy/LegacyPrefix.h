//=========================================================================
//                      Legacy Prefix
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Stores / Handles legacy prefixies for a single instruction.
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include "../Rules.h"



namespace INSANE_DASM64_NAMESPACE::LEGACY_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct LegacyPrefix_t
    {
        LegacyPrefix_t();

        void Clear();
        bool PushPrefix(Byte iByte);
        int  PrefixCount() const;

        int32_t m_nPrefix = 0;
        Byte    m_legacyPrefix[Rules::MAX_LEGACY_PREFIX];
    };
}
