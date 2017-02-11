
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char lolsh[] = 
#include "_lol.inc"
;

int main( int argc, char** argv )
{
	int i;

	size_t sz = sizeof(lolsh) + 10;

	for(i=1; i<argc; i++) 
		sz += 1 + strlen(argv[i]);

	char* cmd = (char*)malloc( sizeof(lolsh) + sz + 1 );

	strncpy(cmd,"bash -c '",sz);
	strncat(cmd,lolsh,sz);

	for(i=1; i<argc; i++) {
		strncat(cmd,argv[i],sz);
		strncat(cmd," ",sz);
	}

	strncat(cmd,"'",sz);

	system(cmd);

	return(0);
}


