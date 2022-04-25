# oflex

[![CMakematrix](https://github.com/Jiangshan00001/oflex/actions/workflows/cmake_matrix.yml/badge.svg?branch=master)](https://github.com/Jiangshan00001/oflex/actions/workflows/cmake_matrix.yml)


open flex.
parse lex file.



output jump table/pure-cpp header file.
so that user can:
1. use it on any platform which support c++11.
2. use the jump table to add custom code.

do what they want to do.


## how to use:

### download the exe from releases.
### run: oflex -i mylex.l -c my_lex_class -o my_output_header.h
    to generate header file

then, in your c++ code:

```
#include "my_output_header.h"

int main()
{
    my_lex_class the_parse_class;
    the_parse_class.set_file_name(file_to_parse);
    auto tk = the_parse_class.yylex();
    while(!tk.is_eof)
    {
        std::cout<<"got token: text="<<tk.yy_text<<". line:"<< tk.m_column<<":"<<tk.m_line<<". tk_ret_num"<<tk.m_ret<<"\n";
        tk = the_parse_class.yylex();
    }
    return 0;
}


```





reference:

http://gokcehan.github.io/notes/recursive-descent-parsing.html

https://www.geeksforgeeks.org/flex-fast-lexical-analyzer-generator/
