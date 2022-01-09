#include "NFAState.h"
#include "ORegexParse.h"
#include "nfaconvert.h"
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

    //FSA_TABLE nfa = mRegex.CreateNFA("abca*");
    FSA_TABLE nfa = mRegex.CreateNFA("(a|b)*(aa|bb)*");
    FSA_TABLE dfa = mConvert.NFAtoDFA(nfa);
    fsa_to_dot(nfa, "nfa1.dot");
    fsa_to_dot(dfa, "dfa1.dot");




    return 0;
}

#endif

