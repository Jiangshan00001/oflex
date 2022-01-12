#ifndef NFAState_h__
#define NFAState_h__
#include <assert.h>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <vector>
#include <string>
#include <sstream>
///42---'*'
///124--'|'
///40---'('
///41---')'
///8-----'(.)'

#define TRACE  
#define ASSERT assert


#define  OPERATOR_STAR '*'
#define  OPERATOR_UNION '|'
#define  OPERATOR_LEFTP '('
#define  OPERATOR_RIGHTP ')'
#define  OPERATOR_CONCAT 8



/// 代表NFA/DFA里的一个状态。
//! State Class
class NFAState
{
protected:
	//! Transitions from this state to other 
	std::multimap<char, NFAState*> m_Transition;
	
	///当前转换包含的字符集合
	std::set<char> m_TransChar;
    /// State ID
	int m_nStateID;

    /// 由多个状态合并为1个状态时，此处用于记录合并前的状态
    /// Set of NFA state from which this state is constructed
	std::set<NFAState*> m_NFAStates;

public:
	int m_MarkFlag;

    //! 0--middle state. 1--start state. 2--final state
    int m_bAcceptingState;
    /// accepting state. used only if m_bAcceptingState is 2
    std::set<int> m_AcceptingState;
    
    
public:

    /// parameterized constructor
    /// 传入一个参数，代表状态id
	NFAState(int nID) : m_nStateID(nID), m_bAcceptingState(0) {};

	//! Constructs new state from the set of other states
	/*! This is necessary for subset construction algorithm
		because there a new DFA state is constructed from 
        one or more NFA states.
        mNFAState:对于由多个状态合并为一个状态时，用于记录之前的状态集合，例如子集构造

	*/
	NFAState(std::set<NFAState*> mNFAState, int nID)
	{
		m_NFAStates			= mNFAState;
		m_nStateID			= nID;
        updateAcceptingState();

    }
    int updateAcceptingState()
    {
        // DFA state is accepting state if it is constructed from
        // an accepting NFA state
        m_bAcceptingState	= 0;
        std::set<NFAState*>::iterator iter;
        for(iter=m_NFAStates.begin(); iter!=m_NFAStates.end(); ++iter)
            if((*iter)->m_bAcceptingState!=m_bAcceptingState)
                m_bAcceptingState |= (*iter)->m_bAcceptingState;

        return 0;
    }



	//! Copy Constructor
	NFAState(const NFAState &other)
	{ *this = other; };

	//! Destructor
	virtual ~NFAState() {};



    void ClearTransition()
    {
        m_TransChar.clear();
        m_Transition.clear();
    }


	//! Adds a transition from this state to the other
	void AddTransition(char chInput, NFAState *pState)
	{
		assert(pState != NULL);
		m_TransChar.insert(chInput);
		m_Transition.insert(std::make_pair(chInput, pState));
    }


	std::set<char> GetTransChar(){return m_TransChar;}


	//! Returns all transitions from this state on specific input
	void GetTransition(char chInput, std::set<NFAState*> &States)
	{
		// clear the array first
		States.clear();

		// Iterate through all values with the key chInput
		std::multimap<char, NFAState*>::iterator iter;
		for(iter = m_Transition.lower_bound(chInput);
			iter!= m_Transition.upper_bound(chInput);
			++iter)
			{
				NFAState *pState = iter->second;
				ASSERT(pState != NULL);
				//States.push_back(pState);
				States.insert(pState);
			}
    }
	std::multimap<char, NFAState*>* GetTransition()
	{
		return &m_Transition;
    }


	//! Returns the state id in form of string
	std::string GetStateID()
	{
		std::stringstream mA;
		mA<<m_nStateID;
		return mA.str();
    }

	/*! Returns the set of NFA states from 
		which this DFA state was constructed
	*/
	std::set<NFAState*>& GetNFAState()
	{ return m_NFAStates; };

	//! Returns 1 if this state is dead end
	/*! By dead end I mean that this state is not
		accepting state and there are no transitions
		leading away from this state. This function
		is used for reducing the DFA.
	*/
    bool IsDeadEnd(int loop_max=0)
	{
		if(m_bAcceptingState)
        {
            ///初始态或终止态
			return 0;
        }
		if(m_Transition.empty())
        {
			return 1;
        }

        if (loop_max>1000)
        {
            ///循环太多，找不到是不是有终点，认为不是dead
            return 0;
        }
		
		std::multimap<char, NFAState*>::iterator iter;
		for(iter=m_Transition.begin(); iter!=m_Transition.end(); ++iter)
		{
			NFAState *toState = iter->second;
			if(toState != this)
            {
                if (!toState->IsDeadEnd(loop_max+1))
                    return 0;
            }
		}

		TRACE("State %d is dead end.\n", m_nStateID); 
		
		return 1;
    }

	//! Override the assignment operator
	NFAState& operator=(const NFAState& other)
	{ 
		m_Transition	= other.m_Transition; 
		m_nStateID		= other.m_nStateID;
		m_NFAStates		= other.m_NFAStates;

		m_MarkFlag = other.m_MarkFlag;
        return *this;
    }

	//! Override the comparison operator
	bool operator==(const NFAState& other)
	{
		if(m_NFAStates.empty())
			return(m_nStateID == other.m_nStateID);
		else return(m_NFAStates == other.m_NFAStates);
    }
};

typedef std::deque<NFAState*> FSA_TABLE;
typedef std::stack<FSA_TABLE> FSA_STACK;


#endif // NFAState_h__

