#ifndef NFACONVERT_H
#define NFACONVERT_H

#include "NFAState.h"

class NFAConvert
{
public:
    NFAConvert();

    ///多个合1个
    FSA_TABLE NFAListCont(std::vector<FSA_TABLE> &nfa_list);
    /// NFA转DFA
    FSA_TABLE NFAtoDFA(FSA_TABLE &nfa, int startId=0);
    /// DFA最小化
    FSA_TABLE DFAmin(FSA_TABLE &dfa, int start_id=0);

    int m_nNextStateID;
private:



    /// mC=0时，为eps的跳转
    std::set<NFAState*> MoveOne(int mC, std::set<NFAState*> mNFAs);
    //所有通过eps能相互连在一起的
    std::set<NFAState*> MoveZero(std::set<NFAState *> &mNFAs, std::set<NFAState*> &mRes1);

    /// 检查mA在mDFAs中是否存在,如果存在，则返回已存在的指针，否则返回0
    NFAState* GetExistState(NFAState* mA, FSA_TABLE &mDFAs);
    NFAState* GetExistState(const std::set<NFAState*> &mA, FSA_TABLE &mDFAs);
public:
    int ReNumber(const FSA_TABLE &mDFAs, int start_id, FSA_TABLE &newDFA);


};

#endif // NFACONVERT_H
