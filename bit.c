#include "bit.h"

void setbit(unsigned int *bm, int i)
{
	bm[i/32] |= 1 << ( i % 32 );
}

void clearbit(unsigned int *bm, int i)
{
	bm[i/32] &= ~(1 << ( i % 32 ));	
}

int isbitset(unsigned  int *bm, int i)
{
	return bm[i/32] & (1 << ( i % 32 ));
}

int isbitclear(unsigned int *bm, int i)
{
	return (bm[i/32] & (1 << ( i % 32 ))) == 0;
}

