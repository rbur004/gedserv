#define TOKEN_C
#include "token.h"


int token_to_num(token_table *table, char *type)
{
token_table *t;

	if(table == 0 || type == 0) // check for obvious errors
		return 0;
		
	for(t = table; t->token; t++)
		if( strcmp(type, t->token) == 0)
			return t->tokenid;
	return 0;
}

