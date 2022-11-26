#ifndef FLEX_SAMPLE1_H
#define FLEX_SAMPLE1_H
#include <string>

class flex_sample1
{
public:
    flex_sample1();
    std::string render(std::string core_txt, std::string headers, std::string other_code, std::string class_name, std::string token_header_file, std::string token_class_namne,
                       std::string name_space);
    std::string token_header(std::string token_class_name, std::string name_space);
};

#endif // FLEX_SAMPLE1_H
