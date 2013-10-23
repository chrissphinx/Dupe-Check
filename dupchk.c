/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *	Main file for duplicate check. Takes single command line argument for the
 *	target directory and validates it. Then calls Dig() which locates all of
 *	the duplicate files (files with the same content) and symlinks them into
 *	the /dups folder.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "digger.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>

// GLOBALS ___________________________________________________________________
char dups[MAXPATHLEN];										// Oops

// MAIN ______________________________________________________________________
int main(int argc, char **argv) {
	// LOCALS
	char	tdir[MAXPATHLEN];
	struct	stat s;

	// Keep the path of our dups directory
	snprintf(dups, MAXPATHLEN, "%s/%s", getcwd(NULL, MAXPATHLEN), "dups");

	if(argc > 1) {
		// Append target directory to the current working directory
		snprintf(tdir, MAXPATHLEN, "%s/%s", getcwd(NULL, MAXPATHLEN), argv[1]);

		// Check that the target is valid
		stat(tdir, &s);
		if(errno) {
			// Stat returned an error
			perror("ERROR");
		} else {
			if(S_ISDIR(s.st_mode)) {
				// Target is a directory, dig out the duplicates
				if(Dig(tdir)) { perror("ERROR"); }
			} else {
				// Target is not a directory
				printf("ERROR: Target is not a directory");
			}
		}
	} else {
		// Display usage if no arguments are given
		printf("usage: dupchk [directory]\n");
	}

	CleanUp();
	return 0;
}
