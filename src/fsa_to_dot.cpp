#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "fsa_to_dot.h"

using namespace std;

std::string fsa_to_dot_ss(FSA_TABLE & fsa)
{
    std::stringstream stream;
    //..fsa.
    //std::stringstream str_stream;
    //std::string m_str;
    int i;
    stream<<"digraph G {\n";


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
        std::multimap<int, NFAState*>::iterator it;
        for (it=trans->begin();it!=trans->end();++it)
        {
            stream<<fsa[i]->GetStateID()<<"->"<<it->second->GetStateID();


            char mT[4]={0,0,0,0};
            if(it->first==EPS_CHAR)
            {
                mT[0] = 'E';
                mT[1] = 'p';
                mT[2] = 's';
            }
            else
            {
                if(it->first=='"')
                {
                    mT[0] = '\\';
                    mT[1] = it->first;
                }
                else if(it->first=='\\')
                {
                    mT[0] = '\\';
                    mT[1] = it->first;
                }
                else
                {
                    mT[0] = it->first;
                }
            }
            //cout<<"contact operator:"<< (unsigned int)it->first<<endl;

            stream<<"[label= \"";
            stream<<mT<<"\" ";
            stream<<"]\n";
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


	mFile.flush();
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
