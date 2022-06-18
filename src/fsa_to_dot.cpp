#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "fsa_to_dot.h"

using namespace std;


std::string get_label_from_one_int(int val)
{

    char mT[6]={0,0,0,0,0,0};
    if(val==EPS_CHAR)
    {
        mT[0] = 'E';
        mT[1] = 'p';
        mT[2] = 's';
    }
    else if(!((val>=0x20)&&(val<=0x7e)))
    {
        //convert to number
        int b = (val/100)%10;
        int s = (val/10 )%10;
        int g = val%10;
        mT[0]='0';
        mT[1]='X';
        mT[2] = b+'0';
        mT[3] = s+'0';
        mT[4] = g+'0';
    }
    else
    {

        if(val=='"')
        {
            mT[0] = '\\';
            mT[1] = val;
        }
        else if(val=='\\')
        {
            mT[0] = '\\';
            mT[1] = val;
        }
        else
        {
            mT[0] = val;
        }
    }
    return std::string(mT);
}

std::vector<std::string> get_label_from_vec(std::vector<int> & vec)
{
    std::vector<std::string> ret;
#define START_INT_EMPTY -11
    int start_int=START_INT_EMPTY;
    int end_int = START_INT_EMPTY;
    for(unsigned i=0;i<vec.size();++i)
    {
        int val = vec[i];
        if(val==EPS_CHAR)
        {
            ret.push_back("EPS");
            continue;
        }
        if(start_int==START_INT_EMPTY)
        {
            start_int=val;
            end_int=val;
            continue;
        }
        if(end_int+1==val)
        {
            end_int++;
            continue;
        }
        else
        {
            if(start_int==end_int)
            {
                ret.push_back(get_label_from_one_int(start_int));
                start_int = START_INT_EMPTY;
                end_int  = START_INT_EMPTY;
            }
            else
            {
                ret.push_back(get_label_from_one_int(start_int)+"-"+get_label_from_one_int(end_int));
                start_int = START_INT_EMPTY;
                end_int  = START_INT_EMPTY;
            }
        }
    }
    if(start_int!=START_INT_EMPTY)
    {
        if(start_int==end_int)
        {
            ret.push_back(get_label_from_one_int(start_int));
            start_int = START_INT_EMPTY;
            end_int  = START_INT_EMPTY;
        }
        else
        {
            ret.push_back(get_label_from_one_int(start_int)+"-"+get_label_from_one_int(end_int));
            start_int = START_INT_EMPTY;
            end_int  = START_INT_EMPTY;
        }
    }

    return ret;

}

void draw_state_jmp_compress(FSA_TABLE  &fsa,std::stringstream &stream)
{
    int i=0;
    for (i=0;i<fsa.size();i++)
    {

        std::multimap<int, NFAState*>* trans = fsa[i]->GetTransition();

        //std::multimap<int, NFAState*>::iterator it;
        ///找到所有字符
        std::map<NFAState*, std::vector<int> > curr_chars_next;
        for (auto it=trans->begin();it!=trans->end();++it)
        {
            curr_chars_next[it->second].push_back(it->first);
        }

        for(auto it=curr_chars_next.begin();it!=curr_chars_next.end();++it)
        {
            ///字符排序
            std::sort(it->second.begin(), it->second.end());
            std::vector<std::string> mT=get_label_from_vec(it->second);
            for(auto mStr=mT.begin();mStr!=mT.end();++mStr)
            {
                stream<<fsa[i]->GetStateID()<<"->"<< (it->first)->GetStateID();
                stream<<"[label= \"";
                stream<<*mStr<<"\" ";
                stream<<"]\n";
            }
        }

    }
}
void draw_state_jmp_uncompress(FSA_TABLE  &fsa,std::stringstream &stream)
{
    int i=0;
    for (i=0;i<fsa.size();i++)
    {

        std::multimap<int, NFAState*>* trans = fsa[i]->GetTransition();
        //std::multimap<int, NFAState*>::iterator it;
        ///找到所有字符
        std::vector<int> curr_chars;
        for (auto it=trans->begin();it!=trans->end();++it)
        {
            curr_chars.push_back(it->first);
        }
        ///字符排序
        std::sort(curr_chars.begin(), curr_chars.end());

        ///对每个字符，进行设置
        for(int j=0;j<curr_chars.size();++j)
        {
            if(j>0)
            {
                if(curr_chars[j]==curr_chars[j-1])continue;
            }
            std::set<NFAState*> States;
            fsa[i]->GetTransition(curr_chars[j], States);
            std::vector<NFAState*> StatesVec;
            StatesVec.assign(States.begin(), States.end());

            std::sort(StatesVec.begin(), StatesVec.end(), cmp_state_id);
            for(auto it=StatesVec.begin();it!=StatesVec.end();++it)
            {
                stream<<fsa[i]->GetStateID()<<"->"<< (*it)->GetStateID();

                int val = curr_chars[j];
                std::string mT = get_label_from_one_int(val);

                stream<<"[label= \"";
                stream<<mT<<"\" ";
                stream<<"]\n";
            }

        }
    }
}

std::string fsa_to_dot_ss(FSA_TABLE  fsa, int is_compress)
{
    std::stringstream stream;
    //..fsa.
    //std::stringstream str_stream;
    //std::string m_str;
    int i;
    stream<<"digraph G {\n";


    std::sort(fsa.begin(), fsa.end(),cmp_state_id);


    //cout<<"fsa_table size:"<<fsa.size()<<endl;
    for (i=0;i<fsa.size();i++)
    {
        stream<<"subgraph cluster_"<<i<<" {\n";
        stream<<"style=filled;\n";
        stream<<"color=lightgrey;\n";


        if(fsa[i]->m_bAcceptingState==FINAL_STATE)
        {//end
            stream<<fsa[i]->GetStateID()<<"[shape=doublecircle color=red]"<<endl;
        }
        else if (fsa[i]->m_bAcceptingState==START_STATE)
        {//start
            stream<<fsa[i]->GetStateID()<<"[shape=circle color=green]"<<endl;
        }
        else if (fsa[i]->m_bAcceptingState==(FINAL_STATE|START_STATE))
        {//start&end
            stream<<fsa[i]->GetStateID()<<"[shape=doublecircle  color=green]"<<endl;
        }

        stream<<"label="<<"cluster"<<i<<";\n";
        stream<<"}\n";
    }



    if(is_compress)
    {
        draw_state_jmp_compress(fsa, stream);
    }
    else
    {
        draw_state_jmp_uncompress(fsa, stream);
    }


    stream<<"}\n";

    return stream.str();
}






void fsa_to_dot(FSA_TABLE & fsa, std::string mFileName, int is_compress)
{
	//..fsa.
	std::ofstream mFile(mFileName.c_str(), std::ifstream::binary);
	std::stringstream str_stream;
	std::string m_str;
	int i;

    mFile<<fsa_to_dot_ss(fsa);


    //mFile.flush();
	//mFile<<str_stream.str();
	mFile.close();
	return;
}


void fsa_to_cout(FSA_TABLE & fsa, std::string mFileName, int is_compress)
{
	//..fsa.

	std::string m_str;
	int i;

    cout<<fsa_to_dot_ss(fsa);
	cout.flush();

	return;
}
