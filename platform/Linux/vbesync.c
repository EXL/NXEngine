
#include <stdio.h>
#include <fcntl.h>

#if 0
#include <libdrm/drm.h>
#endif

#include <errno.h>
#define DRM_VBLANK_RELATIVE		0x1
#if 0
void platform_sync_to_vblank(void)
{
static char initilized = 0;
static int fd;
int ret;

	if (!initilized)
	{
		initilized = 1;
		
		printf("Linux vbesync: opening DRM card interface...\n");
		fd = open("/dev/dri/card0", O_RDWR);
		if (fd == -1)
		{
			printf("Failed to open DRM interface; sync-to-vblank will not be done.\n");
			return;
		}
		
		printf("Linux vbesync: DRM-based sync-to-vblank activated.\n");
	}
	else if (fd == -1)
	{
		return;
	}
	
	drm_wait_vblank_t vbl;
	
	vbl.request.type = DRM_VBLANK_RELATIVE;
	vbl.request.sequence = 1;
	
	do
	{
		ret = ioctl(fd, DRM_IOCTL_WAIT_VBLANK, vbl);
		vbl.request.type &= ~DRM_VBLANK_RELATIVE;
	}
	while(ret && errno == EINTR);
	
}
#endif
