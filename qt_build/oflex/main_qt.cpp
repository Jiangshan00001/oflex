#include <QCoreApplication>
#include <iostream>
#include <fstream>
#include "NFAState.h"
#include "ORegexParse.h"
#include "dot_generator.h"
#include "nfaconvert.h"
#include "lexfileparse.h"
#include "num2str.h"




int test_lex(std::string file_name)
{
    std::ifstream ifile;
    ifile.open(file_name, std::ifstream::in);
    ifile.seekg(0, ifile.end);
    int len = ifile.tellg();
    ifile.seekg(0, ifile.beg);
    char *buf = new char [len+2];
    memset(buf, 0 , len+2);

    ifile.read(buf, len);
    ifile.close();
    std::string file_cont;
    file_cont.assign(buf, buf+len);


    std::vector< std::map<std::string, std::string > > regex_rule;
    std::string add_code;
    std::string includes;
    lex_file_parse2(file_cont, regex_rule,includes, add_code);
}

int test_lex_str(std::string lex_str, int index)
{
    ORegexParse mRegex;
    NFAConvert mConvert;
    int i=index;

    int start_id = 0;
    FSA_TABLE nfa = mRegex.CreateNFAFlex(lex_str, start_id);
    std::cout<<i<<". "<<lex_str<<"\n";

    fsa_to_dot(nfa, num2str(i)+"nfa"+".dot");

    FSA_TABLE dfa = mConvert.NFAtoDFA(nfa);
    fsa_to_dot(dfa, num2str(i)+"dfa"+".dot");

    FSA_TABLE dfa_min = mConvert.DFAmin(dfa);
    fsa_to_dot(dfa_min, num2str(i)+"mindfa"+".dot");
}
int main(int argc, char *argv[])
{

    /**
    test_lex("../test/lex_add_code.ll");
    test_lex("../test/lex_head_skip_1.ll");
    test_lex("../test/lex_head_def.ll");

    test_lex("../test/lex_inc.ll");
    */
    /// todo: l->cpp code generate
    test_lex("../../test/quut_com_ansi_c.l");



    QCoreApplication a(argc, argv);

    //(0[xX])[a-fA-F0-9]+(((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))?
    test_lex_str("(0[xX])[a-fA-F0-9]+",0);

    ORegexParse mRegex;
    NFAConvert mConvert;

    int start_id = 0;

    std::map<std::string, std::string> lex = lex_file_parse("../../test/quut_com_ansi_c.l");
    for(auto it=lex.begin();it!=lex.end();++it)
    {
        std::cout<<it->first<<". "<< it->second<<"\n";
    }


    FSA_TABLE nfa = mRegex.CreateNFA("abcd");
    fsa_to_dot(nfa, "nfa1.dot");

    start_id = mRegex.m_nNextStateID;
    FSA_TABLE nfa2 = mRegex.CreateNFA("efg", start_id);
    //FSA_TABLE nfa = mRegex.CreateNFA("abc*");
    fsa_to_dot(nfa2, "nfa2.dot");

    FSA_TABLE dfa = mConvert.NFAtoDFA(nfa);
    fsa_to_dot(dfa, "dfa1.dot");

    FSA_TABLE dfa2 = mConvert.NFAtoDFA(nfa2);
    fsa_to_dot(dfa2, "dfa2.dot");


    FSA_TABLE dfa_min = mConvert.DFAmin(dfa);
    fsa_to_dot(dfa_min, "dfa_min.dot");






    return 0;



    return a.exec();
}

int main22(int argc, char *argv[])
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
