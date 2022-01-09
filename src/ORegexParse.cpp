#include <iostream>
#include "ORegexParse.h"
#include "dot_generator.h"

using namespace std;

ORegexParse::ORegexParse()
{
	m_nNextStateID = 0;
}


int ORegexParse::NFAStackPop(FSA_STACK &stk, FSA_TABLE &NFATable)
{
    // If the stack is empty we cannot pop anything
    if(stk.size()>0)
    {
        NFATable = stk.top();
#if DEBUG_ALL
        static int i=0;
        stringstream istr;
        istr<<"pop_file"<<i++<<".dot";
        fsa_to_dot(NFATable, istr.str());
#endif

        stk.pop();
        return 1;
    }

    return 0;
}

int ORegexParse::NFAStackPush(FSA_STACK &stk,FSA_TABLE &NFATable)
{
#if DEBUG_ALL
	static int i=0;
	stringstream istr;
	istr<<"push_file"<<i++<<".dot";
	fsa_to_dot(NFATable, istr.str());
#endif

    stk.push(NFATable);
	return 0;
}

int ORegexParse::PushOneByte(char chInput, FSA_STACK &dst)
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


    // push it onto the operand stack
    dst.push(NFATable);

    TRACE("PUSH %c\n", chInput);
    return 0;
}


bool ORegexParse::Concat(FSA_STACK &stk)
{
	// Pop 2 elements
	FSA_TABLE A, B;
    if(!NFAStackPop(stk, B) || !NFAStackPop(stk, A))
		return 0;
	
	// Now evaluate AB
	// Basically take the last state from A
	// and add an epsilon transition to the
	// first state of B. Store the result into
	// new NFA_TABLE and push it onto the stack
	A[A.size()-1]->AddTransition(0, B[0]);
	A.insert(A.end(), B.begin(), B.end());
	
	// Push the result onto the stack
    NFAStackPush(stk, A);
	
	TRACE("CONCAT\n");
	
	return 1;
}

bool ORegexParse::Star(FSA_STACK &stk)
{
	// Pop 1 element
	FSA_TABLE A, B;
    if(!NFAStackPop(stk, A))
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
    NFAStackPush(stk, A);
	
	TRACE("STAR\n");
	
	return 1;
}

bool ORegexParse::Union(FSA_STACK &stk)
{
	// Pop 2 elements
	FSA_TABLE A, B;
    if(!NFAStackPop(stk, B) || !NFAStackPop(stk, A))
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
    NFAStackPush(stk, A);
	
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

bool ORegexParse::Eval(FSA_STACK &OperandStack, std::stack<char> &OperatorStack)
{

	// First pop the operator from the stack
    if(OperatorStack.size()>0)
	{
        char chOperator = OperatorStack.top();
        OperatorStack.pop();

		
		// Check which operator it is
		switch(chOperator)
		{
        case  '*'://#42
            return Star(OperandStack);
			break;
		case 124:
            return Union(OperandStack);
			break;
		case   8:
            return Concat(OperandStack);
			break;
		}
		
		return 0;
	}
	
	return 0;
}


FSA_TABLE ORegexParse::CreateNFA(string strRegEx)
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

    FSA_TABLE m_NFATable;

    FSA_STACK OperandStack;
    std::stack<char> OperatorStack;

	for(int i=0; i<strRegEx.size(); ++i)
	{
		// get the charcter
		char c = strRegEx[i];

#if DEBUG_ALL
		cout<<"i="<<i<<endl;
#endif

		if(IsInput(c))///有一个字符，生成合集并放入操作数堆栈
		{
            PushOneByte(c, OperandStack);
		}
        else if(OperatorStack.empty())/// 如果算符为空，直接放入
		{
            OperatorStack.push(c);
		}
		else if(IsLeftParanthesis(c))///左括号
		{
            OperatorStack.push(c);
		}
		else if(IsRightParanthesis(c))///右括号
		{
			// Evaluate everyting in paranthesis
            while(!IsLeftParanthesis(OperatorStack.top()))
                if(!Eval(OperandStack, OperatorStack))
                    return m_NFATable;
				// Remove left paranthesis after the evaluation
                OperatorStack.pop();
		}
		else
		{
			/// 如果之前已经有算符了，并且当前算符不是括号
            while(!OperatorStack.empty() && Presedence(c, OperatorStack.top()))
			{///如果内部原有算符，切当前算符的优先级小，则先计算				
                if(!Eval(OperandStack, OperatorStack))
                    return m_NFATable;
			}
            OperatorStack.push(c);
		}
	}
	
	// Evaluate the rest of operators
    while(!OperatorStack.empty())
	{
        if(!Eval(OperandStack, OperatorStack))
            return m_NFATable;
	}
		
	// Pop the result from the stack
    if(!NFAStackPop(OperandStack, m_NFATable))
	{
        return m_NFATable;
	}
	
	// Last NFA state is always accepting state
	m_NFATable[m_NFATable.size()-1]->m_bAcceptingState = 1;
	
    return m_NFATable;
}

