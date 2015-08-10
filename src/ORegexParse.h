#ifndef ORegexParse_h__
#define ORegexParse_h__
#include "NFAState.h"

//! NFA/DFA Table
typedef std::deque<NFAState*> FSA_TABLE;

class ORegexParse
{
private:
	int m_nNextStateID;	
	//! Operator Stack
	/*! Operators are simple characters like "*" & "|" */
	std::stack<char> m_OperatorStack;

	//! Operand Stack
	/*! If we use the Thompson's Algorithm then we realize
		that each operand is a NFA automata on its own!
	*/
	std::stack<FSA_TABLE> m_OperandStack;
		/*! NFA Table is stored in a deque of CAG_States.
		Each CAG_State object has a multimap of 
		transitions where the key is the input
		character and values are the references to
		states to which it transfers.
	*/
	FSA_TABLE m_NFATable;
	FSA_TABLE m_DFATable;
public:
	ORegexParse();


	int NFAToDFA();

	/// ��һ��������ʽת��ΪNFA
	/// �����õ�2���㷨��1��Ϊ��ջ������ֵ
	/// http://blog.csdn.net/liuhuiyi/article/details/8433203
	///  Algorithm Thompson's construction
	/// http://www.cppblog.com/woaidongmao/archive/2010/10/21/97245.html
	bool CreateNFA(std::string strRegEx);

private:
	///�������ַ�֮��ʡ�Ե����ӷ������
	std::string ConcatExpand(std::string strRegEx);
	

	int NFAStackPop(FSA_TABLE &NFATable);
	int NFAStackPush(FSA_TABLE &NFATable);

	///��һ���ַ���װ��Ϊһ��״̬�ϼ�������operand stack	
	int PushOneByte(char chInput);



	//! Evaluates the concatenation operator
	/*! This function pops two operands from the stack 
		and evaluates the concatenation on them, pushing
		the result back on the stack.
	*/
	bool Concat();

	//! Evaluates the Kleen's closure - star operator
	/*! Pops one operator from the stack and evaluates
		the star operator on it. It pushes the result
		on the operand stack again.
	*/
	bool Star();

	//! Evaluates the union operator
	/*! Pops 2 operands from the stack and evaluates
		the union operator pushing the result on the
		operand stack.
	*/
	bool Union();

//////////////////////////////////////////////////////////////////////////

	//! Checks is a specific character and operator
	bool IsOperator(char ch) { 
		return ((ch == OPERATOR_STAR) || (ch == OPERATOR_UNION) ||
			(ch == OPERATOR_LEFTP) || (ch == OPERATOR_RIGHTP) || (ch == OPERATOR_CONCAT)); };
	
	//! Checks if the specific character is input character
	bool IsInput(char ch) { return(!IsOperator(ch)); };
	
	/// ������
	//! Checks is a character left parantheses
	bool IsLeftParanthesis(char ch) { return(ch == OPERATOR_LEFTP); };
	
	/// ������
	//! Checks is a character right parantheses
	bool IsRightParanthesis(char ch) { return(ch == OPERATOR_RIGHTP); };


	//! Evaluates the next operator from the operator stack
	bool Eval();


	/// �鿴���ȼ������opLeft<=opRight �򷵻�1�����򷵻�0
	//! Returns operator presedence
	/*! Returns 1 if presedence of opLeft <= opRight.
	
			Kleens Closure	- highest ()
			Concatenation	- middle  ab
			Union			- lowest  |
	*/
	bool Presedence(char opLeft, char opRight)
	{
		if(opLeft == opRight)
			return 1;

		if(opLeft == OPERATOR_STAR)/// star���ȼ����
			return 0;

		if(opRight == OPERATOR_STAR)
			return 1;

		if(opLeft == OPERATOR_CONCAT)///���ȼ��е�
			return 0;

		if(opRight == OPERATOR_CONCAT)
			return 1;

		if(opLeft == OPERATOR_UNION)
			return 0;
		
		return 1;
	};

	/// mC=0ʱ��Ϊeps����ת
	std::set<NFAState*> MoveOne(char mC, std::set<NFAState*> mNFAs);
	//����ͨ��eps���໥����һ���
	std::set<NFAState*> MoveZero(std::set<NFAState*> mNFAs, std::set<NFAState*> &mRes1);

    /// ���mA��mDFAs���Ƿ����,������ڣ��򷵻��Ѵ��ڵ�ָ�룬���򷵻�0
    NFAState* GetExistState(NFAState* mA, FSA_TABLE &mDFAs);
};

#endif // ORegexParse_h__

