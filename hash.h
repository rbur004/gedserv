#ifndef HASH_H
#define HASH_H

#define MAXREHASH   15  /* Max. num rehashes before crashing */

int hash_mem(
	void *h, 		/*A pointer to the data being hashed*/
	int nbytes, 	/*The number of bytes in the data being hashed*/
	int hashsize	/*The size of the hashtable*/
);

int hash_string(
	unsigned char *n,	/*The string to hash*/
	int hashsize		/*The size of the hashtable*/
);

short
name_hash
(	/* Hash function for accessing hash file */
	char *name
);

short
sid_hash
(/*hash function for accessing sidhashfile*/
	char *studentid
);

#endif

