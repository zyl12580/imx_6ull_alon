#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

static char g_buf[1000];
static sem_t g_sem;

static void *my_thread_func(void *data)
{
	while(1)
	{
		sem_wait(&g_sem);
		printf("recv:%s\n",g_buf);
	}
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t tid;
	int ret;
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
		fgets(g_buf,1000,stdin);
		sem_post(&g_sem);
	}
	return 0;
}

//gcc -o pthread pthread1.c -lpthread
//
