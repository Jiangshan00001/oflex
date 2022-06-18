#ifndef dot_generator_h__
#define dot_generator_h__
#include <string>
#include "ORegexParse.h"


void fsa_to_dot(FSA_TABLE & fsa, std::string mFileName, int is_compress=0);

void fsa_to_cout(FSA_TABLE & fsa, std::string mFileName, int is_compress=0);
std::string fsa_to_dot_ss(FSA_TABLE fsa, int is_compress=0);



#endif // dot_generator_h__
