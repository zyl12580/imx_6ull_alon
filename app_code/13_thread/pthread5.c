#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

static char g_buf[1000];
static pthread_mutex_t g_tMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_tConVar = PTHREAD_COND_INITIALIZER;

static void *my_thread_func(void *data)
{
	while(1)
	{
		pthread_mutex_lock(&g_tMutex);
		pthread_cond_wait(&g_tConVar,&g_tMutex);
		printf("recv:%s\n",g_buf);
		pthread_mutex_unlock(&g_tMutex);
	}
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t tid;
	int ret;
	char buf[1000];

	
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
		fgets(buf,1000,stdin);
		pthread_mutex_lock(&g_tMutex);
		memcpy(g_buf,buf,1000);
		pthread_cond_signal(&g_tConVar);
		pthread_mutex_unlock(&g_tMutex);
	}
	return 0;
}

//gcc -o pthread pthread1.c -lpthread
//
