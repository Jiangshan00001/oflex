#include <iostream>
#include "ORegexParse.h"
#include "dot_generator.h"

using namespace std;

ORegexParse::ORegexParse()
{
	m_nNextStateID = 0;
}

int ORegexParse::NFAStackPop(FSA_TABLE &NFATable)
{
	// If the stack is empty we cannot pop anything
	if(m_OperandStack.size()>0)
	{
		NFATable = m_OperandStack.top();
#if DEBUG_ALL
		static int i=0;
		stringstream istr;
		istr<<"pop_file"<<i++<<".dot";
		fsa_to_dot(NFATable, istr.str());
#endif

		m_OperandStack.pop();
		return 1;
	}
	
	return 0;
}
int ORegexParse::NFAStackPush(FSA_TABLE &NFATable)
{
#if DEBUG_ALL
	static int i=0;
	stringstream istr;
	istr<<"push_file"<<i++<<".dot";
	fsa_to_dot(NFATable, istr.str());
#endif

	m_OperandStack.push(NFATable);
	return 0;
}

int ORegexParse::PushOneByte(char chInput)
{
	// Create 2 new states on the heap
	NFAState *s0 = new NFAState(++m_nNextStateID);
	NFAState *s1 = new NFAState(++m_nNextStateID);
	
	// Add the transition from s0->s1 on input character
	s0->AddTransition(chInput, s1);
	
	// Create a NFA from these 2 states 
	FSA_TABLE NFATable;
	NFATable.push_back(s0);
	NFATable.push_back(s1);

#if DEBUG_ALL
	//fsa_to_dot(NFATable);
#endif
	
	// push it onto the operand stack
	NFAStackPush(NFATable);
	
	// Add this character to the input character set
	//m_InputSet.insert(chInput);
	
	TRACE("PUSH %c\n", chInput);	
	return 0;
}


bool ORegexParse::Concat()
{
	// Pop 2 elements
	FSA_TABLE A, B;
	if(!NFAStackPop(B) || !NFAStackPop(A))
		return 0;
	
	// Now evaluate AB
	// Basically take the last state from A
	// and add an epsilon transition to the
	// first state of B. Store the result into
	// new NFA_TABLE and push it onto the stack
	A[A.size()-1]->AddTransition(0, B[0]);
	A.insert(A.end(), B.begin(), B.end());
	
	// Push the result onto the stack
	NFAStackPush(A);
	
	TRACE("CONCAT\n");
	
	return 1;
}

bool ORegexParse::Star()
{
	// Pop 1 element
	FSA_TABLE A, B;
	if(!NFAStackPop(A))
		return 0;
	
	// Now evaluate A*
	// Create 2 new states which will be inserted 
	// at each end of deque. Also take A and make 
	// a epsilon transition from last to the first 
	// state in the queue. Add epsilon transition 
	// between two new states so that the one inserted 
	// at the begin will be the source and the one
	// inserted at the end will be the destination
	NFAState *pStartState	= new NFAState(++m_nNextStateID);
	NFAState *pEndState	= new NFAState(++m_nNextStateID);
	pStartState->AddTransition(0, pEndState);
	
	// add epsilon transition from start state to the first state of A
	pStartState->AddTransition(0, A[0]);
	
	// add epsilon transition from A last state to end state
	A[A.size()-1]->AddTransition(0, pEndState);
	
	// From A last to A first state
	A[A.size()-1]->AddTransition(0, A[0]);
	
	// construct new DFA and store it onto the stack
	A.push_back(pEndState);
	A.push_front(pStartState);
	
	// Push the result onto the stack
	NFAStackPush(A);
	
	TRACE("STAR\n");
	
	return 1;
}

bool ORegexParse::Union()
{
	// Pop 2 elements
	FSA_TABLE A, B;
	if(!NFAStackPop(B) || !NFAStackPop(A))
		return 0;
	
	// Now evaluate A|B
	// Create 2 new states, a start state and
	// a end state. Create epsilon transition from
	// start state to the start states of A and B
	// Create epsilon transition from the end 
	// states of A and B to the new end state
	NFAState *pStartState	= new NFAState(++m_nNextStateID);
	NFAState *pEndState	= new NFAState(++m_nNextStateID);
	pStartState->AddTransition(0, A[0]);
	pStartState->AddTransition(0, B[0]);
	A[A.size()-1]->AddTransition(0, pEndState);
	B[B.size()-1]->AddTransition(0, pEndState);
	
	// Create new NFA from A
	B.push_back(pEndState);
	A.push_front(pStartState);
	A.insert(A.end(), B.begin(), B.end());
	
	// Push the result onto the stack
	NFAStackPush(A);
	
	TRACE("UNION\n");
	
	return 1;
}


string ORegexParse::ConcatExpand(string strRegEx)
{
	string strRes;
	
	for(int i=0; i<strRegEx.size()-1; ++i)
	{
		char cLeft	= strRegEx[i];
		char cRight = strRegEx[i+1];
		strRes	   += cLeft;
		if((IsInput(cLeft)) || (IsRightParanthesis(cLeft)) || (cLeft == '*'))
			if((IsInput(cRight)) || (IsLeftParanthesis(cRight)))
				strRes += char(8);
	}
	strRes += strRegEx[strRegEx.size()-1];
	
	return strRes;
}


bool ORegexParse::Eval()
{
#if DEBUG_ALL
	cout<< "eval"<<endl;
	cout<<"operator size="<<m_OperatorStack.size()<<endl;
	if (m_OperatorStack.size()>0)
	{
	cout<<"top operator="<<(unsigned int)m_OperatorStack.top()<<endl;
	}


#endif

	// First pop the operator from the stack
	if(m_OperatorStack.size()>0)
	{
		char chOperator = m_OperatorStack.top();
		m_OperatorStack.pop();

		
		// Check which operator it is
		switch(chOperator)
		{
		case  42:
			return Star();
			break;
		case 124:
			return Union();
			break;
		case   8:
			return Concat();
			break;
		}
		
		return 0;
	}
	
	return 0;
}


bool ORegexParse::CreateNFA(string strRegEx)
{
	// Parse regular expresion using similar 
	// method to evaluate arithmetic expressions
	// But first we will detect concatenation and
	// insert char(8) at the position where 
	// concatenation needs to occur

#if DEBUG_ALL
	cout<< strRegEx<<endl;
#endif

	strRegEx = ConcatExpand(strRegEx);

#if DEBUG_ALL
	cout<<"after ConcatExpand"<<strRegEx.size()<<": "<< strRegEx<<endl;
#endif

	for(int i=0; i<strRegEx.size(); ++i)
	{
		// get the charcter
		char c = strRegEx[i];

#if DEBUG_ALL
		cout<<"i="<<i<<endl;
#endif

		if(IsInput(c))///有一个字符，生成合集并放入操作数堆栈
		{
			PushOneByte(c);
		}
		else if(m_OperatorStack.empty())/// 如果算符为空，直接放入
		{
			m_OperatorStack.push(c);
		}
		else if(IsLeftParanthesis(c))///左括号
		{
			m_OperatorStack.push(c);
		}
		else if(IsRightParanthesis(c))///右括号
		{
			// Evaluate everyting in paranthesis
			while(!IsLeftParanthesis(m_OperatorStack.top()))
				if(!Eval())
					return 0;
				// Remove left paranthesis after the evaluation
				m_OperatorStack.pop(); 
		}
		else
		{
			/// 如果之前已经有算符了，并且当前算符不是括号
			while(!m_OperatorStack.empty() && Presedence(c, m_OperatorStack.top()))
			{///如果内部原有算符，切当前算符的优先级小，则先计算				
				if(!Eval())
					return 0;
			}
				m_OperatorStack.push(c);
		}
	}
	
	// Evaluate the rest of operators
	while(!m_OperatorStack.empty())
	{
		if(!Eval())
			return 0;
	}
		
	// Pop the result from the stack
	if(!NFAStackPop(m_NFATable))
	{
		return 0;
	}
	
	// Last NFA state is always accepting state
	m_NFATable[m_NFATable.size()-1]->m_bAcceptingState = 1;
	
	fsa_to_dot(m_NFATable,"nfa_out.dot");
	return 1;
}

std::set<NFAState*> ORegexParse::MoveOne(char mC, std::set<NFAState*> mNFAs)
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
std::set<NFAState*> ORegexParse::MoveZero(std::set<NFAState*> mNFAs, std::set<NFAState*> &mRes1)
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

NFAState* ORegexParse::GetExistState(NFAState* mA, FSA_TABLE& mDFAs)
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
int ORegexParse::NFAToDFA()
{
	//std::set<NFAState*> mStateTable;
	//std::set<NFAState*> mStateSet;
	std::set<NFAState*> mNFAs;//一个DFA内部的NFA

	m_nNextStateID = 0;
	m_DFATable.clear();//清空DFA

	if(m_NFATable.empty())return 0;

	/// 找到开始状态的eps转换，并设置为未标记
	mNFAs.clear();
	mNFAs.insert(m_NFATable[0]);
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

	return 0;
}

