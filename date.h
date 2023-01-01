#ifndef DATE_H
#define DATE_H

#ifdef DATE_C

char *months[] =
{
"jan",	"feb",	"mar", 	"apr", 	"may",	"jun",	"jul",	"aug",	"sep", "oct", "nov", "dec"
};

#define MONTHS_SIZE 12
#define NMONTHS 12

int month_lengths[] =
{
31,		28,		31,		30,		31,		30,		31,		31,		30,		31,		30,	 	31
};

char *Calendar_Escape_Sequence[] =
{ "@#DHEBREW@", "@#DROMAN@", "@#DFRENCH R@", "@#DGREGORIAN@", "@#DJULIAN@", "@#DUNKNOWN@" };

#define CALENDAR_ESCAPE_SEQUENCE_SIZE 6

//    Qualifies the meaning of a date.
//       ABT = About
//       AFT = After
//       BEF = Before
//       EST = Estimated
char *Date_Modifier[] = { "ABT" , "AFT" , "BEF", "EST"};

#define DATE_MODIFIER_SIZE 4

char *Date_Range[] = { "BET", "AND" };

#define DATE_RANGE_SIZE 2

char *Date_With_BC[] = { "BC", "AD" };

#define DATE_WITH_BC_SIZE 2

#endif

int which_month(char *m);
int dates_match(char *d1, char *d2);
int parse_date(char *date);
int return_year(char *date);
int cmp_dates(char *d1, char *d2);

#endif
