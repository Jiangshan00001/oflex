#include <iostream>
#include <algorithm>
#include "ORegexParse.h"
#include "fsa_to_dot.h"

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

#if 0
int ORegexParse::PushOneDot(FSA_STACK &dst)
{
    // Create 2 new states on the heap
    NFAState *s0 = new NFAState(++m_nNextStateID);
    NFAState *s1 = new NFAState(++m_nNextStateID);

    // Add the transition from s0->s1 on input character
    for(int i=0x20;i<0x7f;++i)
    {
        if(i=='\n')continue;
        s0->AddTransition(i, s1);
    }


    // Create a NFA from these 2 states
    FSA_TABLE NFATable;
    NFATable.push_back(s0);
    NFATable.push_back(s1);


    // push it onto the operand stack
    dst.push(NFATable);

    return 0;
}
#endif

void ORegexParse::add_concat_if_need(FSA_STACK &dst, std::stack<operator_stack_t> &operator_stack)
{
    ///after push one byte, check if last operand has operator or not.
    /// if not, add one concat operator
    ///
    if ((operator_stack.size()==0)&& (dst.size()>1))
    {
        operator_stack.push(operator_stack_t(dst.size()-1,OPERATOR_CONCAT));
        return;
    }
    if(operator_stack.size()==0)return;
    if(operator_stack.top().operand_num<dst.size()-1)
    {
        operator_stack.push(operator_stack_t(dst.size()-1,OPERATOR_CONCAT));
    }
}

int ORegexParse::PushMiddleLR(std::vector<int> chars, int is_inv,FSA_STACK &dst, std::stack<operator_stack_t> &operator_stack)
{
    // Create 2 new states on the heap
    NFAState *s0 = new NFAState(++m_nNextStateID);
    NFAState *s1 = new NFAState(++m_nNextStateID);

    if(is_inv)
    {
        for(int i=0;i<256;++i)
        {
            if (std::find(chars.begin(), chars.end(),i)!= chars.end())
            {
                continue;
            }
            // Add the transition from s0->s1 on input character
            s0->AddTransition(i, s1);
        }
    }
    else
    {
        for(int i=0;i<chars.size();++i)
        {// Add the transition from s0->s1 on input character
            s0->AddTransition(chars[i], s1);
        }
    }

    // Create a NFA from these 2 states
    FSA_TABLE NFATable;
    NFATable.push_back(s0);
    NFATable.push_back(s1);




    // push it onto the operand stack
    dst.push(NFATable);

    add_concat_if_need(dst, operator_stack);
    ///----------------------------

    TRACE("PushMiddleLR %c\n", is_inv);
    return 0;

}


int ORegexParse::PushDotByte(FSA_STACK &dst, std::stack<operator_stack_t> &operator_stack)
{
    // Create 2 new states on the heap
    NFAState *s0 = new NFAState(++m_nNextStateID);
    NFAState *s1 = new NFAState(++m_nNextStateID);
    ///add 10->\r in . seq
    for(int i=10;i<0x7f;++i)
    {
        if(i=='\n')continue;
        // Add the transition from s0->s1 on input character
        s0->AddTransition(i, s1);
    }

    // Create a NFA from these 2 states
    FSA_TABLE NFATable;
    NFATable.push_back(s0);
    NFATable.push_back(s1);


    // push it onto the operand stack
    dst.push(NFATable);

    add_concat_if_need(dst, operator_stack);

    TRACE("PUSHDOT \n");
    return 0;
}

int ORegexParse::PushOneByte(int chInput, FSA_STACK &dst, std::stack<operator_stack_t> &operator_stack)
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

    add_concat_if_need(dst, operator_stack);

    TRACE("PUSH %c\n", chInput);
    return 0;
}


bool ORegexParse::OPRWhy(FSA_STACK &stk)
{
    // Pop 1 element
    FSA_TABLE A;
    if(!NFAStackPop(stk, A))
        return 0;

    // Now evaluate A?
    // Create 2 new states which will be inserted
    // at each end of deque. Also take A and make
    // a epsilon transition from last to the first
    // state in the queue. Add epsilon transition
    // between two new states so that the one inserted
    // at the begin will be the source and the one
    // inserted at the end will be the destination
    NFAState *pStartState	= new NFAState(++m_nNextStateID);
    NFAState *pEndState	= new NFAState(++m_nNextStateID);
    pStartState->AddTransition(EPS_CHAR, pEndState);

    // add epsilon transition from start state to the first state of A
    pStartState->AddTransition(EPS_CHAR, A[0]);
    pStartState->AddTransition(EPS_CHAR, A[A.size()-1]);

    // add epsilon transition from A last state to end state
    A[A.size()-1]->AddTransition(EPS_CHAR, pEndState);

    // construct new DFA and store it onto the stack
    A.push_back(pEndState);
    A.push_front(pStartState);

    // Push the result onto the stack
    NFAStackPush(stk, A);

    TRACE("WHY\n");

    return 1;
}
bool ORegexParse::OPRPlus(FSA_STACK &stk)
{
    // Pop 1 element
    FSA_TABLE A;
    if(!NFAStackPop(stk, A))
        return 0;

    // Now evaluate A+
    // Create 2 new states which will be inserted
    // at each end of deque. Also take A and make
    // a epsilon transition from last to the first
    // state in the queue. Add epsilon transition
    // between two new states so that the one inserted
    // at the begin will be the source and the one
    // inserted at the end will be the destination
    NFAState *pStartState	= new NFAState(++m_nNextStateID);
    NFAState *pEndState	= new NFAState(++m_nNextStateID);

    // add epsilon transition from start state to the first state of A
    pStartState->AddTransition(EPS_CHAR, A[0]);


    // add epsilon transition from A last state to end state
    A[A.size()-1]->AddTransition(EPS_CHAR, pEndState);
    A[A.size()-1]->AddTransition(EPS_CHAR, pStartState);

    // construct new DFA and store it onto the stack
    A.push_back(pEndState);
    A.push_front(pStartState);

    // Push the result onto the stack
    NFAStackPush(stk, A);

    TRACE("Plus\n");

    return 1;
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
    A[A.size()-1]->AddTransition(EPS_CHAR, B[0]);
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
    pStartState->AddTransition(EPS_CHAR, pEndState);
	
	// add epsilon transition from start state to the first state of A
    pStartState->AddTransition(EPS_CHAR, A[0]);
	
	// add epsilon transition from A last state to end state
    A[A.size()-1]->AddTransition(EPS_CHAR, pEndState);
	
	// From A last to A first state
    A[A.size()-1]->AddTransition(EPS_CHAR, A[0]);
	
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
    pStartState->AddTransition(EPS_CHAR, A[0]);
    pStartState->AddTransition(EPS_CHAR, B[0]);
    A[A.size()-1]->AddTransition(EPS_CHAR, pEndState);
    B[B.size()-1]->AddTransition(EPS_CHAR, pEndState);
	
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
                strRes += char(OPERATOR_CONCAT);
	}
	strRes += strRegEx[strRegEx.size()-1];
	
	return strRes;
}

bool ORegexParse::Eval(FSA_STACK &OperandStack, std::stack<operator_stack_t> &OperatorStack)
{

    bool ret = false;
	// First pop the operator from the stack
    if(OperatorStack.size()>0)
	{
        char chOperator = OperatorStack.top().val;
        OperatorStack.pop();

		
		// Check which operator it is
		switch(chOperator)
		{
        case  OPERATOR_STAR://#42
            ret= Star(OperandStack);
			break;
        case OPERATOR_UNION://124
            ret= Union(OperandStack);
			break;
        case   OPERATOR_CONCAT:
            ret= Concat(OperandStack);
			break;
        case OPERATOR_WHY:
            ret= OPRWhy(OperandStack);
            break;
        case OPERATOR_PLUS:
            ret= OPRPlus(OperandStack);
            break;
        case OPERATOR_LEFTP:
            std::cerr<<"ORegexParse::Eval: unknown opr:"<<chOperator<<std::endl;
            break;
        case OPERATOR_RIGHTP:
            std::cerr<<"ORegexParse::Eval: unknown opr:"<<chOperator<<std::endl;
            break;
        default:
            std::cerr<<"ORegexParse::Eval: unknown opr:"<<chOperator<<std::endl;
            break;
		}

        add_concat_if_need(OperandStack, OperatorStack);
        return ret;
	}
	
	return 0;
}



FSA_TABLE ORegexParse::CreateNFAFlex(string strRegEx, int startId)
{

#if DEBUG_ALL
    cout<< strRegEx<<endl;
#endif

    strRegEx = strRegEx;


    FSA_TABLE mNFATable;

    /// current stack design is limit.
    /// as stack operator could not assign it's position with operand
    ///
    /// refactor: add OperatorStackOperandNum. information about the operator pos in operand_stack.
    FSA_STACK OperandStack;
    std::stack<operator_stack_t> OperatorStack;

    m_nNextStateID = startId-1;

    // Create 1 start states on the heap
    NFAState *s0 = new NFAState(++m_nNextStateID);
    s0->m_bAcceptingState=START_STATE;

    // change start state to add in the end of the regrex parse.
    // Create a NFA from these 2 states
    FSA_TABLE startNFATable;
    startNFATable.push_back(s0);

    // push it onto the operand stack
    //OperandStack.push(startNFATable);


    int curr_status=0;//1--dquote. 0--normal

    std::vector<int> status_middle_left_chars;//store [] chars
    int status_middle_left_inv=0;


    for(int i=0; i<strRegEx.size(); ++i)
    {
        // get the character
        char c = strRegEx[i];
#if DEBUG_ALL
        cout<<"i="<<i<<endl;
#endif
        if (curr_status==1)
        {//"
            ///dquote mode
            /// just concat all characters

            if(c == OPERATOR_DQUOTE)
            {
                curr_status=0;
                //OperatorStack.pop();
                while(OperatorStack.top().val!=OPERATOR_DQUOTE)
                {
                    Eval(OperandStack, OperatorStack);
                }
                OperatorStack.pop();//pop "
                continue;
            }
            if(c=='\\')
            {
                ++i;
                c = strRegEx[i];
                //PushOneByte(c, OperandStack);
                //continue;
            }

            PushOneByte(c, OperandStack,OperatorStack);

            continue;
        }
        else if (curr_status==2)
        {//[
            if (c=='^')
            {
                status_middle_left_inv=1;
                continue;
            }
            //中括号[]

            if (c=='-')
            {
                ///A-Z. when parse -, A already pushed to operand stack.
                //OperandStack.pop();
                //push_one_opd=0;
                if (status_middle_left_chars.size()>0)
                {
                    status_middle_left_chars.pop_back();
                    unsigned char c1=strRegEx[i-1];
                    unsigned char c2=strRegEx[i+1];
                    for(int cc=c1;cc<=c2;++cc)
                    {
                        status_middle_left_chars.push_back(cc);
                    }
                    ++i;//skip c2
                    continue;
                }
                else
                {///FIXME this issue: [-/+*()\n]
                    status_middle_left_chars.push_back(c);
                }
            }
            if (c==OPERATOR_RIGHTMID)
            {
                OperatorStack.pop();//pop [ operator

                //if(push_one_opd)OperatorStack.push(OPERATOR_UNION);
                PushMiddleLR(status_middle_left_chars, status_middle_left_inv,OperandStack,OperatorStack);
                //中括号
                //while(OperatorStack.top()!=OPERATOR_LEFTMID)
                //    Eval(OperandStack, OperatorStack);

                curr_status=0;
                continue;
            }

            if(c=='\\')
            {
                std::map<int, int> control_char={{'n','\n'},{'t','\t'},{'v','\v'},{'f','\f'},{'r','\r'} };
                ++i;
                if(control_char.find(strRegEx[i])!=control_char.end())
                {
                    c=control_char[strRegEx[i]];
                }
                else
                {
                    // \\
                    c = strRegEx[i];
                }

            }


            //if(push_one_opd)OperatorStack.push(OPERATOR_UNION);
            //push_one_opd=0;
            //PushOneByte(c, OperandStack);
            //push_one_opd=1;
            status_middle_left_chars.push_back(c);

            continue;
#if 0

#endif
        }
        else
        {
            //normal mode
            if(IsInput(c))///有一个字符，生成合集并放入操作数堆栈
            {
                if(c=='\\')
                {
                    ++i;
                    c = strRegEx[i];
                    //PushOneByte(c, OperandStack);
                    //continue;
                }


                if(c=='.')
                {
                    PushDotByte(OperandStack,OperatorStack);
                }
                else
                {
                    PushOneByte(c, OperandStack,OperatorStack);
                }
            }
            else if(IsRightParanthesis(c))///右括号
            {
                // Evaluate everyting in paranthesis
                while(!IsLeftParanthesis(OperatorStack.top().val))
                {
                    if(!Eval(OperandStack, OperatorStack))
                    {
                        return mNFATable;
                    }
                }

                    // Remove left paranthesis after the evaluation
                    OperatorStack.pop();
            }
            else
            {
                /// 如果之前已经有算符了，并且当前算符不是括号
                while(!OperatorStack.empty() && Presedence(c, OperatorStack.top().val))
                {///如果内部原有算符，且当前算符的优先级小，则先计算

                    if(!Eval(OperandStack, OperatorStack))
                        return mNFATable;
                }
                if (c==OPERATOR_LEFTMID)
                {
                    curr_status=2;
                    status_middle_left_inv=0;
                    status_middle_left_chars.clear();
                }
                else if(c==OPERATOR_DQUOTE)
                {
                    curr_status=1;
                }

                OperatorStack.push(operator_stack_t(OperandStack.size(), c));
            }
        }


    }

    // Evaluate the rest of operators
    while(!OperatorStack.empty())
    {
        if(!Eval(OperandStack, OperatorStack))
            return mNFATable;
    }

    // Pop the result from the stack
    if(!NFAStackPop(OperandStack, mNFATable))
    {
        return mNFATable;
    }
    OperandStack.push(startNFATable);
    OperandStack.push(mNFATable);
    OperatorStack.push( operator_stack_t(OperandStack.size(), OPERATOR_CONCAT) );
    Eval(OperandStack, OperatorStack);
    NFAStackPop(OperandStack, mNFATable);




    // Last NFA state is always accepting state
    mNFATable[mNFATable.size()-1]->m_bAcceptingState |= FINAL_STATE;
    mNFATable[mNFATable.size()-1]->m_accepting_regrex=strRegEx;

    int start_state_id = mNFATable[0]->m_nStateID;
    int final_state_id = mNFATable[mNFATable.size()-1]->m_nStateID;

#if 0
    if (start_state_id+1< final_state_id)
    {

        //m_nNextStateID this should be the last id
        mNFATable[0]->ChangeStateID(final_state_id, final_state_id+2);
        mNFATable[0]->ChangeStateID(start_state_id+1, final_state_id);
        mNFATable[0]->ChangeStateID(final_state_id+2, start_state_id+1);

    }
#endif


    return mNFATable;

}
