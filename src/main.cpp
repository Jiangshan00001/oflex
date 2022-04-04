#include "NFAState.h"
#include "ORegexParse.h"
#include "nfaconvert.h"
#include "fsa_to_dot.h"

/// 堆栈方法求值
/// http://blog.csdn.net/liuhuiyi/article/details/8433203

/// 当前表达式支持:
/// *
/// |
/// +
/// ()
/// abcd

#ifndef QT_BUILD
///
int main()
{
    ORegexParse mRegex;
    NFAConvert mConvert;

    FSA_TABLE nfa = mRegex.CreateNFA("(a|b)*(aa|bb)(a|b)*");
    //FSA_TABLE nfa = mRegex.CreateNFA("abc*");
    fsa_to_dot(nfa, "nfa1.dot");
    FSA_TABLE dfa = mConvert.NFAtoDFA(nfa);
    fsa_to_dot(dfa, "dfa1.dot");
    FSA_TABLE dfa_min = mConvert.DFAmin(dfa);
    fsa_to_dot(dfa_min, "dfa_min.dot");


    return 0;
}

#endif

