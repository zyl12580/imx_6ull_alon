/* 2021-03-27 17:26 */

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>



int main(int argc, char **argv)
{
	int fd;
	int err;
	int len;
	int i;
	unsigned char byte;
	int bit;
	struct input_id id;
	unsigned int evbit[2];
	struct input_event event;
	
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

	if(argc < 2)
	{
		printf("Usage: %s <dev>\n",argv[0]);
		return -1;
	}

	if(argc == 3 && !strcmp(argv[2],"noblock"))
		fd = open(argv[1],O_RDWR | O_NONBLOCK);
	else 
		fd = open(argv[1],O_RDWR);
	
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

	while(1)
	{
		len = read(fd, &event, sizeof(event));
		if(len == sizeof(event))
			printf("get event: type = 0x%x, code = 0x%x, value = 0x%x \n",event.type,event.code,event.value);
		else 
			printf("read err %d\n", len);
	}
	close(fd);
	return 0;
}

