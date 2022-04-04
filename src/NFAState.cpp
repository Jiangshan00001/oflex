

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
