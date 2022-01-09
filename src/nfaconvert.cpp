#include <iostream>
#include "dot_generator.h"

#include "nfaconvert.h"

NFAConvert::NFAConvert()
{
}

FSA_TABLE NFAConvert::NFAtoDFA(FSA_TABLE &nfa)
{
#if 1
    //std::set<NFAState*> mStateTable;
    //std::set<NFAState*> mStateSet;
    std::set<NFAState*> mNFAs;//一个DFA内部的NFA

    FSA_TABLE m_DFATable;
    int m_nNextStateID = 0;
    m_DFATable.clear();//清空DFA

    if(nfa.empty())return m_DFATable;

    /// 找到开始状态的eps转换，并设置为未标记
    mNFAs.clear();
    mNFAs.insert(nfa[0]);
    mNFAs = MoveZero(mNFAs,mNFAs);
    NFAState *pState = new NFAState(mNFAs, ++m_nNextStateID);
    pState->m_MarkFlag = 0;

    m_DFATable.push_back(pState);


    /// 将所有未标记的标记，并处理
    for (int i=0;i<m_DFATable.size();++i)
    {
        if (!m_DFATable[i]->m_MarkFlag)
        {
            m_DFATable[i]->m_MarkFlag = 1;
            ///处理

            mNFAs = m_DFATable[i]->GetNFAState();

            std::set<char> mChars;
            //////////////////////////////////////////////////////////////////////////
            std::set<NFAState*>::iterator itn;
            for (itn=mNFAs.begin();itn!=mNFAs.end();++itn)
            {
                //搜索所有字符
                std::set<char> mChars1;
                mChars1 = (*itn)->GetTransChar();
                mChars.insert(mChars1.begin(), mChars1.end());
            }

            /// 遍历所有字符
            for (std::set<char>::iterator itc=mChars.begin();itc!=mChars.end();++itc)
            {
                std::set<NFAState*> mNFAMove;
                std::set<NFAState*> mNFAEps;
                if(*itc == 0)continue;//eps character, ignore
                /// MOVE(T,a)
                mNFAMove = MoveOne(*itc, mNFAs);
                mNFAEps = MoveZero(mNFAMove, mNFAEps);

                pState = new NFAState(mNFAEps, ++m_nNextStateID);
                if(!GetExistState(pState, m_DFATable))
                {
                //if(m_DFATable.find())
                pState->m_MarkFlag = 0;
                m_DFATable.push_back(pState);
                m_DFATable[i]->AddTransition(*itc, pState);
                }
                else
                {
                    delete pState;
                    pState = GetExistState(pState, m_DFATable);
                    m_DFATable[i]->AddTransition(*itc, pState);
                }
            }

            //////////////////////////////////////////////////////////////////////////
            //i = -1;///重新查找未标记的状态
        }
    }


    fsa_to_dot(m_DFATable, "dfa_out.dot");
#endif
    return m_DFATable;
}
std::set<NFAState*> NFAConvert::MoveOne(char mC, std::set<NFAState*> mNFAs)
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
std::set<NFAState*> NFAConvert::MoveZero(std::set<NFAState*> mNFAs, std::set<NFAState*> &mRes1)
{
    std::set<NFAState*> mRes1Bak;
    std::set<NFAState*> mResCurr;
    std::set<NFAState*> mResNeedRefresh;

    mRes1.insert(mNFAs.begin(), mNFAs.end());
    mRes1Bak.insert(mRes1.begin(), mRes1.end());

    while (!mRes1Bak.empty())
    {
        mResNeedRefresh.clear();
        mResCurr = MoveOne(0, mRes1Bak);///新生成的

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
