#include <unordered_map>
#include <vector>
#include <iostream>
#include <assert.h>
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
    {"b", "8"}, {"bcd", "80dec"}, {"bs", "8"}, {"bsq", "!"}, {"bss", "8"},
    {"c", "!"},
    {"d", "32"}, {"di", "32int"}, {"dqp", "32_64"}, {"ds", "32"},
    {"dq", "128"},
    {"dr", "64real"}, // No equivalent in intel's manual.
    {"e", "14_28"}, {"er", "80real"}, // Not equivalent in intel's manual.
    {"p", "p"}, 
    {"pi", "64mmx"}, 
    {"pd", "!"}, 
    {"ps", "128pf"},
    {"psq", "64"},
    {"pt", "!"},
    {"ptp", "ptp"},
    {"q", "64"},
    {"qi", "64int"},
    {"qp", "64"},
    // Note : Nothing seems to be connecting to the qq operandtype ( 256 bits ).
    {"s", "!"}, {"sd", "!"}, {"si", "!"},
    {"ss", "!"},
    {"sr", "32real"}, {"st", "94_108"}, {"stx", "512"},
    {"t", "!"},
    {"v", "16_32"}, {"vds", "16_32"}, {"vs", "16_32"}, {"vqp", "16_32_64"},
    {"vq", "64_16"},
    {"w", "16"}, {"wi", "16int"}
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
struct EntryID_t
{
    EntryID_t() { m_iID = 0; }
    EntryID_t(int iOpcdExt, bool bNoMem, int iSecOpcd, int iPrefix) { Set(iOpcdExt, bNoMem, iSecOpcd, iPrefix); }
    

    void Set(int iOpcdExt, bool bNoMem, int iSecOpcd, int iPrefix)
    {
        // 0 - 7, prefix.
        m_iID = static_cast<uint64_t>(iPrefix);

        uint64_t iStartBitIndex = 8;
        m_iID |= static_cast<uint64_t>(iSecOpcd & 0b111) << iStartBitIndex; iStartBitIndex += 3llu;
        m_iID |= static_cast<uint64_t>(iOpcdExt & 0b111) << iStartBitIndex; iStartBitIndex += 3llu;
        m_iID |= (bNoMem == true ? 1llu : 0llu) << iStartBitIndex; iStartBitIndex += 1llu;
    }


    bool operator==(EntryID_t other)
    {
        return m_iID == other.m_iID;
    }


    uint64_t m_iID = 0;
};


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
    }

    std::string m_szName    = "xx_INVALID_xx";
    bool        m_bIsValid  = false;
    bool        m_bIsEscape = false;
    Byte        m_iByte     = 0x00;
    std::string m_szBrief   = "Invalid Instruction in 64-Bit Mode";
    EntryID_t   m_iID;

    struct Operand_t
    {
        Operand_t() { Clear(); }
        void Clear() { m_iOperandType = OperandCatagory_t::OperandCatagory_None; }

        enum OperandCatagory_t { OperandCatagory_None = -1, OperandCatagory_Legacy = 0, OperandCatagory_Literal, OperandCatagory_Register };
        OperandCatagory_t m_iOperandType = OperandCatagory_t::OperandCatagory_None;
        
        int         m_iLiteral      = 0;

        std::string m_szRegister    = "NIGGA";

        std::string m_szOperandMode = "NIGGA";
        std::string m_szOperandType = "NIGGA";
    };
    Operand_t m_operand[4];
    int       m_nOperands = 0;


    enum VarientKey_t : int16_t
    {
        VarientKey_None = -1,
        VarientKey_ModRM_REG = 0,
        VarientKey_ModRM_MOD,
        VarientKey_ModRM_RM,
        VarientKey_LegacyPrefix,
    };
    VarientKey_t          m_iVarientKey = VarientKey_None; // Do I need to store any more of these?
    std::vector<Entry_t*> m_vecVarients;

    int m_iSecOpcd = -1;
    int m_iPrefix  = -1;
    int m_iOpcdExt = -1;
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void        ToUpperInPlace       (std::string& szInput);
int         HexStringToInt       (const char* szInput, int iSize);
void        ExtractSquareBrackets(std::string& szInput);
XMLElement* FindChildRecurse     (XMLElement* pParent, const char* szChildName);
void        RemoveInvalidEntries(std::vector<XMLElement*>& vecEntries, Byte iPrimOpCode, const char* szGroupName = nullptr);

// Actual parsing logic...
void CollectEntires(XMLElement* pRoot, std::vector<Entry_t*>& vecOutput);
void ElementToEntry(XMLElement* pElem, Entry_t* pDest, Byte iByte, bool bPrefix, bool bSecOpcd, bool bOpcdExt);
void CombineEntries(Byte iByte, Entry_t* pOutput,
    const std::vector<XMLElement*>& vecLevel1,
    const std::vector<XMLElement*>& vecLevel1Prefix, const std::vector<XMLElement*>& vecLevel2Mem,
    const std::vector<XMLElement*>& vecLevel2MemPrefix,
    const std::vector<XMLElement*>& vecLevel2NoMem,
    const std::vector<XMLElement*>& vecLevel2SecOpcd, const std::vector<XMLElement*>& vecLevel2SecOpcdPrefix);
void DumpEntryInfo(std::vector<Entry_t*>& vecEntries);



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int main(void)
{
    // Load file.
    XMLDocument g_doc(true, tinyxml2::PRESERVE_WHITESPACE);
    if (g_doc.LoadFile("F:\\tinyxml2-11.0.0\\TableGen\\src\\x86reference.xml") != XMLError::XML_SUCCESS) { printf("Failed to open file\n"); return 0; }


    // Root element.
    XMLElement* pRootElem = g_doc.RootElement();
    if (pRootElem == nullptr)
    {
        printf("Root Element of the table couldn't be acquired!\n");
        return 0;
    }
    printf("Table acquired successfuly!\n");


    // One-byte opcodes...
    // std::vector<Entry_t*> vecOneByteOpCodes; 
    // vecOneByteOpCodes.reserve(0xFF); vecOneByteOpCodes.clear(); 
    // CollectEntires(pRootElem->FirstChildElement("one-byte"), vecOneByteOpCodes);
    

    // Two byte opcodes...
    std::vector<Entry_t*> vecTwoByteOpCodes; 
    vecTwoByteOpCodes.reserve(0xFF); vecTwoByteOpCodes.clear(); 
    CollectEntires(pRootElem->FirstChildElement("two-byte"), vecTwoByteOpCodes);

    
    // DumpEntryInfo(vecOneByteOpCodes);
    DumpEntryInfo(vecTwoByteOpCodes);


    return 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void CollectEntires(XMLElement* pRoot, std::vector<Entry_t*>& vecOutput)
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


        // Level 1 : All entires that don't have a <opcd_ext> tag are catagorized under level 1.
        //           Idea is that these instructions are stored in the opcode table itself in the intel's manual. ( no sub-stable )
        //           Only way to produce varients in these instructions is using legacy prefixies.

        // Level 2 : Any entries that have a <opcd_ext> tag are catagorised under level2. 
        //           Idea is that these entires are stored in a subtable in the intel's manual,
        //           and require the modrm.REG bits to be accessed.

        
        // Holds all level1 entries, which don't have any prefix tag.
        std::vector<XMLElement*> vecLevel1;              vecLevel1.clear();

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
        

        // Checking if any entries for this opcode has 64-bit mode mentioned explicity.
        // In which case we will only record "explicit 64-bit mode" entries...
        bool bExplicitLongModeOnly = false;
        for (XMLElement* pEntry = pPriOpcd->FirstChildElement("entry"); pEntry != nullptr; pEntry = pEntry->NextSiblingElement("entry"))
        {
            if (const char* szEntryMode = pEntry->Attribute("mode"); szEntryMode != nullptr)
            {
                if (std::string(szEntryMode) == std::string("e"))
                {
                    printf(RED "<!-- Explicit long mode for opcode [ 0x%02X ] -->\n" RESET, iByte);
                    bExplicitLongModeOnly = true; 
                    break;
                }
            }
        }


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
                assert(bSecOpcd     == false            && "Both <syntax> tag and <entry> tag have mem/nomem property.");
                assert(bPrefixSplit == false            && "Both <syntax> tag and <entry> tag have mem/nomem property.");
            }


            // NOTE: Now this entry has a mnemonic tag, and is valid in long mode. So we can store this.
            vecTempEntries.push_back(pEntry);


            // Sorting Entries and storing under correct catagory.
            if (bOpcdExt == false) 
            {
                if (bPrefixSplit == true && bOnlyMem == false && bOnlyNoMem == false) 
                {
                    vecLevel1Prefix.push_back(pEntry);
                }
                else 
                {
                    vecLevel1.push_back(pEntry);
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
                vecLevel1.size()        + vecLevel1Prefix.size() +
                vecLevel2Mem.size()     + vecLevel2MemPrefix.size() + vecLevel2NoMem.size() +
                vecLevel2SecOpcd.size() + vecLevel2SecOpcdPrefix.size();

            iSizeCombined -= static_cast<size_t>(iDuplicates);

            if (iSizeCombined != vecTempEntries.size())
            {
                printf("Size Combined : %llu, Total Entries : %llu\n", iSizeCombined, vecTempEntries.size());
                assert(iSizeCombined == vecTempEntries.size() && "Combined entries don't match total entries");
            }
        }


        RemoveInvalidEntries(vecLevel1,              iByte, "vecLevel1");
        RemoveInvalidEntries(vecLevel1Prefix,        iByte, "vecLevel1Prefix");
        RemoveInvalidEntries(vecLevel2Mem,           iByte, "vecLevel2Mem");
        RemoveInvalidEntries(vecLevel2MemPrefix,     iByte, "vecLevel2MemPrefix");
        RemoveInvalidEntries(vecLevel2NoMem,         iByte, "vecLevel2NoMem");
        RemoveInvalidEntries(vecLevel2SecOpcd,       iByte, "vecLevel2SecOpcd");
        RemoveInvalidEntries(vecLevel2SecOpcdPrefix, iByte, "vecLevel2SecOpcdPrefix");

        size_t iSizeCombined = vecLevel1.size() + vecLevel1Prefix.size() + vecLevel2Mem.size() + 
            vecLevel2MemPrefix.size() + vecLevel2NoMem.size() + vecLevel2SecOpcd.size() + vecLevel2SecOpcdPrefix.size();
        iSizeCombined -= static_cast<size_t>(iDuplicates);

        printf("Merging %llu Entry for \"0x%02X\"...\n", iSizeCombined, iByte);

        Entry_t* pEntry = new Entry_t();
        pEntry->m_iByte = iByte; // Gotta store it somewhere :(
        CombineEntries(iByte, pEntry, vecLevel1, vecLevel1Prefix, vecLevel2Mem, vecLevel2MemPrefix, vecLevel2NoMem, vecLevel2SecOpcd, vecLevel2SecOpcdPrefix);
        vecOutput.push_back(pEntry);
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
    pEntry->m_szBrief = FindChildRecurse(pElem, "brief")->GetText();


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

    
    int iOperandIndex = 0;
    auto ExtractOperands = [&](const char* szTag) -> void
        {
            for(XMLElement* pOperand = FindChildRecurse(pSyntax, szTag); pOperand != nullptr; pOperand = pOperand->NextSiblingElement(szTag))
            {
                Entry_t::Operand_t* pDestOperand = &pEntry->m_operand[iOperandIndex];


                // Ignore operands with "displayed" set to no.
                if (const char* szDisplayed = pOperand->Attribute("displayed"); szDisplayed != nullptr && std::string(szDisplayed) == std::string("no"))
                {
                    printf(YELLOW "Skipping hidden operand!\n" RESET);
                    continue;
                }


                // If operand < dst > or < src > has tag <a> then its legacy prefix.
                if (XMLElement* pOperandMode = pOperand->FirstChildElement("a"); pOperandMode != nullptr)
                {
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
void CombineEntries(Byte iByte, Entry_t* pOutput, const std::vector<XMLElement*>& vecLevel1, const std::vector<XMLElement*>& vecLevel1Prefix, const std::vector<XMLElement*>& vecLevel2Mem, const std::vector<XMLElement*>& vecLevel2MemPrefix, const std::vector<XMLElement*>& vecLevel2NoMem, const std::vector<XMLElement*>& vecLevel2SecOpcd, const std::vector<XMLElement*>& vecLevel2SecOpcdPrefix)
{
    //x TODO: Store Bytes in each entry.
    //x TODO: Don't print invalid entries.
    // TODO 0x0f 0x01 ain't getting registerd / printed.


    pOutput->Clear();
    pOutput->m_iByte = iByte; // Common in all entries.


    size_t iLevel1Size = vecLevel1.size() + vecLevel1Prefix.size();
    size_t iLevel2Size = vecLevel2Mem.size() + vecLevel2MemPrefix.size() + vecLevel2NoMem.size() + vecLevel2SecOpcd.size() + vecLevel2SecOpcdPrefix.size();

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
            for (XMLElement* pElem : vecLevel1)
            {
                Entry_t* pChildEntry = new Entry_t();
                ElementToEntry(pElem, pChildEntry, pOutput->m_iByte,false, false, false);
                
                pOutput->m_vecVarients.push_back(pChildEntry);
            }


            // There can be upto 3 or 4 and maybe even more.
            for (XMLElement* pElem : vecLevel1Prefix)
            {
                Entry_t* pChildEntry = new Entry_t();
                ElementToEntry(pElem, pChildEntry, pOutput->m_iByte,true, false, false);

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


            auto CollectSimilarEntries = [&](const std::vector<XMLElement*>& vecElements, int iREG, std::vector<XMLElement*>& vecOutput) -> int
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
            int nMemEntries           = CollectSimilarEntries(vecLevel2Mem,           i, vecMem);
            int nMemPrefixEntries     = CollectSimilarEntries(vecLevel2MemPrefix,     i, vecMem);

            int nNoMemEntries         = CollectSimilarEntries(vecLevel2NoMem,         i, vecNoMem);

            int nSecOpcdEntries       = CollectSimilarEntries(vecLevel2SecOpcd,       i, vecSecOpcd);
            int nSecOpcdPrefixEntries = CollectSimilarEntries(vecLevel2SecOpcdPrefix, i, vecSecOpcd);

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
                    assert(nMemEntries == 1 && "Mem entries is not 1 and prefix entries are 0 and sum is greather than 0??!?!");

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
                    Byte iRegisteredRMVarients[8] = { 0 };


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
                        int iRM = iRegisteredRMVarients[iRMSlotIndex];
                        
                        if (iRM == 0)
                        {
                            pNoMem->m_vecVarients.push_back(pDefaultEntry);
                            nDefaultEntries++;
                            printf("%d, ", iRMSlotIndex);
                        }
                    }
                    printf("\n");
                    assert(nDefaultEntries < 8 && nDefaultEntries >= 0 && "We can't store all RM varients to default entries if there is a collision!");
                }
            }


            // now pMem and pNoMem should have their entries placed in them nicely.
            // So now we can just place them in the output's ass.
            Entry_t* pRegEntry       = new Entry_t();
            pRegEntry->m_iVarientKey = Entry_t::VarientKey_ModRM_MOD;
            pRegEntry->m_iOpcdExt    = i;

            pRegEntry->m_vecVarients.push_back(pMem);
            pRegEntry->m_vecVarients.push_back(pNoMem);

            pOutput->m_vecVarients.push_back(pRegEntry);
        }
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void DumpEntryRecurse(std::vector<Entry_t*> vecEntries, int iIndentation)
{
    for (Entry_t* pEntry : vecEntries)
    {
        if (pEntry->m_szName != "xx_INVALID_xx")
        {
            for (int i = 0; i < iIndentation; i++)
                printf(".   ");


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

        DumpEntryRecurse(pEntry->m_vecVarients, iIndentation + 1);

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
void RemoveInvalidEntries(std::vector<XMLElement*>& vecEntries, Byte iPrimOpCode, const char* szGroupName)
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

            // Collision occured.
            if (id.m_id == it->m_id)
            {
                // Compare Priority of this entry and the entry we found to be already stored in this std::vector.
                // respectively.
                int iThisEntryPriority   = OperationModeToPriority(id.m_pEntry->Attribute("mode"));
                int iStoredEntryPriority = OperationModeToPriority(it->m_pEntry->Attribute("mode"));


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
                        printf(RED "Failed to resolve priority conflict.\n" RESET);
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
                
                printf(RED "Discarded invalid entry!\n" RESET);
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
