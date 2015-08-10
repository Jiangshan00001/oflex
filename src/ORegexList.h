#ifndef OREGEXLIST_H
#define OREGEXLIST_H

#include <string>
#include "NFAState.h"

typedef enum TAG_E_REGEX_MATCH_PATTERN
{
    MATCH_MIN =0,
    MATCH_MAX=1
}E_REGEX_MATCH_PATTERN;
    


class ORegexList
{
public:
    ORegexList();
    int AppendRegex(std::string mRex, E_REGEX_MATCH_PATTERN mPattern );
    int BuildDFA();
    
private:
    /// 
    int m_StateId;
    NFAState *m_StartState;
 
};


#endif

