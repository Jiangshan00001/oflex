#ifndef NUM2STR_H
#define NUM2STR_H

#include <string>
#include <sstream>


template<class tt> std::string num2str(tt num)
{
    std::stringstream ss;
    ss<<num;
    return ss.str();
}

#endif // NUM2STR_H
