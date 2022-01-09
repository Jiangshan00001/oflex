#include "NFAState.h"
#include "ORegexParse.h"
/// 堆栈方法求值
/// http://blog.csdn.net/liuhuiyi/article/details/8433203

/// 当前表达式支持:
/// *
/// |
/// +
/// ()
/// abcd

#ifndef QT_BUILD
///
int main()
{
	ORegexParse mRegex;
	
	mRegex.CreateNFA("abca*");
	
	mRegex.NFAToDFA();




	return 0;
}

#endif

