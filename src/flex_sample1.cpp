#include <replace.h>
#include "flex_sample1.h"


flex_sample1::flex_sample1()
{

}

std::string flex_sample1::render(std::string core_txt, std::string headers, std::string other_code)
{
    std::string temp=R"AAA(//raw;

#ifndef OFLEX_SAMPLE_H
#define OFLEX_SAMPLE_H
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>

TEMPLATE_HEADER_POSITION

class oflex_sample
{
public:
   class token
   {
   public:
       token()
       {
          m_ret= is_eof=m_line=m_column=0;
       }
       std::string m_yytext;
       int m_ret;
       int is_eof;
       int m_line;
       int m_column;

       int m_state_id;
       std::vector<token> m_children;
   };

public:
   int set_file_name(std::string file_name)
   {
       std::ifstream m_filein;
       std::stringstream ss;
       m_file_name=file_name;
       m_filein.open(m_file_name);
       ss<<m_filein.rdbuf();
       m_text_to_parse=ss.str();
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
               unput();
               m_status_stack.pop_back();
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
               std::cerr<<"unknown pattern:"<<m_line<<":"<<m_column<<"\n";
           }
       }


//        if(curr_lut[curr_char]!=-1)
//        {
//            std::cerr<<"char not known\n";
//            return tt;
//        }


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
   //get next char
   int input()
   {
       if(m_text_index>=m_text_to_parse.size())
       {
           return -1;
       }
       int ret = m_text_to_parse[m_text_index];
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

private:
   std::string m_file_name;
   std::string m_text_to_parse;
   unsigned m_text_index;
   int m_line;
   int m_column;
   const int NORETURN_ID=-12345;

   int m_curr_status=0;
   std::vector<int> m_status_stack;


private:
#if 1//core code
TEMPLATE_CORE_POSITION
#endif
#if 1//other code
TEMPLATE_OTHER_CODE_POSITION
#endf
};

#endif


                       )AAA";


    replace(temp, "TEMPLATE_HEADER_POSITION", headers);
    replace(temp, "TEMPLATE_CORE_POSITION", core_txt);
    replace(temp, "TEMPLATE_OTHER_CODE_POSITION", other_code);

    return temp;
}
