/*
*登陆和保持链接的程序
*参数三个
*第一个为用户名，如：xxxxxxx@shufe
*第二个为密码
*第三个为网卡名，默认eth0
*用法：shufenet xxxxxx@xxxxxx（用户名） *******（密码） eth0（网卡名）
*/

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
	return 0;
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
	libnet_build_ethernet(
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
	char *bpf_exp_head = "ether proto 0x888e and ether dst ";
	u_int8_t payload[4] = {0x01, 0x01, 0x00, 0x00};
	u_int8_t mac_str[12];
	u_int8_t bpf_exp[46] = {0};
	sprintf((char * __restrict__)mac_str, "%02x%02x%02x%02x%02x%02x",
					enet_src[0], enet_src[1], enet_src[2],
					enet_src[3], enet_src[4], enet_src[5]);
	memcpy(bpf_exp, bpf_exp_head, 33);
	memcpy(bpf_exp+33, mac_str, 12);
	libnet_build_ethernet(
		enet_dst,
		enet_src,
		(u_int16_t)0x888e,
		(u_int8_t *)payload,
		(u_int32_t)4,
		l,
		0
	);
	libnet_write (l);
	pcap_compile(handle, &bp, (const char *)bpf_exp, 0, 0);
	pcap_setfilter(handle, &bp);
	pcap_loop(handle, 1, get_id_addr, NULL);
}

void identity()
{
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
	libnet_build_ethernet(
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
	libnet_build_ethernet(
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
		int len;
		int pl_len;
		u_int8_t head[9] =
				{0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x01};
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
		libnet_build_ethernet(
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
