
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main( int argc, char** argv ) 
{
	int i;

	char* infile = argv[1];
	char* outfile = argv[2];

	int fd = open(infile,O_RDONLY);

	struct stat fs;

	fstat(fd,&fs);

	size_t sz = fs.st_size;

	char* buf = (char*)malloc(sz);

	read(fd,buf,sz);
	
	close(fd);
	
	fd = creat(outfile,S_IRUSR|S_IWUSR);

	char q = '\"';
	char b = '\\';
	char n = 'n';

	write(fd,&q,1);

	for(i=0; i<sz; i++) {
		char c = buf[i];
		
		if (c=='\n') {

			if (i != sz-1) {
				write(fd,&b,1);
				write(fd,&n,1);
			}

			write(fd,&q,1);
			write(fd,&c,1);

			if (i != sz-1) 
				write(fd,&q,1);

		} else if (c==q || c==b)  {
			write(fd,&b,1);
			write(fd,&c,1);

		} else {

			write(fd,&c,1);

		}

	}

	close(fd);		

	return(0);

}

