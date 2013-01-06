#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libnet.h>
#include <pcap.h>

char *uname;
char *passwd;
int u_len;
int pw_len;
char *dev = "eth0";
int id;
pcap_t *handle;
libnet_t *l;
u_int8_t enet_src[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
u_int8_t enet_dst[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
char *err_buf;
struct bpf_program bp;

void getid(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet);
void get_id_addr(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet);
void get_ok(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet);
void start();
void identity();
void allocated();
void loop_identity();
void get_loop(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet);

int main(int argc,char *argy[])
{
  int id;
	uname = argy[1];
	passwd = argy[2];
	u_len = strlen(uname);
	pw_len = strlen(passwd);
	if (argc > 3)
		dev = argy[3];
	handle = pcap_open_live(dev, 65535, 0, 0, err_buf);
	if (handle == NULL)
	{
		printf("error: pcap_open_live(): %s\n", err_buf);
		exit(1);
	}
	l = libnet_init(LIBNET_LINK, dev, err_buf);
	if (l == NULL)
	{
		printf("error: libnet_init(): %s\n", err_buf);
		exit(1);
	}
	memcpy(enet_src, libnet_get_hwaddr(l)->ether_addr_octet, 6);
	start();
	identity();
	allocated();
	loop_identity();
	pcap_close(handle);
	libnet_destroy(l);
}

void getid(u_char *arg, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
	id = (int)packet[19];
}

void get_id_addr(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet)
{
	id = (int)(packet[19]);
	memcpy(enet_dst, packet+6, 6);
}

void get_ok(u_char *arg, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
	if (packet[18] != 3) {
		printf("Failure");
		exit(1);
	}
	libnet_ptag_t frame;
	int len;
	int pl_len;
	u_int8_t enet_brdcst[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u_int8_t head[9] = {0x00,0x01,0x08,0x00,0x06,0x04,0x00,0x01};
	u_int8_t *payload;
	len = 5 + u_len;
	pl_len = 4 + len;
	payload = (u_int8_t *)malloc(pl_len * sizeof(u_int8_t));
	head[2] = (u_int8_t)(len / 216);
	head[3] = (u_int8_t)(len % 216);
	head[6] = (u_int8_t)(len / 216);
	head[7] = (u_int8_t)(len % 216);
	head[5] = (u_int8_t)id;
	memcpy(payload, head, 9);
	memcpy(payload + 9, uname, u_len);
	libnet_clear_packet(l);
	frame = libnet_build_ethernet(
		enet_brdcst,
		enet_src,
		(u_int16_t)0x0806,
		(u_int8_t *)payload,
		(u_int32_t)pl_len,
		l,
		0
	);
	libnet_write (l);
}

void start()
{
	libnet_ptag_t frame;
	u_int8_t payload[4] = {0x01, 0x01, 0x00, 0x00};
	frame = libnet_build_ethernet(
		enet_dst,
		enet_src,
		(u_int16_t)0x888e,
		(u_int8_t *)payload,
		(u_int32_t)4,
		l,
		0
	);
	libnet_write (l);
	pcap_compile(handle, &bp, "ether proto 0x888e and ether dst 0025b36bc05d", 0, 0);
	pcap_setfilter(handle, &bp);
	pcap_loop(handle, 1, get_id_addr, NULL);
}

void identity()
{
	libnet_ptag_t frame;
	int len;
	int pl_len;
	u_int8_t head[9] = {0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x01};
	u_int8_t *payload;
	len = 5 + u_len;
	pl_len = 4 + len;
	payload = (u_int8_t *)malloc(pl_len * sizeof(u_int8_t));
	head[2] = (u_int8_t)(len / 216);
	head[3] = (u_int8_t)(len % 216);
	head[6] = (u_int8_t)(len / 216);
	head[7] = (u_int8_t)(len % 216);
	head[5] = (u_int8_t)id;
	memcpy(payload, head, 9);
	memcpy(payload + 9, uname, u_len);
	libnet_clear_packet(l);
	frame = libnet_build_ethernet(
		enet_dst,
		enet_src,
		(u_int16_t)0x888e,
		(u_int8_t *)payload,
		(u_int32_t)pl_len,
		l,
		0
	);
	libnet_write (l);
	pcap_loop(handle, 1, getid, NULL);
}

void allocated()
{
	libnet_ptag_t frame;
	int len;
	int pl_len;
	u_int8_t head[10] = {0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x07,0x0c};
	u_int8_t *payload;
	len = 6 + u_len + pw_len;
	pl_len = 4 + len;
	payload = (u_int8_t *)malloc(pl_len * sizeof(u_int8_t));
	head[2] = (u_int8_t)(len / 216);
	head[3] = (u_int8_t)(len % 216);
	head[6] = (u_int8_t)(len / 216);
	head[7] = (u_int8_t)(len % 216);
	head[5] = (u_int8_t)id;
	head[10] = (u_int8_t)pw_len;
	memcpy(payload, head, 10);
	memcpy(payload + 10, passwd, pw_len);
	memcpy(payload + 10 + pw_len, uname, u_len);
	libnet_clear_packet(l);
	frame = libnet_build_ethernet(
		enet_dst,
		enet_src,
		(u_int16_t)0x888e,
		(u_int8_t *)payload,
		(u_int32_t)pl_len,
		l,
		0
	);
	libnet_write (l);
	pcap_loop(handle, 1, get_ok, NULL);
}

void loop_identity()
{
	pcap_loop(handle, -1, get_loop, NULL);
}

void get_loop(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet)
{
	if (packet[18] == 4) {
		printf("Disconnected.");
		exit(1);
	} else if (packet[18] == 1 && packet[22] == 1) {
		id = (int)(packet[19]);
		libnet_ptag_t frame;
		int len;
		int pl_len;
		u_int8_t head[9] = {0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x01};
		u_int8_t *payload;
		len = 5 + u_len;
		pl_len = 4 + len;
		payload = (u_int8_t *)malloc(pl_len * sizeof(u_int8_t));
		head[2] = (u_int8_t)(len / 216);
		head[3] = (u_int8_t)(len % 216);
		head[6] = (u_int8_t)(len / 216);
		head[7] = (u_int8_t)(len % 216);
		head[5] = (u_int8_t)id;
		memcpy(payload, head, 9);
		memcpy(payload + 9, uname, u_len);
		libnet_clear_packet(l);
		frame = libnet_build_ethernet(
			enet_dst,
			enet_src,
			(u_int16_t)0x888e,
			(u_int8_t *)payload,
			(u_int32_t)pl_len,
			l,
			0
		);
		libnet_write (l);
	}
}
