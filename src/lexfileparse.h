#ifndef LEXFILEPARSE_H
#define LEXFILEPARSE_H

#include <string>
#include <vector>
#include <map>

std::map<std::string, std::string> lex_file_parse(const std::string &file_cont);
int lex_file_parse2(const std::string &file_cont,std::vector<std::map<std::string, std::string> > &regex_rule, std::string &includes,std::string &add_code, int is_debug=0);
#endif // LEXFILEPARSE_H
