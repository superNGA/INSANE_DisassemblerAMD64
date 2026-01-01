// TODO Document this bullshit.
// BY : INSANE december of 2025


//x TODO: Store Bytes in each entry.
//x TODO: Don't print invalid entries.
//x TODO 0x0f 0x01 ain't getting registerd / printed.
// TODO level1 explicit nomem fix


#include <unordered_map>
#include <vector>
#include <iostream>
#include <assert.h>
#include <fstream>
#include <format>
#include <string>
#include "tinyxml2.h"


#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[93m"
#define RESET   "\033[0m"
#define CYAN    "\033[96m"
#define BAD_OPERAND_TYPE "\033[30m\033[47m"


typedef unsigned char Byte;
using namespace tinyxml2;


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::unordered_map<std::string, std::string> g_mapAddressingModeLinker = 
{
    {"A", "ptr"},
    {"BA", "m"}, {"BB", "m"}, {"BD", "m"},
    {"C", "CRn"},
    {"D", "DRn"},
    {"E", "rm"}, {"ES", "STim"}, {"EST", "STi"},
    {"F", "!"},
    {"G", "r"},
    {"H", "r"}, // ref x86 added this, not present in official intel's manual.
    {"I", "imm"},
    {"J", "rel"},
    // {"K", "!"}, // K doesn't exist in either of the refrences.
    {"M", "m"},
    {"N", "mm"},
    {"O", "moffs"},
    {"P", "mm"},
    {"Q", "mmm64"},
    {"R", "r"},
    {"S", "Sreg"},
    {"SC", "!"},
    {"T", "TRn"}, // No intel equivalent exist for this addressing method.
    {"U", "xmm"},
    {"V", "xmm"},
    {"W", "xmmm"},
    {"X", "m"},
    {"Y", "m"},
    {"Z", "r"} // custom addressing mode by ref x86. Not present in intel's manual.
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::unordered_map<std::string, std::string> g_mapOperandTypeLinker = 
{
    {"a", "16or32_twice"},
    {"b", "8"}, {"bcd", "80dec"}, {"bs", "8"}, {"bsq", "Invalid"}, {"bss", "8"},
    {"c", "Invalid"},
    {"d", "32"}, {"di", "32int"}, {"dqp", "32_64"}, {"ds", "32"},
    {"dq", "128"},
    {"dr", "64real"}, // No equivalent in intel's manual.
    {"e", "14_28"}, {"er", "80real"}, // Not equivalent in intel's manual.
    {"p", "p"}, 
    {"pi", "64mmx"}, 
    {"pd", "Invalid"}, 
    {"ps", "128pf"},
    {"psq", "64"},
    {"pt", "Invalid"},
    {"ptp", "ptp"},
    {"q", "64"},
    {"qi", "64int"},
    {"qp", "64"},
    // Note : Nothing seems to be connecting to the qq operandtype ( 256 bits ).
    {"s", "Invalid"}, {"sd", "Invalid"}, {"si", "Invalid"},
    {"ss", "Invalid"},
    {"sr", "32real"}, {"st", "94_108"}, {"stx", "512"},
    {"t", "Invalid"},
    {"v", "16_32"}, {"vds", "16_32"}, {"vs", "16_32"}, {"vqp", "16_32_64"},
    {"vq", "64_16"},
    {"w", "16"}, {"wi", "16int"}
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
struct EntryID_t
{
    EntryID_t(uint64_t ID = 0llu) { m_iID = ID; }
    EntryID_t(int iOpcdExt, bool bNoMem, int iSecOpcd, int iPrefix) { Set(iOpcdExt, bNoMem, iSecOpcd, iPrefix); }
    EntryID_t(const EntryID_t& x) { m_iID = x.m_iID; }
    
    inline void Clear() { m_iID = 0llu; }

    // Extended SecOpcd in case of 3 byte opcode table. Cause in case of 3 byte opcode talbe
    // secOpcd is used as the 3rd opcde byte and can range from 0x00 to 0xFF
    void Set(int iOpcdExt, bool bNoMem, int iSecOpcd, int iPrefix, bool bExtendedSecOpcd = false)
    {
        // 0 - 7, prefix.
        m_iID = static_cast<uint64_t>(iPrefix);

        uint64_t iStartBitIndex = 8;
        m_iID |= (bNoMem == true ? 1llu : 0llu) << iStartBitIndex; iStartBitIndex += 1llu;
        m_iID |= static_cast<uint64_t>(iOpcdExt & 0b111) << iStartBitIndex; iStartBitIndex += 3llu;
        m_iID |= static_cast<uint64_t>(iSecOpcd & (/*bExtendedSecOpcd == false ? 0b111 : */0xFF)) << iStartBitIndex; iStartBitIndex += 3llu;
    }


    bool operator==(EntryID_t other) const
    {
        return m_iID == other.m_iID;
    }
    void operator=(const EntryID_t& other)
    {
        m_iID = other.m_iID;
    }


    uint64_t m_iID = 0;
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
#define LEVEL1                "vecLevel1"
#define LEVEL1_NOMEM          "vecLevel1NoMem"
#define LEVEL1_PREFIX         "vecLevel1Prefix"
#define LEVEL2_MEM            "vecLevel2Mem"
#define LEVEL2_MEM_PREFIX     "vecLevel2MemPrefix"
#define LEVEL2_NOMEM          "vecLevel2NoMem"
#define LEVEL2_SECOPCD        "vecLevel2SecOpcd"
#define LEVEL2_SECOPCD_PREFIX "vecLevel2SecOpcdPrefix"

struct CollisionSolution_t
{
    CollisionSolution_t()
    {
        m_iEntryID.Clear();
        m_iByte       = 0x00;
        m_iTableID    = -1;
        m_iSkipsLeft  = 0;
        m_szGroupName = nullptr;
    }

    CollisionSolution_t(int iByte, int iTableID, EntryID_t iEntryID, int nSkips, const char* szGroupName) :
        m_iByte(iByte), m_iTableID(iTableID), m_iEntryID(iEntryID), m_iSkipsLeft(nSkips), m_szGroupName(szGroupName)
    {}

    int         m_iByte       = 0x00;
    int         m_iTableID    = -1;
    EntryID_t   m_iEntryID; // ID of entry, to consider in the index.
    
    // if Skips Left != 0, we keep the already stored entry and discard the new one.
    // if skips Left == 0, we discrad the old one with the same ID and keep the lastest one.
    // So skips left, lets use skip 'n' number of entries before storing the 'n+1'th and keep it.
    // Or we can say, skips left, lets use stored 'm_iSkipsLeft'th index entry and discard the rest.
    int         m_iSkipsLeft  = 0;   
    const char* m_szGroupName = nullptr;
};

CollisionSolution_t g_collisionSolutionTable[] = 
{
    //                  Byte,   Table ID,  Entry ID,     Entry Index,  GroupName
    CollisionSolution_t(0x20,   2,         EntryID_t(0), 3,            LEVEL1),
    CollisionSolution_t(0x21,   2,         EntryID_t(0), 3,            LEVEL1),
    CollisionSolution_t(0x22,   2,         EntryID_t(0), 3,            LEVEL1),
    CollisionSolution_t(0x23,   2,         EntryID_t(0), 3,            LEVEL1),
    CollisionSolution_t(0x90,   1,         EntryID_t(0), 1,            LEVEL1),
};
const size_t g_iCollisionSolTableSize = sizeof(g_collisionSolutionTable) / sizeof(CollisionSolution_t);




///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
struct Entry_t
{
    void Clear()
    {
        m_szName = "xx_INVALID_xx";
        m_bIsEscape = false; m_bIsValid = false; m_iByte = 0x00; m_szBrief = "Invalid Instruction in 64-Bit Mode";

        m_operand[0].Clear(); m_operand[1].Clear(); m_operand[2].Clear(); m_operand[3].Clear(); 
        m_nOperands = 0;

        m_iVarientKey = VarientKey_None;
    }

    std::string m_szName    = "xx_INVALID_xx";
    bool        m_bIsValid  = false;
    bool        m_bIsEscape = false;
    Byte        m_iByte     = 0x00;
    bool        m_bModRm    = false;
    bool        m_bEntryGenNeeded = false;
    std::string m_szBrief   = "Invalid Instruction in 64-Bit Mode";
    EntryID_t   m_iID;

    struct Operand_t
    {
        Operand_t() { Clear(); }
        void Clear() { m_iOperandType = OperandCatagory_t::OperandCatagory_None; }

        enum OperandCatagory_t { OperandCatagory_None = -1, OperandCatagory_Legacy = 0, OperandCatagory_Literal, OperandCatagory_Register };
        OperandCatagory_t m_iOperandType = OperandCatagory_t::OperandCatagory_None;
        
        int         m_iLiteral      = 0;

        std::string m_szRegister    = "Invalid";

        std::string m_szOperandMode = "Invalid";
        std::string m_szOperandType = "Invalid";

        Operand_t& operator=(const Operand_t& other)
        {
            m_iOperandType  = other.m_iOperandType;
            m_iLiteral      = other.m_iLiteral;
            m_szRegister    = other.m_szRegister;
            m_szOperandMode = other.m_szOperandMode;
            m_szOperandType = other.m_szOperandType;

            return *this;
        }
    };
    Operand_t m_operand[4];
    int       m_nOperands = 0;


    enum VarientKey_t : int16_t
    {
        VarientKey_None = 0,
        VarientKey_ModRM_REG,
        VarientKey_ModRM_MOD,
        VarientKey_ModRM_RM,
        VarientKey_LegacyPrefix,
    };
    VarientKey_t          m_iVarientKey = VarientKey_None; // Do I need to store any more of these?
    std::vector<Entry_t*> m_vecVarients;

    int m_iSecOpcd = -1;
    int m_iPrefix  = -1;
    int m_iOpcdExt = -1;

    Entry_t& operator=(const Entry_t& other)
    {
        m_szName          = other.m_szName;
        m_bIsValid        = other.m_bIsValid;
        m_bIsEscape       = other.m_bIsEscape;
        m_iByte           = other.m_iByte;
        m_bModRm          = other.m_bModRm;
        m_bEntryGenNeeded = other.m_bEntryGenNeeded;
        m_szBrief         = other.m_szBrief;
        m_iID             = other.m_iID;
        m_nOperands       = other.m_nOperands;

        for (int i = 0; i < 4; i++)
            m_operand[i] = other.m_operand[i];

        m_iVarientKey = other.m_iVarientKey;
        // Don't copy varients. cause thats irelevant.
        m_iSecOpcd    = other.m_iSecOpcd;
        m_iPrefix     = other.m_iPrefix;
        m_iOpcdExt    = other.m_iOpcdExt;

        return *this;
    }
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void        ToUpperInPlace       (std::string& szInput);
int         HexStringToInt       (const char* szInput, int iSize);
void        ExtractSquareBrackets(std::string& szInput);
XMLElement* FindChildRecurse     (XMLElement* pParent, const char* szChildName);
void        RemoveInvalidEntries (std::vector<XMLElement*>& vecEntries, Byte iPrimOpCode, int iTableID, const char* szGroupName = nullptr);
static void RemoveNonAsciiChars  (std::string& sz);

// Actual parsing logic...
void CollectThreeByteOpCodeEntries(XMLElement* pRoot, std::vector<Entry_t*>& vecOutput, Byte iParentIndex);
void CollectEntires(XMLElement* pRoot, std::vector<Entry_t*>& vecOutput, int iTableID);
void ElementToEntry(XMLElement* pElem, Entry_t* pDest, Byte iByte, bool bPrefix, bool bSecOpcd, bool bOpcdExt);
void CombineEntries(Byte iByte, Entry_t* pOutput,
    const std::vector<XMLElement*>& vecLevel1, 
    const std::vector<XMLElement*>& vecLevel1NoMem,
    const std::vector<XMLElement*>& vecLevel1Prefix, 
    const std::vector<XMLElement*>& vecLevel2Mem,
    const std::vector<XMLElement*>& vecLevel2MemPrefix,
    const std::vector<XMLElement*>& vecLevel2NoMem,
    const std::vector<XMLElement*>& vecLevel2SecOpcd, 
    const std::vector<XMLElement*>& vecLevel2SecOpcdPrefix);
void DumpEntryInfo(std::vector<Entry_t*>& vecEntries);
void DumpTable(std::vector<Entry_t*>& vecEntries, const char* szTableName, std::ofstream& hFile);

// Linear searching "g_collisionSolutionTable" i.e. Collision Solution talbe, and
// return if an entry exists for this entry, returns nullptr on fail.
CollisionSolution_t* FindIDCollisionSol(int iOpCode, int iTableID, EntryID_t iID, const char* szGroupName);


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int main(void)
{
    // Load file.
    XMLDocument g_doc(true, tinyxml2::PRESERVE_WHITESPACE);
    // if (g_doc.LoadFile("F:\\tinyxml2-11.0.0\\TableGen\\src\\x86reference.xml") != XMLError::XML_SUCCESS) { printf("Failed to open file\n"); return 0; }
    if (g_doc.LoadFile("x86reference.xml") != XMLError::XML_SUCCESS) { printf("Failed to open file\n"); return 0; }


    // Root element.
    XMLElement* pRootElem = g_doc.RootElement();
    if (pRootElem == nullptr)
    {
        printf("Root Element of the table couldn't be acquired!\n");
        return 0;
    }
    printf("Table acquired successfuly!\n");


    // One-byte opcodes...
    std::vector<Entry_t*> vecOneByteOpCodes;
    if (true)
    {
        vecOneByteOpCodes.reserve(0xFF); vecOneByteOpCodes.clear();
        CollectEntires(pRootElem->FirstChildElement("one-byte"), vecOneByteOpCodes, 1);
    }
    

    // Two byte opcodes...
    std::vector<Entry_t*> vecTwoByteOpCodes;
    if (true)
    {
        vecTwoByteOpCodes.reserve(0xFF); vecTwoByteOpCodes.clear();
        CollectEntires(pRootElem->FirstChildElement("two-byte"), vecTwoByteOpCodes, 2);
    }


    // Three Byte OpCodes...
    std::vector<Entry_t*> vecThreeByteOpCodes38, vecThreeByteOpCodes3A;
    if (true)
    {
        vecThreeByteOpCodes38.reserve(65); vecThreeByteOpCodes3A.reserve(40);
        CollectThreeByteOpCodeEntries(pRootElem->FirstChildElement("two-byte"), vecThreeByteOpCodes38, 0x38);
        CollectThreeByteOpCodeEntries(pRootElem->FirstChildElement("two-byte"), vecThreeByteOpCodes3A, 0x3A);
    }


    std::ofstream hFile("Tables.cpp");
    if (hFile.is_open() == false)
    {
        printf(RED "Failed to open dump file!\n");
        return 0;
    }

    
    // DumpTable(vecOneByteOpCodes,     "m_opCodeTable1",    hFile); printf("\n"); DumpEntryInfo(vecOneByteOpCodes);
    // DumpTable(vecTwoByteOpCodes,     "m_opCodeTable2",    hFile); printf("\n"); DumpEntryInfo(vecTwoByteOpCodes);
    // DumpTable(vecThreeByteOpCodes38, "m_opCodeTable3_38", hFile); printf("\n");  DumpEntryInfo(vecThreeByteOpCodes38);
    DumpTable(vecThreeByteOpCodes3A, "m_opCodeTable3_3A", hFile); printf("\n");  DumpEntryInfo(vecThreeByteOpCodes3A);


    hFile.close();


    return 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void CollectEntires(XMLElement* pRoot, std::vector<Entry_t*>& vecOutput, int iTableID)
{
    std::vector<XMLElement*> vecTempEntries; vecTempEntries.clear();

    for (XMLElement* pPriOpcd = pRoot->FirstChildElement("pri_opcd"); pPriOpcd != nullptr; pPriOpcd = pPriOpcd->NextSiblingElement("pri_opcd"))
    {
        printf("\n\n");

        vecTempEntries.clear();

        Byte iByte = 0x00;
        if(const char* szValue = pPriOpcd->Attribute("value"); szValue != nullptr)
        {
            iByte = HexStringToInt(szValue, 2);
        }


        // We handle the 3rd byte opcodes seperately.
        if ((iByte == 0x3A || iByte == 0x38) && iTableID == 2)
            continue;

        // Mazegen doesn't remove mark these are invalid for 64 bit mode. But we don't want them.
        if ((iByte >= 0x40 && iByte <= 0x4F) && iTableID == 1)
            continue;


        // Level 1 : All entires that don't have a <opcd_ext> tag are catagorized under level 1.
        //           Idea is that these instructions are stored in the opcode table itself in the intel's manual. ( no sub-stable )
        //           Only way to produce varients in these instructions is using legacy prefixies.

        // Level 2 : Any entries that have a <opcd_ext> tag are catagorised under level2. 
        //           Idea is that these entires are stored in a subtable in the intel's manual,
        //           and require the modrm.REG bits to be accessed.

        
        // Holds all level1 entries, which don't have any prefix tag.
        std::vector<XMLElement*> vecLevel1;              vecLevel1.clear();

        // Holds all level1 entries, which has attribute mode="mem" or "nomem" in the <entry> tag itself.
        std::vector<XMLElement*> vecLevel1NoMem;         vecLevel1NoMem.clear();

        // Holds any level1 entries, which have prefix tags.
        std::vector<XMLElement*> vecLevel1Prefix;        vecLevel1Prefix.clear();

        // Holds any level2 entries, that either have a <syntax> tag with mod="mem" or 
        // no <syntax> tag and no <sec_opcd> tag. Hence this is the default for level2.
        std::vector<XMLElement*> vecLevel2Mem;           vecLevel2Mem.clear();

        // Holds any level2 entries, that don't have any <syntax> tag with mod attribute 
        // and don't have any <sec_opcd> tags.
        std::vector<XMLElement*> vecLevel2MemPrefix;     vecLevel2MemPrefix.clear();

        // Holds all entries @ level2, with <sec_opcd> tag and no prefix tag.
        std::vector<XMLElement*> vecLevel2SecOpcd;       vecLevel2SecOpcd.clear();

        // Holds all entries @ level2, with <sec_opcd> tag and a prefix tag.
        std::vector<XMLElement*> vecLevel2SecOpcdPrefix; vecLevel2SecOpcdPrefix.clear();

        // Holds all entries @ level2, with a <syntax> or <entry> tag with a mod="nomem"
        std::vector<XMLElement*> vecLevel2NoMem;         vecLevel2NoMem.clear();
        
        // When an entry has multiple <syntax> tags most of the times one of tag holds mod="mem"
        // and the other holds mod="nomem" and this forces use to store that same entry twice
        // under different catagories. Hence we need this counter to we can accuratly do the count check at the end.
        int iDuplicates = 0; 


        // Iterate all entries..
        for (XMLElement* pEntry = pPriOpcd->FirstChildElement("entry"); pEntry != nullptr; pEntry = pEntry->NextSiblingElement("entry"))
        {
            // If even one of the entry says "attr == invd" clear entry history and break out immediately.
            if (const char* szAttribute = pEntry->Attribute("attr"); szAttribute != nullptr)
            {
                std::string szAttr(szAttribute);
                if(szAttr == std::string("undef") || szAttr == std::string("invd") || szAttr == std::string("null"))
                {
                    vecTempEntries.clear();
                    break;
                }
            }


            // Must have mnenomic tag, else skip that shit...
            if (FindChildRecurse(pEntry, "mnem") == nullptr)
            {
                printf("OpCode 0x%02X doesn't have a mnemonic tag\n", iByte);
                vecTempEntries.clear(); break;
            }


            if(XMLElement* pProcEnd = FindChildRecurse(pEntry, "proc_end"); pProcEnd != nullptr)
            {
                if (atoi(pProcEnd->GetText()) < 10)
                {
                    printf(RED "An Entry in instuction \"0x%02X\" has proc_end less than 10. Discarding unsupported instuction.\n" RESET, iByte);
                    continue;
                }
            }


            // Mem & NoMem attribute in <syntax> tag?
            bool bOnlyMem = false, bOnlyNoMem = false;
            if (pEntry->Attribute("mod", "mem")   != nullptr) bOnlyMem   = true;
            if (pEntry->Attribute("mod", "nomem") != nullptr) bOnlyNoMem = true;


            bool bMem = false, bNoMem = false;
            for (XMLElement* pSyntax = FindChildRecurse(pEntry, "syntax"); pSyntax != nullptr; pSyntax = pSyntax->NextSiblingElement("syntax"))
            {
                if (const char* szMod = pSyntax->Attribute("mod"); szMod != nullptr)
                {
                    if (std::string(szMod) == std::string("mem"))   bMem = true;
                    if (std::string(szMod) == std::string("nomem")) bNoMem = true;
                }
            }

            assert(bMem == bNoMem && "Different mem and nomem value. Either both false or both true, but they are different.");

            bool bOpcdExt     = FindChildRecurse(pEntry, "opcd_ext") != nullptr; // If we can find a opcd_ext tag, that means this entry is @ level 1. 100%
            bool bSecOpcd     = FindChildRecurse(pEntry, "sec_opcd") != nullptr;
            bool bPrefixSplit = FindChildRecurse(pEntry, "pref")     != nullptr;
            

            if (bOnlyMem == true || bOnlyNoMem == true)
            {
                assert(bMem == false && bNoMem == false && "Both <syntax> tag and <entry> tag have mem/nomem property.");
                assert(bSecOpcd     == false            && "SecOpcd found when <entry> has mem/nomem property");
                assert(bPrefixSplit == false            && "Prefix  found when <entry> has mem/nomem property");
            }


            // NOTE: Now this entry has a mnemonic tag, and is valid in long mode. So we can store this.
            vecTempEntries.push_back(pEntry);


            // Sorting Entries and storing under correct catagory.
            if (bOpcdExt == false) 
            {
                if (bPrefixSplit == true)
                {
                    assert(bOnlyMem == false && bOnlyNoMem == false && "Prefix, Mem and NoMem can't go together.!");
                    vecLevel1Prefix.push_back(pEntry);
                }
                else
                {
                    // Prefix : false, OnlyMem : false, OnlyNoMem : false
                    // That means, either simple entry or has multiple syntax blocks ( one mem and one nomem )
                    //? We are not bothering with varients within an entry, we only care about entry varients.
                    if (bOnlyNoMem == false)
                    {
                        vecLevel1.push_back(pEntry);
                    }
                    else if (bOnlyNoMem == true && bOnlyMem == false)
                    {
                        vecLevel1NoMem.push_back(pEntry);
                    }
                    else
                    {
                        // we shouldn't ever reach this brach.
                        assert(false && "Fucked up somewhere for level1. Reached unreachable branch.");
                        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
                    }
                }
            }
            else 
            {
                if (bMem == true && bNoMem == true) 
                {
                    vecLevel2Mem.push_back(pEntry);
                    vecLevel2NoMem.push_back(pEntry); iDuplicates++;

                    assert(bPrefixSplit == false && "Prefix split with mem and nomem split. Can't be together when mem and nomem is explicit.");
                }
                else if (bOnlyNoMem == true || bOnlyMem == true)
                {
                    // This is to handle a very special case, when we are handing those instructions.
                    // That need to escape to the coprocessor instruction set, ( 0xD8 to 0xDF ).
                    // They have the mem/nomem attribute attached to the <entry> tag instead of the <syntax> tag.

                    if (bOnlyNoMem == true)
                    {
                        vecLevel2NoMem.push_back(pEntry);
                    }
                    else if (bOnlyMem == true)
                    {
                        vecLevel2Mem.push_back(pEntry);
                    }
                }
                else
                {
                    if (bSecOpcd == true)
                    {
                        if (bPrefixSplit == true)
                        {
                            vecLevel2SecOpcdPrefix.push_back(pEntry);
                        }
                        else vecLevel2SecOpcd.push_back(pEntry);
                    }
                    else
                    {
                        if (bPrefixSplit == true)
                        {
                            vecLevel2MemPrefix.push_back(pEntry);
                        }
                        else vecLevel2Mem.push_back(pEntry);
                    }
                }
            }
        }

        
        // No relevant entires...
        if (vecTempEntries.empty() == true)
            continue;


        // Size check, just in case we fuck up somewhere.
        {
            size_t iSizeCombined = 
                vecLevel1.size()        + vecLevel1Prefix.size() + vecLevel1NoMem.size() +
                vecLevel2Mem.size()     + vecLevel2MemPrefix.size() + vecLevel2NoMem.size() +
                vecLevel2SecOpcd.size() + vecLevel2SecOpcdPrefix.size();

            iSizeCombined -= static_cast<size_t>(iDuplicates);

            if (iSizeCombined != vecTempEntries.size())
            {
                printf("Size Combined : %llu, Total Entries : %llu\n", iSizeCombined, vecTempEntries.size());
                assert(iSizeCombined == vecTempEntries.size() && "Combined entries don't match total entries");
            }
        }


        RemoveInvalidEntries(vecLevel1,              iByte, iTableID, LEVEL1);
        RemoveInvalidEntries(vecLevel1NoMem,         iByte, iTableID, LEVEL1_NOMEM);
        RemoveInvalidEntries(vecLevel1Prefix,        iByte, iTableID, LEVEL1_PREFIX);
        RemoveInvalidEntries(vecLevel2Mem,           iByte, iTableID, LEVEL2_MEM);
        RemoveInvalidEntries(vecLevel2MemPrefix,     iByte, iTableID, LEVEL2_MEM_PREFIX);
        RemoveInvalidEntries(vecLevel2NoMem,         iByte, iTableID, LEVEL2_NOMEM);
        RemoveInvalidEntries(vecLevel2SecOpcd,       iByte, iTableID, LEVEL2_SECOPCD);
        RemoveInvalidEntries(vecLevel2SecOpcdPrefix, iByte, iTableID, LEVEL2_SECOPCD_PREFIX);

        size_t iSizeCombined = 
            vecLevel1.size()          + vecLevel1Prefix.size() + vecLevel2Mem.size()     + vecLevel1NoMem.size() +
            vecLevel2MemPrefix.size() + vecLevel2NoMem.size()  + vecLevel2SecOpcd.size() + vecLevel2SecOpcdPrefix.size();
        iSizeCombined -= static_cast<size_t>(iDuplicates);

        printf("Merging %llu Entry for \"0x%02X\"...\n", iSizeCombined, iByte);


        // Now combine all entries into one tree like structure.
        Entry_t* pEntry = new Entry_t(); // This will be the head of the tree.
        CombineEntries(iByte, pEntry, vecLevel1, vecLevel1NoMem, vecLevel1Prefix, vecLevel2Mem, vecLevel2MemPrefix, vecLevel2NoMem, vecLevel2SecOpcd, vecLevel2SecOpcdPrefix);
        vecOutput.push_back(pEntry);


        // In case any of the operands have the addressing mode "Z", we need to create
        // 7 more entries with same specs but the byte, cause mazegen simply marks
        // one operand with Z where opcode's lower 3 bits determine register index for
        // that operand.
        {
            if (pEntry != nullptr)
            {
                // Single entry. ( madatory for entry generation. )
                if (pEntry->m_iVarientKey == Entry_t::VarientKey_None && pEntry->m_bEntryGenNeeded == true)
                {
                    printf(GREEN "@@@@  Generating entries for { 0x%02X }...  @@@@\n" RESET, iByte);
                    
                    for (int i = 0; i < 7; i++)
                    {
                        Entry_t* pGeneratedEntry = new Entry_t();

                        *pGeneratedEntry         = *pEntry;
                        pGeneratedEntry->m_iByte = pEntry->m_iByte + i + 1;
                        
                        vecOutput.push_back(pGeneratedEntry);
                    }
                }
            }
        }
    }
    std::cout << std::endl;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void ElementToEntry(XMLElement* pElem, Entry_t* pEntry, Byte iByte, bool bPrefix, bool bSecOpcd, bool bOpcdExt)
{
    pEntry->Clear();
    pEntry->m_iByte = iByte;  

    // This function collects the following information from XMLElement : 
    // Mnemonic, brief, operands, prefix ( if any ), 

    XMLElement* pSyntax = FindChildRecurse(pElem, "syntax");
    if (pSyntax == nullptr)
    {
        printf("SYNTAX is null for entry [ 0x%02X ]", pEntry->m_iByte);
        return;
    }

    pEntry->m_szName  = FindChildRecurse(pSyntax, "mnem")->GetText();
    pEntry->m_szBrief = FindChildRecurse(pElem,   "brief")->GetText();

    RemoveNonAsciiChars(pEntry->m_szBrief); // causes bullshit

    printf("Brief : %s\n", pEntry->m_szBrief.c_str());

    pEntry->m_bModRm  = pElem->Attribute("r", "yes") != nullptr || FindChildRecurse(pElem, "opcd_ext") != nullptr;


    // Storing tags.. ( Prefix, secOpcd, OpcdExt )
    auto StoreTag = [&](int& pOut, const char* szTag, bool bUseAtoi = false) -> void
        {
            XMLElement* pTag = FindChildRecurse(pElem, szTag);
            
            if(pTag == nullptr)
            {
                printf(RED "tag [ %s ] not found for { 0x%02X }\n" RESET, szTag, pEntry->m_iByte);
                // assert(pTag != nullptr && "Tag not found!!!");
            }
            else
            {
                pOut = bUseAtoi == true ? atoi(pTag->GetText()) : HexStringToInt(pTag->GetText(), 2);
            }
        };
    if (bPrefix  == true) StoreTag(pEntry->m_iPrefix,  "pref",     false);
    if (bOpcdExt == true) StoreTag(pEntry->m_iOpcdExt, "opcd_ext", true);
    if (bSecOpcd == true) StoreTag(pEntry->m_iSecOpcd, "sec_opcd", false);


    // Little sanity check for out of bound sec_opcd in non - "escape to 3 byte opcod table" case.
    if (XMLElement* pSecOpcd = FindChildRecurse(pElem, "sec_opcd"); pSecOpcd != nullptr)
        if (pSecOpcd->Attribute("escape", "yes") == nullptr)
            assert((pEntry->m_iSecOpcd & 0b11000000) == 0b11000000 && "Out of bound sec_opcd in non-escape case");

    
    int iOperandIndex = 0;
    auto ExtractOperands = [&](const char* szTag) -> void
        {
            for(XMLElement* pOperand = FindChildRecurse(pSyntax, szTag); pOperand != nullptr; pOperand = pOperand->NextSiblingElement(szTag))
            {
                Entry_t::Operand_t* pDestOperand = &pEntry->m_operand[iOperandIndex];


                // Ignore operands with "displayed" set to no.
                if (const char* szDisplayed = pOperand->Attribute("displayed"); szDisplayed != nullptr && std::string(szDisplayed) == std::string("no"))
                {
                    printf(YELLOW "Skipping hidden operand...\n" RESET);
                    continue;
                }


                // If operand < dst > or < src > has tag <a> then its legacy prefix.
                if (XMLElement* pOperandMode = pOperand->FirstChildElement("a"); pOperandMode != nullptr)
                {
                    if (pOperandMode->GetText()[0] == 'Z')
                        pEntry->m_bEntryGenNeeded = true;

                    auto it = g_mapAddressingModeLinker.find(std::string(pOperandMode->GetText()));
                    
                    // In case this operand addressing mode is not present in our "geek edition to coder's edition linker map"
                    // let me know.
                    if(it == g_mapAddressingModeLinker.end())
                    {
                        printf("Addressing mode not found in linker map [ %s ]\n", pOperandMode->GetText());
                        assert(it != g_mapAddressingModeLinker.end() && "Entry not found in map!");
                        continue; // This continue will cause overwritting this operand entry; ( which is invalid. )
                    }

                    // Store operand addressing mode.
                    pDestOperand->m_iOperandType  = Entry_t::Operand_t::OperandCatagory_Legacy;
                    pDestOperand->m_szOperandMode = it->second;


                    // Try to find operand type...
                    if (XMLElement* pOperandType = pOperand->FirstChildElement("t"); pOperandType != nullptr)
                    {
                        auto it = g_mapOperandTypeLinker.find(std::string(pOperandType->GetText()));

                        // Operand type not present in linker map.
                        if(it == g_mapOperandTypeLinker.end())
                        {
                            printf("Operand type not found in linker map [ %s ]\n", pOperandType->GetText());
                            assert(it != g_mapOperandTypeLinker.end() && "Entry not found in map!");
                            continue; // This continue will cause overwritting this operand entry; ( which is invalid. )
                        }

                        // store operand type.
                        pDestOperand->m_szOperandType = it->second;
                    }
                    else // No operand type is present, only addressing mode. This is possible so don't abort in this case.
                    {
                        printf("No operand type is present for opcode [ 0x%02X ] only addressing mode\n", pEntry->m_iByte);
                    }
                }
                else
                {
                    // if there is no "a" tag in the operand tags, then its either a literal or a register.
                    if (const char* szAdrsAttr = pOperand->Attribute("address"); szAdrsAttr != nullptr && szAdrsAttr[0] == 'I')
                    {
                        pDestOperand->m_iLiteral     = atoi(pOperand->GetText());
                        pDestOperand->m_iOperandType = Entry_t::Operand_t::OperandCatagory_Literal;
                    }
                    else
                    {
                        pDestOperand->m_szRegister   = pOperand->GetText();
                        ExtractSquareBrackets(pEntry->m_operand[iOperandIndex].m_szRegister);

                        pDestOperand->m_iOperandType = Entry_t::Operand_t::OperandCatagory_Register;
                    }
                }

                iOperandIndex++;
            }
        };

    ExtractOperands("dst"); ExtractOperands("src");

    // Operand index now should hold the count of operands that we have successfully stored.
    pEntry->m_nOperands = iOperandIndex;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void CombineEntries(Byte iByte, Entry_t* pOutput,
    const std::vector<XMLElement*>& vecLevel1,
    const std::vector<XMLElement*>& vecLevel1NoMem,
    const std::vector<XMLElement*>& vecLevel1Prefix,
    const std::vector<XMLElement*>& vecLevel2Mem,
    const std::vector<XMLElement*>& vecLevel2MemPrefix,
    const std::vector<XMLElement*>& vecLevel2NoMem,
    const std::vector<XMLElement*>& vecLevel2SecOpcd,
    const std::vector<XMLElement*>& vecLevel2SecOpcdPrefix)
{
    pOutput->Clear();
    pOutput->m_iByte = iByte; // Common in all entries.


    size_t iLevel1Size = vecLevel1.size() + vecLevel1Prefix.size() + vecLevel1NoMem.size();
    size_t iLevel2Size = vecLevel2Mem.size() + vecLevel2MemPrefix.size() + vecLevel2NoMem.size() + vecLevel2SecOpcd.size() + vecLevel2SecOpcdPrefix.size();


    assert((vecLevel1.empty() == true && vecLevel1NoMem.empty() == false) == false && "vecLevel1 and vecLevel1NoMem have different seperate empty status.");
    assert((iLevel1Size == 0llu || iLevel2Size == 0llu) && "Both levels can't have entries at the same time.");


    //! NOTE : Level 1 cases means, either we have a single entry or we have prefix based split. 
    if (iLevel1Size > 0llu)
    {
        // We only have a single entry.
        if (iLevel1Size == 1llu && vecLevel1.size() == 1llu)
        {
            // Very specific case :)
            assert(vecLevel1.size() == 1llu && vecLevel1Prefix.size() == 0llu && "We only have 1 entry and thats for level1 prefix. Not possible!");
            printf("Entry format [ No Varients ]\n");


            pOutput->m_iVarientKey = Entry_t::VarientKey_None;
            XMLElement* pEntryElem = vecLevel1.back();
            
            ElementToEntry(pEntryElem, pOutput, pOutput->m_iByte, false, false, false);

            return;
        }
        // We have a prefix based split in level 1.
        else
        {
            pOutput->m_iVarientKey = Entry_t::VarientKey_LegacyPrefix;
            printf("Entry format [ Prefix Split level 1 ]\n");
            

            // There shouldn't be more than 1 of these anyways. But who tf knows?
            Entry_t* pNoMemLevel1 = nullptr;
            assert(vecLevel1NoMem.size() <= 1llu && "Assumed NoMem entries <= 1 for level1 prefix split!");
            if (vecLevel1NoMem.empty() == false)
            {
                pNoMemLevel1 = new Entry_t();
                ElementToEntry(vecLevel1NoMem.front(), pNoMemLevel1, pOutput->m_iByte, false, false, false);
            }


            // There shouldn't be more than 1 of these anyways. But who tf knows?
            assert(vecLevel1.size() <= 1llu && "Assumed Mem entries <= 1 for level1 prefix split!");
            if(vecLevel1.empty() == false)
            {
                Entry_t* pChildEntry = new Entry_t();
                ElementToEntry(vecLevel1.front(), pChildEntry, pOutput->m_iByte, false, false, false);
                
                // Incase explicit mem and nomem exist with prefix, put them under one entry
                // and mart that as "split @ modrm.MOD" :) kinda sketchy ik
                if (pNoMemLevel1 != nullptr)
                {
                    Entry_t* pTempHead = new Entry_t();
                    pTempHead->m_iVarientKey = Entry_t::VarientKey_ModRM_MOD;
                    pTempHead->m_vecVarients.push_back(pChildEntry);
                    pTempHead->m_vecVarients.push_back(pNoMemLevel1);
                    
                    pOutput->m_vecVarients.push_back(pTempHead);
                }
                else
                {
                    pOutput->m_vecVarients.push_back(pChildEntry);
                }
            }


            // There can be upto 3 or 4 and maybe even more.
            for (XMLElement* pElem : vecLevel1Prefix)
            {
                Entry_t* pChildEntry = new Entry_t();
                ElementToEntry(pElem, pChildEntry, pOutput->m_iByte, true, false, false);

                pOutput->m_vecVarients.push_back(pChildEntry);
            }
        }
    }
    else
    {
        pOutput->m_iVarientKey = Entry_t::VarientKey_t::VarientKey_ModRM_REG;
        printf("Entry format [ REG based split ]\n");

        // Find all entries with same opcd_ext. 
        // Put them accordingly in their respsective tree places n shit
        // put all tree heads in that fucking parent's list.
        std::vector<XMLElement*> vecMem, vecNoMem, vecSecOpcd;
        for (int i = 0; i < 8; i++)
        {
            vecMem.clear(); vecNoMem.clear(); vecSecOpcd.clear();


            auto CollectSimilarEntries = [&](const std::vector<XMLElement*>& vecElements, std::vector<XMLElement*>& vecOutput) -> int
                {
                    int nEntries = 0;

                    for (XMLElement* pElem : vecElements)
                    {
                        XMLElement* pOpcdExt = pElem->FirstChildElement("opcd_ext");
                        assert(pOpcdExt != nullptr && "Entry in level2 list doesn't have a opcd_ext tag!!");

                        int iOpcdExt = pOpcdExt->IntText(-1);
                        assert(iOpcdExt >= 0 && iOpcdExt <= 7 && "Invalid opcd_ext value!");

                        if (iOpcdExt == i)
                        {
                            vecOutput.push_back(pElem);
                            nEntries++;
                        }
                    }

                    return nEntries;
                };

            // Put entries with same <opcd_ext> in "vecTemp"
            int nMemEntries           = CollectSimilarEntries(vecLevel2Mem,           vecMem);
            int nMemPrefixEntries     = CollectSimilarEntries(vecLevel2MemPrefix,     vecMem);

            int nNoMemEntries         = CollectSimilarEntries(vecLevel2NoMem,         vecNoMem);

            int nSecOpcdEntries       = CollectSimilarEntries(vecLevel2SecOpcd,       vecSecOpcd);
            int nSecOpcdPrefixEntries = CollectSimilarEntries(vecLevel2SecOpcdPrefix, vecSecOpcd);

            printf("%s[ Mem : %llu, NoMem : %llu, SecOpcd : %llu ] entries for opcd_ext %d\n" RESET, 
                (vecNoMem.size() != 0llu && vecSecOpcd.size() != 0llu) ? RED : "", 
                vecMem.size(), vecNoMem.size(), vecSecOpcd.size(), i);

            // assert((vecNoMem.size() == 0llu || vecSecOpcd.size() == 0llu) && "Both NoMem and sec_opcd fields have entries!");


            Entry_t* pMem   = new Entry_t();
            Entry_t* pNoMem = new Entry_t();

            // Filling up "mem" entries ( one or many )...
            if(nMemEntries + nMemPrefixEntries > 0)
            {
                // If the mem split doesn't have any preifx splits, then there is one less branch to worry bout.
                if (nMemPrefixEntries == 0)
                {
                    assert(nMemEntries == 1 && "Mem entries is not 1 and prefix entries are 0 and sum is greather than 0?!");

                    pMem->m_iVarientKey = Entry_t::VarientKey_None; // This should be been None by default, but just to make sure.
                    ElementToEntry(vecMem.back(), pMem, pOutput->m_iByte,false, false, true);
                }
                else
                {
                    pMem->m_iVarientKey = Entry_t::VarientKey_LegacyPrefix;
                    for(XMLElement* pElem : vecMem)
                    {
                        Entry_t* pMemPrefixed = new Entry_t();
                        ElementToEntry(vecMem.back(), pMem, pOutput->m_iByte,true, false, true);
                        
                        // Store children.
                        pMem->m_vecVarients.push_back(pMemPrefixed);
                    }
                }
            }


            // Filling up "nomem" entries ( one or many )...
            if (vecNoMem.size() + vecSecOpcd.size() > 0llu) // NOTE : Alteast one entry ( either in NoMem or sec_opcd )
            {
                if (vecNoMem.empty() == false && vecSecOpcd.empty() == true)
                {
                    pNoMem->m_iVarientKey = Entry_t::VarientKey_None;
                    ElementToEntry(vecNoMem.back(), pNoMem, pOutput->m_iByte, false, false, true);
                }
                else if (vecNoMem.empty() == true && vecSecOpcd.empty() == false)
                {
                    pNoMem->m_iVarientKey = Entry_t::VarientKey_ModRM_RM; // Some fool set it to LegacyPrefix. I wonder if that was for some reason.?

                    for (XMLElement* pElem : vecSecOpcd)
                    {
                        Entry_t* pSecOpcdEntry = new Entry_t();
                        pSecOpcdEntry->m_iVarientKey = Entry_t::VarientKey_None;

                        ElementToEntry(pElem, pSecOpcdEntry, pOutput->m_iByte, true, true, true);
                        pNoMem->m_vecVarients.push_back(pSecOpcdEntry);
                    }
                }
                else
                {
                    // Must be only one of these.
                    assert(vecNoMem.size() == 1 && "Multipe NoMem entries, with sec_opcd / NoMem collision");

                    printf(RED "<!-- Handling Multiple \"NoMem\" and \"sec_opcd\" collision. Setting default to NoMem -->\n" RESET);
                    //Byte iRegisteredRMVarients[8] = { 0 };
                    Byte iRegisteredRMVarients[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


                    pNoMem->m_iVarientKey = Entry_t::VarientKey_ModRM_RM;

                    Entry_t* pDefaultEntry = new Entry_t();
                    ElementToEntry(vecNoMem.back(), pDefaultEntry, pOutput->m_iByte, false, false, true);

                    // Filling in all the sec_opcd entries we have first.
                    // so we can fill in the remainig with nomem entry. ( should be only one )
                    for (XMLElement* pElem : vecSecOpcd)
                    {
                        Entry_t* pSecOpcdEntry = new Entry_t();
                        pSecOpcdEntry->m_iVarientKey = Entry_t::VarientKey_None;


                        ElementToEntry(pElem, pSecOpcdEntry, pOutput->m_iByte, true, true, true);
                        pNoMem->m_vecVarients.push_back(pSecOpcdEntry);


                        // Mark the RM for which we have stored.
                        int iRM = pSecOpcdEntry->m_iSecOpcd & 0b111;
                        assert(iRM >= 0 && iRM < 8 && "RM stored is out of bound!");
                        iRegisteredRMVarients[iRM] = pSecOpcdEntry->m_iSecOpcd;


                        printf("--> Stored sec_opcd in collision case @ %d\n", pSecOpcdEntry->m_iSecOpcd);
                    }


                    // Filling in all the remaining RM slots with default ( NoMem ) entry.
                    printf(GREEN "Set Following RM varient entries with default : " RESET);

                    int nDefaultEntries = 0;
                    for (int iRMSlotIndex = 0; iRMSlotIndex < sizeof(iRegisteredRMVarients); iRMSlotIndex++)
                    {
                        //int iRM = iRegisteredRMVarients[iRMSlotIndex];
                        Byte iRM = iRegisteredRMVarients[iRMSlotIndex];
                        
                        if (iRM == 0xFF)
                        {
                            Entry_t* pNew       = new Entry_t();
                            *pNew                = *pDefaultEntry;
                            pNew->m_iSecOpcd    = iRMSlotIndex;

                            pNoMem->m_vecVarients.push_back(pNew);
                            nDefaultEntries++;
                            printf("%d { %s }, ", pNew->m_iSecOpcd, pNew->m_szName.c_str());
                        }
                    }
                    printf("\n");
                    assert(nDefaultEntries < 8 && nDefaultEntries >= 0 && "We can't store all RM varients to default entries if there is a collision!");
                }
            }


            // now pMem and pNoMem should have their entries placed in them nicely.
            // So now we can just place them in the output's ass.
            if (pNoMem->m_iVarientKey == Entry_t::VarientKey_None && pNoMem->m_szName == "xx_INVALID_xx")
            {
                pOutput->m_iOpcdExt = i;
                pOutput->m_vecVarients.push_back(pMem);

                printf(CYAN "Storing mem operator directly\n" RESET);
            }
            else
            {
                Entry_t* pRegEntry = new Entry_t();
                pRegEntry->m_iOpcdExt = i;

                pRegEntry->m_iVarientKey = Entry_t::VarientKey_ModRM_MOD;
                pRegEntry->m_vecVarients.push_back(pMem);
                pRegEntry->m_vecVarients.push_back(pNoMem);

                pOutput->m_vecVarients.push_back(pRegEntry);

                printf("Storing both mem and nomem\n");
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void DumpEntryRecurse(std::vector<Entry_t*>& vecEntries, int iIndentation, Entry_t::VarientKey_t iSplitMethod = Entry_t::VarientKey_None)
{
    if (iSplitMethod != Entry_t::VarientKey_None)
    {
        for (int i = 0; i < iIndentation - 1; i++)
            printf("    ");

        const char* szSplitMethod = nullptr;
        switch (iSplitMethod)
        {
        case Entry_t::VarientKey_ModRM_REG: szSplitMethod = "REG Split ( opcd_ext )"; break;
        case Entry_t::VarientKey_ModRM_MOD: szSplitMethod = "MOD Split"; break;
        case Entry_t::VarientKey_ModRM_RM: szSplitMethod = "RM Split"; break;
        case Entry_t::VarientKey_LegacyPrefix: szSplitMethod = "LegacyPrefix"; break;

        case Entry_t::VarientKey_None:
        default: break;
        }

        if (szSplitMethod != nullptr)
            printf(CYAN "%s\n" RESET, szSplitMethod);
    }


    for (Entry_t* pEntry : vecEntries)
    {
        if (pEntry->m_szName != "xx_INVALID_xx")
        {
            for (int i = 0; i < iIndentation; i++)
                printf(".   ");


            // Index according to split method.
            int iIndex = -1; int* pIndex = nullptr;
            switch (iSplitMethod)
            {
            case Entry_t::VarientKey_ModRM_REG:    pIndex = &pEntry->m_iOpcdExt; iIndex = *pIndex;         break;
            case Entry_t::VarientKey_ModRM_RM:     pIndex = &pEntry->m_iSecOpcd; iIndex = *pIndex & 0b111; break;
            case Entry_t::VarientKey_LegacyPrefix: pIndex = &pEntry->m_iPrefix;  iIndex = *pIndex;         break;
            
            case Entry_t::VarientKey_ModRM_MOD:
            case Entry_t::VarientKey_None:
            default: 
                break;
            }

            if(iIndex >= 0 && pIndex != nullptr)
                printf("# %d [ %d ]", iIndex, *pIndex);


            // OpCode's original name, will be valid only if there are varients.
            printf("{ 0x%02X } %s ", pEntry->m_iByte, pEntry->m_szName.c_str());


            // Printing operands.
            for (int i = 0; i < pEntry->m_nOperands; i++)
            {
                Entry_t::Operand_t* pOperand = &pEntry->m_operand[i];
                
                switch (pOperand->m_iOperandType)
                {
                case Entry_t::Operand_t::OperandCatagory_Legacy:   printf("{ %s%s } ", pOperand->m_szOperandMode.c_str(), pOperand->m_szOperandType.c_str()); break;
                case Entry_t::Operand_t::OperandCatagory_Register: printf("{ %s } ",   pOperand->m_szRegister.c_str());                                       break;
                case Entry_t::Operand_t::OperandCatagory_Literal:  printf("{ %d } ",   pOperand->m_iLiteral);                                                 break;
                default: break;
                }
            }

            printf("\n");
        }

        DumpEntryRecurse(pEntry->m_vecVarients, iIndentation + 1, pEntry->m_iVarientKey);

        if (iIndentation == 0)
            printf("\n");
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void DumpEntryInfo(std::vector<Entry_t*>& vecEntries)
{
    printf(GREEN "Starting Entry Dump...\n" RESET);


    DumpEntryRecurse(vecEntries, 0);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
CollisionSolution_t* FindIDCollisionSol(int iOpCode, int iTableID, EntryID_t iID, const char* szGroupName)
{
    assert(iOpCode >= 0x00 && iOpCode <= 0xFF && iTableID > 0 && szGroupName != nullptr && "Invalid data received in fn FindIDCollisionSol");


    for (size_t iSolIndex = 0; iSolIndex < g_iCollisionSolTableSize; iSolIndex++)
    {
        CollisionSolution_t* pSol = &g_collisionSolutionTable[iSolIndex];

        bool bSolutionFound =
            pSol->m_iByte            == iOpCode &&
            pSol->m_iTableID         == iTableID &&
            pSol->m_iEntryID         == iID &&
            std::string(szGroupName) == std::string(pSol->m_szGroupName);

        if (bSolutionFound == false)
            continue;

        // This entry is already "used".
        if (pSol->m_iSkipsLeft < 0)
            return nullptr;

        return pSol;
    }


    return nullptr;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void ExtractSquareBrackets(std::string& szInput)
{
    if(szInput.empty() == true)
        return;


    int iStart = -1;
    int iEnd   = static_cast<int>(szInput.size() - 1LLU);


    for(int i = 0; i < szInput.size(); i++)
    {
        if(szInput[i] == '[')
        {
            iStart = i;
        }
        else if(szInput[i] == ']')
        {
            iEnd = i;
        }
    }


    // Start bracket not found. Invalid input.
    if(iStart == -1 || iStart == iEnd)
        return;


    szInput = szInput.substr(iStart + 1, iEnd - iStart - 1);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int HexStringToInt(const char* szInput, int iSize)
{
    if(szInput == nullptr || iSize == -1)
        return -1;

    if(iSize == 0)
        return 0;


    int iOutput = 0;

    for(int i = 0; i < iSize; i++)
    {
        iOutput  *= 0x10;
        char character = szInput[i];

        if(character >= '0' && character <= '9')
        {
            iOutput += character - '0';
        }
        else if(character >= 'A' && character <= 'F')
        {
            iOutput += character - 'A' + 10;
        }
        else if(character >= 'a' && character <= 'f')
        {
            iOutput += character - 'a' + 10;
        }
        else 
        {
            return iOutput;
        }
    }

    return iOutput;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void ToUpperInPlace(std::string& szInput)
{
    std::string szOutput = "";

    for(char c : szInput)
    {
        if(c >= 'a' && c <= 'z')
        {
            c = c - 'a' + 'A';
        }

        szOutput.push_back(c);
    }

    szInput = std::move(szOutput);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
XMLElement* FindChildRecurse(XMLElement* pParent, const char* szChildName)
{
    if(pParent == nullptr ||  szChildName == nullptr || *szChildName == NULL)
        return nullptr;


    XMLElement* pChild = pParent->FirstChildElement(nullptr);

    if(pChild == nullptr)
        return nullptr;


    while(pChild != nullptr)
    {
        // If this child the target child element.
        if(std::string(pChild->Name()) == std::string(szChildName))
            return pChild;


        // Search all of its children elements...
        XMLElement* pTargetElement = FindChildRecurse(pChild, szChildName);


        // If we found something then return it.
        if(pTargetElement != nullptr)
            return pTargetElement;


        // Next element @ the same level.
        pChild = pChild->NextSiblingElement(nullptr);
    }


    return nullptr;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
inline int OperationModeToPriority(const char* pAttr)
{
    if (pAttr == nullptr)
        return 0;

    switch (pAttr[0])
    {
    case 'r': return 1;
    case 'p': return 2;
    case 'e': return 3;
    
    default: break;
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void RemoveInvalidEntries(std::vector<XMLElement*>& vecEntries, Byte iPrimOpCode, int iTableID, const char* szGroupName)
{
    struct IDs { IDs() : m_id(), m_pEntry(nullptr) {}  EntryID_t m_id; XMLElement* m_pEntry; };
    std::vector<IDs> vecIDs; vecIDs.clear();


    for (auto iEntryIterator = vecEntries.begin(); iEntryIterator != vecEntries.end(); )
    {
        //x TODO Pull out info and construct ID.
        //x TODO Check if any entry has the same ID, and remove accordingly on match.
        //x TODO Store ID.

        XMLElement* pEntry = *iEntryIterator;
        bool bIncrementEntryIterator = true;


        IDs id;
        id.m_pEntry = pEntry;

        int iSecOpcd = 0;
        if (XMLElement* pAttr = FindChildRecurse(pEntry, "sec_opcd"); pAttr != nullptr) iSecOpcd = HexStringToInt(pAttr->GetText(), 2);

        int iOpcdExt = 0;
        if (XMLElement* pAttr = FindChildRecurse(pEntry, "opcd_ext"); pAttr != nullptr) iOpcdExt = atoi(pAttr->GetText());

        int iPrefix = 0;
        if (XMLElement* pAttr = FindChildRecurse(pEntry, "pref");     pAttr != nullptr) iPrefix  = HexStringToInt(pAttr->GetText(), 2);

        // NOTE : Setting nomem to false for all IDs cause we run this function on data processed data
        // that has been split up according to mem and nomem.
        bool bNoMem = false; 

        id.m_id.Set(iOpcdExt, bNoMem, iSecOpcd, iPrefix);
        printf(CYAN "ID constructed : %llu, OpcdExt : %d, NoMem : %d, SecOpcd : %d, Prefix : %d\n" RESET, 
            id.m_id.m_iID, iOpcdExt, bNoMem, iSecOpcd, iPrefix);


        bool bStoreThisID = true;
        for (auto it = vecIDs.begin(); it != vecIDs.end(); )
        {
            bool bIncrementIterator = true;

            // Collision occured !.
            if (id.m_id == it->m_id)
            {
                // Compare Priority of this entry and the entry we found to be already stored in this std::vector.
                // respectively.
                int iThisEntryPriority   = OperationModeToPriority(id.m_pEntry->Attribute("mode"));
                int iStoredEntryPriority = OperationModeToPriority(it->m_pEntry->Attribute("mode"));


                // Check if we already have solution for this ID collision.
                CollisionSolution_t* pSol = FindIDCollisionSol(iPrimOpCode, iTableID, id.m_id, szGroupName);
                if (pSol != nullptr)
                {
                    printf(CYAN "Detected predefined solution for this collision...\n" RESET);
                    
                    // We intentionaly do this before != 0 check. Cause the collision will occur 
                    // after storing the first entry with "this" ID. So Skip count naturally has
                    // +1 offset error, and this fixies it.
                    pSol->m_iSkipsLeft--;

                    // we have solution, so we right the priority variables.
                    if (pSol->m_iSkipsLeft != 0)
                    {
                        iThisEntryPriority = 0; iStoredEntryPriority = 100;
                        printf(YELLOW "Discarding new entry according to predefined collision solution.\n" RESET);
                    }
                    else
                    {
                        iThisEntryPriority = 100; iStoredEntryPriority = 0;
                        printf(GREEN "Storing this entry according to predefined collision solution.\n" RESET);
                    }


                    // It shouldn't get lower than 0.
                    assert(pSol->m_iSkipsLeft >= 0 && "Collision solution used after being exhausted.");
                }


                // 2 entires seem to have same IDs and same priority. Gotta resolve this conflict.
                if (iThisEntryPriority == iStoredEntryPriority)
                {
                    printf(RED "Failed for group %s. OpCode { 0x%02X } opcdExt : 0x%02X, sec_opcd : 0x%02X, prefix : 0x%02X. ID : %llu\n" RESET, 
                        szGroupName != nullptr ? szGroupName : "nullptr",
                        iPrimOpCode, iOpcdExt, iSecOpcd, iPrefix, id.m_id.m_iID);

                    printf(YELLOW "Trying to resolve conflict using proc_start\n" RESET);

                    if (XMLElement* pAttr = FindChildRecurse(pEntry,       "proc_start"); pAttr != nullptr) iThisEntryPriority   = atoi(pAttr->GetText());
                    if (XMLElement* pAttr = FindChildRecurse(it->m_pEntry, "proc_start"); pAttr != nullptr) iStoredEntryPriority = atoi(pAttr->GetText());
                    
                    if (iThisEntryPriority != iStoredEntryPriority)
                    {
                        printf(GREEN "Resolved ID conflict in 0x%02X successfully\n" RESET, iPrimOpCode);
                    }
                    else
                    {
                        printf(RED "Failed to resolve ID conflict using priority.\n" RESET);
                    }
                }


                if (iThisEntryPriority > iStoredEntryPriority)
                {
                    auto badEntry = std::find(vecEntries.begin(), vecEntries.end(), it->m_pEntry);
                    
                    assert(badEntry != vecEntries.end() && "We have stored a entry that doesn't exist in the input. bizzare ass error!");
                    if (badEntry != vecEntries.end())
                    {
                        // Remove bad entry from input.
                        iEntryIterator = vecEntries.erase(badEntry);

                        // Also remove bad entry from stored IDs.
                        it = vecIDs.erase(it);
                        bIncrementIterator = false;
                    }
                }
                else if(iStoredEntryPriority > iThisEntryPriority)
                {
                    // Don't store this entry's ID, cause this is of lower priority, and we don't want it.
                    bStoreThisID = false; 

                    // Find this entry and remove it.
                    auto badEntry = std::find(vecEntries.begin(), vecEntries.end(), pEntry);

                    assert(badEntry != vecEntries.end() && "We have stored a entry that doesn't exist in the input. bizzare ass error!");
                    if (badEntry != vecEntries.end())
                    {
                        // Remove bad entry from input.
                        iEntryIterator = vecEntries.erase(badEntry);
                    }
                }
                else
                {
                    assert(iStoredEntryPriority != iThisEntryPriority && "ID collision occured which couldn't be resolved! Fuck you bitch nigga :(");
                }
                
                printf("ID collision solved...\n");
            }


            if(bIncrementIterator == true)
                it++;
        }


        // Only store this ID if this doens't cause any conflicts
        // or is the superior option in case of conflict.
        if (bStoreThisID == true)
            vecIDs.push_back(id);


        // Don't increment in case we erase something from entry std::vector.
        if (bIncrementEntryIterator == true)
            iEntryIterator++;
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void CollectThreeByteOpCodeEntries(XMLElement* pRoot, std::vector<Entry_t*>& vecOutput, Byte iParentIndex)
{
    assert((iParentIndex == 0x38 || iParentIndex == 0x3A) && "Invalid parent opcode index for three byte entry collection.");

    // Acquire the actual parent.
    XMLElement* pParent = nullptr;
    for (pParent = pRoot->FirstChildElement("pri_opcd"); pParent != nullptr; pParent = pParent->NextSiblingElement("pri_opcd"))
    {
        Byte iByte = 0x00;
        if(const char* szValue = pParent->Attribute("value"); szValue != nullptr)
        {
            iByte = HexStringToInt(szValue, 2);
        }

        if (iByte == iParentIndex)
            break;
    }

    assert(pParent != nullptr && "Failed to find parent opcode from two byte opcode table.");

    
    //? Delete this.
    static Byte s_iCounter[0xFF] = {0};


    // Holds elements with same <sec_opcd> ( i.e. same third opcode byte. )
    std::vector<XMLElement*> vecElements;

    for (XMLElement* pEntry = pParent->FirstChildElement("entry"); pEntry != nullptr; pEntry = pEntry->NextSiblingElement("entry"))
    {
        // Get the byte that actually represents this three byte opcode's last byte.
        static Byte s_iLastByte = 0xFF;
        Byte iByte = 0xFF;
        {
            XMLElement* pSecOpcd = pEntry->FirstChildElement("sec_opcd");
            assert(pSecOpcd != nullptr && "Three byte opcode can't exist without <sec_opcd> tag");

            const char* szSecOpcd = pSecOpcd->GetText();
            assert(szSecOpcd != nullptr && "Three byte opcode can't have an empty <sec_opcd> tag");

            iByte = HexStringToInt(szSecOpcd, 2);
        }


        // Entry properties.... Prefix, opcd_ext, mem, nomem.
        bool bPrefix    = FindChildRecurse(pEntry, "pref")     != nullptr;
        bool bOpcdExt   = FindChildRecurse(pEntry, "opcd_ext") != nullptr;
        bool bOnlyMem   = pEntry->Attribute("mod", "mem")      != nullptr;
        bool bOnlyNoMem = pEntry->Attribute("mod", "nomem")    != nullptr;
        bool bMem = false, bNoMem = false;
        for (XMLElement* pSyntax = FindChildRecurse(pEntry, "syntax"); pSyntax != nullptr; pSyntax = pSyntax->NextSiblingElement("syntax"))
        {
            if (pSyntax->Attribute("mod", "mem")   != nullptr) bMem   = true;
            if (pSyntax->Attribute("mod", "nomem") != nullptr) bNoMem = true;
        }


        // According to my observations, No <entry> tag has mem or nomem tag. 
        // So we are assuming that there will never be one. And break if one appears.
        assert(bOnlyMem == false && bOnlyNoMem == false && "Only Mem and Only NoMem can't exist in three byte opcode table");
        assert(bOpcdExt == false && "Three byte opcodes can't have <opcd_ext> tags");


        s_iCounter[iByte]++;


        // All entries of a third byte opcode has been stored?
        if (s_iLastByte != iByte || pEntry->NextSiblingElement("entry") == nullptr) // NOTE this also handles flushing of vecElements. So we don't leave anything ramining in it.
        {
            if (vecElements.empty() == false)
            {
                printf("%s" "%llu entries found for opcode 0x%02X 0x%02X 0x%02x\n" RESET, 
                    vecElements.size() > 1llu ? YELLOW : "",
                    vecElements.size(), 0x0F, iParentIndex, s_iLastByte);
                

                Byte iEffectiveByte = s_iLastByte;
                if (pEntry->NextSiblingElement("entry") == nullptr)
                {
                    iEffectiveByte = iByte;
                    vecElements.push_back(pEntry);
                }


                // Single element? store as it is. No branching.
                if (vecElements.size() == 1llu)
                {
                    XMLElement* pElement = vecElements.front();
                    Entry_t* pOpCode = new Entry_t();
                    pOpCode->m_iVarientKey = Entry_t::VarientKey_None;

                    ElementToEntry(pElement, pOpCode, iEffectiveByte, false, false, false);

                    vecOutput.push_back(pOpCode);
                }
                else
                {
                    Entry_t* pHead = new Entry_t();

                    for (XMLElement* pElem : vecElements)
                    {
                        Entry_t* pOpCode       = new Entry_t();
                        pOpCode->m_iVarientKey = Entry_t::VarientKey_None;

                        ElementToEntry(pElem, pOpCode, iEffectiveByte, true, false, false);

                        pHead->m_iVarientKey = Entry_t::VarientKey_LegacyPrefix;
                        pHead->m_iByte       = iEffectiveByte;
                        pHead->m_vecVarients.push_back(pOpCode);
                    }

                    vecOutput.push_back(pHead);
                }

                
                vecElements.clear();
            }


            // New third byte.
            s_iLastByte = iByte;
        }


        vecElements.push_back(pEntry);
    }



    // Just so the next batch is spaced out.
    std::cout << std::endl << std::endl;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void Indent(int iIndentation, std::ofstream& hFile) { for (int i = 0; i < iIndentation; i++) hFile << "    "; }


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void PrintInvalidEntry(Byte iByte, const char* szGroupName, std::ofstream& hFile, int iIndentation, bool bValid = false)
{
    // Byte we are printing.
    Indent(iIndentation, hFile); 
    hFile << "// 0x" << std::hex << std::uppercase << static_cast<int>(iByte) << std::nouppercase << std::dec << std::endl;

    // Note about instruction.
    Indent(iIndentation, hFile); 
    hFile << "//? Brief : Invalid instruction in 64-bit mode.\n";

    // Fn call
    Indent(iIndentation, hFile);     
    hFile << szGroupName << "[0x" << std::hex << std::uppercase << static_cast<int>(iByte) << std::nouppercase << std::dec << "].Init(\n";

    // Inst. name
    Indent(iIndentation + 1, hFile); 
    hFile << "/*szName         = */" << "\"xx_INVALID_xx\"" << ",\n";

    // Inst. valid ?
    Indent(iIndentation + 1, hFile); 
    hFile << "/*bValidOpcd     = */" << (bValid == true ? "true" : "false") << ",\n";

    // Inst. escape?
    Indent(iIndentation + 1, hFile); 
    hFile << "/*bEscapeOpcd    = */" << "false" << ",\n";

    // Inst. requires modrm?
    Indent(iIndentation + 1, hFile); 
    hFile << "/*bModrmRequired = */" << "false" << ",\n";

    // Inst. Byte
    Indent(iIndentation + 1, hFile); 
    hFile << "/*iByte          = */0x" << std::hex << std::uppercase << static_cast<int>(iByte) << std::dec << std::nouppercase << ",\n";

    // Inst. operand count.
    Indent(iIndentation + 1, hFile); 
    hFile << "/*nOperands      = */" << 0 << ",\n";
   
    // Operand 1.
    Indent(iIndentation + 1, hFile); 
    hFile << "/*operand1       = */" << "Operand_t()" << ",\n";

    // Opearnd 2. 
    Indent(iIndentation + 1, hFile); 
    hFile << "/*operand2       = */" << "Operand_t()" << ",\n";

    // Operand 3. 
    Indent(iIndentation + 1, hFile); 
    hFile << "/*operand3       = */" << "Operand_t()" << ",\n";

    // Operand 4. 
    Indent(iIndentation + 1, hFile); 
    hFile << "/*operand4       = */" << "Operand_t()" << ");\n";
}


std::unordered_map<std::string, std::string> g_mapRegisterToType =
{
    { "ST", "Register_t( Register_t::RegisterClass_FPU, -1, 64 )" },
    { "FS", "Register_t( Register_t::RegisterClass_Segment, 4, 16 )" },
    { "GS", "Register_t( Register_t::RegisterClass_Segment, 5, 16 )" },

    // ======================
    // 64-bit
    // ======================
    { "rAX", "Register_t( Register_t::RegisterClass_GPR, 0, 64 )" },
    { "rCX", "Register_t( Register_t::RegisterClass_GPR, 1, 64 )" },
    { "rDX", "Register_t( Register_t::RegisterClass_GPR, 2, 64 )" },
    { "rBX", "Register_t( Register_t::RegisterClass_GPR, 3, 64 )" },
    { "rSP", "Register_t( Register_t::RegisterClass_GPR, 4, 64 )" },
    { "rBP", "Register_t( Register_t::RegisterClass_GPR, 5, 64 )" },
    { "rSI", "Register_t( Register_t::RegisterClass_GPR, 6, 64 )" },
    { "rDI", "Register_t( Register_t::RegisterClass_GPR, 7, 64 )" },
    { "r8",  "Register_t( Register_t::RegisterClass_GPR, 8, 64 )" },
    { "r9",  "Register_t( Register_t::RegisterClass_GPR, 9, 64 )" },
    { "r10", "Register_t( Register_t::RegisterClass_GPR, 10, 64 )" },
    { "r11", "Register_t( Register_t::RegisterClass_GPR, 11, 64 )" },
    { "r12", "Register_t( Register_t::RegisterClass_GPR, 12, 64 )" },
    { "r13", "Register_t( Register_t::RegisterClass_GPR, 13, 64 )" },
    { "r14", "Register_t( Register_t::RegisterClass_GPR, 14, 64 )" },
    { "r15", "Register_t( Register_t::RegisterClass_GPR, 15, 64 )" },

    // ======================
    // 32-bit
    // ======================
    { "eAX", "Register_t( Register_t::RegisterClass_GPR, 0, 32 )" },
    { "eCX", "Register_t( Register_t::RegisterClass_GPR, 1, 32 )" },
    { "eDX", "Register_t( Register_t::RegisterClass_GPR, 2, 32 )" },
    { "eBX", "Register_t( Register_t::RegisterClass_GPR, 3, 32 )" },
    { "eSP", "Register_t( Register_t::RegisterClass_GPR, 4, 32 )" },
    { "eBP", "Register_t( Register_t::RegisterClass_GPR, 5, 32 )" },
    { "eSI", "Register_t( Register_t::RegisterClass_GPR, 6, 32 )" },
    { "eDI", "Register_t( Register_t::RegisterClass_GPR, 7, 32 )" },
    { "r8d", "Register_t( Register_t::RegisterClass_GPR, 8, 32 )" },
    { "r9d", "Register_t( Register_t::RegisterClass_GPR, 9, 32 )" },
    { "r10d","Register_t( Register_t::RegisterClass_GPR, 10, 32 )" },
    { "r11d","Register_t( Register_t::RegisterClass_GPR, 11, 32 )" },
    { "r12d","Register_t( Register_t::RegisterClass_GPR, 12, 32 )" },
    { "r13d","Register_t( Register_t::RegisterClass_GPR, 13, 32 )" },
    { "r14d","Register_t( Register_t::RegisterClass_GPR, 14, 32 )" },
    { "r15d","Register_t( Register_t::RegisterClass_GPR, 15, 32 )" },

    // ======================
    // 16-bit
    // ======================
    { "AX",  "Register_t( Register_t::RegisterClass_GPR, 0, 16 )" },
    { "CX",  "Register_t( Register_t::RegisterClass_GPR, 1, 16 )" },
    { "DX",  "Register_t( Register_t::RegisterClass_GPR, 2, 16 )" },
    { "BX",  "Register_t( Register_t::RegisterClass_GPR, 3, 16 )" },
    { "SP",  "Register_t( Register_t::RegisterClass_GPR, 4, 16 )" },
    { "BP",  "Register_t( Register_t::RegisterClass_GPR, 5, 16 )" },
    { "SI",  "Register_t( Register_t::RegisterClass_GPR, 6, 16 )" },
    { "DI",  "Register_t( Register_t::RegisterClass_GPR, 7, 16 )" },
    { "r8w", "Register_t( Register_t::RegisterClass_GPR, 8, 16 )" },
    { "r9w", "Register_t( Register_t::RegisterClass_GPR, 9, 16 )" },
    { "r10w","Register_t( Register_t::RegisterClass_GPR, 10, 16 )" },
    { "r11w","Register_t( Register_t::RegisterClass_GPR, 11, 16 )" },
    { "r12w","Register_t( Register_t::RegisterClass_GPR, 12, 16 )" },
    { "r13w","Register_t( Register_t::RegisterClass_GPR, 13, 16 )" },
    { "r14w","Register_t( Register_t::RegisterClass_GPR, 14, 16 )" },
    { "r15w","Register_t( Register_t::RegisterClass_GPR, 15, 16 )" },

    // ======================
    // 8-bit LOW
    // ======================
    { "AL",  "Register_t( Register_t::RegisterClass_GPR, 0, 8 )" },
    { "CL",  "Register_t( Register_t::RegisterClass_GPR, 1, 8 )" },
    { "DL",  "Register_t( Register_t::RegisterClass_GPR, 2, 8 )" },
    { "BL",  "Register_t( Register_t::RegisterClass_GPR, 3, 8 )" },
    { "spl", "Register_t( Register_t::RegisterClass_GPR, 4, 8 )" },
    { "bpl", "Register_t( Register_t::RegisterClass_GPR, 5, 8 )" },
    { "sil", "Register_t( Register_t::RegisterClass_GPR, 6, 8 )" },
    { "dil", "Register_t( Register_t::RegisterClass_GPR, 7, 8 )" },
    { "r8b", "Register_t( Register_t::RegisterClass_GPR, 8, 8 )" },
    { "r9b", "Register_t( Register_t::RegisterClass_GPR, 9, 8 )" },
    { "r10b","Register_t( Register_t::RegisterClass_GPR, 10, 8 )" },
    { "r11b","Register_t( Register_t::RegisterClass_GPR, 11, 8 )" },
    { "r12b","Register_t( Register_t::RegisterClass_GPR, 12, 8 )" },
    { "r13b","Register_t( Register_t::RegisterClass_GPR, 13, 8 )" },
    { "r14b","Register_t( Register_t::RegisterClass_GPR, 14, 8 )" },
    { "r15b","Register_t( Register_t::RegisterClass_GPR, 15, 8 )" },
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline std::string FixRegisterName(std::string szRegister)
{
    auto it = g_mapRegisterToType.find(szRegister);
    if (it == g_mapRegisterToType.end())
    {
        printf(RED "@@@@@@@@    Failed to find register { %s } in g_mapRegisterToType map\n" RESET, szRegister.c_str());
        return szRegister;
    }

    return it->second;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void RemoveNonAsciiChars(std::string& sz)
{
    for (auto it = sz.begin(); it != sz.end();)
    {
        char c = *it;
        
        if ((c > 0x7E || c < 0x20) && c != '\n')
            it = sz.erase(it);
        else
            it++;
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void PrintValidEntry(Entry_t* pEntry, std::string szGroupName, std::ofstream& hFile, int iIndentation, bool bUseArrow)
{
    // Byte we are printing.
    Indent(iIndentation, hFile); 
    hFile << "// 0x" << std::hex << std::uppercase << static_cast<int>(pEntry->m_iByte) << std::nouppercase << std::dec << std::endl;

    // Note about instruction.
    Indent(iIndentation, hFile);
    hFile << "// Brief : " << pEntry->m_szBrief << std::endl;

    // Fn call
    Indent(iIndentation, hFile);     
    hFile << szGroupName << (bUseArrow == true ? "->" : ".") << "Init(\n";

    // Inst. name
    Indent(iIndentation + 1, hFile); 
    hFile << "/*szName         = */\"" << pEntry->m_szName << "\",\n";

    // Inst. valid ?
    Indent(iIndentation + 1, hFile); 
    hFile << "/*bValidOpcd     = */" << "true" << ",\n";

    // Inst. escape?
    Indent(iIndentation + 1, hFile); 
    hFile << "/*bEscapeOpcd    = */" << "false" << ",\n";

    // Inst. requires modrm?
    Indent(iIndentation + 1, hFile); 
    hFile << "/*bModrmRequired = */" << (pEntry->m_bModRm == true ? "true" : "false") << ",\n";

    // Inst. Byte
    Indent(iIndentation + 1, hFile); 
    hFile << "/*iByte          = */0x" << std::hex << std::uppercase << static_cast<int>(pEntry->m_iByte) << std::dec << std::nouppercase << ",\n";

    // Inst. operand count.
    Indent(iIndentation + 1, hFile); 
    hFile << "/*nOperands      = */" << pEntry->m_nOperands << ",\n";

    // Operands.
    for(int i = 0; i < 4; i++)
    {
        Indent(iIndentation + 1, hFile);

        if(i < pEntry->m_nOperands)
        {
            hFile << "/*operand" << i + 1<< "       = */" << "Operand_t( ";

            switch (pEntry->m_operand[i].m_iOperandType)
            {
            case Entry_t::Operand_t::OperandCatagory_Literal:  hFile << pEntry->m_operand[i].m_iLiteral; break;
            case Entry_t::Operand_t::OperandCatagory_Register: hFile << FixRegisterName(pEntry->m_operand[i].m_szRegister); break;
            case Entry_t::Operand_t::OperandCatagory_Legacy:   hFile << "OperandMode_" << pEntry->m_operand[i].m_szOperandMode << ", " << "OperandType_" << pEntry->m_operand[i].m_szOperandType; break;
            default: break;
            }

            hFile << " )";
        }
        else
        {
            hFile << "/*operand" << i + 1 << "       = */" << "Operand_t()";
        }

        if (i == 3)
        {
            hFile << ");\n";
        }
        else
        {
            hFile << ",\n";
        }
    }

    // hFile << std::endl;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int GetLegacyPrefixIndex(int iByte)
{
    //! NOTE : This fn is one to one copy of what is present in the disassembler.
    switch (iByte)
    {
    case 0xF0: return 1;
    case 0xF2: return 2;
    case 0xF3: return 3;
    case 0x2E: return 4;
    case 0x36: return 5;
    case 0x3E: return 6;
    case 0x26: return 7;
    case 0x64: return 8;
    case 0x65: return 9;
    case 0x66: return 10;
    case 0x67: return 11;

    default: break;
    }

    return 0;
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void PrintEntry(Entry_t* pEntry, std::string szGroupName, std::ofstream& hFile, int iIndentation)
{
    // This is just a tiny map, to help us link our types to the disassemblers types.
    struct TypeToString_t { int m_iType; const char* m_szString; };
    static TypeToString_t s_typeToString[5] = {
        {Entry_t::VarientKey_None,         "OpCodeDesc_t::VarientType_t::VarientKey_None"},
        {Entry_t::VarientKey_ModRM_REG,    "OpCodeDesc_t::VarientType_t::VarientKey_ModRM_REG"},
        {Entry_t::VarientKey_ModRM_MOD,    "OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD"},
        {Entry_t::VarientKey_ModRM_RM,     "OpCodeDesc_t::VarientType_t::VarientKey_ModRM_RM"},
        {Entry_t::VarientKey_LegacyPrefix, "OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix"},
    };


    static int s_iRecurseCounter = 0;
    s_iRecurseCounter++;


    if (pEntry->m_iVarientKey == Entry_t::VarientKey_None)
    {
        // Don't dump invalid / filler entry.
        if(pEntry->m_szName != "xx_INVALID_xx")
            PrintValidEntry(pEntry, szGroupName, hFile, iIndentation, s_iRecurseCounter > 1);
    }
    else
    {
        // Dump this too, so its marked as valid entry.
        // Indent(iIndentation, hFile);
        PrintValidEntry(pEntry, szGroupName, hFile, iIndentation, s_iRecurseCounter > 1);
        hFile << std::endl;


        Indent(iIndentation, hFile);
        std::string szFnName = s_iRecurseCounter == 1 ? 
            std::format("{}.InitVarientType(",  szGroupName) : 
            std::format("{}->InitVarientType(", szGroupName);

        //? Delete this
        for (int i = 0; i < iIndentation; i++) printf("    ");
        printf("VarientKey : %d { %s }\n", pEntry->m_iVarientKey, s_typeToString[pEntry->m_iVarientKey].m_szString);

        hFile << szFnName << s_typeToString[pEntry->m_iVarientKey].m_szString << ");\n\n";


        for (size_t iChildIndex = 0llu; iChildIndex < pEntry->m_vecVarients.size(); iChildIndex++)
        {
            Entry_t* pChildEntry = pEntry->m_vecVarients[iChildIndex];
            assert(pChildEntry != nullptr && "nullptr child entry");


            // Incase this child is a filler, don't dump it.
            if (pChildEntry->m_iVarientKey == Entry_t::VarientKey_None && pChildEntry->m_szName == "xx_INVALID_xx")
                continue;


            // Child's key
            int iChildKey = -1;
            switch (pEntry->m_iVarientKey)
            {
            case Entry_t::VarientKey_LegacyPrefix: iChildKey = GetLegacyPrefixIndex(pChildEntry->m_iPrefix); break;
            case Entry_t::VarientKey_ModRM_REG:    iChildKey = pChildEntry->m_iOpcdExt;                      break;
            case Entry_t::VarientKey_ModRM_MOD:    iChildKey = iChildIndex;                                  break; // TODO Gotta handle this seperatly.
            case Entry_t::VarientKey_ModRM_RM:     iChildKey = (pChildEntry->m_iSecOpcd & 0b111);            break;
            default: break;
            }


            // Incase of mod split, we will copy the pointer at index 0 to index 1 and 2.
            if(pEntry->m_iVarientKey == Entry_t::VarientKey_ModRM_MOD)
            {
                if (iChildKey == 0)
                {
                    Indent(iIndentation, hFile);
                    hFile << "// Copying index 0 to index 1 and 2, cause modrm.mod == 0, 1 or 2 collectively represents \"mem\" catagory.\n";


                    // Dump InsertVarient(int iIndex); fn call.
                    Indent(iIndentation, hFile);
                    std::string szFnName = s_iRecurseCounter == 1 ? 
                        std::format("{}.InsertVarient(",  szGroupName) : 
                        std::format("{}->InsertVarient(", szGroupName);

                    hFile << szFnName << iChildKey << ");\n";


                    for(int i = 1; i <= 2; i++)
                    {
                        Indent(iIndentation, hFile);

                        std::string szFnCall = s_iRecurseCounter == 1 ?
                            std::format("{}.m_pVarients[0x{:02X}] = {}.m_pVarients[0x00];",   szGroupName, i, szGroupName) :
                            std::format("{}->m_pVarients[0x{:02X}] = {}->m_pVarients[0x00];", szGroupName, i, szGroupName);

                        hFile << szFnCall << std::endl;
                    }

                    // hFile << std::endl;
                }
                else if (iChildKey == 1) // One is actually for index 3
                {
                    iChildKey = 3;


                    // Dump InsertVarient(int iIndex); fn call.
                    Indent(iIndentation, hFile);
                    std::string szFnName = s_iRecurseCounter == 1 ? 
                        std::format("{}.InsertVarient(",  szGroupName) : 
                        std::format("{}->InsertVarient(", szGroupName);

                    hFile << szFnName << iChildKey << ");\n";

                }
            }
            else
            {
                // Dump InsertVarient(int iIndex); fn call.
                Indent(iIndentation, hFile);
                std::string szFnName = s_iRecurseCounter == 1 ? 
                    std::format("{}.InsertVarient(",  szGroupName) : 
                    std::format("{}->InsertVarient(", szGroupName);

                hFile << szFnName << iChildKey << ");\n";
            }

            
            std::string szObjName = s_iRecurseCounter == 1 ? 
                std::format("{}.m_pVarients[0x{:02X}]",  szGroupName, iChildKey) : 
                std::format("{}->m_pVarients[0x{:02X}]", szGroupName, iChildKey);

            Indent(iIndentation, hFile); hFile << "{\n";
            PrintEntry(pChildEntry, szObjName, hFile, iIndentation + 1);
            Indent(iIndentation, hFile); hFile << "}\n";
        }
    }


    s_iRecurseCounter--;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void DumpTable(std::vector<Entry_t*>& vecEntries, const char* szTableName, std::ofstream& hFile)
{
    Byte iByte = 0x00;


    for (Entry_t* pEntry : vecEntries)
    {
        while (pEntry->m_iByte > iByte)
        {
            PrintInvalidEntry(iByte, szTableName, hFile, 1);
            hFile << std::endl;
            iByte++;
        }

        std::string szObjName = std::format("{}[0x{:02X}]", szTableName, static_cast<int>(pEntry->m_iByte));
        PrintEntry(pEntry, szObjName, hFile, 1);
        hFile << std::endl;
        iByte++;
    }
}