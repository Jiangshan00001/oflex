#include "NFAState.h"
#include "ORegexParse.h"
/// ��ջ������ֵ
/// http://blog.csdn.net/liuhuiyi/article/details/8433203

/// ��ǰ���ʽ֧��:
/// *
/// |
/// +
/// ()
/// abcd

///
int main()
{
	ORegexParse mRegex;
	
	mRegex.CreateNFA("abca*");
	
	mRegex.NFAToDFA();




	return 0;
}

