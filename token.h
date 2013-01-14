#ifndef TOKEN_H
#define TOKEN_H

typedef struct { char * token; int tokenid; } token_table;

#define GET 1
#define GET_HEAD 2

#ifdef TOKEN_C

token_table Commands[] =
{
	"GET", GET,
	"HEAD", GET_HEAD,
	0, 0
};

#else

extern token_table Commands[];

#endif

int token_to_num(token_table *table, char *type);

#endif //TOKEN_H
