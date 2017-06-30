#include "ft_ping.h"

s_infos	struct_infos;

void	sighandler(int sig)
{
	struct timespec iend={0,0};
	clock_gettime(CLOCK_MONOTONIC, &iend);
	double time = ((double)(iend.tv_sec - struct_infos.start_s) * 1000000 + (double)(iend.tv_nsec - struct_infos.start_ns) / 1000) / 1000;
	signal(sig, SIG_IGN);
	double average = struct_infos.avg / struct_infos.nb_good;
	double mdev = struct_infos.mdev / struct_infos.nb_good;
	printf("\n--- %s ping statistics ---\n", struct_infos.ip);
	printf("%.f packets transmitted, %.f received, %.f%% packet loss, time %.fms\n",
		struct_infos.nb_packet, struct_infos.nb_good, ((struct_infos.nb_packet - struct_infos.nb_good) / struct_infos.nb_packet) * 100, time);
	if (struct_infos.nb_good > 0)
		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", struct_infos.min, average, struct_infos.max, mdev);
	else
		printf("\n");
	exit(0);
}

void ping(s_addrinfo *addr_info, char *ip)
{
	const int val=64;
	int sd, iter, i = 0;
	s_packet packet;
	int pid = getpid();
	struct timespec istart={0,0};
	clock_gettime(CLOCK_MONOTONIC, &istart);
	struct_infos.ip = ip;
	struct_infos.nb_packet = 1;
	struct_infos.nb_good = 0;
	struct_infos.nb_bad = 0;
	struct_infos.start_s = istart.tv_sec;
	struct_infos.start_ns = istart.tv_nsec;
	struct_infos.min = 0;
	struct_infos.max = 0;
	struct_infos.avg = 0;
	struct_infos.mdev = 0;

	sd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	if ( sd < 0 ) {
		perror("socket");
		return;
	}
	if (setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0)
		perror("Set TTL option");

	s_sockaddr_in *r_addr = (struct sockaddr_in *)addr_info->ai_addr;
	char adr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(r_addr->sin_addr), adr, INET_ADDRSTRLEN);
	printf("PING %s (%s) 56(84) bytes of data.\n" , ip, adr);
	
	while (1) {
		int len = sizeof(r_addr);
		struct timespec tstart={0,0}, tend={0,0};

		signal(SIGINT, sighandler);
		bzero(&packet, sizeof(packet));
		packet.hdr.type = ICMP_ECHO;
		packet.hdr.un.echo.id = pid;
		packet.hdr.un.echo.sequence = iter + 1;
		packet.hdr.checksum = checksum(&packet, sizeof(packet));
		if (sendto(sd, &packet, sizeof(packet), 0, addr_info->ai_addr, sizeof(*addr_info->ai_addr)) <= 0)
			perror("ping: sendmsg");
		clock_gettime(CLOCK_MONOTONIC, &tstart);

		struct timeval tv;
		tv.tv_sec = WAIT_TO_RECEIVE;
		tv.tv_usec = 0;
		setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));

		if (recvfrom(sd, &packet, sizeof(packet), 0, (s_sockaddr*)&r_addr, (socklen_t *)&len) > 0 ) {
			clock_gettime(CLOCK_MONOTONIC, &tend);
			double diff = ((double)(tend.tv_sec - tstart.tv_sec) * 1000000 + (double)(tend.tv_nsec - tstart.tv_nsec) / 1000) / 1000;
			if (struct_infos.previous != 0) {
				if(diff < struct_infos.previous)
					struct_infos.mdev += (struct_infos.previous - diff);
				else
					struct_infos.mdev += (diff - struct_infos.previous);
			}
			struct_infos.previous = diff;
			if (i == 0) {
				struct_infos.min = diff;
				i = 1;
			}
			if(struct_infos.min > diff)
				struct_infos.min = diff;
			else if(struct_infos.max < diff)
				struct_infos.max = diff;
			struct_infos.avg += diff;
			struct icmp *pkt;
			struct iphdr *iphdr = (struct iphdr *) &packet;
			pkt = (struct icmp *) (&packet + (iphdr->ihl << 2));
			if (pkt->icmp_type == ICMP_ECHOREPLY){
				struct_infos.nb_good++;
				display(1, ip, iter, diff, val);
			} else {
				struct_infos.nb_bad++;
				display(0, ip, iter, 0.0, val);
			}
		}
		else
		  display(0, ip, iter, 0.0, val);
		sleep(1);
		iter++;
		struct_infos.nb_packet++;
	}
}
