//raw;

#ifndef oflex_sample_H
#define oflex_sample_H
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include "token_header.h"

 

 class oflex_sample
 {
 public:
    typedef OToken token;
    int m_is_debug=1;

 public:
    int set_file_name(std::string file_name)
    {
        std::ifstream m_filein;
        std::stringstream ss;
        m_file_name=file_name;
        m_filein.open(m_file_name);
        ss<<m_filein.rdbuf();
        m_text_to_parse=ss.str();
        if(m_is_debug){std::cout<<"read file size:"<<m_text_to_parse.size()<<"\n";}
        m_text_index = 0;
        m_line=0;
        m_column=0;
        return 0;
    }

    /// get next token
    token yylex(){
        token tt;
        while(1)
        {
            int curr_char = input();
            if(m_is_debug){std::cout<<"==================\n";};
            if(m_is_debug){std::cout<<"----read one char:"<<curr_char<<"\n";}
            if(m_is_debug){
                std::cout<<"curr_status_stack:\n";
                for(auto i=0;i<m_status_stack.size();++i)
                {
                    std::cout<<i<<". " << m_status_stack[i]<<"\n";
                }
                std::cout<<"---\n";
            }
            int curr_stat=0;
            if(!m_status_stack.empty()){
                curr_stat= m_status_stack.back();
            }
            if(curr_char!=-1)
            {
                auto curr_lut = m_state_jmp_table[curr_stat];
                curr_stat = curr_lut[curr_char];
                m_status_stack.push_back(curr_stat);
            }
            if((curr_stat!=-1)&&(curr_char!=-1))
            {
                continue;
            }
            //找不到合适的了，找到最近的一个最终态
            int is_match=0;
            while(m_status_stack.size()>0)
            {

                int cc = m_status_stack.back();
                auto it = std::find(m_end_state_id.begin(), m_end_state_id.end(), cc);
                if(it!=m_end_state_id.end())
                {
                    ///状态找到
                    /// 此处返回给用户当前状态，并清空状态
                    ///
                    tt.m_yytext.assign(m_text_to_parse.begin()+m_text_index-m_status_stack.size(),
                                    m_text_to_parse.begin()+m_text_index);
                    int yylval;
                    int ret = state_call(cc, tt.m_yytext, &tt.m_yytext[0],yylval);
                    m_status_stack.clear();

                    if(ret!=NORETURN_ID)
                    {
                        tt.m_ret=ret;
                        tt.m_line=m_line;
                        tt.m_column=m_column;
                        tt.m_state_id = cc;
                        return tt;
                    }
                    else
                    {
                        is_match=1;
                        break;
                    }
                }
                else
                {///未匹配状态。如果有多个字符，则切换为之前的状态。如果只有1个字符，则出错，直接报错误，并恢复
                    if(m_status_stack.size()>1){
                        unput();
                        m_status_stack.pop_back();
                    }
                    else
                    {
                        std::string tmp_str;
                        tmp_str.assign(m_text_to_parse.begin()+m_text_index-m_status_stack.size(),
                                        m_text_to_parse.begin()+m_text_index);
                        m_status_stack.pop_back();
                        if(m_is_debug){std::cout<<"drop str:"<<tmp_str<<"\n";}
                    }
                }
            }
            if(is_eof())
            {
                tt.m_ret=EOF_20220422_EOF;
                tt.is_eof=1;
                return tt;
            }
            if(is_match==0)
            {
                ///没有任何匹配的字符
                ///
                std::cerr<<"unknown pattern LINE/COLUMN:"<<m_line<<"/"<<m_column<<". char:"<< (char)curr_char<<"("<<curr_char<< ")\n";
            }
        }

        return tt;
    }

    int unput()
    {
        m_text_index--;
        if(m_text_to_parse[m_text_index]=='\n')
        {
            m_line--;
            ///FIXME: 此处如果退回到上一行，应该是上一行的最大列，而不是0
            m_column=0;
        }
        else
        {
            m_column--;
        }
        return 0;
    }

    int peek(int i=0)
    {
        if(m_text_index+i>=m_text_to_parse.size())
        {
            return -1;
        }
        int ret = m_text_to_parse[m_text_index+i];
        return ret;
    }
    //get next char
    int input()
    {
        //if(m_is_debug)std::cerr<<"input:LCI:"<<m_line<<":"<<m_column<<":"<< m_text_index<<"\n";
        if(m_text_index>=m_text_to_parse.size())
        {
            return -1;
        }
        int ret = m_text_to_parse[m_text_index];
        //if(m_is_debug){std::cerr<<ret<<"\'"<< (char)ret<<"\':\n";}
        m_text_index++;
        if(ret=='\n')
        {
            m_line++;
            m_column=0;
        }
        else
        {
            m_column++;
        }
        return ret;
    }

 public:
    int is_eof(){
        if(m_text_index>=m_text_to_parse.size())
        {
            return 1;
        }
        return 0;
    }

    oflex_sample()
    {
        m_text_index=0;
    }

    std::string get_state_str(int state_id)
    {
        auto it = std::find(m_end_state_id.begin(), m_end_state_id.end(), state_id);
        if(it==m_end_state_id.end())
        {
            return "";
        }
        int index = it - m_end_state_id.begin();
        return m_end_state_regex[index];
    }
    void yyerror(const char* err)
    {
        std::cerr<<err<<"\n";

    }
public:
    int m_line;
    int m_column;
 private:
    std::string m_file_name;
    std::string m_text_to_parse;
    unsigned m_text_index;

    const int NORETURN_ID=-12345;

    int m_curr_status=0;
    std::vector<int> m_status_stack;


 private:
 #if 1//core code
 int m_state_cnt=0;
int m_regex_cnt=0;
std::vector<int> m_end_state_id={

};
int m_end_state_cnt=0;
std::vector<std::string> m_end_state_regex={
};
int m_end_state_diff_cnt=0;
int m_state_jmp_table[0][257]={
};
int state_call(int state_id, std::string curr_text, const char* yytext, int &yylval){
switch(state_id)
{
default:
break;

};
return NORETURN_ID;
}

 #endif
 #if 1//other code
 
 #endif
 };

 #endif



                       