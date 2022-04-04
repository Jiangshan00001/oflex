#include <iostream>
#include <fstream>


#include "NFAState.h"
#include "ORegexParse.h"
#include "nfaconvert.h"
#include "fsa_to_dot.h"
#include "argv.h"
#include "fsa_to_dot.h"
#include "nfaconvert.h"
#include "lexfileparse.h"
#include "num2str.h"
#include "string_eval.h"


/// 堆栈方法求值
/// http://blog.csdn.net/liuhuiyi/article/details/8433203

/// 当前表达式支持:
/// *
/// |
/// +
/// ()
/// abcd
///


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


#if 1
/// -s abcd -d dfa.dot -o out.c -i input_lex.l
int main(int argc, char *argv[])
{
    ArgsParser parse(argc, argv);



    if(parse.HaveOption('s'))
    {
        std::string istr = parse.GetOption('s');
        std::string out_dot_file_name = "default.dot";
        if(parse.HaveOption('d'))
        {
            out_dot_file_name = parse.GetOption('d');
        }

        //string parse
        ORegexParse mRegex;
        NFAConvert mConvert;

        FSA_TABLE nfa = mRegex.CreateNFAFlex(istr);
        FSA_TABLE dfa = mConvert.NFAtoDFA(nfa);
        fsa_to_dot(dfa, out_dot_file_name);
        return 0;
    }
    if(parse.HaveOption('i'))
    {
        std::ifstream ifile;
        std::string file_name = parse.GetOption('i');
        std::string file_out = "default.c";
        if(parse.HaveOption('o'))
        {
            file_out = parse.GetOption('o');
        }

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

    std::cout<<"usage: prog -s string_regrex -d dfa_file_name.dot\n";
    std::cout<<"usage: prog -i lex.l -o output.c/cpp\n";

    return 0;
}

#endif

