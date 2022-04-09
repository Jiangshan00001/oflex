#ifndef NFAState_h__
#define NFAState_h__
#include <assert.h>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
///42---'*'
///124--'|'
///40---'('
///41---')'
///8-----'(.)'

#define TRACE  
#define ASSERT assert


/// | or
///
/// [] one of char
/// - between. a-z between a to z. include a and z.
/// * zero or more
/// ? 0 or 1
/// | 1 or more
/// ^ except
///
///
/// \ seq
/// . all char except \n. equal to ^\n



#define  OPERATOR_STAR '*'
#define  OPERATOR_UNION '|'
#define  OPERATOR_LEFTP '('
#define  OPERATOR_RIGHTP ')'
#define  OPERATOR_CONCAT 8  //EPS concat
#define OPERATOR_DQUOTE '\"'
//#define OPERATOR_DOT '.'
#define OPERATOR_LEFTMID '['
#define OPERATOR_RIGHTMID ']'
#define OPERATOR_PLUS '+' //1个或多个重复
#define OPERATOR_WHY '?'  //0个或1个
#define OPERATOR_NOT '^'

#define MIDDLE_STATE 0
#define START_STATE 1
#define FINAL_STATE 2


#define EPS_CHAR (-1)

bool cmp_state_id(const class NFAState* a, const class NFAState* b);


/// 代表NFA/DFA里的一个状态。
//! State Class
class NFAState
{

protected:
public:
	//! Transitions from this state to other 
    std::multimap<int, NFAState*> m_transition;
	
    ///curr chars to trans
    std::set<int> m_trans_char;
    /// State ID
	int m_nStateID;

    /// Set of NFA state from which this state is constructed
    std::set<NFAState*> m_nfa_states;

public:
    int m_mark_flag;

    //! 0--middle state. 1--start state. 2--final state
    int m_bAcceptingState;
    /// accepting state. used only if m_bAcceptingState is 2
    std::set<int> m_AcceptingState;
    //used only when m_AcceptingState==FINAL_STATE.
    std::string m_accepting_regrex;
    
    
public:

    /// parameterized constructor
    /// 传入一个参数，代表状态id
    NFAState(int nID) : m_nStateID(nID), m_bAcceptingState(0) {m_mark_flag = 0;};

	//! Constructs new state from the set of other states
	/*! This is necessary for subset construction algorithm
		because there a new DFA state is constructed from 
        one or more NFA states.
        mNFAState:对于由多个状态合并为一个状态时，用于记录之前的状态集合，例如子集构造

	*/
	NFAState(std::set<NFAState*> mNFAState, int nID)
	{
        m_nfa_states			= mNFAState;
		m_nStateID			= nID;
        m_mark_flag = 0;
        updateAcceptingState();

    }
    int updateAcceptingState()
    {
        // DFA state is accepting state if it is constructed from
        // an accepting NFA state
        m_bAcceptingState	= MIDDLE_STATE;
        std::vector<NFAState*> nfa_state_v;
        nfa_state_v.assign(m_nfa_states.begin(), m_nfa_states.end());
        std::sort(nfa_state_v.begin(), nfa_state_v.end(), cmp_state_id);
        //printf("updateAcceptingState\n");
        for(auto iter=nfa_state_v.begin(); iter!=nfa_state_v.end(); ++iter)
        {
            //printf("updateAcceptingState: %d\n", (*iter)->m_nStateID);
            if((*iter)->m_bAcceptingState==FINAL_STATE)
            {
                   m_bAcceptingState =FINAL_STATE;
                   m_accepting_regrex = (*iter)->m_accepting_regrex;
                   break;
            }
            else if((*iter)->m_bAcceptingState==START_STATE)
            {
                m_bAcceptingState =START_STATE;
                //m_accepting_regrex = (*iter)->m_accepting_regrex;
                break;
            }
        }
        return 0;
    }


    int ChangeStateID(int oldId,int newId, int depth=0)
    {
        if(this->m_nStateID==oldId)
        {
            this->m_nStateID=newId;
        }
        if(depth>1)return 0;
        for(auto it=m_transition.begin();it!=m_transition.end();++it)
        {
            it->second->ChangeStateID(oldId, newId,depth+1);
        }

        return 0;
    }



	//! Copy Constructor
	NFAState(const NFAState &other)
    { *this = other; }

	//! Destructor
    virtual ~NFAState() {}



    void ClearTransition()
    {
        m_trans_char.clear();
        m_transition.clear();
    }


	//! Adds a transition from this state to the other
    void AddTransition(int chInput, NFAState *pState)
	{
		assert(pState != NULL);
        m_trans_char.insert(chInput);
        m_transition.insert(std::make_pair(chInput, pState));
    }


    std::set<int> GetTransChar(){return m_trans_char;}


	//! Returns all transitions from this state on specific input
    void GetTransition(int chInput, std::set<NFAState*> &States)
	{
		// clear the array first
		States.clear();

		// Iterate through all values with the key chInput
        std::multimap<int, NFAState*>::iterator iter;
        for(iter = m_transition.lower_bound(chInput);
            iter!= m_transition.upper_bound(chInput);
			++iter)
			{
				NFAState *pState = iter->second;
				ASSERT(pState != NULL);
				//States.push_back(pState);
				States.insert(pState);
			}
    }
    std::multimap<int, NFAState*>* GetTransition()
	{
        return &m_transition;
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
    { return m_nfa_states; };

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
        if(m_transition.empty())
        {
			return 1;
        }

        if (loop_max>1000)
        {
            ///循环太多，找不到是不是有终点，认为不是dead
            return 0;
        }
		
        std::multimap<int, NFAState*>::iterator iter;
        for(iter=m_transition.begin(); iter!=m_transition.end(); ++iter)
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
        m_transition	= other.m_transition;
		m_nStateID		= other.m_nStateID;
        m_nfa_states		= other.m_nfa_states;

        m_mark_flag = other.m_mark_flag;
        return *this;
    }

	//! Override the comparison operator
	bool operator==(const NFAState& other)
	{
        if(m_nfa_states.empty())
			return(m_nStateID == other.m_nStateID);
        else return(m_nfa_states == other.m_nfa_states);
    }
};

typedef std::deque<NFAState*> FSA_TABLE;
typedef std::stack<FSA_TABLE> FSA_STACK;
NFAState* find_fsa_table_final_state(const FSA_TABLE &fsa);
NFAState* find_fsa_table_start_state(const FSA_TABLE &fsa);



#endif // NFAState_h__

