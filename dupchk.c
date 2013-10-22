#include "digger.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv) {
	char	*tdir = malloc(MAXPATHLEN);
	struct	stat s;

	if(argc > 1) {
		snprintf(tdir, MAXPATHLEN, "%s/%s", getcwd(NULL, MAXPATHLEN), argv[1]);

		stat(tdir, &s);
		if(errno) {
			perror("ERROR");
		} else {
			if(S_ISDIR(s.st_mode)) {
				dig(tdir);
			} else {
				printf("ERROR: Target is not a directory");
			}
		}
	} else {
		printf("usage: dupchk [directory]\n");
	}

	free(tdir);
	Clean();
	return 0;
}
