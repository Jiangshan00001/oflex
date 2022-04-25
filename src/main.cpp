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
#include "flex_sample1.h"

/// 堆栈方法求值
/// http://blog.csdn.net/liuhuiyi/article/details/8433203

/// 当前表达式支持:
/// *
/// |
/// +
/// ()
/// abcd
///


std::string lex_file_out(std::string includes, std::string add_code,
                         std::vector< std::map<std::string, std::string > > regex_rule,
                         int is_debug)
{
    std::stringstream iss;

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
        if(is_debug)fsa_to_dot(nfa, num2str(i)+"nfa"+".dot");


        FSA_TABLE dfa = mConvert.NFAtoDFA(nfa,start_id);
        FSA_TABLE dfamin = mConvert.DFAmin(dfa, start_id);
        FSA_TABLE dfamin2;
        mConvert.ReNumber(dfamin,start_id,dfamin2);
        start_id = mConvert.m_nNextStateID+1;

        if(is_debug)fsa_to_dot(dfa, num2str(i)+"dfa"+".dot");
        NFAState* final_state = find_fsa_table_final_state(dfa);
        end_state_ids.push_back(final_state->m_nStateID);
        int curr_len = nfa_all.size();
        nfa_all.insert(nfa_all.end(), dfa.begin(), dfa.end());
        if (curr_len>0)
        {
            nfa_all[0]->AddTransition(EPS_CHAR, nfa_all[curr_len]);
        }
    }
    if(is_debug)fsa_to_dot(nfa_all, "nfa_all.dot");
    ///here need to implementation the high low poriority of the final state
    FSA_TABLE dfa2 = mConvert.NFAtoDFA(nfa_all,0);
    FSA_TABLE dfa;
    mConvert.ReNumber(dfa2,0,dfa);
    if(is_debug)fsa_to_dot(dfa, "dfa_all.dot");

    iss<<"int m_state_cnt="<<dfa.size()<<";\n";
    iss<<"int m_regex_cnt="<<regex_rule.size()<<";\n";

    iss<<"std::vector<int> m_end_state_id={\n";
    int final_state_cnt=0;
    for(int i=0;i<dfa.size();++i)
    {
        if(dfa[i]->m_bAcceptingState!=FINAL_STATE)continue;
        iss<<dfa[i]->m_nStateID <<",";
        final_state_cnt++;
    }
    iss<<"\n};\n";
    iss<<"int m_end_state_cnt="<<final_state_cnt<<";\n";

    std::set<std::string> final_state_str;
    iss<<"std::vector<std::string> m_end_state_regex={\n";
    for(int i=0;i<dfa.size();++i)
    {
        if(dfa[i]->m_bAcceptingState!=FINAL_STATE)continue;
        std::string str_pkd=string_pack(dfa[i]->m_accepting_regrex) ;
        iss<<"\""<<str_pkd <<"\"" <<",\n";
        final_state_str.insert(str_pkd);
    }
    iss<<"};\n";
    iss<<"int m_end_state_diff_cnt="<<final_state_str.size()<<";\n";


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



    iss<<"int m_state_jmp_table["<<m_jmp_table.size() <<"][257]={\n";
    for(unsigned i=0;i<m_jmp_table.size();++i)
    {
        iss<<"{";
        for(unsigned j=0;j<m_jmp_table[i].size();++j)
        {
            iss<<m_jmp_table[i][j]<<",";
        }
        iss<<"},\n";
    }
    iss<<"};\n";

    iss<<"int state_call(int state_id, std::string curr_text, const char* yytext, int &yylval){\n";
    iss<<"switch(state_id)\n";
    iss<<"{\n";

    for(unsigned j=0;j<regex_rule.size();++j)
    {
        int has_case =0;
        for(unsigned i=0;i<dfa.size();++i)
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
    iss<<"\n};\nreturn 0;\n}\n";

    return iss.str();
}


#ifndef QT_BUILD
int main(int argc, char *argv[])
#else
int main_qt(int argc, char *argv[])
#endif
/// -s abcd -d dfa.dot -o out.c -i input_lex.l
{
    ArgsParser parse(argc, argv);

    int is_debug = 0;

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
        std::cout<<"istr:"<<istr<<"\n";
        FSA_TABLE nfa = mRegex.CreateNFAFlex(istr);
        fsa_to_dot(nfa, out_dot_file_name+"nfa.dot");

        FSA_TABLE dfa = mConvert.NFAtoDFA(nfa);
        fsa_to_dot(dfa, out_dot_file_name+"dfa.dot");
        FSA_TABLE dfamin = mConvert.DFAmin(dfa);
        FSA_TABLE dfamin2;
        mConvert.ReNumber(dfamin, 0, dfamin2);
        fsa_to_dot(dfamin2, out_dot_file_name);
        return 0;
    }
    if(parse.HaveOption('i'))
    {
        std::string file_name = parse.GetOption('i');
        std::string file_out = "oflex_sample.h";
        std::string class_name = "flex_sample";
        std::string jmp_file_name="";
        if(parse.HaveOption('o'))
        {
            file_out = parse.GetOption('o');
        }
        if(parse.HaveOption('c'))
        {
            class_name = parse.GetOption('c');
        }
        if(parse.HaveOption('d'))
        {
            is_debug=1;
        }

        if(parse.HaveOption('d'))
        {
            jmp_file_name=parse.GetOption('j');
        }



        std::ifstream ifile;
        ifile.open(file_name, std::ifstream::in);

        std::stringstream file_cont_ss;

        file_cont_ss<<ifile.rdbuf();
        std::string file_cont;
        file_cont=file_cont_ss.str();
        ifile.close();
        if(is_debug)
        {
            std::cout<<"file_cont_size:"<<file_cont.size()<<"\n";
        }

        std::vector< std::map<std::string, std::string > > regex_rule;
        std::string add_code;
        std::string includes;
        lex_file_parse2(file_cont, regex_rule,includes, add_code,0);

        if(is_debug)
        {
            std::cout<<"rule_size:"<<regex_rule.size()<<"\n";
            for(int i=0;i<regex_rule.size();++i)
            {
                std::cout<<i<<". "<< regex_rule[i].begin()->first<<"--->"<< regex_rule[i].begin()->second<<"\n";
            }
        }


        std::string ret = lex_file_out(includes, add_code,regex_rule, is_debug);

        if(!jmp_file_name.empty())
        {
            std::ofstream ofile;
            ofile.open(jmp_file_name);
            ofile<<ret;
            ofile.close();
        }


        flex_sample1 sample1;
        ret = sample1.render(ret, includes, add_code,class_name);
        std::ofstream ofile;
        ofile.open(file_out);
        ofile<<ret;
        ofile.close();



        return 0;
    }

    std::cout<<"usage: prog -s string_regrex -d dfa_file_name.dot\n";
    std::cout<<"usage: prog -i lex.l -o output.h -c class_name\n";

    return 0;
}


