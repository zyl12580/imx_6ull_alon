#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

static char g_buf[1000];
static int g_hasData = 0;

static void *my_thread_func(void *data)
{
	while(1)
	{
		//sleep(1);
		while(g_hasData == 0);

		printf("recv:%s\n",g_buf);
		g_hasData = 0;
	}
	return NULL;
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
		//sleep(1);
		fgets(g_buf,1000,stdin);
		g_hasData = 1;
	}
	return 0;
}

//gcc -o pthread pthread1.c -lpthread
//
