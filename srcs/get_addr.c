#include "ft_ping.h"

s_addrinfo get_addr(const char *ip)
{
	s_addrinfo hints;
	s_addrinfo *result, *rp;

	memset(&hints, 0, sizeof(s_addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	int s = getaddrinfo(ip, NULL, &hints, &result);
	if (s != 0) {
		printf("ft_ping: unknown host %s\n", ip);
		exit(EXIT_FAILURE);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		int sfd = socket(rp->ai_family, rp->ai_socktype,
				rp->ai_protocol);
		if (sfd == -1)
			continue;

		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			close(sfd);
			break;
		}

		close(sfd);
	}

	if (rp == NULL) {
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}
	return (*rp);
}
