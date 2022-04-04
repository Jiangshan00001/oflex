%{
#include <stdio.h>
%}

%%

"auto"			{ count(); return(AUTO); }
"break"		{ count(); return (BREAK); }
.			{ /* Add code to complain about unmatched characters */ }

%%

int yywrap(void)
{
	return 1;
}


