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
#include "../ObjectNomenclature.h"


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
    struct OpCodeDesc_t
    {
        OpCodeDesc_t() { Reset(); }

        inline void Reset()
        {

            m_iVarientType   = VarientKey_None;
            m_nVarients      = 0;
            m_pVarients      = nullptr;
            strcpy_s(m_szName, sizeof(m_szName), Rules::OPCODE_NAME_SENTINAL);
            m_bIsValidCode   = false;
            m_bIsEscapeCode  = false;
            m_bModrmRequired = false;
            m_iByte          = 0x00;
            m_nOperands      = 0;
        }


        // NOTE :
        //      VarientKey tells us, if this operands has others varients or not ( as in the case with 0x0f 0x00, 0x0f 0x01, etc... ).
        // And how to access the other varients. Pointer to all varients are stored inside the m_pVarients ( array ).
        // 
        //      Ex. VarientKey_ModRM_REG, means m_pVarients[modrm.REG] will gives us all different 
        // varients of this instruction.
        enum VarientType_t
        {
            VarientKey_None = -1,
            VarientKey_ModRM_REG = 0,
            VarientKey_ModRM_RM,
            VarientKey_ModRM_MOD,
            VarientKey_LegacyPrefix, // NOTE : Run legacy prefix through "the" look-up-table before using as index in m_pVarients.
        };


        // Max varients that can be stored in each of the varient types...
        static const size_t MAX_REG_VARIENTS           = 8llu;
        static const size_t MAX_MOD_VARIENTS           = 4llu;
        static const size_t MAX_RM_VARIENTS            = 8llu;
        static const size_t MAX_LEGACY_PREFIX_VARIENTS = 12llu;

        size_t GetMaxVarients(VarientType_t iVarientType);


        void Init(
            const char* szName, bool bValidOpcd, bool bEscapeOpcd, bool bModrmRequired, Byte iByte, 
            int nOperands, Operand_t operand1, Operand_t operand2, Operand_t operand3, Operand_t operand4);

        bool InsertVarient(int iIndex);

        // Initialize struct for one varient type ( now it doesn't hold any instruction info by itself, just varient array. )
        bool InitVarientType(VarientType_t iVarientType);


        // Braching info, to reach varients of this opcode.
        VarientType_t  m_iVarientType   = VarientKey_None;
        int            m_nVarients      = 0;
        OpCodeDesc_t** m_pVarients      = nullptr;

        // OpCode's info...
        char           m_szName[Rules::MAX_INST_NAME_SIZE];
        bool           m_bIsValidCode   = false;
        bool           m_bIsEscapeCode  = false;
        bool           m_bModrmRequired = false;
        Byte           m_iByte          = 0x00;

        // Operands...
        Operand_t      m_operands[Rules::MAX_OPERANDS];
        int            m_nOperands      = 0;
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
        OpCodeDesc_t* GetOpCodeTable(int iTableIndex, Byte iEscapeByte);


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
        OpCodeDesc_t                   m_opCodeTable1   [0x100llu];

        void                           _InitTwoByteOpCodeTable();
        OpCodeDesc_t                   m_opCodeTable2   [0x100llu];

        void                           _InitThreeByteOpCodeTable_38();
        OpCodeDesc_t                   m_opCodeTable3_38[0x100llu];

        void                           _InitThreeByteOpCodeTable_3A();
        OpCodeDesc_t                   m_opCodeTable3_3A[0x100llu];


        // Tables... ( VEX encoded instuctions. )
        INSANE_DASM64_NAMESPACE::IDASMErrorCode_t _InitializeVEXOpCodeTables();
        bool                           m_bVEXOpCodeTables = false;

        void                           _InitVEXTwoByteOpCodes();
        OpCodeDesc_t                   m_VEXTwoByteOpCodes[0x100llu];

        void                           _InitVEXThreeByteOpCodes_38();
        OpCodeDesc_t                   m_VEXThreeByteOpCodes_38[0x100llu];

        void                           _InitVEXThreeByteOpCodes_3A();
        OpCodeDesc_t                   m_VEXThreeByteOpCodes_3A[0x100llu];
    };
    DEFINE_GLOBAL_OBJECT(g_tables, Tables_t)

}
