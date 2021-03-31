#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

static void *my_thread_func(void *data)
{
	while(1)
	{
		sleep(1);
	}
}

int main(int argc, char **argv)
{
	pthread_t tid;
	int ret;
	//1. create thread
	ret = pthread_create(&tid,NULL,my_thread_func,NULL);
	if(ret)
	{
		printf("pthread create fail\n");
		return -1;
	}

	while(1)
	{
		sleep(1);
	}
}
