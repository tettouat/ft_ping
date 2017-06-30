#ifndef PING_H
#define PING_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

#define PACKETSIZE  64
#define WAIT_TO_RECEIVE 2
#define NB_PACKET 20

typedef struct sockaddr_in s_sockaddr_in;
typedef struct sockaddr s_sockaddr;
typedef struct icmphdr s_icmphdr;
typedef struct addrinfo s_addrinfo;

typedef struct	packet
{
	s_icmphdr	hdr;
	char 		msg[PACKETSIZE - sizeof(s_icmphdr)];
}				s_packet;

typedef struct 	info
{
	char		*ip;
	float		nb_packet;
	float		nb_good;
	float		nb_bad;
	double		start_s;
	double		start_ns;
	double		min;
	double		max;
	double		avg;
	double		mdev;
	double		previous;
}				s_infos;

extern s_infos	struct_infos;

unsigned short	checksum(void *b, int len);
void			display(int succeeded, const char *ip, int iter, double timedif, int ttl);
s_addrinfo		get_addr(const char *ip);
void			sighandler(int sig);
void			ping(s_addrinfo *addr_info, char *ip);

#endif
