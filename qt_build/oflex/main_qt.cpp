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


std::string lex_file_out(std::string includes, std::string add_code, std::vector< std::map<std::string, std::string > > regex_rule)
{
    std::stringstream iss;

    iss<<"#if 1// start_code\n";
    iss<<includes;
    iss<<"\n#endif //start_code finish\n";

    int start_id = 0;
    ORegexParse mRegex;
    NFAConvert mConvert;
    FSA_TABLE nfa_all;
    std::vector<int> end_state_ids;
    for(int i=0;i<regex_rule.size();++i)
    {
        std::string lex_str = regex_rule[i].begin()->first;
        FSA_TABLE nfa = mRegex.CreateNFAFlex(lex_str, start_id);

        std::cout<<i<<". "<<lex_str<<"\n";

        fsa_to_dot(nfa, num2str(i)+"nfa"+".dot");

        FSA_TABLE dfa = mConvert.NFAtoDFA(nfa,start_id);
        start_id = mConvert.m_nNextStateID+1;

        fsa_to_dot(dfa, num2str(i)+"dfa"+".dot");
        NFAState* final_state = find_fsa_table_final_state(dfa);
        end_state_ids.push_back(final_state->m_nStateID);
        int curr_len = nfa_all.size();
        nfa_all.insert(nfa_all.end(), dfa.begin(), dfa.end());
        if (curr_len>0)
        {
            nfa_all[0]->AddTransition(EPS_CHAR, nfa_all[curr_len]);
        }
    }
    FSA_TABLE dfa = mConvert.NFAtoDFA(nfa_all,0);
    fsa_to_dot(dfa, "dfa.dot");

    iss<<"static int state_cnt="<<dfa.size()<<";\n";

    iss<<"static int end_state_id[]={\n";
    for(int i=0;i<dfa.size();++i)
    {
        if(dfa[i]->m_bAcceptingState!=FINAL_STATE)continue;
        iss<<dfa[i]->m_nStateID <<",";
    }
    iss<<"\n};\n";

    iss<<"static int end_state_regex[]={\n";
    for(int i=0;i<dfa.size();++i)
    {
        if(dfa[i]->m_bAcceptingState!=FINAL_STATE)continue;
        iss<<"\""<<string_pack(dfa[i]->m_accepting_regrex) <<"\"" <<",\n";
    }
    iss<<"};\n";


    std::vector< std::vector<int> > m_jmp_table;

    for(int i=0;i<dfa.size();++i)
    {
        while(m_jmp_table.size()<=dfa[i]->m_nStateID)
        {
            std::vector<int> one_jmp;
            for(int j=0;j<257;++j) one_jmp.push_back(-1);
            m_jmp_table.push_back(one_jmp);
        }

        for(auto it=dfa[i]->m_transition.begin();it!=dfa[i]->m_transition.end();++it)
        {
            int jmp_char = it->first;
            int jmp_to_id = it->second->m_nStateID;
            assert(jmp_char>=0);
            m_jmp_table[dfa[i]->m_nStateID][jmp_char] = jmp_to_id;
        }
        m_jmp_table[dfa[i]->m_nStateID][256] = dfa[i]->m_nStateID;
    }



    iss<<"static int state_jmp_table[][257]={\n";
    for(int i=0;i<m_jmp_table.size();++i)
    {
        iss<<"{";
        for(int j=0;j<m_jmp_table[i].size();++j)
        {
            iss<<m_jmp_table[i][j]<<",";
        }
        iss<<"},\n";
    }
    iss<<"};\n";

    iss<<"static int state_call(int state_id, std::string curr_text){\n";
    iss<<"switch(state_id)\n";
    iss<<"{\n";

    for(int j=0;j<regex_rule.size();++j)
    {
        int has_case =0;
        for(int i=0;i<dfa.size();++i)
        {
            if(dfa[i]->m_bAcceptingState!=FINAL_STATE)continue;
            if(dfa[i]->m_accepting_regrex==regex_rule[j].begin()->first)
            {
                has_case =1;
                iss<<"case "<<dfa[i]->m_nStateID<<":\n";
            }
        }
        if(has_case)
        {
            iss<<"{\n";
            iss<<regex_rule[j].begin()->second;
            iss<<"\nbreak;\n";
            iss<<"}\n";
        }
    }


    iss<<"default:\nbreak;\n";
    iss<<"\n};\n}\n";


    iss<<"#if 1// end_code\n";
    iss<<add_code;
    iss<<"\n#endif //end_code finish\n";
    return iss.str();
}

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

void test1()
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
