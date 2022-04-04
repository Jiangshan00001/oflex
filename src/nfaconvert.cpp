#include <iostream>
#include "fsa_to_dot.h"

#include "nfaconvert.h"


std::set<int> GetStatesTransChars(std::set<NFAState*> mNFAs)
{
    std::set<int> mChars;
    //////////////////////////////////////////////////////////////////////////
    std::set<NFAState*>::iterator itn;
    for (itn=mNFAs.begin();itn!=mNFAs.end();++itn)
    {
        //搜索所有字符
        std::set<int> mChars1;
        mChars1 = (*itn)->GetTransChar();
        mChars.insert(mChars1.begin(), mChars1.end());
    }
    return mChars;
}

NFAConvert::NFAConvert()
{
}

FSA_TABLE NFAConvert::NFAListCont(std::vector<FSA_TABLE> &nfa_list)
{
    FSA_TABLE ret;

    return ret;
}

FSA_TABLE NFAConvert::NFAtoDFA(FSA_TABLE &nfa, int startId)
{
    std::set<NFAState*> mNFAs;//一个DFA内部的NFA

    FSA_TABLE m_DFATable;
    m_nNextStateID = startId-1;

    if(nfa.empty())return m_DFATable;

    /// NFA->DFA algorithm

    mNFAs.clear();
    /// add start nfa state
    mNFAs.insert(nfa[0]);
    /// get all zero-move nfa states
    mNFAs = MoveZero(mNFAs,mNFAs);
    NFAState *pState = new NFAState(mNFAs, ++m_nNextStateID);
    pState->m_mark_flag = 0;

    m_DFATable.push_back(pState);


    ///
    for (int i=0;i<m_DFATable.size();++i)
    {
        if (!m_DFATable[i]->m_mark_flag)
        {
            m_DFATable[i]->m_mark_flag = 1;
            ///处理

            mNFAs = m_DFATable[i]->GetNFAState();

            /////搜索所有字符
            std::set<int> mChars = GetStatesTransChars(mNFAs);

            /// 遍历所有字符
            for (std::set<int>::iterator itc=mChars.begin();itc!=mChars.end();++itc)
            {
                std::set<NFAState*> mNFAMove;
                std::set<NFAState*> mNFAEps;
                if(*itc == EPS_CHAR)continue;//eps character, ignore
                /// MOVE(T,a)
                mNFAMove = MoveOne(*itc, mNFAs);
                mNFAEps = MoveZero(mNFAMove, mNFAEps);

                pState = new NFAState(mNFAEps, ++m_nNextStateID);
                if(!GetExistState(pState, m_DFATable))
                {
                    pState->m_mark_flag = 0;
                    m_DFATable.push_back(pState);
                    m_DFATable[i]->AddTransition(*itc, pState);
                }
                else
                {
                    pState = GetExistState(pState, m_DFATable);
                    m_DFATable[i]->AddTransition(*itc, pState);
                }
            }

        }
    }

#ifdef DEBUG
    fsa_to_dot(m_DFATable, "dfa_out.dot");
#endif

    return m_DFATable;
}


/// 查找dfa中组成的nfa，哪个里有指定的nfa状态
NFAState* GetDfaFromNfa(NFAState* nfa_state, FSA_TABLE &dfa)
{
    for(int i=0;i<dfa.size();++i)
    {
        auto nfas = dfa[i]->GetNFAState();
        for(auto j=nfas.begin();j!=nfas.end();++j)
        {
            if (nfa_state==(*j))
            {
                return dfa[i];
            }
        }
    }
    return NULL;
}

FSA_TABLE NFAConvert::DFAmin(FSA_TABLE &dfa)
{
    FSA_TABLE dfa_min;

    if (dfa.empty())return dfa_min;

    int id=0;

    //将状态分2组，可接受状态和非可接受状态
    std::set<NFAState*> st_acc;
    std::set<NFAState*> st_not_acc;

    for(auto it=dfa.begin();it!=dfa.end();++it)
    {
        if((*it)->m_bAcceptingState&0x02)
        {
            st_acc.insert(it, it+1);
        }
        else
        {
            st_not_acc.insert(it, it+1);
        }
    }

    dfa_min.push_back(new NFAState(st_not_acc,++id ));
    dfa_min.push_back(new NFAState(st_acc,++id ));

    int is_changed=0;

    do{
        is_changed = 0;
        for(int i=0;i<dfa_min.size();++i)
        {
            auto curr_st = dfa_min[i];
            curr_st->ClearTransition();

            //对于所有输入符号，状态s和状态t必须转换到等价的状态里。 否则就要拆分
            auto & sub_states=curr_st->GetNFAState();
            if (sub_states.empty())continue;

            for(auto j=sub_states.begin();j!=sub_states.end();++j)
            {
                std::multimap<int, NFAState*>* one_st_trans = (*j)->GetTransition();
                for (auto k=one_st_trans->begin();k!=one_st_trans->end();++k)
                {
                    std::set<NFAState*> curr_already_status;

                    NFAState * new_state= GetDfaFromNfa(k->second, dfa_min);
                    if(j==sub_states.begin())
                    {
                        //直接添加: 第0个子状态，直接添加
                        curr_st->AddTransition(k->first, new_state);
                        continue;
                    }

                    curr_st->GetTransition(k->first, curr_already_status);
                    if (curr_already_status.size()==0)

                    {
                        //没有这个转换状态
                        is_changed=1;
                    }
                    else if ((*curr_already_status.begin())==new_state)
                    {
                        //和当前是等价状态，下一个
                        continue;
                    }

                    {
                        //遇到状态不等价，拆分
                        is_changed=1;
                        std::set<NFAState*> stn;
                        stn.insert(*j);
                        sub_states.erase(j);
                        dfa_min.push_back(new NFAState(stn,++id ));
                        break;
                    }
                }
                if (is_changed)break;
            }
            if (is_changed)break;
        }
    } while(is_changed);



    /// 去除无效状态
    /// 不能从起始状态到达的状态
    /// 不能到达结束态的状态
    for(int i=0;i<dfa_min.size();++i)
    {
        if (dfa_min[i]->GetNFAState().size()==0)
        {
            dfa_min.erase(dfa_min.begin()+i);
            i-=1;
        }
        if (dfa_min[i]->IsDeadEnd())
        {
            dfa_min.erase(dfa_min.begin()+i);
            i-=1;
        }
    }


    /// 更新accept state 状态
    for(int i=0;i<dfa_min.size();++i)
    {
        dfa_min[i]->updateAcceptingState();

    }

    return dfa_min;

}

std::set<NFAState*> NFAConvert::MoveOne(int mC, std::set<NFAState*> mNFAs)
{
    std::set<NFAState*> mRes;

    for (std::set<NFAState*>::iterator it=mNFAs.begin();it!=mNFAs.end();++it)
    {
        std::set<NFAState*> mTmp;
        (*it)->GetTransition(mC, mTmp);
        mRes.insert(mTmp.begin(), mTmp.end());
    }

    return mRes;

}
std::set<NFAState*> NFAConvert::MoveZero(std::set<NFAState*> &mNFAs, std::set<NFAState*> &mRes1)
{
    std::set<NFAState*> mRes1Bak;
    std::set<NFAState*> mResCurr;
    std::set<NFAState*> mResNeedRefresh;

    mRes1.insert(mNFAs.begin(), mNFAs.end());
    mRes1Bak.insert(mRes1.begin(), mRes1.end());

    while (!mRes1Bak.empty())
    {
        mResNeedRefresh.clear();
        mResCurr = MoveOne(EPS_CHAR, mRes1Bak);///新生成的

        for (std::set<NFAState*>::iterator it2=mResCurr.begin();it2!=mResCurr.end();++it2)
        {
            if (mRes1.find(*it2) == mRes1.end())
            {
                mResNeedRefresh.insert(*it2);
            }
            mRes1.insert(*it2);
        }
        mRes1Bak = mResNeedRefresh;
    }

    return mRes1;
}

NFAState* NFAConvert::GetExistState(NFAState* mA, FSA_TABLE& mDFAs)
{
    for(FSA_TABLE::iterator it=mDFAs.begin();it!=mDFAs.end();++it)
    {
        if (*mA == *(*it))
        {
            return *it;
        }
    }
    return 0;
}
