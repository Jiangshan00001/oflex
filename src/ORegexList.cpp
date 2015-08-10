#include "ORegexList.h"
#include "ORegexParse.h"

ORegexList::ORegexList()
{
}

int ORegexList::AppendRegex(std::string mRex, E_REGEX_MATCH_PATTERN mPattern)
{
    ORegexParse mParse;
    mParse.CreateNFA(mRex);
    
    ///
    
    
    return 0;
}

int ORegexList::BuildDFA()
{
    return 0;
}

