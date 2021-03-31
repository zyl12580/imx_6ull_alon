#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

static char g_buf[1000];
static sem_t g_sem;
static pthread_mutex_t g_tMutex = PTHREAD_MUTEX_INITIALIZER;

static void *my_thread_func(void *data)
{
	while(1)
	{
		sem_wait(&g_sem);
		pthread_mutex_lock(&g_tMutex);
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

	
	sem_init(&g_sem,0,0);
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
		pthread_mutex_unlock(&g_tMutex);
		sem_post(&g_sem);
	}
	return 0;
}

//gcc -o pthread pthread1.c -lpthread
//
