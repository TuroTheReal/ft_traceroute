#include "ft_traceroute.h"

void	send_trace(t_trace *trace, int ttl, int probe){
	char payload[PAYLOAD_SIZE];
	memset(payload, 0, PAYLOAD_SIZE);

	uint16_t dest_port = trace->base_port + (ttl - 1) * trace->nprobes + probe;
	struct sockaddr_in dest = trace->dest_addr;
	dest.sin_port = htons(dest_port);

	gettimeofday(&trace->start_time, NULL);

	ssize_t sent = sendto(trace->send_fd, payload, sizeof(payload), 0, (struct sockaddr *)&dest, sizeof(dest));
	if (sent < 0){
		perror("sendto");
		cleanup(trace);
		exit(EXIT_FAILURE);
	}
}

int recv_trace(t_trace *trace, t_stats *stats){
	struct timeval recv_time;
	char buffer[1024];
	struct sockaddr_in from;
	socklen_t fromlen = sizeof(from);

	while (1) {
		ssize_t ret = recvfrom(trace->recv_fd, buffer, sizeof(buffer), 0,
							   (struct sockaddr *)&from, &fromlen);

		if (ret < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return 0; // Timeout
			}
			perror("recvfrom");
			return -1;
		}

		gettimeofday(&recv_time, NULL);

		// Parser le paquet IP
		struct ip *ip_hdr = (struct ip *)buffer;
		int ip_header_len = ip_hdr->ip_hl * 4;

		// Vérifier qu'il y a assez de données pour ICMP
		size_t min_icmp_size = (size_t)(ip_header_len + ICMP_MINLEN);
		if ((size_t)ret < min_icmp_size) {
			continue;
		}

		// Parser l'en-tête ICMP
		struct icmp *icmp_hdr = (struct icmp *)(buffer + ip_header_len);
		int type = icmp_hdr->icmp_type;

		if (type != ICMP_TIME_EXCEEDED && type != ICMP_DEST_UNREACH) {
			continue;
		}

		// Extraire le paquet UDP original encapsulé
		struct ip *orig_ip = (struct ip *)(buffer + ip_header_len + ICMP_MINLEN);

		// Vérifier qu'il y a assez de place pour l'en-tête UDP
		size_t min_udp_size = (size_t)(ip_header_len + ICMP_MINLEN +
								(orig_ip->ip_hl * 4) + sizeof(struct udphdr));
		if ((size_t)ret < min_udp_size) {
			continue;
		}

		struct udphdr *orig_udp = (struct udphdr *)((char *)orig_ip + (orig_ip->ip_hl * 4));
		uint16_t recv_port = ntohs(orig_udp->uh_dport);

		// Vérifier que c'est bien notre paquet
		if (orig_ip->ip_dst.s_addr != trace->dest_addr.sin_addr.s_addr) {
			continue;
		}

		if (recv_port < trace->base_port ||
			recv_port >= trace->base_port + trace->max_ttl * trace->nprobes) {
			continue;
		}

		// C'est notre paquet !
		stats->triptime = (recv_time.tv_sec - trace->start_time.tv_sec) * 1000.0 +
						  (recv_time.tv_usec - trace->start_time.tv_usec) / 1000.0;
		stats->addr = from;
		stats->icmp_type = type;

		return 1;
	}
}

void do_trace(t_trace *trace, t_stats *stats) {
	for (int ttl = 1; ttl <= trace->max_ttl; ttl++) {
		set_ttl(trace, ttl);
		printf("%2d  ", ttl);

		struct sockaddr_in current_hop = {0};
		int reached = 0;

		for (int probe = 0; probe < trace->nprobes; probe++) {
			send_trace(trace, ttl, probe);
			int ret = recv_trace(trace, stats);

			if (ret > 0) {
				// Afficher le hop si c'est le premier probe OU si l'IP change
				if (probe == 0 || stats->addr.sin_addr.s_addr != current_hop.sin_addr.s_addr) {
					if (probe > 0) {
						printf("  ");
					}
					print_hop(stats);
				}

				printf("  %.3fms", stats->triptime);
				current_hop = stats->addr;

				if (stats->addr.sin_addr.s_addr == trace->dest_addr.sin_addr.s_addr)
					reached = 1;

				// Sleep "here" SEULEMENT si on a reçu une réponse
				// ET qu'on n'est pas au dernier probe
				if (probe < trace->nprobes - 1 && trace->here > 0) {
					usleep(trace->here * 1000000);
				}
			}
			else
				printf("  *");
		}

		printf("\n");

		if (reached) break;

		// Délai entre différents TTL
		if (ttl < trace->max_ttl && trace->near > 0) {
			usleep(trace->near * 1000000);
		}
	}
}