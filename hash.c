//HASH Function
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "hashtable.h"
#include "hash.h"

int hash_mem(
	void *h, 		/*A pointer to the data being hashed*/
	int nbytes, 	/*The number of bytes in the data being hashed*/
	int hashsize	/*The size of the hashtable*/
)
/*Returns a hash index in the range 0 - hashsize - 1*/
{
unsigned int hash;
int i ;
unsigned char *n;

    hash = 0;
    for(n = (unsigned char *) h, i = 0; i < nbytes; i++)
    {
        hash=(hash<<31 | hash>>1) ^ random_table[*n++];
    }
    hash = hash % hashsize;
    if(hash < 0)
    {   printf("Mod produces - nums\n");
    sleep(1000);
        exit(-1);
    }
    return (hash);

}

int hash_string(
	unsigned char *n,	/*The string to hash*/
	int hashsize		/*The size of the hashtable*/
)
/*Returns a hash index in the range 0 - hashsize - 1*/
{
unsigned int hash;

	hash = 0;
	while(*n)
	{
	    hash=(hash<<31 | hash>>1) ^ random_table[*n++];
	}
	hash = hash % hashsize;
	if(hash < 0)
	{	printf("Mod produces - nums\n");
    sleep(1000);
		exit(-1);
	}
	return (hash);
}

