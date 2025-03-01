/*
 * Copyright (c) 1984,...,1992 by Computer Science Department of the
 *		University of Auckland, Auckland, New Zealand.
 *
 * Permission is granted to any individual or institution to use,
 * copy, or redistribute this software so long as it is not sold for
 * profit, provided that this notice and the original copyright
 * notices are retained.  Auckland University nor the author make no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <ctype.h>

 #include "ged.h"
 #include "stringfunc.h"

 /*
    - - - - - - - - - -- - - - -- - - - -- - - - -- - - - -- - - - -

    strip_trailing_spaces

    strips the trailing whitespace from the string inline and
    returns a pointer to it.

    This version takes the easy way out and uses strlen. Maybe it
    could be optimised later on.
    - JRH 26/10/93

    - - - - - - - - - -- - - - -- - - - -- - - - -- - - - -- - - - -
*/
char *strip_trailing_spaces(char *string) {
  char *strPtr;
  long len = strlen(string);

  if (len == 0)
    return (string);

  strPtr = &string[len - 1];

  while (strPtr >= string && *strPtr == ' ')
    strPtr--;
  strPtr[1] = '\0';

  return (string);
}

#ifdef RECOMP
/* re_exec and re_comp are not provided in the TPM ANSI library. */
int patternmatch(char *s, char *p)
/*
 * Return true if string s matches pattern p, where a pattern is a
 * string in which asterisks can match any arbitrary substring including
 * the null string, and all other characters can match themselves only.
 */
{
  char *err_message;
  char fudge[65]; /*re_exec returns a match when pattern matches the first part
                     of a string*/
                  /*So we tack on a $ to each string to match the null*/
  int l;

  if ((l = strlen(p)) > 62) {
    printf(
        "Internal error in patternmatch(), length of pattern > 62 chars (%d)\n",
        l);
    return (0);
  }
  strncpy(&fudge[1], p, 64);
  fudge[65] = '\0';

  fudge[0] = '^'; /* match from start of line. */
  fudge[l + 1] = '$';
  fudge[l + 2] = '\0';
  if (err_message = (char *)re_comp(fudge)) {
    printf("%s\n", err_message);
    return (0);
  }
  if (re_exec(s) == 1)
    return (1);
  else
    return (0);
}
#endif

int matches(char *s, char *f, int fsize) /* true if string s matches f.*/
/*NB s is padded with blanks to the length of f */
/*   They are only compared for fsize-1 chars */
{
  return (strncmp(s, f, fsize) == 0);
}

int findstring(char *s, char **sa, int n)
/* searches for string s in array of string sa (n elements).
   Returns index to reqd string if found, else -1  */
{
  int i;
  for (i = 0; i < n; i++)
    if (!strcmp(s, sa[i]))
      return (i);
  return (-1);
}

int strcasecmp(const char *s1, const char *s2) {
  if (s1 == 0 && s2 == 0)
    return 0; // Both Null so lets say they are the same
  if (s1 == 0)
    return -1; // s1 null so s2 must be greater
  if (s2 == 0)
    return 1; // s2 null so s1 must be greater
  while (*s1) {
    if (toupper(*s1++) != toupper(*s2++))
      return (toupper(*--s1) > toupper(*--s2)) ? 1 : -1; // Not equal.
  }
  if (*s2)
    return -1; // s2 longer.
  else
    return 0; // Strings are equal
}

#ifdef NO_STRCASESTR
char *strcasestr(char *s1, char *s2) {
  int diff, i;
  char *p1, *p2;
  if (s1 == 0 || s2 == 0)
    return (char *)0; // return no match on null strings

  diff = strlen(s1) - strlen(s2);

  if (diff < 0)
    return (char *)0; // s2 can't be in s1 as s2 is longer

  // printf("Looking for \"%s\" in \"%s\"\n", s2, s1);

  for (i = 0; i <= diff; i++) // scan string s1 looking for a match on s2
  {
    if (toupper(*s1) == toupper(*s2)) {
      p1 = s1;
      p2 = s2;
      while (1) {
        if (*p2 == '\0') // matched to end of p2
          return s1;
        if (*p1 == '\0') // don't go further than the end of p1
          break;
        if (toupper(*p1++) != toupper(*p2++))
          break; // Not equal.
      }
    }
    s1++; // try again
  }
  return (char *)0; // s2 not in s1.
}
#endif

int findcasestring(char *s, char **sa, int n)
/* searches for string s in array of string sa (n elements).
   Returns index to reqd string if found, else -1  */
{
  int i;
  if (s == 0 || sa == 0)
    return -1;
  for (i = 0; i < n; i++)
    if (!strcasecmp(s, sa[i]))
      return (i);
  return (-1);
}

char *concat(char *s1,
             char *s2) /* return pointer to concatenation of 2 strings */
{
  int l1, l2;
  static char concatbuff[2048];
  l1 = strlen(s1);
  l2 = strlen(s2);
  if ((l1 >= 0) && (l2 >= 0) && ((l1 + l2) <= 2047)) {
    memcpy(concatbuff, s1, l1);
    memcpy(&concatbuff[l1], s2, l2 + 1);
    return (concatbuff);
  } else {
    // syslog( LOG_ERR, "concat: System error.  Bad call to CONCAT");
    return "";
  }
}

void padwithspaces(char *s,
                   int size) /* add ' 's to s until its length is (size-1) */
{
  int i;
  size--; /* make it 0 origin */
  for (i = strlen(s); i < size; i++)
    s[i] = ' ';
  s[size] = '\0';
}

void PadWithNulls(char *s, int size)
/* add '\0's to s until its length is (size-1) */
{
  int i;
  size--; /* make it 0 origin */
  for (i = strlen(s); i < size; i++)
    s[i] = '\0';
  s[size] = '\0';
}

void setfield(char *f, char *s, int size)
/* Set field f to string s padded with spaces to "size"*/
/* (which includes the terminating null */
{
  strncpy(f, s, size - 1);
  PadWithNulls(f, size);
}

int getarglist(char *s, char **argv, int size)
/*
 * Takes a string containing a space or ',' separated list of arguments,
 * and sets up an array "argv" of pointers to the individual arguments.
 * The value returned is the number of such pointers defined; "size" is
 * the maximum. The original string is modified by the
 * insertion of nulls as argument terminators.
 */
{
  int i;
  i = 0;
  while (*s != '\0') {
    if (isspace(*s) || *s == ',')
      s++; /* step over leading spaces */
    else { /* arg found */
      if (i >= size)
        break;
      argv[i++] = s++;
      while (*s != '\0' && !isspace(*s) && *s != ',')
        s++;
      if (*s == '\0')
        break;
      *s++ = '\0'; /* plug in arg delimiter */
    }
  }
  return (i);
}

char *skip_to_next_line(char *p) {
  while (*p && *p != '\012' && *p != '\015')
    p++;
  if (*p == '\0')     // No more Lines
    return (char *)0; // So return null pointer
  if ((*p == '\012' && *(p + 1) == '\015') ||
      (*p == '\015' && *(p + 1) == '\012')) {
    *p++ = '\0';
    p++;
  } else
    *p++ = '\0';
  return p;
}

char *skip_spaces(char *p) {
  while (*p && isspace(*p))
    p++;
  if (*p == '\0')     // No more fields
    return (char *)0; // So return null pointer
  return p;
}

char *skip_chars(char *p) {
  while (*p && !isspace(*p))
    p++;
  if (*p == '\0')     // No more fields
    return (char *)0; // So return null pointer
  return p;
}

char *next_field(char *p) {
  while (*p && !isspace(*p)) /*skip non space chars*/
    p++;
  if (*p == '\0')     // No More Fields
    return (char *)0; // So return null pointer
  *p++ = '\0';        /*Null terminate the string*/
  while (*p && isspace(*p)) {
    p++;
  } /*Skip Spaces between fields*/
  if (*p == '\0')     // No more fields
    return (char *)0; // So return null pointer
  return p;           /*return pointer to next field*/
}

int field_match(char *f1, char *f2) {
  char *v1[LINE_LIMIT];
  char *v2[LINE_LIMIT];
  static char buff1[1024];
  static char buff2[1024];
  int nargs1, nargs2;
  int i;

  if (f1 == 0 && f2 == 0)
    return 1;
  if (f1 == 0 || f2 == 0)
    return 0;
  strncpy(buff1, f1, LINE_LIMIT);
  strncpy(buff2, f2, LINE_LIMIT);
  // Longer lines don't get Null terminated :()
  buff1[LINE_LIMIT - 1] = '\0';
  buff2[LINE_LIMIT - 1] = '\0';
  nargs1 =
      getarglist(buff1, v1, LINE_LIMIT); // make arg list for easier matching
  nargs2 = getarglist(buff2, v2, LINE_LIMIT);
  if (nargs1 != nargs2)
    return 0;
  else
    for (i = 0; i < nargs1; i++)
      if (strcasecmp(v1[i], v2[i]) != 0)
        return 0;
  return 1;
}
