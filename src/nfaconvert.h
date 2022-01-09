#ifndef NFACONVERT_H
#define NFACONVERT_H

#include "NFAState.h"

class NFAConvert
{
public:
    NFAConvert();
    FSA_TABLE NFAListCont(std::vector<FSA_TABLE> &nfa_list);
    FSA_TABLE NFAtoDFA(FSA_TABLE &nfa);
    FSA_TABLE DFAmin(FSA_TABLE &dfa);

private:



    /// mC=0时，为eps的跳转
    std::set<NFAState*> MoveOne(char mC, std::set<NFAState*> mNFAs);
    //所有通过eps能相互连在一起的
    std::set<NFAState*> MoveZero(std::set<NFAState *> &mNFAs, std::set<NFAState*> &mRes1);

    /// 检查mA在mDFAs中是否存在,如果存在，则返回已存在的指针，否则返回0
    NFAState* GetExistState(NFAState* mA, FSA_TABLE &mDFAs);

};

#endif // NFACONVERT_H
