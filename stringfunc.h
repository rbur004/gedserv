#ifndef STRINGFUNC_H
#define STRINGFUNC_H


#include <string.h>

#define instring(c,s) ((char *)strchr(s,c) != (char *)0)

char *strip_trailing_spaces( char *string );
int patternmatch(char *s, char * p);
int matches(char *s,char *f, int fsize) ;
int findstring(char *s, char **sa, int n);
int strcasecmp(const char *s1,const  char *s2);
#ifdef NO_STRCASESTR
char * strcasestr(char *s1, char *s2);
#endif
int findcasestring(char *s, char **sa, int n);
char *concat(char *s1, char *s2) ;
void padwithspaces(char *s, int size);
void PadWithNulls(char *s, int size);
void setfield(char *f, char *s, int size);
int getarglist(char *s, char **argv, int size);

char *skip_to_next_line(char *p);
char *skip_spaces(char *p);
char *skip_chars(char *p);
char *next_field(char *p);

int field_match(char *f1, char *f2);

#endif

