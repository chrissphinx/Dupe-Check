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

void dig(const char * dir_name)
{
	DIR * d;

	/* Open the directory specified by "dir_name". */

	d = opendir (dir_name);

	/* Check it was opened. */
	if (! d) {
		fprintf (stderr, "Cannot open directory '%s': %s\n",
				 dir_name, strerror(errno));
		// exit (EXIT_FAILURE);
	}
	while (1) {
		struct dirent * entry;
		// const char * d_name;

		/* "Readdir" gets subsequent entries from "d". */
		entry = readdir (d);
		if (! entry) {
			/* There are no more entries in this directory, so break
			   out of the while loop. */
			break;
		}
		// entry->d_name = entry->entry->d_name;
		/* Print the name of the file and directory. */
		if(entry->d_type & DT_REG && strcmp(entry->d_name, ".DS_Store")) {
			char opening[MAXPATHLEN];
			char dups[MAXPATHLEN] = "/Users/chrissphinx/cs2240/224/assignment2/dups";
			snprintf(opening, MAXPATHLEN, "%s/%s", dir_name, entry->d_name);
			snprintf(dups, MAXPATHLEN, "%s/%s", dups, entry->d_name);
			int fd = open(opening, O_RDONLY, S_IROTH);
			size_t size = lseek(fd, 0, SEEK_END);
			char *buf = calloc(1, size + 1);
			lseek(fd, 0, SEEK_SET);
			int bytes = read(fd, buf, size);
			if(bytes < 0) perror("ERROR");
			printf("READ %d BYTES FROM FILE %s CONTENTS: %s", bytes, entry->d_name, buf);
			int result = Hash(buf);
			if (!result)
			{
				// DUPLICATE
				printf(">> MAKING SYMLINK\n");
				if(symlink(opening, dups) < 0) {
					if(errno == 17) {
						int i = 1;
						char duppath[MAXPATHLEN] = { 0 };
						strncpy(duppath, dups, MAXPATHLEN);
						do {
							snprintf(duppath, MAXPATHLEN, "%s%d", dups, i++);
						} while(symlink(opening, duppath) < 0);
					} else {
						perror("ERROR");
					}
				}
			} else {
				printf(">> NO SYMLINK MADE\n");
			}
		}

		/* See if "entry" is a subdirectory of "d". */

		if (entry->d_type & DT_DIR) {

			/* Check that the directory is not "d" or d's parent. */
			
			if (strcmp (entry->d_name, "..") != 0 &&
				strcmp (entry->d_name, ".") != 0) {
				int path_length;
				char path[MAXPATHLEN];
 
				path_length = snprintf (path, MAXPATHLEN,
										"%s/%s", dir_name, entry->d_name);
				printf ("%s\n", path);
				if (path_length >= MAXPATHLEN) {
					fprintf (stderr, "Path length has got too long.\n");
					// exit (EXIT_FAILURE);
				}
				/* Recursively call "dig" with the new path. */
				dig (path);
			}
		}
	}
	/* After going through all the entries, close the directory. */
	if (closedir (d)) {
		fprintf (stderr, "Could not close '%s': %s\n",
				 dir_name, strerror(errno));
		// exit (EXIT_FAILURE);
	}

}

#endif
