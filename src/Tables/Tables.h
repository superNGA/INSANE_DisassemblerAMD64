//=========================================================================
//                      Tables
//=========================================================================
// by      : INSANE
// created : 13/12/2025
// 
// purpose : Holds Look-up-tables of instruction information.
//-------------------------------------------------------------------------
#pragma once
#include <cstdint>

#include "../../Include/INSANE_DisassemblerAMD64.h"
#include "../../Include/Aliases.h"
#include "../../Include/Standard/Operand/Operand_t.h"


namespace INSANE_DASM64_NAMESPACE::STANDARD_NAMESPACE{ struct OpCodeDesc_t; }

namespace INSANE_DASM64_NAMESPACE
{

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    enum InstTypes_t : uint16_t
    {
        InstTypes_LegacyPrefixGrp1   = (1 << 0),
        InstTypes_LegacyPrefixGrp2   = (1 << 1),
        InstTypes_LegacyPrefixGrp3   = (1 << 2),
        InstTypes_LegacyPrefixGrp4   = (1 << 3),

        // All legacy prefixies.
        InstTypes_LegacyPrefixGrpAll =
            InstTypes_LegacyPrefixGrp1 | InstTypes_LegacyPrefixGrp2 | InstTypes_LegacyPrefixGrp3 | InstTypes_LegacyPrefixGrp4,

        InstTypes_REX                = (1 << 4),

        InstTypes_OpCode             = (1 << 5),

        // For AVX opcodes. According to their leading opcodes.
        InstTypes_AVXLeadBy0x0F      = (1 << 6),
        InstTypes_AVXLeadBy0x38      = (1 << 7),
        InstTypes_AVXLeadBy0x3A      = (1 << 8),
    };


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    class Tables_t
    {
    public:
        Tables_t();

        INSANE_DASM64_NAMESPACE::IDASMErrorCode_t Initialize();
        bool          IsInitialized() const;
        uint16_t      GetInstType(Byte iOpCode) const;
        Standard::OpCodeDesc_t* GetOpCodeTable(int iTableIndex, Byte iEscapeByte);
        Standard::OpCodeDesc_t* GetVEXOpCodeTable(Byte iEscapeByte);
        Standard::OpCodeDesc_t* GetEVEXOpCodeTable(Byte iEscapeByte);


        // To associate all leagacy prefix with simple consecutive numbers.
        // Returns 0 on default case.
        int GetLegacyPrefixIndex(Byte iByte);


    private:
        InsaneDASM64::IDASMErrorCode_t _InitializeInstTypeLUT();
        uint16_t                       m_instTypeLUT[0x100llu]; // 256 entries...
        bool                           m_bInstTypeLUTInit = false;


        // Tables... ( Legacy encoded instructions. )
        InsaneDASM64::IDASMErrorCode_t _InitializeOpCodeTable();
        bool                           m_bOpCodeTableInit = false;

        void                           _InitOneByteOpCodeTable();
        Standard::OpCodeDesc_t*         m_opCodeTable1;

        void                           _InitTwoByteOpCodeTable();
        Standard::OpCodeDesc_t*         m_opCodeTable2;

        void                           _InitThreeByteOpCodeTable_38();
        Standard::OpCodeDesc_t*         m_opCodeTable3_38;

        void                           _InitThreeByteOpCodeTable_3A();
        Standard::OpCodeDesc_t*         m_opCodeTable3_3A;


        // Tables... ( VEX encoded instuctions. )
        INSANE_DASM64_NAMESPACE::IDASMErrorCode_t _InitializeVEXOpCodeTables();
        bool                           m_bVEXOpCodeTables = false;

        void                           _InitVEXTwoByteOpCodes();
        Standard::OpCodeDesc_t*        m_VEXTwoByteOpCodes;

        void                           _InitVEXThreeByteOpCodes_38();
        Standard::OpCodeDesc_t*        m_VEXThreeByteOpCodes_38;

        void                           _InitVEXThreeByteOpCodes_3A();
        Standard::OpCodeDesc_t*        m_VEXThreeByteOpCodes_3A;
        


        // Tables... ( VEX encoded instuctions. )
        // These tables are all the instructions from the linux kernel's x86 opcode map
        // which either start with a 'v' and have no AVX superscript or have (evo), (ev), (es) AVX superscript
        INSANE_DASM64_NAMESPACE::IDASMErrorCode_t _InitializeEVEXOpCodeTables();
        bool                           m_bEVEXOpCodeTables = false;

        void                           _InitEVEXTwoByteOpCodes();
        Standard::OpCodeDesc_t*        m_EVEXTwoByteOpCodes;

        void                           _InitEVEXThreeByteOpCodes_38();
        Standard::OpCodeDesc_t*        m_EVEXThreeByteOpCodes_38;

        void                           _InitEVEXThreeByteOpCodes_3A();
        Standard::OpCodeDesc_t*        m_EVEXThreeByteOpCodes_3A;
    };
    DEFINE_GLOBAL_OBJECT(g_tables, Tables_t)

}
