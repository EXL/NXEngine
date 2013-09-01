
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <fcntl.h>

#include "../config.h"
#include "fbdev.fdh"

static int fd = -1;
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
static uint8_t *framebuffer = NULL;
static uint8_t *start_addr = NULL;
static int depth, screen_pitch;
static int vram_length;

bool fbdev_init(void)
{
	stat("Initilizing framebuffer...");
	
	fd = open("/dev/fb0", O_RDWR);
	if (fd == -1)
	{
		staterr("Could not open framebuffer.");
		return 1;
	}

	// Get fixed screen information
	if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo))
	{
		staterr("Could not read fixed screen info.");
		return 1;
	}

	// Get variable screen information
	if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo))
	{
		staterr("Could not read variable screen info.");
		return 1;
	}

	stat("Framebuffer %dx%d, %dbpp", vinfo.xres_virtual, vinfo.yres_virtual, vinfo.bits_per_pixel);
	depth = (vinfo.bits_per_pixel / 8);
	screen_pitch = (vinfo.xres_virtual * depth);
	vram_length = (vinfo.yres_virtual * screen_pitch);

	// Map the device to memory
	framebuffer = (uint8_t *)mmap(0, vram_length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (framebuffer == MAP_FAILED)
	{
		staterr("Framebuffer mmap failed!");
		return 1;
	}

	stat("Framebuffer init success!");
	start_addr = framebuffer;
	return 0;
}

void fbdev_close(void)
{
	if (framebuffer)
		munmap(framebuffer, vram_length);
	
	if (fd != -1)
		close(fd);
}

/*
void c------------------------------() {}
*/

void fbdev_blit(uint8_t *source, int width, int height)
{
	uint8_t *ptr = start_addr;
	int line_len = (width * depth);
	
	for(int y=0;y<height;y++)
	{
		memcpy(ptr, source, line_len);
		
		source += line_len;
		ptr += screen_pitch;
	}
}

void fbdev_test(void)
{
	stat("Performing fbdev test!");
	uint8_t *crap = (uint8_t *)malloc(40000);
	
	uint64_t end = timer() + 1000;
	int c = 281;
	int fps = 0;
	
	while(timer() < end)
	{
		uint8_t *ptr = framebuffer;
		for(int y=0;y<480;y++)
		{
			memcpy(ptr, crap, 640*2);
			ptr += 1024*2;
		}
		
		c ^= 4;
		fps++;
	}
	
	stat("Got %d fps", fps);
}



static uint64_t timer(void)
{
struct timeval tv;
uint64_t result;

	gettimeofday(&tv, NULL);
	
	result = tv.tv_sec;
	result *= 1000;
	result += (tv.tv_usec / 1000);
	return (uint64_t)result;
}










