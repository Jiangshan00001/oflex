

#include "NFAState.h"




NFAState *find_fsa_table_final_state(const FSA_TABLE &fsa)
{
    for(auto it=fsa.begin();it!=fsa.end();++it)
    {
        if((*it)->m_bAcceptingState==FINAL_STATE)
            return (*it);
    }
    return NULL;
}


NFAState *find_fsa_table_start_state(const FSA_TABLE &fsa)
{
    for(auto it=fsa.begin();it!=fsa.end();++it)
    {
        if((*it)->m_bAcceptingState==START_STATE)
            return (*it);
    }
    return NULL;
}


bool cmp_state_id(const NFAState* a, const NFAState* b)
{
    if(a==NULL )return false;
    if(b==NULL)return true;
    if(a->m_nStateID<b->m_nStateID)
        return true;
    return false;

}


