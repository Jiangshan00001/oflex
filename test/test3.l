D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			([Ee][+-]?{D}+)
P                       ([Pp][+-]?{D}+)
FS			(f|F|l|L)
IS                      ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))


%{
#include <stdio.h>

%}

%%

breaeee		{ count(); return (BREAK); }
C[^\\"\n]*	{ count(); return(STRING_LITERAL); }
A(\\.|[^\\"\n])*  { count(); return(STRING_LITERAL); }



%%

int yywrap(void)
{
	return 1;
}


