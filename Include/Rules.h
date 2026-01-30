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
    constexpr uint64_t   MAX_OPERANDS             = 4llu;
    constexpr uint64_t   MAX_LEGACY_PREFIX        = 4llu;
    constexpr uint64_t   MAX_OPBYTES              = 3llu;
    constexpr uint64_t   MAX_DISPLACEMENT_BYTES   = 4llu;
    constexpr uint64_t   MAX_IMMEDIATE_BYTES      = 8llu;
    constexpr uint64_t   MAX_INST_NAME_SIZE       = 0x18llu;
    constexpr uint64_t   MAX_VEX_PREFIX_BYTES     = 2llu; // Excluding the prefix byte ( 0xC4 0xC5 )

    constexpr uint64_t   MIN_VEX_INST_BYTES       = 3llu;

    constexpr uint64_t   MIN_EVEX_INST_BYTES      = 6llu;

    inline const char* OPCODE_NAME_SENTINAL     = "xx_INVALID_xx";
    inline const char* REGISTER_NAME_SENTINAL   = "xx_INVALID_REG_xx";


    constexpr uint64_t   DASMINST_ARG_BUFFER_SIZE = 0x28llu;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace INSANE_DASM64_NAMESPACE::SpecialChars
{
    // Acts as escape opcode character when appears as first byte.
    constexpr Byte ESCAPE_OPCODE_FIRST_INDEX     = 0x0F;

    // Acts as escape opcode character when appears as second byte.
    constexpr Byte ESCAPE_OPCODE_SECOND_INDEX_38 = 0x38;
    constexpr Byte ESCAPE_OPCODE_SECOND_INDEX_3A = 0x3A;


    // First byte for VEX encoded instructions.
    constexpr Byte VEX_PREFIX_C4                 = 0xC4;
    constexpr Byte VEX_PREFIX_C5                 = 0xC5;

    // Mandatory first byte of EVEX encoded instructions...
    constexpr Byte EVEX_PREFIX_62                = 0x62;

    // Prefix represented by vex.pp 
    constexpr uint64_t VEX_PP_PREFIX_66 = 0b01;
    constexpr uint64_t VEX_PP_PREFIX_F3 = 0b10;
    constexpr uint64_t VEX_PP_PREFIX_F2 = 0b11;
}
