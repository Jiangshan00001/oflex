#include "ORegexParse.h"
#include "regex_match.h"
#include <iostream>
#include <NFAConvert.h>
#include <fsa_to_dot.h>


///
/// \brief regex_match
/// \param to_match_str
/// \param regex_str
/// \return
/// FIXME: 此处没有内存管理
int regex_match(const std::string &to_match_str, const std::string &regex_str)
{
    int is_debug = 1;
    ORegexParse mRegex;
    NFAConvert mConvert;
    FSA_TABLE nfa_all;
    std::vector<int> end_state_ids;
    int start_id = 0;
    FSA_TABLE nfa = mRegex.CreateNFAFlex(regex_str, start_id);

    if(is_debug)fsa_to_dot(nfa, "nfa.dot");
    FSA_TABLE dfa = mConvert.NFAtoDFA(nfa,start_id);
    FSA_TABLE dfamin = mConvert.DFAmin(dfa, start_id);
    FSA_TABLE dfamin2;
    mConvert.ReNumber(dfamin,start_id,dfamin2);
    start_id = mConvert.m_nNextStateID+1;

    if(is_debug)fsa_to_dot(dfa, "dfa.dot");
    NFAState* final_state = find_fsa_table_final_state(dfa);

    NFAState* curr_state = find_fsa_table_start_state(dfa);


    for(unsigned i=0;i<to_match_str.size();++i)
    {
        if(curr_state->m_transition.find(to_match_str[i])==curr_state->m_transition.end())
        {
            return 0;
        }
        curr_state=curr_state->m_transition.find(to_match_str[i])->second;
    }
    return 1;
}

void *regex_contruct(const std::string &regex_str)
{
    int is_debug = 1;
    ORegexParse mRegex;
    NFAConvert mConvert;
    FSA_TABLE nfa_all;
    std::vector<int> end_state_ids;
    int start_id = 0;
    FSA_TABLE nfa = mRegex.CreateNFAFlex(regex_str, start_id);

    if(is_debug)fsa_to_dot(nfa, "nfa.dot");
    FSA_TABLE dfa = mConvert.NFAtoDFA(nfa,start_id);
    FSA_TABLE dfamin = mConvert.DFAmin(dfa, start_id);
    FSA_TABLE dfamin2;
    mConvert.ReNumber(dfamin,start_id,dfamin2);
    start_id = mConvert.m_nNextStateID+1;

    if(is_debug)fsa_to_dot(dfa, "dfa.dot");
    NFAState* final_state = find_fsa_table_final_state(dfa);

    NFAState* curr_state = find_fsa_table_start_state(dfa);
    return (void*)curr_state;
}

int regex_match2(const std::string &to_match_str, void *regex)
{
    NFAState* curr_state=(NFAState*)regex;
    for(unsigned i=0;i<to_match_str.size();++i)
    {
        if(curr_state->m_transition.find(to_match_str[i])==curr_state->m_transition.end())
        {
            return 0;
        }
        curr_state=curr_state->m_transition.find(to_match_str[i])->second;
    }
    if(curr_state->m_bAcceptingState==2)return 1;


    return 0;

}
