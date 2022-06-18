#ifndef REGEX_MATCH_H
#define REGEX_MATCH_H

#include <string>

int regex_match(const std::string &to_match_str, const std::string &regex_str);
int regex_match2(const std::string &to_match_str, void* regex);
void * regex_contruct(const std::string &regex_str);

#endif // REGEX_MATCH_H
