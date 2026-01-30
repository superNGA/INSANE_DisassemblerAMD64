//=========================================================================
//                     EVEX inst table gen. 
//=========================================================================
// by      : INSANE
// created : 13/01/2026
//
// purpose : Generate tables for EVEX instructions using linux kernel x86 opcode map. 
//-------------------------------------------------------------------------

/*
   This is literally a copy of the VEX table generator located @ "../VEXTableGen/" with 
   minimal changes.
*/

#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <unordered_set>
#include <unordered_map>


extern const char* g_szX86OpCodeMap;


// Color terminal logs
#define RED          "\033[31m"
#define GREEN        "\033[32m"
#define YELLOW       "\033[93m"
#define RESET        "\033[0m"
#define CYAN         "\033[96m"
#define ITALIC       "\x1b[3m"
#define ITALIC_RESET "\x1b[0m"


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace Delimiters
{
    const std::string g_szTableStart     = "Table:";
    const std::string g_szTableEnd       = "EndTable";

    const std::string g_szEVEXPrefNoScal = "(ev)";
    const std::string g_szEVEXPrefScal   = "(es)";
    const std::string g_szEVEXOverride   = "(evo)";
    const std::string g_szVEXPref        = "(v)";
    const std::string g_szVEX128bit      = "(v1)";
    const std::string g_szXOPPref        = "(xop)";

    const std::string g_szReferrer       = "Referrer:";
    const std::string g_szAVXCode        = "AVXcode:";

    const std::string g_szEscapeToTable  = "escape";
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace TableNames
{
    const std::string g_szOneByteOpCodes = "one byte opcode";
    const std::string g_szTwoByteOpCodes    = "2-byte opcode (0x0f)";
    const std::string g_szThreeByteOpCodes38   = "3-byte opcode 1 (0x0f 0x38)";
    const std::string g_szThreeByteOpCodes3A   = "3-byte opcode 2 (0x0f 0x3a)";
    const std::string EVEXmap4      = "EVEX map 4";
    const std::string EVEXmap5      = "EVEX map 5";
    const std::string EVEXmap6      = "EVEX map 6";
    const std::string VEXmap7       = "VEX map 7";
    const std::string XOPmap8h      = "XOP map 8h";
    const std::string XOPmap9h      = "XOP map 9h";
    const std::string XOPmapAh      = "XOP map Ah";
    const std::string Grp1          = "Grp1";
    const std::string Grp1A         = "Grp1A";
    const std::string Grp2          = "Grp2";
    const std::string Grp3_1        = "Grp3_1";
    const std::string Grp3_2        = "Grp3_2";
    const std::string Grp4          = "Grp4";
    const std::string Grp5          = "Grp5";
    const std::string Grp6          = "Grp6";
    const std::string Grp7          = "Grp7";
    const std::string Grp8          = "Grp8";
    const std::string Grp9          = "Grp9";
    const std::string Grp10         = "Grp10";
    const std::string Grp11A        = "Grp11A";
    const std::string Grp11B        = "Grp11B";
    const std::string Grp12         = "Grp12";
    const std::string Grp13         = "Grp13";
    const std::string Grp14         = "Grp14";
    const std::string Grp15         = "Grp15";
    const std::string Grp16         = "Grp16";
    const std::string Grp17         = "Grp17";
    const std::string Grp18         = "Grp18";
    const std::string Grp19         = "Grp19";
    const std::string Grp20         = "Grp20";
    const std::string Grp21         = "Grp21";
    const std::string GrpP          = "GrpP";
    const std::string GrpPDLK       = "GrpPDLK";
    const std::string GrpRNG        = "GrpRNG";
    const std::string GrpXOP1       = "GrpXOP1";
    const std::string GrpXOP2       = "GrpXOP2";
    const std::string GrpXOP3       = "GrpXOP3";
    const std::string GrpXOP4       = "GrpXOP4";
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
enum OperandModes_t : int16_t // Geek's edition operand addressing methods.
{
    //      This is an enum holding all operand addressing methods
    // according to the Geek's edition mazegen's .xml ( https://github.com/mazegen/x86reference/blob/master/x86reference.xml ), 
    // that is reference for this project. ( along with some other shit ) 
    // 

    OperandMode_Invalid = -1,
    OperandMode_A,
    OperandMode_BA,
    OperandMode_BB,
    OperandMode_BD,
    OperandMode_C,
    OperandMode_D,
    OperandMode_E,
    OperandMode_ES,
    OperandMode_EST,
    OperandMode_F,
    OperandMode_G,
    OperandMode_H,
    OperandMode_I,
    OperandMode_J,
    OperandMode_M,
    OperandMode_N,
    OperandMode_O,
    OperandMode_P,
    OperandMode_Q,
    OperandMode_R,
    OperandMode_S,
    OperandMode_SC,
    OperandMode_T,
    OperandMode_U,
    OperandMode_V,
    OperandMode_W,
    OperandMode_X,
    OperandMode_Y,
    OperandMode_Z,

    OperandMode_Count,
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::unordered_map<char, std::string> g_mapCustomOpAddressingModes = 
{
    {'B', "VG"},
    {'H', "VXY"},
    {'L', "IXY"}, 
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
struct Inst_t
{
    void Reset()
    {
        m_szName       = "xx_INVALID_xx";
        m_iSplitMethod = SplitMethod_None;
        m_iPrefix      = 0x00;
        m_vecChildren.clear();
        m_vecOperands.clear();
    }

    int                      m_iOpCode;
    std::string              m_szName = "xx_INVALID_xx";
    std::string              m_szStructure;

    enum SplitMethod_t { SplitMethod_None, SplitMethod_Prefix, SplitMethod_Modrm, };
    SplitMethod_t m_iSplitMethod = SplitMethod_None;
    std::vector<Inst_t*> m_vecChildren;

    struct Operand_t { std::string szAdrsMode = "Invalid", szOperandType = "Invalid"; };
    std::vector<std::string> m_vecOperands;
    int                      m_iPrefix = 0x00;


    Inst_t& operator=(const Inst_t& other)
    {
        // NOTE : We are not copying any children cause we they are poitner to objects and 
        // all we won't be running it on any nodes with children.
        // So copying while having children or a legit split method is prohibited.
        
        assert(other.m_iSplitMethod == SplitMethod_None && other.m_vecChildren.size() == 0llu && "Cannot run copy operator with children or a split type.");

        m_iOpCode      = other.m_iOpCode;
        m_szName       = other.m_szName;
        m_szStructure  = other.m_szStructure;
        m_iSplitMethod = other.m_iSplitMethod;

        m_vecOperands.reserve(4); m_vecOperands.clear();
        for(std::string szOperands : other.m_vecOperands) m_vecOperands.push_back(szOperands);

        m_iPrefix = other.m_iPrefix;

        assert(m_vecOperands.size() == other.m_vecOperands.size() && "Operand copy failed. Fix this shit nigga.");

        return *this;
    }
};


std::unordered_set<int> g_vecValidPrefix = { 0xF0, 0xF2, 0xF3, 0x2E, 0x36, 0x3E, 0x26, 0x64, 0x65, 0x66, 0x67};

static  int HexStringToInt          (const char* sz);
static void ParseTable              (const std::vector<std::string>& vecLines, const std::string& szTableName, int* iPrefixDistOut, std::vector<Inst_t*>& vecOut);
static void ExtractInstInfo         (std::string& szInst, Inst_t& instOut);
static void ConstructOperandTypeDist(std::unordered_map<uint64_t, int>& mapOut, const std::vector<Inst_t*>& vecInst);
static void DumpInstToFile          (const std::vector<Inst_t*>& vecInst, const char* szFileName, const char* szTableName);
static void ConstructTreeStruct     (std::vector<Inst_t*>& vecInst, std::vector<Inst_t*>& vecOut);
static void PrintInstInfo           (const std::vector<Inst_t*> vecInst, int iIndentation = 0);

constexpr size_t INVALID_CHAR_POS = std::string::npos;





///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int main(int nArgs, char** args)
{
    std::string szFile(g_szX86OpCodeMap);
    std::vector<std::string> vecLines; vecLines.clear();

    std::string szTemp; 
    for(char c : szFile)
    {
        if(c == '\n')
        {
            vecLines.push_back(szTemp); szTemp.clear();
        }
        else 
        {
            szTemp.push_back(c);
        }
    }
    if(szTemp.empty() == false) vecLines.push_back(szTemp); szTemp.clear();


    // Operand type distribution.
    std::unordered_map<uint64_t, int> mapOpTypes; mapOpTypes.clear();

    // Operand addressing methods distribution.
    static int s_iAdrsModeDist[26]; memset(s_iAdrsModeDist, 0, sizeof(s_iAdrsModeDist));

    printf("Leading OpCode : 0x0F \n\n");
    std::vector<Inst_t*> vecTwoByte; ParseTable(vecLines, TableNames::g_szTwoByteOpCodes, s_iAdrsModeDist, vecTwoByte);
    printf("\n\n");

    printf("Leading OpCode : 0x0F 0x38\n\n");
    std::vector<Inst_t*> vecThreeByte38; ParseTable(vecLines, TableNames::g_szThreeByteOpCodes38, s_iAdrsModeDist, vecThreeByte38);
    printf("\n\n");

    printf("Leading OpCode : 0x0F 0x3A\n\n");
    std::vector<Inst_t*> vecThreeByte3A; ParseTable(vecLines, TableNames::g_szThreeByteOpCodes3A, s_iAdrsModeDist, vecThreeByte3A);


    // Operand addressind mode distribution.
    printf(GREEN "Operand Addressing Method Distribution\n" RESET);
    for(int i = 0; i < 26; i++)
    {
        if(s_iAdrsModeDist[i] == 0)
            continue;

        printf("    %c : %d\n", i + 'A', s_iAdrsModeDist[i]);
    }


    // Operand type distribution.
    ConstructOperandTypeDist(mapOpTypes, vecTwoByte);
    ConstructOperandTypeDist(mapOpTypes, vecThreeByte38);
    ConstructOperandTypeDist(mapOpTypes, vecThreeByte3A);


    // Operand type distribution.
    printf(GREEN "Operand type distribution\n" RESET);
    for(const auto& it : mapOpTypes)
    {
        printf("%5s : %d\n", reinterpret_cast<const char*>(&it.first), it.second);
    }


    // Constructing tree like structure.
    std::vector<Inst_t*> vecFinalTwoBytes; ConstructTreeStruct(vecTwoByte, vecFinalTwoBytes);      
    printf("Contructed Tree Struct { %zu }entries\n", vecFinalTwoBytes.size());

    std::vector<Inst_t*> vecFinalThreeBytes_38; ConstructTreeStruct(vecThreeByte38, vecFinalThreeBytes_38); 
    printf("Contructed Tree Struct { %zu }entries\n", vecFinalThreeBytes_38.size());

    std::vector<Inst_t*> vecFinalThreeBytes_3A; ConstructTreeStruct(vecThreeByte3A, vecFinalThreeBytes_3A); 
    printf("Contructed Tree Struct { %zu }entries\n", vecFinalThreeBytes_3A.size());

    PrintInstInfo(vecFinalTwoBytes);
    PrintInstInfo(vecFinalThreeBytes_38);
    PrintInstInfo(vecFinalThreeBytes_3A);


    // Dumping to file.
    DumpInstToFile(vecFinalTwoBytes,      "TwoByte.cpp",     "m_EVEXTwoByteOpCodes");
    DumpInstToFile(vecFinalThreeBytes_38, "ThreeByte38.cpp", "m_EVEXThreeByteOpCodes_38");
    DumpInstToFile(vecFinalThreeBytes_3A, "ThreeByte3A.cpp", "m_EVEXThreeByteOpCodes_3A");


    return 0;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline int HexStringToInt(const char* sz)
{
    int iOutput = 0;

    while(*sz != '\0')
    {
        char c = *sz; sz++;

        int iNum = 0;

        if(c >= '0' && c <= '9')
            iNum = c - '0';
        else if(c >= 'a' && c <= 'f')
            iNum = c - 'a' + 10;
        else if(c >= 'A' && c <= 'F')
            iNum = c - 'A' + 10;
        else
            return iOutput;

        iOutput *= 0x10;
        iOutput += iNum;
    }

    return iOutput;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void ParseTable(const std::vector<std::string>& vecLines, const std::string& szTableName, int* iPrefixDistOut, std::vector<Inst_t*>& vecOut)
{
    bool bTableStarted = false;
    std::vector<std::string> vecInst;

    for(size_t iLineIndex = 0llu; iLineIndex < vecLines.size(); iLineIndex++)
    {
        vecInst.clear();
        const std::string& szLine = vecLines[iLineIndex];


        // Skip empty or comment lines.
        if(szLine.empty() == true || szLine[0] == '#')
            continue;


        // Unwanted lines.
        if(szLine.find(Delimiters::g_szReferrer) != INVALID_CHAR_POS || szLine.find(Delimiters::g_szAVXCode) != INVALID_CHAR_POS)
            continue;
        
        
        // Check table start.
        if(szLine.find(szTableName) != INVALID_CHAR_POS)
        {
            bTableStarted = true;
            continue;
        }


        // Desired table ended?
        if(bTableStarted == true && szLine.find(Delimiters::g_szTableEnd) != INVALID_CHAR_POS)
            break;


        // Unwanted lines.
        if(bTableStarted == false)
            continue;


        // Escape opcodes?
        if(szLine.find(Delimiters::g_szEscapeToTable) != INVALID_CHAR_POS)
        {
            // printf(CONSOLE_RED "OpCode [ 0x%02X ] is escaping to another table\n" CONSOLE_RESET, iOpCode);
            continue;
        }


        int    iOpCode    = HexStringToInt(szLine.c_str());
        size_t iInstStart = szLine.find(':');
        size_t iInstEnd   = iInstStart;
        assert(iInstStart != INVALID_CHAR_POS && "line doesn't have a colon in it.");


        int iInstCount = 0;
        while(iInstEnd != INVALID_CHAR_POS)
        {
            iInstEnd = szLine.find('|', iInstStart + 1llu);

            std::string szInst = szLine.substr(iInstStart + 1llu, iInstEnd - iInstStart - 1llu);

            // Filter out BS inst.
            bool bShouldStore = false;

            // This is not a valid check.
            for(char c : szInst)
            {
                if(c != ' ')
                {
                    if(c == 'v') // Mnemonics starting with v are either VEX or EVEX.
                        bShouldStore = true;

                    break;
                }
            }


            // Presence of (evo) || (ev) || (es) tag confirms that this instruction is EVEX encoding.
            if(bShouldStore == false)
                bShouldStore = szInst.find("(evo)") != INVALID_CHAR_POS || szInst.find("(ev)") != INVALID_CHAR_POS || szInst.find("(es)") != INVALID_CHAR_POS;

            else if(szInst.find("(v)") != INVALID_CHAR_POS || 
                    szInst.find("(v1)") != INVALID_CHAR_POS)
                bShouldStore = false;

            if(bShouldStore == true)
            {
                vecInst.push_back(szInst);
            }

            iInstStart = iInstEnd;
            iInstCount++;
        }


        if(vecInst.empty() == true)
            continue;


        printf(CYAN ITALIC "{ Captured %zu / %d inst }  " ITALIC_RESET RESET, vecInst.size(), iInstCount);
        for(std::string& szInst : vecInst)
        {
            // Parse and extract information from this instruction string.
            Inst_t* pInst = new Inst_t();
            ExtractInstInfo(szInst, *pInst);
            pInst->m_iOpCode     = iOpCode;
            pInst->m_szStructure = szInst;

            vecOut.push_back(pInst);


            // Printing instruction's extracted information to check if its correct or not.
            printf("0x%02X : %s || ", pInst->m_iOpCode, szInst.c_str());
            printf("{ " CYAN "%s" YELLOW " (", pInst->m_szName.c_str());
            for(std::string& szOperand : pInst->m_vecOperands) printf("%s, ", szOperand.c_str());
            if(pInst->m_iPrefix != 0x00)
                printf(") " GREEN " 0x%02X } " RESET, pInst->m_iPrefix);
            else
                printf(")  } " RESET);
            std::cout << std::endl;


            // Collecting operand addressing method distribution data.
            for(std::string& szOperand : pInst->m_vecOperands)
            {
                const char adrsMode = szOperand[0];
                int iIndex = (adrsMode >= 'A' && adrsMode <= 'Z') ? adrsMode - 'A' : (adrsMode >= 'a' && adrsMode <= 'z') ? adrsMode - 'a' : -1;
                assert(iIndex != -1 && "Invalid operand addressing method spotted");
                iPrefixDistOut[iIndex]++;
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void ExtractInstInfo(std::string& szInst, Inst_t& instOut)
{
    assert(szInst.empty() == false && "Shit input nigga");

    size_t it = 0llu; while(szInst[it] == ' ') it++;

    int iMenmByte = 0;
    while(szInst[it + iMenmByte] != ' ' && szInst[it + iMenmByte] != '\0') { iMenmByte++; }

    instOut.m_szName = szInst.substr(it, iMenmByte);
    it += static_cast<size_t>(iMenmByte);


    std::string szTemp; bool bWordStarted = false;
    for(; it < szInst.size(); it++)
    {
        char c = szInst[it];

        if(c == '(' || c == ')')
            break;

        if(c != ' ') 
            bWordStarted = true;

        if(c == ' ' && bWordStarted == false) continue;
        else if(c == ' ' && bWordStarted == true) break;
        else if(c == ',') { instOut.m_vecOperands.push_back(szTemp); szTemp.clear(); }
        else szTemp.push_back(c);
    }
    if(szTemp.empty() == false) instOut.m_vecOperands.push_back(szTemp); szTemp.clear();


    {
        bool bWordStarted = false;
        bool bBracet      = false;
        std::string szTemp;
        for(; it < szInst.size(); it++)
        {
            char c = szInst[it];

            if(c != ' ')
                bWordStarted = true;

            if(c == '(')
                bBracet = true;
            else if(c == ')')
            {
                bBracet = false;
                int iPrefix = HexStringToInt(szTemp.c_str());
                if(g_vecValidPrefix.count(iPrefix) > 0llu)
                {
                    instOut.m_iPrefix = iPrefix;
                    break;
                }
                szTemp.clear();
            }
            else if(bBracet == true)
            {
                szTemp.push_back(c);
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void ConstructOperandTypeDist(std::unordered_map<uint64_t, int>& mapOut, const std::vector<Inst_t*>& vecInst)
{
    for(const Inst_t* pInst : vecInst)
    {
        for(const std::string& szOperand : pInst->m_vecOperands)
        {
            uint64_t iOpType = 0llu;


            // If first letter is lower case than this must be a register or some shit.
            // not a real operand type.
            if(szOperand[0] >= 'a' && szOperand[0] <= 'z')
                continue;


            // Bitch ass operands with option in between. MF
            if(szOperand.find('/') != INVALID_CHAR_POS)
                printf(ITALIC RED "0x%02X -> %s\n" RESET ITALIC_RESET, pInst->m_iOpCode, szOperand.c_str());


            // starting from 1 to skip the single character operand addresing method.
            for(size_t i = 1; i < szOperand.size(); i++)
            {
                char c = szOperand[i];

                // Handling little endian here.
                iOpType |= ((static_cast<uint64_t>(c) & 0xFF) << ((i - 1llu) * 8llu));
            }


            // Invalid operand??
            if(iOpType == 0llu)
            {
                printf(RED "Invalid Operand : %s\n" RESET, szOperand.c_str());
                assert(iOpType != 0 && "Some operand has no operand type.");
            }


            auto it = mapOut.find(iOpType);
            if(it == mapOut.end())
                mapOut.emplace(iOpType, 0);

            it = mapOut.find(iOpType);
            it->second++;
        }
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static inline void Indent(std::ofstream& hFile, int iIndentation)
{
    for(int i = 0; i < iIndentation; i++)
        hFile << "    ";
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void DumpInstInvalid(const char* szTableName, std::ofstream& hFile, int iIndentation, int iByte)
{
    Indent(hFile, iIndentation);
    hFile << "// 0x" << std::uppercase << std::hex << iByte << std::dec << std::nouppercase << '\n';

    Indent(hFile, iIndentation);
    hFile << "// This opcode does not repesent a valid VEX encodable intruction.\n";

    Indent(hFile, iIndentation);
    hFile << szTableName << "Init(" << '\n';

    Indent(hFile, iIndentation + 1);
    hFile << "/*szName         = */\"xx_INVALID_xx\",\n";

    Indent(hFile, iIndentation + 1);
    hFile << "/*bValidOpcd     = */false,\n";

    Indent(hFile, iIndentation + 1); // None are escape
    hFile << "/*bEscapeOpcd    = */false,\n";

    Indent(hFile, iIndentation + 1);
    hFile << "/*bModrmRequired = */false,\n";

    Indent(hFile, iIndentation + 1);
    hFile << "/*iByte          = */0x" << std::uppercase << std::hex << iByte << std::dec << std::nouppercase << ",\n";

    Indent(hFile, iIndentation + 1);
    hFile << "/*nOperands      = */0,\n";

    Indent(hFile, iIndentation + 1);
    hFile << "/*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),\n";
    Indent(hFile, iIndentation + 1);
    hFile << "/*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),\n";
    Indent(hFile, iIndentation + 1);
    hFile << "/*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),\n";
    Indent(hFile, iIndentation + 1);
    hFile << "/*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));\n";

    Indent(hFile, iIndentation + 1);
    hFile << "\n";

}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void DumpInst(const Inst_t* pInst, const char* szTableName, std::ofstream& hFile, int iIndentation, int iByte)
{
    Indent(hFile, iIndentation);
    hFile << "// 0x" << std::uppercase << std::hex << iByte << std::dec << std::nouppercase << '\n';

    Indent(hFile, iIndentation); int iSpaces = 0; for(char c : pInst->m_szStructure) {if(c == ' ') iSpaces++; else break; }
    hFile << "// " << pInst->m_szStructure.c_str() + static_cast<uint64_t>(iSpaces) << '\n';

    Indent(hFile, iIndentation);
    hFile << szTableName << "Init(" << '\n';

    Indent(hFile, iIndentation + 1);
    hFile << "/*szName         = */\"" << pInst->m_szName << "\",\n";

    Indent(hFile, iIndentation + 1);
    hFile << "/*bValidOpcd     = */true" << ",\n";

    Indent(hFile, iIndentation + 1); // None are escape
    hFile << "/*bEscapeOpcd    = */false,\n";

    Indent(hFile, iIndentation + 1);
    hFile << "/*bModrmRequired = */true,\n";

    Indent(hFile, iIndentation + 1);
    hFile << "/*iByte          = */0x" << std::uppercase << std::hex << iByte << std::dec << std::nouppercase << ",\n";

    Indent(hFile, iIndentation + 1);
    hFile << "/*nOperands      = */" << pInst->m_vecOperands.size() << ",\n";

    for(int i = 0; i < 4; i++)
    {
        Indent(hFile, iIndentation + 1);

        if(i < pInst->m_vecOperands.size())
        {
            char adrsMode = pInst->m_vecOperands[i][0];
            if(adrsMode >= 'A' && adrsMode <= 'Z')
            {
                // Handling bizzare operand addressing modes.
                std::string szAdrsMode("~"); szAdrsMode[0] = pInst->m_vecOperands[i][0];
                if(g_mapCustomOpAddressingModes.find(szAdrsMode[0]) != g_mapCustomOpAddressingModes.end())
                    szAdrsMode = g_mapCustomOpAddressingModes.find(szAdrsMode[0])->second;

                // Handling bizzare operand types.
                std::string szOperandType = pInst->m_vecOperands[i].substr(1);
                if(szOperandType == "y")
                    szOperandType = "dqp";
                else if(szOperandType == "v")
                    szOperandType = "vqp";

                hFile << "/*operand" << i + 1 << "       = */Operand_t( OperandMode_" << szAdrsMode << ", OperandType_" << szOperandType << ")";
            }
            else // It must be a registe if first letter is lowercase.
            {
                hFile << "/*operand" << i + 1 << "       = */Operand_t( Register_t(" << pInst->m_vecOperands[i] << "))";
            }
        }
        else 
        {
            hFile << "/*operand" << i + 1 << "       = */Operand_t( OperandMode_Invalid, OperandType_Invalid)";
        }

        // So we don't put comman on the last argument of this fn.
        if(i < 3)
            hFile << ",";
        else 
            hFile << ");";

        hFile << '\n';
    }

    Indent(hFile, iIndentation + 1);
    hFile << "\n";
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
void DumpInstRecurse(Inst_t* pInst, std::ofstream& hFile, const char* szFileName, std::string& szTableName, int iIndentation, int iByte)
{
    if(pInst->m_iSplitMethod == Inst_t::SplitMethod_None)
    {
        DumpInst(pInst, szTableName.c_str(), hFile, iIndentation, iByte);
    }
    else 
    {
        // Write the parent as well.
        pInst->m_szStructure = 
            std::string("Parent instruction. Split type [ ") + 
            std::string(pInst->m_iSplitMethod == Inst_t::SplitMethod_Prefix ? "Prefix Split" : "ModRM Split") + 
            std::string("]");
        DumpInst(pInst, szTableName.c_str(), hFile, iIndentation, iByte);


        if(pInst->m_iSplitMethod == Inst_t::SplitMethod_Prefix)
        {
            Indent(hFile, iIndentation);
            hFile << szTableName << "InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);\n";
        }
        else if(pInst->m_iSplitMethod == Inst_t::SplitMethod_Modrm)
        {
            Indent(hFile, iIndentation);
            hFile << szTableName << "InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_ModRM_MOD);\n";
        }
        else assert(false && "Some instuction has some bitch ass split method.");


        for(Inst_t* pChild : pInst->m_vecChildren)
        {
            // insert varient.
            int iVarientIndex = 0;
            if(pInst->m_iSplitMethod == Inst_t::SplitMethod_Prefix)
            {
                iVarientIndex = GetLegacyPrefixIndex(pChild->m_iPrefix);
            }
            else if(pInst->m_iSplitMethod == Inst_t::SplitMethod_Modrm)
            {
                // R, M, U
                // R -> modrm == 3; M -> modrm != 3; U -> modrm == 3
                bool bModrmValueFound = false;
                for(std::string& szOperand : pChild->m_vecOperands)
                {
                    char adrsMode = szOperand[0];
                    if(adrsMode == 'R' || adrsMode == 'U')
                    {
                        iVarientIndex = 3;
                        bModrmValueFound = true;
                    }
                    else if(adrsMode == 'M')
                    {
                        iVarientIndex = 0;
                        bModrmValueFound = true;
                    }
                }

                // Make sure we found a modrm value.
                assert(bModrmValueFound == true && "MODRM split instruction doesn't have any operand which can determine the modrm value");
            }
            else assert(false && "Some instuction has some bitch ass split method.");

            Indent(hFile, iIndentation);
            hFile << szTableName << "InsertVarient(" << iVarientIndex << ");\n";


            // modrm split and varientindex = 0, copy index 0 to 1 and 2. after insertvarient.
            if(pInst->m_iSplitMethod == Inst_t::SplitMethod_Modrm && iVarientIndex == 0)
            {
                Indent(hFile, iIndentation); hFile << szTableName << "m_pVarients[1] = " << szTableName << "m_pVarients[0];\n";
                Indent(hFile, iIndentation); hFile << szTableName << "m_pVarients[2] = " << szTableName << "m_pVarients[0];\n";
            }


            // update table name.
            std::stringstream ss; ss << szTableName << "m_pVarients[" << iVarientIndex << "]->";
            std::string szTableNameChild = ss.str();

            Indent(hFile, iIndentation); hFile << "{\n";
            DumpInstRecurse(pChild, hFile, szFileName, szTableNameChild, iIndentation + 1, iByte);
            Indent(hFile, iIndentation); hFile << "}\n";
        }

        hFile << std::endl;
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void DumpInstToFile(const std::vector<Inst_t*>& vecInst, const char* szFileName, const char* szTableName)
{
    printf("Starting file dump to [ %s ] for table [ %s ]\n", szFileName, szTableName);

    std::ofstream hFile(szFileName);

    // Make sure file is valid.
    if(hFile.is_open() == false)
    {
        printf(RED "Failed to create / open file %s\n" RESET, szFileName);
        return;
    }


    int iIndentation = 1;
    int iByte        = 0x00;
    for(Inst_t* pInst : vecInst)
    {
        // Dump all invalid instructions between valid instructions.
        while(iByte < pInst->m_iOpCode)
        {
            printf(RED "Dumped invalid instruction [ 0x%02X ]\n" RESET, iByte);
            std::stringstream ss; ss << szTableName << "[0x" << std::uppercase << std::hex << iByte << std::dec << std::nouppercase << "].";
            DumpInstInvalid(ss.str().c_str(), hFile, iIndentation, iByte);
            iByte++;
        }

        // Dump valid instruction.
        assert(pInst->m_iOpCode == iByte && "File dump look if fucked up nigga");
        printf(GREEN "writting inst 0x%02X\n" RESET, pInst->m_iOpCode);
        std::stringstream ss; ss << szTableName << "[0x" << std::uppercase << std::hex << iByte << std::dec << std::nouppercase << "].";
        std::string str = ss.str();
        DumpInstRecurse(pInst, hFile, szFileName, str, iIndentation, pInst->m_iOpCode);
        iByte++;
    }


    hFile.close();

    printf(GREEN ITALIC "Done dumping to table %s to file %s\n" ITALIC_RESET RESET, szTableName, szFileName);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void HandleModrmSplitRecurse(Inst_t* pInst)
{
    assert(pInst != nullptr && "nullptr instruction received for hanlding modrm split recurse.");

    if(pInst->m_iSplitMethod == Inst_t::SplitMethod_None)
    {
        for(size_t iOperandIndex = 0llu; iOperandIndex < pInst->m_vecOperands.size(); iOperandIndex++)
        {
            std::string& szOperand = pInst->m_vecOperands[iOperandIndex];

            // Needs split
            if(size_t iSlashPos = szOperand.find('/'); iSlashPos != std::string::npos)
            {
                Inst_t* pChildOne = new Inst_t; 
                Inst_t* pChildTwo = new Inst_t;

                *pChildOne = *pInst;
                *pChildTwo = *pInst;

                // Fix splitting operand.
                printf("SlashPos : %zu in %s\n", iSlashPos, szOperand.c_str());
                pChildOne->m_vecOperands[iOperandIndex] = szOperand.substr(0, iSlashPos);
                pChildTwo->m_vecOperands[iOperandIndex] = szOperand.substr(iSlashPos + 1llu);
                printf(GREEN "Converted Operand [ %s ] to [ %s ] & [ %s ]\n" RESET, 
                        szOperand.c_str(), pChildOne->m_vecOperands[iOperandIndex].c_str(), pChildTwo->m_vecOperands[iOperandIndex].c_str());


                // Clear parent instruction and link children to it.
                pInst->Reset();
                pInst->m_iPrefix = pChildOne->m_iPrefix;
                pInst->m_iOpCode = pChildOne->m_iOpCode;
                pInst->m_iSplitMethod = Inst_t::SplitMethod_Modrm;
                pInst->m_vecChildren.push_back(pChildOne);
                pInst->m_vecChildren.push_back(pChildTwo);


                printf(ITALIC RED "{{  Done Modrm handling for byte [ 0x%02X ]  }}\n" RESET ITALIC_RESET, pInst->m_iOpCode);
            }
        }
    }
    else
    {
        for(Inst_t* pChild : pInst->m_vecChildren)
        {
            HandleModrmSplitRecurse(pChild);
        }
    }
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void ConstructTreeStruct(std::vector<Inst_t*>& vecInst, std::vector<Inst_t*>& vecOut)
{
    vecOut.clear();


    auto HandleGroup = [&](std::vector<Inst_t*> vecGroup) -> Inst_t*
    {
        // This is returned.
        Inst_t* pHead = nullptr;
        assert(vecGroup.empty() == false && "Empty group received in handlegroup fn");

        bool bPrefixSplit = vecGroup.size() > 1LLU || vecGroup.front()->m_iPrefix != 0x00;

        if(bPrefixSplit == false)
        {
            pHead = vecGroup.front();
            pHead->m_iSplitMethod = Inst_t::SplitMethod_None;

        }
        else
        {
            pHead = new Inst_t;
            pHead->m_iSplitMethod = Inst_t::SplitMethod_Prefix;
            for(Inst_t* pInst : vecGroup)
            {
                pInst->m_iSplitMethod = Inst_t::SplitMethod_None;
                pHead->m_vecChildren.push_back(pInst);
                pHead->m_iOpCode = pInst->m_iOpCode;
            }
        }

        // Handling modrm split of any of the children.
        HandleModrmSplitRecurse(pHead);

        return pHead;
    };

    
    std::vector<Inst_t*> vecGroup;
    int iLastByte = 0xFF;
    for(Inst_t* pInst : vecInst)
    {
        if(pInst->m_iOpCode != iLastByte)
        {
            if(vecGroup.empty() == false)
                vecOut.push_back(HandleGroup(vecGroup));

            vecGroup.clear();
        }

        vecGroup.push_back(pInst);
        iLastByte = pInst->m_iOpCode;
    }
    if(vecGroup.empty() == false) vecOut.push_back(HandleGroup(vecGroup));

    return;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void PrintInstInfoRecurse(Inst_t* pInst, int iIndentation)
{
    for(int i = 0; i < iIndentation; i++)
        printf("    ");


    printf("0x%02X : ", pInst->m_iOpCode);
    printf("{ " CYAN "%s" YELLOW " (", pInst->m_szName.c_str());
    for(std::string& szOperand : pInst->m_vecOperands) printf("%s, ", szOperand.c_str());
    if(pInst->m_iPrefix != 0x00)
        printf(") " GREEN " 0x%02X } " RESET, pInst->m_iPrefix);
    else
        printf(")  } " RESET);

    // Printing split method.
    const char* szSplitMethod = "";
    switch (pInst->m_iSplitMethod) 
    {
        case Inst_t::SplitMethod_Prefix: szSplitMethod = "Prefix Split"; break;
        case Inst_t::SplitMethod_Modrm:  szSplitMethod = "Modrm Split";  break;
        case Inst_t::SplitMethod_None:   break;
        defaut: break;
    };
    printf("%s", szSplitMethod);
    printf(" [ 0x%02X] ", pInst->m_iPrefix);

    std::cout << std::endl;


    for(Inst_t* pChild : pInst->m_vecChildren)
        PrintInstInfoRecurse(pChild, iIndentation + 1);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void PrintInstInfo(const std::vector<Inst_t*> vecInst, int iIndentation)
{
    for(Inst_t* pInst : vecInst)
        PrintInstInfoRecurse(pInst, iIndentation);
}
