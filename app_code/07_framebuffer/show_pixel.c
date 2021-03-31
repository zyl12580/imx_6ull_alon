#include <stdio.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

int fd_fb;
int line_width;
int pixel_width;
int screen_size;
unsigned char * fb_base;
static struct fb_var_screeninfo var;

void lcd_put_pixel(int x, int y ,unsigned int color)
{
	unsigned char *pen_8 =fb_base + y*line_width + x*pixel_width;
	unsigned short *pen_16;
	unsigned int *pen_32;

	unsigned int red, green, blue;

	pen_16 = (unsigned short *)pen_8;
	pen_32 = (unsigned int *)pen_8;

	switch(var.bits_per_pixel){
		case 8:
			*pen_8 = color;
			break;
		case 16:
			/* 565 */
			red = (color >> 16) & 0xff;
			green = (color >> 8) & 0xff;
			blue = (color >> 0) & 0xff;
			color = ((red>>3) << 11) | ((green >> 2) << 5) | (blue >>3);
			*pen_16 = color;
			break;
		case 32:
			*pen_32 = color;
			break;
		default:
			break;
	}
}
int main(int argc, char **argv)
{
	int i;
	fd_fb = open("/dev/fb0",O_RDWR);
	if(fd_fb < 0)
	{
		printf("cannot open /dev/fb0\n");
		return -1;
	}

	if(ioctl(fd_fb, FBIOGET_VSCREENINFO, &var))
	{
		printf("cannot get fb screen var\n");
		return -1;
	}
	line_width = var.xres * var.bits_per_pixel /8;
	pixel_width = var.bits_per_pixel /8;
	screen_size = var.xres *var.yres*var.bits_per_pixel/8;
	fb_base = (unsigned char*)mmap(NULL,screen_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd_fb,0);
	if(fb_base == (unsigned char*)-1){
		printf("cannot mmap\n");
		return -1;
	}

	memset(fb_base, 0xff, screen_size);

	
	for(i = 0; i < 300; i++)
		lcd_put_pixel(var.xres/2+i, var.yres/2,0xFF0000);
	
	munmap(fb_base,screen_size);

	close(fd_fb);
	return 0;
}
