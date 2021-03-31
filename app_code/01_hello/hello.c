/*
Complile: gcc -o hello hello.c
Execute: ./hello Alon
*/

#include <stdio.h>

int main(int argc, char **argv)
{
	int i;
	if(argc >= 2){
		printf("Hello, %s!\n",argv[1]);
	}else{
		printf("Hello world!\n");
	}
	return 0;
}
