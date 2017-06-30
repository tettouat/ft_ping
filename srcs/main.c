#include "ft_ping.h"

unsigned short checksum(void *b, int len)
{
	unsigned short *buf = b;
	unsigned int sum=0;
	unsigned short result;

	for ( sum = 0; len > 1; len -= 2 )
		sum += *buf++;
	if ( len == 1 )
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

void display(int succeeded, const char *ip, int iter, double timedif, int ttl)
{
	if (succeeded)
		printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
			PACKETSIZE, ip, iter + 1, ttl - 1, timedif);
	else
		printf("Request timeout for icmp_seq %d\n", iter + 1);
}

char	*ip_arg(int argc, char *argv[])
{
	for (int i = 1; i < argc; ++i) {
		char *str = argv[i];
		if (strcmp("-h", str) == 0) {
			printf("usage: %s [-h] destination\n", argv[0]);
			exit(0);
		}
		else if (strcmp("-v", str) == 0)
			;
		else
			return str;
	}
	printf("error, no argument\n");
	exit(EXIT_FAILURE);
}

int		main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("usage: %s <addr_info>\n", argv[0]);
		exit(0);
	}

	char *ip = ip_arg(argc, argv);
	s_addrinfo addr_info = get_addr(ip);
	ping(&addr_info, ip);
	return 0;
}
