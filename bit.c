#include "bit.h"

//Utility bit manipulation functions for arrays of unsigned int

//Set a bit in an array of unsigned int
void setbit(unsigned int *bm, int i)
{
	bm[i/(sizeof(unsigned int) * 8)] |= 1 << ( i % (sizeof(unsigned int) * 8) );
}

//Clear a bit in an array of unsigned int
void clearbit(unsigned int *bm, int i)
{
	bm[i/(sizeof(unsigned int) * 8)] &= ~(1 << ( i % (sizeof(unsigned int) * 8) ));
}

//Test for a bit being set in an array of unsigned int
int isbitset(unsigned  int *bm, int i)
{
	return bm[i/(sizeof(unsigned int) * 8)] & (1 << ( i % (sizeof(unsigned int) * 8) ));
}

//Test a bit is not set in an array of unsigned int
int isbitclear(unsigned int *bm, int i)
{
	return (bm[i/(sizeof(unsigned int) * 8)] & (1 << ( i % (sizeof(unsigned int) * 8) ))) == 0;
}
