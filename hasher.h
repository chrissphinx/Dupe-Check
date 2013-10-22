#ifndef HASHER_H
#define HASHER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

// PROTOTYPES ________________________________________________________________
void PrintDigest(unsigned char*);
void AddDigest(unsigned char**, unsigned char*);
int CompareDigests(unsigned char**, unsigned char*);

// GLOBALS ___________________________________________________________________
int entries = 0;
unsigned char **mds;

// MAIN ______________________________________________________________________
int Hash(char *word)
{
	mds = realloc(mds, (entries + 1) * sizeof(unsigned char*));

	// for(int k = 0; k < num; k++) {
		// printf("%s\n", word);
		unsigned char *tmp = SHA1((const unsigned char*)word, strlen(word), NULL);
		if(!entries) { AddDigest(mds, tmp); return 1; } else {
		if(CompareDigests(mds, tmp)) {
			mds = realloc(mds, (entries + 1) * sizeof(unsigned char*));
			AddDigest(mds, tmp);
			return 1;
		} else {
			printf(">> DUPLICATE FOUND\n");
			return 0;
		}}

		// for(int i = 0; i < entries; i++) {
		// 	printf("%d: %p\n", i, mds[i]);
		// }
		
	// }

	// for(int j = 0; j < entries; j++) {
	// 	printf("%d: ", j);
	// 	PrintDigest(mds[j]);
	// }

	// Clean up, free each digest and then the digests array
	// for (int l = 0; l < entries; l++) {
	//     free(mds[l]);
	// }
	// free(mds);
}

// FUNCTIONS __________________________________________________________________
void PrintDigest(unsigned char *md)
{
	// Prints out a 20-byte digest nicely formatted
	for(int i = 0; i < SHA_DIGEST_LENGTH; i++) {
		printf("%02x ", md[i]);
	} printf("\n");
}

void AddDigest(unsigned char **mds, unsigned char* md)
{
	printf(">> ADDING NEW ENTRY\n");
	// Copy the new digest into the array
	mds[entries] = calloc(1, SHA_DIGEST_LENGTH);
	memcpy(mds[entries], md, SHA_DIGEST_LENGTH);
	// Increment the entries counter
	entries++;
}

int CompareDigests(unsigned char **mds, unsigned char* md)
{
	// Compare digests array to a possibly new entry
	int res;
	for(int j = 0; j < entries; j++) {
		res = memcmp(mds[j], md, SHA_DIGEST_LENGTH);
		// Found a match, return 0
		if(res == 0) return 0;
	}
	// No match found return 1
	return 1;
}

void Clean()
{
	for (int l = 0; l < entries; l++) {
	    free(mds[l]);
	}
	free(mds);
}

#endif
