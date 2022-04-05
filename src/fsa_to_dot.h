#ifndef dot_generator_h__
#define dot_generator_h__
#include <string>
#include "ORegexParse.h"


void fsa_to_dot(FSA_TABLE & fsa, std::string mFileName);

void fsa_to_cout(FSA_TABLE & fsa, std::string mFileName);
std::string fsa_to_dot_ss(FSA_TABLE fsa);



#endif // dot_generator_h__
