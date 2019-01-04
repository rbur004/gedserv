#define DATE_C
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "date.h"
#include "stringfunc.h"

int which_month(char *m)
{
// return -1 if it fails, otherwise the month Jan = 0 , Dec = 12.
// A return value of 13 means it matched a ? ot 0
	return findcasestring(m, months, MONTHS_SIZE) % NMONTHS;
}

int dates_match(char *d1, char *d2)
{
//Brain dead date matcher.
//Should test for variations on date formats

	return field_match(d1, d2);
}

static int day, month, year;

int parse_date(char *date)
{
// We expect dates to be of the form day month year
//								  or month year
//								  or year
//								  or day 
//								  or day month
// We might also get ABT, EST, ... in front
// We will make a wild asumption and say no year will be below 32
char *v1[32];	//Split string into arglist
static char buff1[1024];
int nargs1;
int i;
int base = 0;

	if(date == 0)
		return -1;
	strncpy(buff1, date, 1024);	//make a copy as we are going to fill it full of nulls
	buff1[1023] = 0;			//Ensure null termination
	if( (nargs1 = getarglist(buff1, v1, 32)) ) //make arg list
	{
		if(findcasestring( v1[base], Calendar_Escape_Sequence, CALENDAR_ESCAPE_SEQUENCE_SIZE) != -1)
			base++;

		if(strcmp( v1[base], "BET") == 0)
		{
			base++;
			while(v1[base])
			{
				if(strcmp( v1[base], "AND") == 0)
				{	base++;
					break;
				}
				else
					base++;
			}
		}			

		if(findcasestring( v1[base], Date_Modifier, DATE_MODIFIER_SIZE) != -1)
			base++;
			
		if(base < nargs1 && isdigit(*v1[base]) && (day = atoi(v1[base])) <= 31 && day > 0)
			base++;
		else
		{
			day = 0; //Don't know so will hack something up later
			//printf("No day in %s\n", date);
		}
			
		if(base < nargs1 && (month = findcasestring(v1[base], months, NMONTHS)) != -1)
		{
			if(day == 0)
				day = month_lengths[month]; //HACK: Assume end of the month
			base++;
			month++;
		}
		else
		{
			if(day == 0) //HACK: Assume end of the month of December
				day = 31;
			month = 12; //HACK: Assume end of December
			//printf("No month in %s\n", date);
		}
		
		if(base < nargs1 && isdigit(*v1[base]))
			year = atoi(v1[base]);	
		else 
		{
			year = 0;
			printf("No year in %s\n", date);
		}
		
		return	day + month * 32 + year * 384;					
	}
	return 0;
}

int return_year(char *date)
{
	parse_date(date);
	return year;
}

int cmp_dates(char *d1, char *d2)
{
int i1, i2;
	if((i1 = parse_date(d1)) == (i2 = parse_date(d2)))
		return 0;	//d1 == d2
	else if(i1 > i2)
		return 1;	//d1 > d2
	else 
		return -1; 	//d2 > d1
}

