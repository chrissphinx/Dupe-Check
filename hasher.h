/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  This file contains the hashing functions. It manages a message digest, only
 *  adding a new digest when a new string is encountered (return 1). Otherwise
 *  it returns 0, the digger listens for this result to determine whether or
 *  not to create a symlink to the file in the /dups directory.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef HASHER_H
#define HASHER_H
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// PROTOTYPES ________________________________________________________________
int     Hash(char*);
void    AddDigest(unsigned char**, unsigned char*);
void    Clean();
int     CompareDigests(unsigned char**, unsigned char*);

// GLOBALS ___________________________________________________________________
int             entries = 0;                                // Sorry
unsigned char   **mds;

// FUNCTIONS _________________________________________________________________
int Hash(char *w)
{
    // Realloc for a new spot in the digest array
    mds = realloc(mds, (entries + 1) * sizeof(unsigned char*));

    // Hash the string passed
    unsigned char *tmp = SHA1((const unsigned char*)w, strlen(w), NULL);

    // If there are no entries yet, simply add digest to the array ...
    if(!entries) {
        AddDigest(mds, tmp);
        return 1;
    } else {
        // ... otherwise check against the digest array
        if(CompareDigests(mds, tmp)) {
            // Not a duplicate, add it to the array
            AddDigest(mds, tmp);
            return 1;
        } else {
            // Duplicate found, return 0
            return 0;
        }
    }
}

void AddDigest(unsigned char **mds, unsigned char* md)
{
    // Copy the new digest into the array
    mds[entries] = calloc(1, SHA_DIGEST_LENGTH);
    memcpy(mds[entries], md, SHA_DIGEST_LENGTH);

    // Increment the entries counter
    entries++;
}

void CleanUp()
{
    // Free all memory allocated
    for (int l = 0; l < entries; l++) {
        free(mds[l]);
    } free(mds);
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

#endif
