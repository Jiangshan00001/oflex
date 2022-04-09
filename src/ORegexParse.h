#ifndef ORegexParse_h__
#define ORegexParse_h__
#include "NFAState.h"



class operator_stack_t
{
public:
    operator_stack_t(){operand_num=0;val=0;}
    operator_stack_t(int opd, char v){operand_num=opd;val=v;}
    int operand_num;
    char val;
};


//! NFA/DFA Table


class ORegexParse
{
public:
	int m_nNextStateID;	
	//! Operator Stack
	/*! Operators are simple characters like "*" & "|" */
    //std::stack<char> m_OperatorStack;

	//! Operand Stack
	/*! If we use the Thompson's Algorithm then we realize
		that each operand is a NFA automata on its own!
	*/
    //FSA_STACK m_OperandStack;
		/*! NFA Table is stored in a deque of CAG_States.
		Each CAG_State object has a multimap of 
		transitions where the key is the input
		character and values are the references to
		states to which it transfers.
	*/
    //FSA_TABLE m_NFATable;
    //FSA_TABLE m_DFATable;
public:
	ORegexParse();


	/// 将一个正则表达式转换为NFA
	/// 其中用到2个算法：1个为堆栈方法求值
	/// http://blog.csdn.net/liuhuiyi/article/details/8433203
	///  Algorithm Thompson's construction
	/// http://www.cppblog.com/woaidongmao/archive/2010/10/21/97245.html
    ///FSA_TABLE CreateNFA(std::string strRegEx, int startId=0);

    /// flex regrex expression to nfa
    /// first state is start state(sateID=startId)
    /// second state is end state(sateID=startId+1)
    /// other state is middle state
    FSA_TABLE CreateNFAFlex(std::string strRegEx, int startId=0);


    void add_concat_if_need(FSA_STACK &dst, std::stack<operator_stack_t> &operator_stack);
private:
	///将两个字符之间省略的连接符添加上
	std::string ConcatExpand(std::string strRegEx);
	

    int NFAStackPop(FSA_STACK &stk, FSA_TABLE &NFATable);
    int NFAStackPush(FSA_STACK &stk, FSA_TABLE &NFATable);

	///将一个字符，装换为一个状态合集，放入operand stack	
    int PushOneByte(int chInput, FSA_STACK &dst, std::stack<operator_stack_t> &operator_stack);
    int PushDotByte(FSA_STACK &dst, std::stack<operator_stack_t> &operator_stack);

    int PushMiddleLR(std::vector<int> chars, int is_inv, FSA_STACK &dst, std::stack<operator_stack_t> &operator_stack);


	//! Evaluates the concatenation operator
	/*! This function pops two operands from the stack 
		and evaluates the concatenation on them, pushing
		the result back on the stack.
	*/
    bool Concat(FSA_STACK &stk);

	//! Evaluates the Kleen's closure - star operator
	/*! Pops one operator from the stack and evaluates
		the star operator on it. It pushes the result
		on the operand stack again.
	*/
    bool Star(FSA_STACK &stk);

	//! Evaluates the union operator
	/*! Pops 2 operands from the stack and evaluates
		the union operator pushing the result on the
		operand stack.
	*/
    bool Union(FSA_STACK &stk);
    /// L? zero or one L character
    bool OPRWhy(FSA_STACK &stk);
    /// L+ one or more character
    bool OPRPlus(FSA_STACK &stk);
//////////////////////////////////////////////////////////////////////////

	//! Checks is a specific character and operator
	bool IsOperator(char ch) { 
        //(ch==OPERATOR_DOT)||
		return ((ch == OPERATOR_STAR) || (ch == OPERATOR_UNION) ||
            (ch == OPERATOR_LEFTP) || (ch == OPERATOR_RIGHTP) ||
                (ch == OPERATOR_CONCAT)||(ch==OPERATOR_DQUOTE)||
                (ch==OPERATOR_LEFTMID)||(ch==OPERATOR_RIGHTMID)||(ch==OPERATOR_PLUS)||
                (ch==OPERATOR_WHY)||(ch==OPERATOR_NOT)  ); };
	
	//! Checks if the specific character is input character
	bool IsInput(char ch) { return(!IsOperator(ch)); };
	
	/// 左括号
	//! Checks is a character left parantheses
	bool IsLeftParanthesis(char ch) { return(ch == OPERATOR_LEFTP); };
	
	/// 右括号
	//! Checks is a character right parantheses
	bool IsRightParanthesis(char ch) { return(ch == OPERATOR_RIGHTP); };


	//! Evaluates the next operator from the operator stack
    bool Eval(FSA_STACK &OperandStack, std::stack<operator_stack_t> &OperatorStack);


	/// 查看优先级，如果opLeft<=opRight 则返回1，否则返回0
	//! Returns operator presedence
	/*! Returns 1 if presedence of opLeft <= opRight.
     *  Returns 1 if have to calc prev op first.
     *opLeft=curr op
     *opRight=prev op
	
			Kleens Closure	- highest ()
            * ? +
			Concatenation	- middle  ab
			Union			- lowest  |

             foo|bar*  -->(foo)|(ba(r*))
	*/
	bool Presedence(char opLeft, char opRight)
	{
        int op_list[]={OPERATOR_RIGHTP, OPERATOR_STAR, OPERATOR_WHY,OPERATOR_PLUS, OPERATOR_CONCAT,  OPERATOR_UNION};//OPERATOR_LEFTP

        if ((opLeft==OPERATOR_RIGHTP)&&(opRight==OPERATOR_LEFTP))return 1;
        if (opRight==OPERATOR_LEFTP)return 0;
        if (opLeft==OPERATOR_LEFTP)return 0;

        if(opLeft == opRight)
            return 1;

        if((opLeft == OPERATOR_STAR)||(opLeft == OPERATOR_WHY)||(opLeft == OPERATOR_PLUS))/// star优先级最高
			return 0;

        if((opRight == OPERATOR_STAR)||(opRight == OPERATOR_WHY)||(opRight == OPERATOR_PLUS))
			return 1;

		if(opLeft == OPERATOR_CONCAT)///优先级中等
			return 0;

		if(opRight == OPERATOR_CONCAT)
			return 1;

		if(opLeft == OPERATOR_UNION)
			return 0;

        if(opRight==OPERATOR_UNION)return 0;
		
		return 1;
    }

    int PushOneDot(FSA_STACK &dst);
};

#endif // ORegexParse_h__

