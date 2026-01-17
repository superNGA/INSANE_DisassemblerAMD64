//=========================================================================
//                      Rules
//=========================================================================
// by      : INSANE
// created : 15/01/2026
//
// purpose : Just some constants, to help decoding and disassembling.
//-------------------------------------------------------------------------
#pragma once
#include "Aliases.h"
#include <cstdint>



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace INSANE_DASM64_NAMESPACE::Rules
{
    constexpr size_t   MAX_OPERANDS             = 4llu;
    constexpr size_t   MAX_LEGACY_PREFIX        = 4llu;
    constexpr size_t   MAX_OPBYTES              = 3llu;
    constexpr size_t   MAX_DISPLACEMENT_BYTES   = 4llu;
    constexpr size_t   MAX_IMMEDIATE_BYTES      = 8llu;
    constexpr size_t   MAX_INST_NAME_SIZE       = 0x18llu;
    constexpr size_t   MAX_VEX_PREFIX_BYTES     = 2llu; // Excluding the prefix byte ( 0xC4 0xC5 )

    constexpr size_t   MIN_VEX_INST_BYTES       = 3llu;

    inline const char* OPCODE_NAME_SENTINAL     = "xx_INVALID_xx";
    inline const char* REGISTER_NAME_SENTINAL   = "xx_INVALID_REG_xx";


    constexpr size_t   DASMINST_ARG_BUFFER_SIZE = 0x28llu;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace INSANE_DASM64_NAMESPACE::SpecialChars
{
    // Acts as escape opcode character when appears as first byte.
    constexpr Byte ESCAPE_OPCODE_FIRST_INDEX     = 0X0F;

    // Acts as escape opcode character when appears as second byte.
    constexpr Byte ESCAPE_OPCODE_SECOND_INDEX_38 = 0X38;
    constexpr Byte ESCAPE_OPCODE_SECOND_INDEX_3A = 0X3A;


    // First byte for VEX encoded instructions.
    constexpr Byte VEX_PREFIX_C4                 = 0XC4;
    constexpr Byte VEX_PREFIX_C5                 = 0XC5;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace InsaneDASM64::Masks
{
    constexpr size_t MODRM_MOD = 0b11000000llu;
    constexpr size_t MODRM_REG = 0b111000llu;
    constexpr size_t MODRM_RM  = 0b111llu;

    constexpr size_t SIB_SCALE = 0b11000000llu;
    constexpr size_t SIB_INDEX = 0b111000llu;
    constexpr size_t SIB_BASE  = 0b111llu;

    constexpr size_t REX_W     = 0b1000llu;
    constexpr size_t REX_R     = 0b100llu;
    constexpr size_t REX_X     = 0b10llu;
    constexpr size_t REX_B     = 0b1llu;
}
