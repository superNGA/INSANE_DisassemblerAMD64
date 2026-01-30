//=========================================================================
//                      EVEX Prefix
//=========================================================================
// by      : INSANE
// created : 29/01/2026
//
// purpose : Holds EVEX prefix and allows easy access to its components.
//-------------------------------------------------------------------------
#pragma once
#include "../Aliases.h"
#include <cstdint>



namespace INSANE_DASM64_NAMESPACE::EVEX_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    struct EVEXPrefix_t
    {
        EVEXPrefix_t();
        void Clear();

        void StorePrefix(Byte iPrefix);
        void StorePayload(Byte iPayload, int iIndex);

        // Payload 1
        uint64_t R() const;
        uint64_t X() const;
        uint64_t B() const;
        uint64_t Rdash() const; // R'
        uint64_t mmm() const;

        // Payload 2
        uint64_t W() const;
        uint64_t vvvv() const;
        uint64_t pp() const;

        // Payload 3
        uint64_t z() const;
        uint64_t LL() const;
        uint64_t b() const;
        uint64_t Vdash() const;
        uint64_t aaa() const;


        Byte m_iPrefix;
        Byte m_iPayload1;
        Byte m_iPayload2;
        Byte m_iPayload3;
    };
}
