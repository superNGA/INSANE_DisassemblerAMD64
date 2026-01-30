//=========================================================================
//                      Marks
//=========================================================================
// by      : INSANE
// created : 17/01/2026
//
// purpose : Marks for accessing bitfields across many intruction encoding types.
//-------------------------------------------------------------------------
#pragma once
#include "Aliases.h"
#include <cstdint>



namespace INSANE_DASM64_NAMESPACE
{
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    namespace LEGACY_NAMESPACE::Masks
    {
        constexpr uint64_t MODRM_MOD = 0b11000000llu;
        constexpr uint64_t MODRM_REG = 0b111000llu;
        constexpr uint64_t MODRM_RM  = 0b111llu;

        constexpr uint64_t SIB_SCALE = 0b11000000llu;
        constexpr uint64_t SIB_INDEX = 0b111000llu;
        constexpr uint64_t SIB_BASE  = 0b111llu;

        constexpr uint64_t REX_W     = 0b1000llu;
        constexpr uint64_t REX_R     = 0b100llu;
        constexpr uint64_t REX_X     = 0b10llu;
        constexpr uint64_t REX_B     = 0b1llu;
    }


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    namespace VEX_NAMESPACE::Masks
    {
        constexpr uint64_t M_MMMM = 0b11111;
        constexpr uint64_t VVVV   = 0b1111000;
        constexpr uint64_t PP     = 0b11;
        constexpr uint64_t L      = 0b100;
        constexpr uint64_t W      = 0b10000000;
        constexpr uint64_t R      = 0b10000000;
        constexpr uint64_t X      = 0b1000000;
        constexpr uint64_t B      = 0b100000;
        // This mask is used to extract the upper 4 bits from the immediate, which hold the register index in case of IXY operand type.
        constexpr uint64_t IMM_REGISTER = 0b11110000; 
    }


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    namespace EVEX_NAMESPACE::Masks
    {
        // Payload 1...
        constexpr uint64_t R = 0b10000000;
        constexpr uint64_t X = 0b1000000;
        constexpr uint64_t B = 0b100000;
        constexpr uint64_t R_DASH = 0b10000;
        constexpr uint64_t MMM = 0b111;

        // Payload 2...
        constexpr uint64_t W = 0b10000000;
        constexpr uint64_t VVVV = 0b1111000;
        constexpr uint64_t PP = 0b11;

        // Payload 3...
        constexpr uint64_t Z = 0b10000000;
        constexpr uint64_t LL = 0b1100000;
        constexpr uint64_t b = 0b10000;
        constexpr uint64_t V = 0b1000;
        constexpr uint64_t AAA = 0b111;
    }
}
