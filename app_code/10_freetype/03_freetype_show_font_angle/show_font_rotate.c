/*2021-03-27 08:31*/
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <wchar.h>
#include <sys/ioctl.h>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H


int fd_fb;
int fd_hzk16;
int line_width;
int pixel_width;
int screen_size;
unsigned char * fb_base;
unsigned char * hzkmem;

static struct fb_var_screeninfo var;
struct stat hzk_stat;

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

#if 0
void lcd_put_ascii(int x, int y ,unsigned char c)
{
	unsigned char *dots = (unsigned char *)&fontdata_8x16[c*16];
	int i,b;
	unsigned char byte;

	for ( i= 0; i< 16; i++)
	{
		byte = dots[i];
		for(b = 7; b>= 0; b--)
		{
			if(byte & 1<<b)
			{
				lcd_put_pixel(x+7-b,y+i,0xffffffff);
			}else{
				lcd_put_pixel(x+7-b,y+i,0);
			}
		}
		
	}
}
#endif 
void lcd_put_chinese(int x, int y, unsigned char *str)
{
	unsigned int area = str[0] - 0xA1;
	unsigned int where = str[1] - 0xA1;
	unsigned char *dots = hzkmem + (area * 94 + where)*32;
	unsigned char byte;

	int i, j, b;
	for(i = 0; i < 16; i++)
	{
		for(j = 0; j < 2; j++)
		{
			byte = dots[i*2+j];
			for(b = 7; b>=0; b--)
			{
				if(byte & (1<<b))
				{
					lcd_put_pixel(x+j*8+7-b,y+i,0xff0000);
				}else{
					lcd_put_pixel(x+j*8+7-b,y+i,0);//black
				}
			}
		}
	}
	
}

void draw_bitmap(FT_Bitmap *bitmap, FT_Int x, FT_Int y)
{
	FT_Int i, j, p, q;
	FT_Int x_max = x + bitmap->width;
	FT_Int y_max = y + bitmap->rows;

	for(j=y, q=0; j<y_max;j++,q++)
	{
		for(i=x,p=0;i<x_max;i++,p++){
			if(i<0 || j<0 || x>=var.xres || j>=var.yres)
				continue;
			lcd_put_pixel(i, j, bitmap->buffer[q*bitmap->width + p]);
		}
	}
}
int main(int argc, char **argv)
{
	wchar_t *chinese_str = L"爱";

	FT_Library library;
	FT_Face face;
	FT_Vector pen;
	FT_GlyphSlot slot;
	int error;
	int font_size = 24;
    FT_Matrix matrix;
	double angle;
	if(argc < 2)
	{
		printf("Usage : %s <font_file> <angle> [font_size]\n", argv[0]);
		return -1;
	}

	if(argc == 4)
		font_size = strtoul(argv[3],NULL,0);

	fd_fb = open("/dev/fb0",O_RDWR);
	if(fd_fb < 0)
	{
		printf("cannot open dev/fb0\n");
		return -1;
	}

	if(ioctl(fd_fb, FBIOGET_VSCREENINFO,&var))
	{
		printf("cannot get screen info\n");
		close(fd_fb);
		return -1;
	}

	line_width = var.xres * var.bits_per_pixel/8;
	pixel_width = var.bits_per_pixel/8;
	screen_size = var.xres*var.yres*var.bits_per_pixel/8;
	printf("var %d, %d, %d \n",line_width,pixel_width,screen_size);

	fb_base = (unsigned char *)mmap(NULL , screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
	if (fb_base == (unsigned char *)-1)
	{
		printf("can't mmap\n");
		close(fd_fb);
		return -1;
	}

	/* clear screen ,set full  black pixel */
	memset(fb_base, 0, screen_size);

	
	FT_Init_FreeType(&library);
	FT_New_Face(library,argv[1],0,&face);
	slot = face->glyph;
	FT_Set_Pixel_Sizes(face,font_size,0);

	angle = (1.0*strtoul(argv[2],NULL,0)/360)*3.14159*2;
	pen.x = 0;
	pen.y = 0;
	matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
	matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
	matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
	matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

	FT_Set_Transform(face,&matrix,&pen);

	
	error = FT_Load_Char(face,chinese_str[0],FT_LOAD_RENDER);
	if (error)
	{
		printf("FT_Load_Char error\n");
		return -1;
	}


	
	draw_bitmap(&slot->bitmap,var.xres/3,var.yres/3);
	
	return 0;
}
//compile command
//arm-linux-gnueabihf-gcc -o show_font_rotate  show_font_rotate.c -lfreetype -lm





