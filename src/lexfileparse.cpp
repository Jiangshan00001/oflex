#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "split.h"
#include "trim.h"
#include "replace.h"

#include "lexfileparse.h"


using namespace std;


int skip_pair(const std::string& line, int start_index, unsigned start_char, unsigned end_char, int depth=0)
{
    int i = start_index;

    if(line[i]==start_char)
    {
        depth++;
        ++i;
        while((line[i]!=0)&&(line[i]!=end_char))
        {
            if (line[i]=='\\')
            {
                ++i;
            }
            ++i;
            //if(depth==0)break;
        }
        depth--;
        return i+1;
    }
    return i;
}




int split_lex_def_line(std::string one_line, std::string& key, std::string& value)
{
    for(int i=0;i<one_line.size();++i)
    {
        i = skip_pair(one_line, i, '"','"');
        i = skip_pair(one_line, i, '[',']');
        i = skip_pair(one_line, i, '{','}');
        i = skip_pair(one_line, i, '(',')');
        if((one_line[i]==' ')|| (one_line[i]=='\t'))
        {
            //split here
            key.assign(one_line.begin(), one_line.begin()+i);
            value.assign(one_line.begin()+i, one_line.end());
            key=trim(key);
            value = trim(value);
            return 0;
        }
    }
    key.assign(one_line.begin(), one_line.end());
    value="";
    return 0;
}

string replace_define(string key, map<string, string> def_token)
{
    for(auto i=def_token.begin();i!=def_token.end();++i)
    {
        string rep_str = "{"+i->first+"}";
        replace(key, rep_str, i->second);
    }

    return key;
}

///
///
map<string, string> lex_file_parse(const std::string &file_cont)
{
    ///
    ///
    ///
    ///
    ///
    map<string, string> def_token;
    string start_code;
    string end_code;
    map<string, string> lex_main;

    ifstream infile;
    infile.open(file_cont);

    if (!infile.is_open())
    {
        return lex_main;
    }

    int curr_status=0;//0--def-token. 1--start-code. 2--end-code 3--lex-main 4--end_code


    string one_line;
    int line_cnt=0;
    while(std::getline(infile, one_line))
    {
        line_cnt++;
        one_line = trim(one_line);
        if (one_line.size()==0)
        {
            //empty line. skip
            continue;
        }

        if (one_line=="%{")
        {
            //start code key
            curr_status=1;
            continue;
        }
        if (one_line=="%}")
        {
            //end code key
            curr_status=2;
            continue;
        }

        if (one_line=="%%")
        {
            //end code key
            curr_status+=1;
            continue;
        }


        if(curr_status==0)
        {
            //0--def-token
            std::vector<string> def_one = split(one_line,"\t", 1);
            if (def_one.size()==1)
            {
                def_one = split(one_line," ", 1);
            }
            if (def_one.size()==2)
            {
                def_token[def_one[0]]=def_one[1];
                cout<<"debug:"<<def_one[0]<<". v="<<def_one[1]<<"\n";
            }
            else
            {
                cout<<"unknown line:"<<line_cnt<<"."<< one_line<<"\n";
            }
        }
        else if(curr_status==1)
        {
            //1--start-code
            start_code=start_code+one_line+"\n";
        }
        else if(curr_status==2)
        {//2--end-code
            cout<<"unknown key:"<<line_cnt<<". "<< one_line<<"\n";
        }
        else if(curr_status==3)
        {//3--lex-main
            string key;
            string value;
            split_lex_def_line(one_line, key,value);
            key = replace_define(key, def_token);
            lex_main[key]=value;
        }
    }










    infile.close();



    return lex_main;
}


unsigned skip_to_blank(const std::string &file_cont, unsigned ipos)
{
    if (file_cont.size()<=(ipos+1))return ipos;
    while((file_cont[ipos]!=' ')&&(file_cont[ipos]!='\t')&&(file_cont[ipos]!='\n')&&(file_cont[ipos]!='\r'))
    {
        unsigned int start=ipos;
        ipos = skip_pair(file_cont, ipos, '"','"');
        ipos = skip_pair(file_cont, ipos, '[',']');
        ipos = skip_pair(file_cont, ipos, '{','}');
        ipos = skip_pair(file_cont, ipos, '(',')');
        ipos = skip_pair(file_cont, ipos, '\'','\'');

        if (file_cont[ipos]=='\\')
        {
            ipos+=2;
        }
        if(start==ipos)
        {
            ipos++;
        }


        if ((ipos+1)==file_cont.size())break;
    }
    return ipos;
}
unsigned skip_to_nblank(const std::string &file_cont, unsigned ipos)
{
    if (file_cont.size()<=(ipos+1))return ipos;
    while((file_cont[ipos]==' ')||(file_cont[ipos]=='\t')||(file_cont[ipos]=='\n')||(file_cont[ipos]==255)||(file_cont[ipos]=='\r'))
    {
        ipos++;
        if ((ipos+1)==file_cont.size())break;
    }
    return ipos;
}

unsigned find_block_end(const std::string &file_cont, unsigned ipos)
{
    ///{xxx {} }
    ///
    ///
    int block_num=0;
    while(ipos+1<file_cont.size())
    {
        ipos = skip_pair(file_cont, ipos,'\'','\'');
        ipos = skip_pair(file_cont, ipos,'\"','\"');
        //ipos = skip_pair(file_cont, ipos,'(',')');
        //ipos = skip_pair(file_cont, ipos,'[',']');
        if (file_cont[ipos]=='{')
        {
            block_num++;
        }
        if (file_cont[ipos]=='}')
        {
            block_num--;
        }
        ipos++;
        if (block_num<=0)break;
    }

    return ipos;
}

unsigned skip_to(const std::string &file_cont, unsigned ipos, const std::string key)
{
    if (file_cont.size()<=(ipos+key.size()))return ipos;



    int not_equal=1;

    while(not_equal)
    {
        unsigned itpos = file_cont.find((const char)key[0], ipos);
        //#std::string::iterator it = std::find(file_cont.begin()+ipos, file_cont.end(), (const char)key[0]);
        if(itpos==std::string::npos)
        {
            //此处有错误发生，未找到key
            return file_cont.size()-1;
        }
        ipos = itpos;
        not_equal=0;
        for(int i=0;i<key.size()-1;++i)
        {
            if(file_cont[ipos + i+1]!=key[i+1])
            {
                not_equal=1;
                break;
            }
        }

        if (not_equal)
        {
            ipos +=1;
        }
        else
        {
            ipos += key.size();
            return ipos;
        }
    }


    return ipos;
}

/// 跳过当前行，ipos指向下一行
unsigned skip_to_next_line(const std::string &file_cont, unsigned ipos)
{


    if (file_cont.size()<=(ipos+1))return ipos;
    while(file_cont[ipos]!='\n')
    {
        ipos++;
        if ((ipos+1)==file_cont.size())break;
    }
    if (file_cont[ipos]=='\n')
    {
        ipos++;
    }
    return ipos;
}


std::string render_regex(std::string raw_input, const std::map<std::string, std::string> &regex_temp)
{
    for(auto i=regex_temp.begin();i!=regex_temp.end();++i)
    {
        //i->first;
        //i->second;
        raw_input = replace(raw_input,"{" + i->first+"}", i->second);
    }

    return raw_input;
}

/// 输入文件内容，输出解析后结果：
/// 状态字符串 -->执行代码字符串
///
std::string get_regex_key(unsigned int &ipos, const std::string & file_cont, int &curr_mode)
{
    std::string regex_key;
    ipos = skip_to_nblank(file_cont, ipos);
    if ((file_cont[ipos]=='%')&&(file_cont[ipos+1]=='%'))
    {
        curr_mode=2;
        ipos+=2;
        return regex_key;
    }

    ipos = skip_to_nblank(file_cont, ipos);
    unsigned int start = ipos;

    ipos = skip_pair(file_cont, ipos, '"','"');
    ipos = skip_pair(file_cont, ipos, '[',']');
    ipos = skip_pair(file_cont, ipos, '{','}');
    ipos = skip_pair(file_cont, ipos, '(',')');

    ipos = skip_to_blank(file_cont, ipos);
    unsigned int end = ipos;
    regex_key.assign(file_cont.begin()+start, file_cont.begin()+end);
    regex_key=trim(regex_key);
    regex_key=trim1(regex_key,'\xba');
    regex_key=trim1(regex_key,'\x0d');
    regex_key=trim1(regex_key,'\xf0');
    regex_key=trim1(regex_key,'\255');

    regex_key=trim1(regex_key,'\xad');
    regex_key=trim1(regex_key,'\0');
    return regex_key;
}

int lex_file_parse2(const string &file_cont, std::vector<std::map<string, string> > &regex_rule, string &includes, string &add_code, int is_debug)
{

    unsigned int ipos = 0;

    int curr_mode=0;//0-- O [0-7] 1--%% tokens 2--%% code

    //std::string includes;

    // O [0-7] D [0-9] H   [a-fA-F0-9]
    std::map<std::string, std::string> regex_temp;
    //std::map<std::string, std::string> regex_rule;
    //std::string add_code;


    //temp var
    unsigned start=0;
    unsigned end = 0;
    std::string regex_key;
    std::string regex_str;

    if(is_debug)
    {
        std::cout<<"lex_file_parse2:file_cont:"<<file_cont.size()<<"\n";
    }


    while((ipos+1)<file_cont.size())
    {
        if(is_debug)
        {
            std::cout<<"pos:"<<ipos<<"\n";
            std::cout<<"mode:"<<curr_mode<<"\n";
            std::cout<<"char:"<<file_cont[ipos]<<"\n";
        }
        if (curr_mode==0)
        {
            ipos = skip_to_nblank(file_cont, ipos);
            if ((file_cont[ipos]=='%')&&(file_cont[ipos+1]=='%'))
            {
                curr_mode=1;
                ipos+=2;
                continue;
            }
            if ((file_cont[ipos]=='%')&&(file_cont[ipos+1]=='{'))
            {
                start=ipos;
                ipos = skip_to(file_cont, ipos, "%}");
                end = ipos;

                includes.assign(file_cont.begin()+start+2, file_cont.begin()+end-2);
                continue;
            }

            if ((file_cont[ipos]=='%')&&(file_cont[ipos+1]!='{'))
            {
                ipos = skip_to_next_line(file_cont, ipos);
                continue;
            }



            start = ipos;
            ipos = skip_pair(file_cont, ipos, '"','"',0);
            ipos = skip_pair(file_cont, ipos, '[',']',0);
            ipos = skip_pair(file_cont, ipos, '{','}',0);
            ipos = skip_pair(file_cont, ipos, '(',')',0);

            ipos = skip_to_blank(file_cont, ipos);
            end = ipos;
            regex_key.assign(file_cont.begin()+start, file_cont.begin()+end);
            regex_key=trim(regex_key);
            regex_key=trim1(regex_key,'\255');
            regex_key=trim1(regex_key,'\0');

            ipos = skip_to_nblank(file_cont, ipos);

            start = ipos;
            ipos = skip_to_next_line(file_cont, ipos);
            end = ipos;
            regex_str.assign(file_cont.begin()+start, file_cont.begin()+end);
            regex_str= trim(regex_str);

            if(regex_key.size()>0)
                regex_temp[regex_key]=render_regex(regex_str, regex_temp);
            if(is_debug)
            {
                std::cout<<"add new defs:"<<regex_key<<":"<<regex_temp[regex_key]<<"\n";
            }
        }
        else if(curr_mode==1)
        {
            std::string regex_key = get_regex_key(ipos, file_cont,curr_mode);
            if(regex_key.empty())
            {
                continue;
            }


            ipos = skip_to_nblank(file_cont, ipos);
            start = ipos;
            ipos = find_block_end(file_cont, ipos);
            ///找到{ }
            ///
            //ipos = skip_to_blank(file_cont, ipos);
            end = ipos;
            regex_str.assign(file_cont.begin()+start, file_cont.begin()+end);

            if(regex_key.size()>0)
            {
                regex_key = render_regex(regex_key, regex_temp);
                std::map<std::string, std::string> mm1;
                mm1[regex_key ] = regex_str;
                regex_rule.push_back(mm1);
                //regex_rule[regex_key]=regex_str;
            }

        }
        else if(curr_mode==2)
        {
            //end_codes
            add_code.assign(file_cont.begin()+ipos, file_cont.end());
            add_code=trim1(add_code,'\0');
            ipos = file_cont.size();
        }
    }

    return 0;
}
