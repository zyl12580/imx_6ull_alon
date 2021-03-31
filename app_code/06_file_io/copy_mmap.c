#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>

/* ./copy 1.txt 2.txt
*/

int main(int argc, char **argv)
{	
	int fd_old, fd_new;
	struct stat stat;
	char *buf;

	//check usage
	if(argc !=3 )
	{
		printf("Usage: %s <old-flie> <new-flie>\n",argv[0]);
		return -1;
	}

	//open old file
	fd_old = open(argv[1],O_RDONLY);
	if(fd_old == -1)
	{
		printf("cannot open %s\n",argv[1]);
		return -1;
	}

	//make sure the voluom of the old file
	if(fstat(fd_old, &stat) == -1)
	{
		printf("cannot get stat of file %s",argv[1]);
		return -1;
	}

	//mmap old file 
	buf = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd_old,0);
	if(buf == MAP_FAILED)
	{
		printf("cannot mmap file %s\n", argv[1]);
		return -1;
	}

	//create new file 
	fd_new = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP
	| S_IWGRP | S_IROTH | S_IWOTH);
	if(fd_new == -1)
	{
		printf("cannot create file %s\n",argv[2]);
		return -1;
	}

	//write new file 
	if(write(fd_new, buf ,stat.st_size) != stat.st_size)
	{
		printf("cannot write %s\n",argv[2]);
		return -1;
	}

	//close file
	close(fd_old);
	close(fd_new);
	return 0;	
}
