/*下线程序 参数只有一个 网卡名称 默认eth0*/

#include <stdio.h>
#include <libnet.h>

char *dev = "eth0";
libnet_t *l;
u_int8_t enet_src[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
u_int8_t enet_dst[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
u_int8_t payload[4] = {0x01, 0x02, 0x00, 0x00};
char *err_buf;

int main(int argc,char *argy[])
{
  if (argc > 1)
		dev = argy[1];
	l = libnet_init(LIBNET_LINK, dev, err_buf);
	memcpy(enet_src, libnet_get_hwaddr(l)->ether_addr_octet, 6);
	libnet_ptag_t frame;
	frame = libnet_build_ethernet(
		enet_dst,
		enet_src,
		(u_int16_t)0x888e,
		(u_int8_t *)payload,
		(u_int32_t)4,
		l,
		0
	);
	if (frame == -1)
	{
		printf( "Cannot build a frame: %s\n",
							libnet_geterror(l));
		exit(1);;
	}
	int c;
	c = libnet_write (l);
	if (c == -1)
	{
		printf( "Write error: %s\n", libnet_geterror(l));
		exit(1);
	}
	libnet_destroy(l);
}
