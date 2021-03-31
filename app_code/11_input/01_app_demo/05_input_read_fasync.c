/* 2021-03-27 17:59 */

#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>


int fd;

void sig_handler(int sig)
{
	struct input_event event;
	while(read(fd,&event,sizeof(event)) == sizeof(event))
	{
		printf("get event: type = 0x%x, code = 0x%x, value = 0x%x \n",event.type,event.code,event.value);
	}
}

int main(int argc, char **argv)
{
	int err;
	int len;
	int i;
	unsigned char byte;
	int bit;
	int ret;
	struct input_id id;
	unsigned int evbit[2];
	int count = 0;
	unsigned int flags;
	
	char *ev_names[] = {
		"EV_SYN ",
		"EV_KEY ",
		"EV_REL ",
		"EV_ABS ",
		"EV_MSC ",
		"EV_SW	",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"EV_LED ",
		"EV_SND ",
		"NULL ",
		"EV_REP ",
		"EV_FF	",
		"EV_PWR ",
	};

	if(argc != 2)
	{
		printf("Usage: %s <dev>\n",argv[0]);
		return -1;
	}

	fd = open(argv[1],O_RDWR | O_NONBLOCK);
	if (fd < 0)
	{
		printf("open %s err\n", argv[1]);
		return -1;
	}

	if(ioctl(fd, EVIOCGID, &id) == 0)
	{
		printf("bustype = 0x%x\n",id.bustype);
		printf("vendor = 0x%x\n",id.vendor);
		printf("produce = 0x%x\n",id.product);
		printf("version = 0x%x\n",id.version);
	}

	len = ioctl(fd, EVIOCGBIT(0,sizeof(evbit)),&evbit);
	if(len > 0  && len <= sizeof(evbit))
	{
		printf("support ev type: ");
		for(i = 0; i < len; i++)
		{
			byte = ((unsigned char *)evbit)[i];
			for(bit = 0; bit < len;  bit++)
			{
				if(byte & (1<<bit))
					printf("%s ",ev_names[i*8 + bit]);
			}
		}
		printf("\n");
	}

	signal(SIGIO, sig_handler);
	fcntl(fd,F_SETOWN,getpid());

	flags = fcntl(fd,F_GETFL);
	fcntl(fd,F_SETFL,flags| FASYNC);
	while(1)
	{
		printf("while loop count = %d \n",count++);
		sleep(3);
	}
	close(fd);
	return 0;
}

