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

int	recv_trace(t_trace *trace, t_stats *stats){
	struct timeval recv_time;
	char buffer[1024];
	struct sockaddr_in from;
	socklen_t fromlen = sizeof(from);

	ssize_t ret = recvfrom(trace->recv_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &fromlen);

	if (ret < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return 0;
		}
		perror("recvfrom");
		return -1;
	}

	gettimeofday(&recv_time, NULL);

	stats->triptime = (recv_time.tv_sec - trace->start_time.tv_sec) * 1000 +
						(recv_time.tv_usec - trace->start_time.tv_usec) / 1000.0;

	stats->addr = from;

	struct ip *ip_hdr = (struct ip *)buffer;
	int ip_header_len = ip_hdr->ip_hl * 4;
	struct icmp *icmp_hdr = (struct icmp *)(buffer + ip_header_len);

	int type = icmp_hdr->icmp_type;

	if (type == ICMP_TIME_EXCEEDED || type == ICMP_DEST_UNREACH){

		// Extraire le paquet UDP original encapsulé dans l'ICMP
		struct ip *orig_ip = (struct ip *)(buffer + ip_header_len + ICMP_MINLEN);
		struct udphdr *orig_udp = (struct udphdr *)((char *)orig_ip + (orig_ip->ip_hl * 4));

		uint16_t recv_port = ntohs(orig_udp->uh_dport);

		// Vérifier que c'est bien notre paquet
		if (orig_ip->ip_dst.s_addr == trace->dest_addr.sin_addr.s_addr &&
			recv_port >= trace->base_port &&
			recv_port < trace->base_port + trace->max_ttl * trace->nprobes) {
			return 1; // Réponse valide
		}
	}

	return 0; // Pas notre paquet, ignorer
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
				if (probe == 0 || stats->addr.sin_addr.s_addr != current_hop.sin_addr.s_addr) {
					print_hop(trace, stats);
				}
				printf("  %.3f ms", stats->triptime);
				current_hop = stats->addr;

				if (stats->addr.sin_addr.s_addr == trace->dest_addr.sin_addr.s_addr)
					reached = 1;
			} else {
				printf(" *");
			}

			//  Délai entre probes du même TTL
			if (probe < trace->nprobes - 1 && trace->here > 0) {
				usleep(trace->here * 1000000);
			}
		}

		printf("\n");

		if (reached) break;

		//  Délai entre différents TTL
		if (ttl < trace->max_ttl && trace->near > 0) {
			usleep(trace->near * 1000000);
		}
	}
}
