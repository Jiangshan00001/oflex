#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "fsa_to_dot.h"

using namespace std;



std::string fsa_to_dot_ss(FSA_TABLE  fsa)
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




    for (i=0;i<fsa.size();i++)
    {

        std::multimap<int, NFAState*>* trans = fsa[i]->GetTransition();
        //std::multimap<int, NFAState*>::iterator it;
        std::vector<int> curr_chars;
        for (auto it=trans->begin();it!=trans->end();++it)
        {
            curr_chars.push_back(it->first);
        }
        std::sort(curr_chars.begin(), curr_chars.end());
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


                char mT[6]={0,0,0,0,0,0};
                int val = curr_chars[j];
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
                //cout<<"contact operator:"<< (unsigned int)it->first<<endl;

                stream<<"[label= \"";
                stream<<mT<<"\" ";
                stream<<"]\n";
            }

        }
    }

    stream<<"}\n";

    return stream.str();
}






void fsa_to_dot(FSA_TABLE & fsa, std::string mFileName)
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


void fsa_to_cout(FSA_TABLE & fsa, std::string mFileName)
{
	//..fsa.

	std::string m_str;
	int i;

    cout<<fsa_to_dot_ss(fsa);
	cout.flush();

	return;
}
