%{
#include <stdio.h>
%}

%%

"auto"			{ count(); return(AUTO); }
"break"			{ count(); return(BREAK); }

%%

int yywrap(void)
{
	return 1;
}


