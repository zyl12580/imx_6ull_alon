/*
 *Data : 2021-04-01
 *Complile command: gcc -o hello hello.c
 *How to execute: ./hello Alon
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
