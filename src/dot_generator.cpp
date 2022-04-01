#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "dot_generator.h"

using namespace std;
void fsa_to_dot(FSA_TABLE & fsa, std::string mFileName)
{
	//..fsa.
	std::ofstream mFile(mFileName.c_str(), std::ifstream::binary);
	std::stringstream str_stream;
	std::string m_str;
	int i;
	mFile<<"digraph G {\n";


	//cout<<"fsa_table size:"<<fsa.size()<<endl;
	for (i=0;i<fsa.size();i++)
	{
		mFile<<"subgraph cluster_"<<i<<" {\n";
		mFile<<"style=filled;\n";
		mFile<<"color=lightgrey;\n";


        if(fsa[i]->m_bAcceptingState==0x02)
        {//end
            mFile<<fsa[i]->GetStateID()<<"[shape=doublecircle color=red]"<<endl;
        }
        else if (fsa[i]->m_bAcceptingState==0x01)
        {//start
            mFile<<fsa[i]->GetStateID()<<"[shape=circle color=green]"<<endl;
        }
        else if (fsa[i]->m_bAcceptingState==0x03)
        {//start&end
            mFile<<fsa[i]->GetStateID()<<"[shape=doublecircle  color=green]"<<endl;
        }

		mFile<<"label="<<"cluster"<<i<<";\n";
		mFile<<"}\n";
	}



	
	for (i=0;i<fsa.size();i++)
	{
		std::multimap<char, NFAState*>* trans = fsa[i]->GetTransition();
		std::multimap<char, NFAState*>::iterator it;
		for (it=trans->begin();it!=trans->end();++it)
		{
			mFile<<fsa[i]->GetStateID()<<"->"<<it->second->GetStateID();
			mFile<<"[label= \"";
			char mT[4]={0,0,0,0};
			if(it->first==0)
			{
				mT[0] = 'E';
				mT[1] = 'p';
				mT[2] = 's';
			}
			else
			{
				mT[0] = it->first;
			}
            //cout<<"contact operator:"<< (unsigned int)it->first<<endl;
			
			mFile<<mT<<"\" ";
			mFile<<"]\n";
		}
	}




	mFile<<"}\n";


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

	cout<<"============================\n";
	cout<<mFileName<<":\n";

	cout<<"digraph G {\n";
	
	
	//cout<<"fsa_table size:"<<fsa.size()<<endl;
	for (i=0;i<fsa.size();i++)
	{
		cout<<"subgraph cluster_"<<i<<" {\n";
		cout<<"style=filled;\n";
		cout<<"color=lightgrey;\n";
		
		
        if(fsa[i]->m_bAcceptingState==0x02)
        {//end
            cout<<fsa[i]->GetStateID()<<"[shape=doublecircle]"<<endl;
        }
        else if (fsa[i]->m_bAcceptingState==0x01)
        {//start
            cout<<fsa[i]->GetStateID()<<"[shape=circle color=green]"<<endl;
        }
        else if (fsa[i]->m_bAcceptingState==0x03)
        {//start&end
            cout<<fsa[i]->GetStateID()<<"[shape=doublecircle  color=green]"<<endl;
        }

		
		cout<<"label="<<"cluster"<<i<<";\n";
		cout<<"}\n";
	}
	
	
	
	
	for (i=0;i<fsa.size();i++)
	{
		std::multimap<char, NFAState*>* trans = fsa[i]->GetTransition();
		std::multimap<char, NFAState*>::iterator it;
		for (it=trans->begin();it!=trans->end();++it)
		{
			cout<<fsa[i]->GetStateID()<<"->"<<it->second->GetStateID();
			cout<<"[label= \"";
			char mT[4]={0,0,0,0};
			if(it->first==0)
			{
				mT[0] = 'E';
				mT[1] = 'p';
				mT[2] = 's';
			}
			else
			{
				mT[0] = it->first;
			}
			cout<<"contact operator:"<< (unsigned int)it->first<<endl;
			
			cout<<mT<<"\" ";
			cout<<"]\n";
		}
	}
	
	
	
	
	cout<<"}\n";
	
	
	cout.flush();

	return;
}
