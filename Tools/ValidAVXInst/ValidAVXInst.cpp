//=========================================================================
//                      Valid AVX instruction table gen.    
//=========================================================================
// by      : INSANE
// created : 11/01/2026
//
// purpose : Parses the intel's architecture software developer manual and dumps
//           a list of all valid AVX instructions in a specific manner.
//-------------------------------------------------------------------------


/*
ReadMe : 
    We use Poppler ( https://github.com/oschwartz10612/poppler-windows ) to parse the 
    ( Intel® 64 and IE-21 Architectures Software Developer's Manual, Combined Volumes: 1, 2, 3, and 4 )
    from page number 3048 to 3084, where a list of all valid AVX instructions can be found. 

    Purpose of the generated output is just to help us check if a opcode is VEX encodable or not. As not all 
    possible VEX encoded instrutions are valid, and some validty checks are always good.

    Update the page number, file name & output file name before running.
*/


#include <cstring>
#include <iomanip>
#include <iostream>
#include <cstdint>
#include <assert.h>
#include <string>
#include <fstream>
#include "poppler/Library/include/poppler/cpp/poppler-document.h"
#include "poppler/Library/include/poppler/cpp/poppler-page.h"


// Color terminal logs
#define RED          "\033[31m"
#define GREEN        "\033[32m"
#define YELLOW       "\033[93m"
#define RESET        "\033[0m"
#define CYAN         "\033[96m"
#define ITALIC       "\x1b[3m"
#define ITALIC_RESET "\x1b[0m"


// Globals
using namespace poppler;
std::string      g_szFileName        = "test.pdf";
constexpr size_t PAGE_START          = 3048;
constexpr size_t PAGE_END            = 3084;

// This bigger encoding hifen character occurs after each instruction name.
const uint32_t   g_iHifenBE          = 0x00E28094; // Big Endian
const uint32_t   g_iHifenLE          = 0x009480E2; // Litten Endian
const size_t     g_iHifenPatternSize = 3llu;
std::string      g_szDelimiter1("C4:");
std::string      g_szDelimiter2("C5:");



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
struct VEXOpCode_t
{
    VEXOpCode_t() { /*Since all members are initialized we want the default constructor to do nothing. ( and eat fivestar :) )*/ }

    enum LeadingOpCodes_t : int
    {
        LeadingOpCode_0x0F = 1, // This is the default value
        LeadingOpCode_0x38,
        LeadingOpCode_0x3A,
    };
    static inline const char* LeadingOpCodeToString(LeadingOpCodes_t iLeadingOpCode)
    {
        switch (iLeadingOpCode) 
        {
            case LeadingOpCode_0x0F: return "0x0F";
            case LeadingOpCode_0x38: return "0x0F 0x38";
            case LeadingOpCode_0x3A: return "0x0F 0x3A";
        }
        return "INVALID LEADING OPCODE";
    }

    std::string      m_szInstName       = "xx_INVALID_xx";
    std::string      m_szBrief          = "xx_INVALID_xx";

    struct OpCode_t
    {
        LeadingOpCodes_t m_iLeadingOpCode   = LeadingOpCodes_t::LeadingOpCode_0x0F;
        unsigned char    m_iByte            = 0x00;
        bool             m_bThreeBytePrefix = false; // if not 3 byte prefix, then 2 byte prefix.
    };

    std::vector<OpCode_t> m_vecValidOpCodes;
};
std::vector<VEXOpCode_t*> g_vecValidVEXOps;
                                        

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool IsLineHeader(const std::string& szLine);
bool IsLineInfo  (const std::string& szLine);
bool ExtractHeaderInfo(VEXOpCode_t* pOut, const std::string& szLine);
bool ExtractOpCodeInfo(VEXOpCode_t::OpCode_t* pOut, const std::string& szLine);
void DumpOpCodes(const std::vector<VEXOpCode_t*>& vec);
inline bool CommonPrefix(const char* szPrefix, const char* szString) { while(*szPrefix != '\0') { if(*szPrefix != *szString) return false; szPrefix++; szString++; } return true; }
inline int HexStringToInt(const char* szBegin, const char* szEnd = nullptr)
{
    int iOut = 0;

    while(*szBegin != '\0' && szBegin != szEnd)
    {
        iOut *= 0x10;

        char c = *szBegin;

        if(c >= 'a' && c <= 'f')
            iOut += c - 'a' + 10;
        else if(c >= 'A' && c <= 'F')
            iOut += c - 'A' + 10;
        else if(c >= '0' && c <= '9')
            iOut += c - '0';
        else return iOut;


        szBegin++;
    }

    return iOut;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int main(void)
{
    printf(RESET); // Just so colors are resetted in case terminal is stainted from last use?
    g_vecValidVEXOps.clear();

    // Open the pdf file
    document* pDoc = poppler::document::load_from_file(g_szFileName);
    if(pDoc == nullptr)
    {
        std::cout << "Failed to open pdf file";
        return 0;
    }


    // Iterate all required pages.
    std::vector<std::string> vecLines;
    for(size_t iPageIndex = PAGE_START; iPageIndex <= PAGE_END; iPageIndex++)
    {
        byte_array sz = pDoc->create_page(iPageIndex)->text().to_utf8();
        vecLines.clear();

        // Convert byte buffer to set of lines.
        std::string szLinesTemp; szLinesTemp.clear();
        for (char c : sz)
        {
            if(c == '\n')
            {
                vecLines.push_back(szLinesTemp);
                szLinesTemp.clear();
            }
            else 
            {
                szLinesTemp.push_back(c);
            }
        }
        if(szLinesTemp.empty() == false) vecLines.push_back(szLinesTemp); szLinesTemp.clear(); // Flush.
        // printf(ITALIC GREEN "String splitted into lines.\n" RESET ITALIC_RESET);


        size_t iTableSize = 0x100llu * sizeof(VEXOpCode_t*);
        VEXOpCode_t** table0x0F = reinterpret_cast<VEXOpCode_t**>(malloc(iTableSize));
        VEXOpCode_t** table0x38 = reinterpret_cast<VEXOpCode_t**>(malloc(iTableSize));
        VEXOpCode_t** table0x3A = reinterpret_cast<VEXOpCode_t**>(malloc(iTableSize));
        memset(table0x0F, 0, iTableSize); memset(table0x38, 0, iTableSize); memset(table0x3A, 0, iTableSize);


        // Iterate each line and pull cool information.
        // printf(ITALIC GREEN "Starting string parsing.\n" RESET ITALIC_RESET);
        for (std::string szLine : vecLines)
        {
            bool        bIsLineHeader   = IsLineHeader(szLine);
            bool        bIsLineInfo     = IsLineInfo(szLine);
            static bool bLastLineHeader = false;

            assert((bIsLineHeader == true && bLastLineHeader == true) == false && "Two opcode headers can't be present consecutively");
            assert((bIsLineHeader == true && bIsLineInfo     == true) == false && "A line can't be header and info at the same time.");


            // Incase this line is useless ( doesn't contain neither header nor information ) skip it.
            if(bIsLineHeader == false && bIsLineInfo == false)
            {
                // printf(RED "[[ INVALID LINE : %s ]]\n" RESET, szLine.c_str());
                continue;
            }


            if(bIsLineHeader == true)
            {
                VEXOpCode_t* pOpCode = new VEXOpCode_t();
                ExtractHeaderInfo(pOpCode, szLine);

                // Store this opcode header.
                g_vecValidVEXOps.push_back(pOpCode);


                printf("%s [ Brief : %s %s ]\n" RESET, pOpCode->m_szInstName.c_str(), YELLOW, pOpCode->m_szBrief.c_str());
            }
            else if(bIsLineInfo == true)
            {
                // This assertion must pass, else we are discarding some important lines.
                assert(g_vecValidVEXOps.empty() == false && "An OpCode info line occured before its header. Bad!");
                // printf("{ %125s }", szLine.c_str());


                VEXOpCode_t::OpCode_t opcode;
                ExtractOpCodeInfo(&opcode, szLine);
                

                // store in parent.
                VEXOpCode_t* pParentHeader = g_vecValidVEXOps.back();


                // Collision check.
                {
                    VEXOpCode_t** pTable = nullptr;
                    switch (opcode.m_iLeadingOpCode) 
                    {
                        case VEXOpCode_t::LeadingOpCode_0x0F: pTable = table0x0F; break;
                        case VEXOpCode_t::LeadingOpCode_0x38: pTable = table0x38; break;
                        case VEXOpCode_t::LeadingOpCode_0x3A: pTable = table0x3A; break;
                        default: break;
                    }
                    assert(pTable != nullptr && "Can't find table for this instruction.");


                    // Assert at collision. Collision must not occur.
                    bool bCollisionOccured = pTable[opcode.m_iByte] != nullptr && pTable[opcode.m_iByte]->m_szInstName != pParentHeader->m_szInstName;
                    if(bCollisionOccured == true)
                    {
                        printf(RED "Collision occured between { %s } && { %s }\n" RESET, pParentHeader->m_szInstName.c_str(), pTable[opcode.m_iByte]->m_szInstName.c_str());
                        assert(bCollisionOccured == false && "Collision occured!!");
                    }


                    // Only store if this opcode isn't already stored.
                    bool bMatchFound = false;
                    for (const VEXOpCode_t::OpCode_t& oldOpCode : pParentHeader->m_vecValidOpCodes)
                    {
                        // Note that we are not considering the prefix's first byte cause we are only concerned about
                        // what opcodes are valid VEX encodeable instructions and nothing else.
                        if(oldOpCode.m_iLeadingOpCode == opcode.m_iLeadingOpCode && oldOpCode.m_iByte == opcode.m_iByte)
                        {
                            // printf(YELLOW "     Collided!, %s 0x%02X\n" RESET, VEXOpCode_t::LeadingOpCodeToString(oldOpCode.m_iLeadingOpCode), oldOpCode.m_iByte);
                            bMatchFound = true; 
                            break;
                        }
                    }
                    if(bMatchFound == false)
                    {
                        // assert(opcode.m_bThreeBytePrefix == true && "Instruction with two byte VEX Prefix.");
                        pParentHeader->m_vecValidOpCodes.push_back(opcode);
                        printf("%s    %s -> %s 0x%02X\n" RESET, 
                                (opcode.m_bThreeBytePrefix == true ? RESET : CYAN), 
                                pParentHeader->m_szInstName.c_str(), 
                                pParentHeader->LeadingOpCodeToString(opcode.m_iLeadingOpCode), 
                                opcode.m_iByte);
                    }
                }
            }

            
            // Remember last line's state.
            bLastLineHeader = bIsLineHeader;
        }
    }
    
    DumpOpCodes(g_vecValidVEXOps);


    return 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool IsLineHeader(const std::string& szLine)
{
    if(szLine.size() <= g_iHifenPatternSize)
        return false;

    // if (szLine[0] == ' ')
    //     return false;

    // hash mask. for validity check. ( bit -> 1 if that bit can be set to 1 in rolling hash, else 0)
    uint32_t iHashMask = [&]() -> uint32_t{ uint32_t iMask = 0u; for(size_t i = 0llu; i < g_iHifenPatternSize; i++) { iMask <<= 8; iMask |= 0xFF; } return iMask; }();

    uint32_t iRollingHash = 0u;
    for(size_t i = 0; i < g_iHifenPatternSize; i++)
    {
        iRollingHash <<= 8;
        iRollingHash |= szLine[i];
    }
    assert((iRollingHash & iHashMask) == iRollingHash && "Fucked up while creating rolling hash");


    // Iterate all characters while maintaining a rolling hash, and match against the predefined hash for the 3 byte hifen characters
    // ( 00 E2 80 94 ). This must be present in a header.
    for(size_t iCharIndex = g_iHifenPatternSize; iCharIndex < szLine.size(); iCharIndex++)
    {
        iRollingHash <<= 8; iRollingHash |= (szLine[iCharIndex] & 0xFF); iRollingHash &= iHashMask;

        if(iRollingHash == g_iHifenBE)
        {
            // printf(GREEN "Rolling hash matched at index { %llu }\n" RESET, iCharIndex - g_iHifenPatternSize + 1llu);
            return true;
        }
    }


    return false;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool IsLineInfo(const std::string& szLine)
{
    for (size_t i = 0llu; i < szLine.size(); i++)
    {
        bool bMatch1 = CommonPrefix(g_szDelimiter1.c_str(), szLine.c_str() + i);
        bool bMatch2 = CommonPrefix(g_szDelimiter2.c_str(), szLine.c_str() + i);
        
        if(bMatch2 == true || bMatch1 == true)
            return true;
    }


    return false;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool ExtractHeaderInfo(VEXOpCode_t* pOut, const std::string& szLine)
{
    // Capture the instruction name.
    size_t it = 0llu; if(szLine[0] == ' ') while(szLine[++it] == ' ');
    for (; it < szLine.size(); it++)
    {
        if(szLine[it] == ' ')
        {
            pOut->m_szInstName = szLine.substr(0, it);
            break;
        }
    }


    // Skip past the hifen to capture the description.
    bool bWordStarted = false;
    for(; it < szLine.size(); it++)
    {
        if(szLine[it] != ' ' && bWordStarted == false)
            bWordStarted = true;
        
        if(szLine[it] == ' ' && bWordStarted == true)
            break;
    }

    while(szLine[++it] == ' ');


    // Capture the brief.
    pOut->m_szBrief = szLine.substr(it, szLine.size() - it);

    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool ExtractOpCodeInfo(VEXOpCode_t::OpCode_t* pOut, const std::string& szLine)
{
    for(size_t iIndex = 0llu; iIndex < szLine.size(); iIndex++)
    {
        if(CommonPrefix(g_szDelimiter1.c_str(), szLine.c_str() + iIndex) == true)
        {
            pOut->m_bThreeBytePrefix = true;

            size_t c1 = szLine.find(':');
            size_t c2 = szLine.find(':', c1 + 1llu);
            size_t c3 = szLine.find(':', c2 + 1llu);
            size_t c4 = szLine.find(':', c3 + 1llu);

            pOut->m_iLeadingOpCode = static_cast<VEXOpCode_t::LeadingOpCodes_t>(szLine[szLine.find('_', c1) + 1llu] - '0');

            bool bValidLeadingOpCode = 
                pOut->m_iLeadingOpCode == VEXOpCode_t::LeadingOpCode_0x38 || 
                pOut->m_iLeadingOpCode == VEXOpCode_t::LeadingOpCode_0x3A ||
                pOut->m_iLeadingOpCode == VEXOpCode_t::LeadingOpCode_0x0F;

            if(bValidLeadingOpCode == false)
            {
                printf(RED "Invalid Leading OpCode : %d\n" RESET, pOut->m_iLeadingOpCode);
                printf(RED "{ LINE : %s }" RESET, szLine.c_str());
                assert(bValidLeadingOpCode == true && "Invalid leading opcode");
            }

            pOut->m_iByte = HexStringToInt(szLine.c_str() + c3 + 1llu, szLine.c_str() + c4);

            return true;
        }
        else if(CommonPrefix(g_szDelimiter2.c_str(), szLine.c_str() + iIndex) == true)
        {
            pOut->m_bThreeBytePrefix = false;

            size_t c1 = szLine.find(':');
            size_t c2 = szLine.find(':', c1 + 1llu);
            size_t c3 = szLine.find(':', c2 + 1llu);

            pOut->m_iLeadingOpCode = VEXOpCode_t::LeadingOpCode_0x0F;
            pOut->m_iByte          = HexStringToInt(szLine.c_str() + c2 + 1llu, szLine.c_str() + c3);

            return true;
        }
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void DumpOpCodes(const std::vector<VEXOpCode_t*>& vec)
{
    static const char* s_szTableSymbol = "m_instTypeLUT";

    static const char* s_szMarker0x0F  = "InstTypes_AVXLeadBy0x0F";
    static const char* s_szMarker0x38  = "InstTypes_AVXLeadBy0x38";
    static const char* s_szMarker0x3A  = "InstTypes_AVXLeadBy0x3A";


    std::ofstream hFile("Output.cpp");

    auto Indent = [](std::ofstream& hFile, int iIndentation) -> void { for(int i = 0; i < iIndentation; i++) hFile << "    "; };

    Indent(hFile, 1); hFile << "{\n";

    for(const VEXOpCode_t* pInst : vec)
    {
        Indent(hFile, 2);
        hFile << "// " << pInst->m_szInstName << " [ Brief : " << pInst->m_szBrief << " ]\n";

        for(const VEXOpCode_t::OpCode_t& opcode : pInst->m_vecValidOpCodes)
        {
            const char* szMarker = nullptr;
            switch (opcode.m_iLeadingOpCode) 
            {
                case VEXOpCode_t::LeadingOpCode_0x0F: szMarker = s_szMarker0x0F; break;
                case VEXOpCode_t::LeadingOpCode_0x38: szMarker = s_szMarker0x38; break;
                case VEXOpCode_t::LeadingOpCode_0x3A: szMarker = s_szMarker0x3A; break;
                default: break;
            }
            assert(szMarker != nullptr && "Marker can't be nullptr!");

            Indent(hFile, 2);
            hFile << s_szTableSymbol << "[0x" << 
                std::uppercase << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(opcode.m_iByte) << 
                std::setfill(' ') << std::dec << std::nouppercase << "] |= " << szMarker << ';' << std::endl;
        }

        hFile << "\n";
    }

    Indent(hFile, 1); hFile << "}\n";
}
