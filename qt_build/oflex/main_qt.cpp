#include <QCoreApplication>
#include <iostream>
#include <fstream>
#include "NFAState.h"
#include "ORegexParse.h"
#include "fsa_to_dot.h"
#include "nfaconvert.h"
#include "lexfileparse.h"
#include "num2str.h"
#include "string_eval.h"
#include "mytests.h"
#include "trim.h"

std::string lex_file_out(std::string includes, std::string add_code, std::vector< std::map<std::string, std::string > > regex_rule, int is_debug);

int main(int argc, char *argv[])
{


    //std::string file_name = "../../test/quut_com_ansi_c.l";
    std::string file_name = "../../test/calc.l";
    std::string file_out = "default.inc";

    std::ifstream ifile;
    ifile.open(file_name, std::ifstream::in);

    std::stringstream file_cont_ss;

    file_cont_ss<<ifile.rdbuf();
    std::string file_cont;
    file_cont=file_cont_ss.str();
    ifile.close();
    std::cout<<"file_cont_size:"<<file_cont.size()<<"\n";

    std::vector< std::map<std::string, std::string > > regex_rule;
    std::string add_code;
    std::string includes;
    lex_file_parse2(file_cont, regex_rule,includes, add_code);

    std::cout<<"rule_size:"<<regex_rule.size()<<"\n";
    for(int i=0;i<regex_rule.size();++i)
    {
        std::cout<<i<<". "<< regex_rule[i].begin()->first<<"--->"<< regex_rule[i].begin()->second<<"\n";
    }


    std::string ret = lex_file_out(includes, add_code,regex_rule,0);

    std::ofstream ofile;
    ofile.open(file_out);
    ofile<<ret;
    ofile.close();
    return 0;
    return 0;
}

int mainsss(int argc, char *argv[])
{
    //string parse
    ORegexParse mRegex;
    NFAConvert mConvert;


    //FSA_TABLE nfa = mRegex.CreateNFAFlex("[1-4]+");
    //FSA_TABLE nfa = mRegex.CreateNFAFlex("[^1-9a-zA-Z]+");
    //FSA_TABLE nfa = mRegex.CreateNFAFlex("((1|2)|3)+");
    //FSA_TABLE nfa = mRegex.CreateNFAFlex("[0-1]*\".\"");
    //FSA_TABLE nfa = mRegex.CreateNFAFlex("\"/*\"");
    //FSA_TABLE nfa = mRegex.CreateNFAFlex(".");
    //FSA_TABLE nfa = mRegex.CreateNFAFlex("\"//\"[^\n]*");
    FSA_TABLE nfa = mRegex.CreateNFAFlex("[a-bA-B_]([a-bA-B_]|[0-1])*");
    fsa_to_dot(nfa, "nfa1.dot");
    FSA_TABLE dfa = mConvert.NFAtoDFA(nfa);
    fsa_to_dot(dfa, "dfa1.dot");
    FSA_TABLE dfamin = mConvert.DFAmin(dfa);
    FSA_TABLE newDFA;
    mConvert.ReNumber(dfamin, 0, newDFA);
    fsa_to_dot(newDFA, "dfa_min1.dot");
    std::cout<<fsa_to_dot_ss(newDFA);
    return 0;
}

#if 0
int test_lex(std::string file_name, std::string file_out)
{
    std::ifstream ifile;
    ifile.open(file_name, std::ifstream::in);

    std::stringstream file_cont_ss;

    file_cont_ss<<ifile.rdbuf();
    std::string file_cont;
    file_cont=file_cont_ss.str();


    std::vector< std::map<std::string, std::string > > regex_rule;
    std::string add_code;
    std::string includes;
    lex_file_parse2(file_cont, regex_rule,includes, add_code);

    for(int i=0;i<regex_rule.size();++i)
    {
        std::cout<<i<<". "<< regex_rule[i].begin()->first<<"--->"<< regex_rule[i].begin()->second<<"\n";
    }


    std::string ret = lex_file_out(includes, add_code,regex_rule);

    std::ofstream ofile;
    ofile.open(file_out);
    ofile<<ret;
    ofile.close();
    return 0;

}

int test_lex_str(std::string lex_str, int index)
{
    ORegexParse mRegex;
    NFAConvert mConvert;
    int i=index;

    int start_id = 0;
    FSA_TABLE nfa = mRegex.CreateNFAFlex(lex_str, start_id);
    std::cout<<i<<". "<<lex_str<<"\n";

    //fsa_to_dot(nfa, num2str(i)+"nfa"+".dot");

    FSA_TABLE dfa = mConvert.NFAtoDFA(nfa,0);
    //fsa_to_dot(dfa, num2str(i)+"dfa"+".dot");

    FSA_TABLE dfa_min = mConvert.DFAmin(dfa);
    //fsa_to_dot(dfa_min, num2str(i)+"mindfa"+".dot");
}

void test1111()
{
    ORegexParse mRegex;
    NFAConvert mConvert;
    FSA_TABLE nfa = mRegex.CreateNFAFlex("\"abcd\"", 0);
    fsa_to_dot(nfa, "test1_nfa1.dot");
    FSA_TABLE dfa = mConvert.NFAtoDFA(nfa, 0);
    fsa_to_dot(dfa, "test1_dfa1.dot");
}

int main(int argc, char *argv[])
{

    //test1();
    //test_lex("../../test/test1.l", "test1_out.c");
    //test_lex("../../test/test2.l", "test2_out.c");
    test_lex("../../test/test3.l", "test3_out.c");


    //test_lex("../../test/quut_com_ansi_c.l", "test_ansi_c_out.c");

    //test1();
    return 0;

    /**
    test_lex("../test/lex_add_code.ll");
    test_lex("../test/lex_head_skip_1.ll");
    test_lex("../test/lex_head_def.ll");

    test_lex("../test/lex_inc.ll");
    */
    /// todo: l->cpp code generate



    return 0;
#if 0
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





#endif
    return 0;



    //return a.exec();
}

int main22(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    ORegexParse mRegex;
    NFAConvert mConvert;

    FSA_TABLE nfa = mRegex.CreateNFA("(a|b)*(aa|bb)(a|b)*");
    //FSA_TABLE nfa = mRegex.CreateNFA("abc*");
    fsa_to_dot(nfa, "nfa1.dot");
    FSA_TABLE dfa = mConvert.NFAtoDFA(nfa,0);
    fsa_to_dot(dfa, "dfa1.dot");
    FSA_TABLE dfa_min = mConvert.DFAmin(dfa);
    fsa_to_dot(dfa_min, "dfa_min.dot");






    return 0;



    return a.exec();
}
#endif

