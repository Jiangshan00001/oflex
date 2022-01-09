#include <QCoreApplication>
#include "NFAState.h"
#include "ORegexParse.h"
#include "dot_generator.h"
#include "nfaconvert.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


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



    return a.exec();
}
