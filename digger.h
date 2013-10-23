/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  This file is the recursive directory traversing function. It takes a
 *  directory name for an argument and continues through it finding files and
 *  reading their contents for the hashing function. If it ever encounters
 *  a duplicate, a symlink is created in the /dups directory.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef DIGGER_H
#define DIGGER_H
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include "hasher.h"
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <unistd.h>

// PROTOTYPES ________________________________________________________________
int MakeLink(char*, char*);

// GLOBALS ___________________________________________________________________
char dups[MAXPATHLEN];

// FUNCTIONS _________________________________________________________________
int Dig(const char *dir)
{
    // Open directory path passed to function
    DIR *d;
    struct dirent *entry;
    if (!(d = opendir(dir))) {
        return errno;
    }

    // Recursively loop until there are no more entries
    while(entry = readdir(d)) {
        // Screen out directories and OS X .DS_Store files
        if(entry->d_type & DT_REG && strcmp(entry->d_name, ".DS_Store")) {
            // Create path to the file
            char filepath[MAXPATHLEN];
            snprintf(filepath, MAXPATHLEN, "%s/%s", dir, entry->d_name);

            // Open the file as read-only and get it's size
            int fd = open(filepath, O_RDONLY);
            size_t size = lseek(fd, 0, SEEK_END);

            // Create a buffer of that size, move file pointer to start
            char *buf = calloc(1, size + 1);
            lseek(fd, 0, SEEK_SET);

            // Read the file contents into buf
            if(read(fd, buf, size) < 0) perror("ERROR");

            // Hash the file contents
            if (!(Hash(buf))) {
                // Create a symlink to the file
                if(MakeLink(filepath, entry->d_name)) {
                    return errno;
                }
            }
        }

        // Otherwise check if it is a subdirectory
        if (entry->d_type & DT_DIR) {
            // Screen out itself and parent directory to avoid infinite loops
            if (strcmp(entry->d_name, "..") && strcmp(entry->d_name, ".")) {
                // Assure we haven't exceeded MAXPATHLEN
                char path[MAXPATHLEN];
                if (!(snprintf(path, MAXPATHLEN, "%s/%s", dir, entry->d_name))){
                    return errno;
                }
                // Call again on this subdirectory
                Dig(path);
            }
        }
    }

    // All entries were checked, close the directory
    if(closedir(d)) {
        return errno;
    }

    return 0;
}


int MakeLink(char *filepath, char *name)
{
    // Get the path for the link we are creating
    char duppath[MAXPATHLEN] = { 0 };
    snprintf(duppath, MAXPATHLEN, "%s/%s", dups, name);

    // Attempt to create a symlink
    if(symlink(filepath, duppath) < 0) {
        // If it failed because of a duplicate name ...
        if(errno == 17) {
            // Angrily try again and again with a number appended to the name
            int i = 1;
            do {
                snprintf(duppath, MAXPATHLEN, "%s/%s%d", dups, name, i++);
            } while(symlink(filepath, duppath) < 0);
        } else {
            // Relay error if duplicate wasn't the problem
            return errno;
        }
    } return 0;
}

#endif
