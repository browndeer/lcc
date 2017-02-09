
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char lccsh[] = 
#include "_lcc.inc"
;

int main( int argc, char** argv )
{
	int i;

	size_t sz = sizeof(lccsh) + 1;

	for(i=1; i<argc; i++) 
		sz += 1 + strlen(argv[i]);

	char* cmd = (char*)malloc( sizeof(lccsh) + sz + 1 );

	strncpy(cmd,lccsh,sz);

	for(i=1; i<argc; i++) {
		strncat(cmd,argv[i],sz);
		strncat(cmd," ",sz);
	}

	system(cmd);

	return(0);
}


