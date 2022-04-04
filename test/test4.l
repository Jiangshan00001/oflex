D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			([Ee][+-]?{D}+)
P                       ([Pp][+-]?{D}+)
FS			(f|F|l|L)
IS                      ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))


%{
#include <stdio.h>
"auto"			{ count(); return(AUTO); }
"break"		{ count(); return (BREAK); }
L?\"(\\.|[^\\"\n])*\"	{ count(); return(STRING_LITERAL); }
.			{ /* Add code to complain about unmatched characters */ }

%}

%%

L?\"(\\.|[^\\"\n])*\" {/*this is zero or one L*/return 0;}



%%

int yywrap(void)
{
	return 1;
}


