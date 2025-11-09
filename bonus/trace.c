#include "ft_traceroute.h"

void send_trace(t_trace *trace, int ttl, int probe){
	char payload[PAYLOAD_SIZE];
	memset(payload, 0, PAYLOAD_SIZE);

	uint16_t dest_port = trace->base_port + (ttl - 1) * trace->nprobes + probe;
	struct sockaddr_in dest = trace->dest_addr;
	dest.sin_port = htons(dest_port);

	gettimeofday(&trace->start_time, NULL);

	ssize_t sent = sendto(trace->send_fd, payload, sizeof(payload), 0,
	                      (struct sockaddr *)&dest, sizeof(dest));
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

	stats->should_stop = 0;

	while (1) {
		ssize_t ret = recvfrom(trace->recv_fd, buffer, sizeof(buffer), 0,
								(struct sockaddr *)&from, &fromlen);

		if (ret < 0) {
			// Timeout normal du socket (SO_RCVTIMEO)
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return 0;

			if (errno == EINTR)
				return 0;  // Traiter comme timeout, pas comme erreur fatale

			perror("recvfrom");
			return -1;  // Erreur réelle du socket
		}

		gettimeofday(&recv_time, NULL);

		// Parser le paquet IP externe (celui du routeur qui répond)
		struct ip *ip_hdr = (struct ip *)buffer;
		int ip_header_len = ip_hdr->ip_hl * 4;

		// Min données pour hdr ICMP
		size_t min_icmp_size = (size_t)(ip_header_len + ICMP_MINLEN);
		if ((size_t)ret < min_icmp_size)
			continue;

		// Parser hdr ICMP de la réponse
		struct icmp *icmp_hdr = (struct icmp *)(buffer + ip_header_len);
		int type = icmp_hdr->icmp_type;
		int code = icmp_hdr->icmp_code;

		// Filtrer les types ICMP qui nous intéressent
		if (type != ICMP_TIME_EXCEEDED && type != ICMP_DEST_UNREACH)
			continue;

		// Extraire le paquet UDP original encapsulé dans le message ICMP
		// messages ICMP d'erreur contiennent hdr IP + 8 premiers octets
		// du paquet original qui a causé l'erreur
		struct ip *orig_ip = (struct ip *)(buffer + ip_header_len + ICMP_MINLEN);

		// Vérifier UDP original complet
		size_t min_udp_size = (size_t)(ip_header_len + ICMP_MINLEN +
								(orig_ip->ip_hl * 4) + sizeof(struct udphdr));

		if ((size_t)ret < min_udp_size)
			continue;

		struct udphdr *orig_udp = (struct udphdr *)((char *)orig_ip + (orig_ip->ip_hl * 4));
		uint16_t recv_port = ntohs(orig_udp->uh_dport);

		// Vérifier que c'est bien notre paquet en comparant l'IP de destination
		if (orig_ip->ip_dst.s_addr != trace->dest_addr.sin_addr.s_addr)
			continue;

		// Vérifier que le port correspond à nos probes
		if (recv_port < trace->base_port || recv_port >= trace->base_port + trace->max_ttl * trace->nprobes)
			continue;

		// OK -> Calculer le temps de réponse
		stats->triptime = (recv_time.tv_sec - trace->start_time.tv_sec) * 1000.0 +
							(recv_time.tv_usec - trace->start_time.tv_usec) / 1000.0;
		stats->addr = from;
		stats->icmp_type = type;
		stats->icmp_code = code;  // Sauvegarder le code ICMP

		// On a atteint la destination (Port Unreachable)
		if (type == ICMP_DEST_UNREACH && code == ICMP_PORT_UNREACH &&
		    from.sin_addr.s_addr == trace->dest_addr.sin_addr.s_addr) {
			stats->should_stop = 1;
		}

		// Erreurs -> Network/Host Unreachable, Protocol Unreachable, etc.
		if (type == ICMP_DEST_UNREACH && (
			code == ICMP_NET_UNREACH || code == ICMP_HOST_UNREACH ||
			code == ICMP_PROT_UNREACH || code == ICMP_NET_ANO ||
			code == ICMP_HOST_ANO || code == ICMP_PKT_FILTERED))
			stats->should_stop = 1;

		return 1;  // Paquet reçu
	}
}

void do_trace(t_trace *trace, t_stats *stats) {
	// Boucle sur les TTL croissants (hops)
	for (int ttl = 1; ttl <= trace->max_ttl && !g_data.interrupted; ttl++) {
		set_ttl(trace, ttl);
		printf("%2d  ", ttl);

		struct sockaddr_in current_hop = {0};
		int reached = 0;

		// Envoyer plusieurs probes pour ce TTL
		for (int probe = 0; probe < trace->nprobes; probe++) {
			// Vérifier si on a été interrompu (Ctrl+C, timeout externe, etc.)
			if (g_data.interrupted) {
				printf("\n");
				return;
			}

			send_trace(trace, ttl, probe);
			int ret = recv_trace(trace, stats);

			if (ret > 0) {
				// On a reçu une réponse ICMP

				// Afficher hop si premier probe OU si l'IP change -> load balancing
				if (probe == 0 || stats->addr.sin_addr.s_addr != current_hop.sin_addr.s_addr) {
					if (probe > 0)
						printf("  ");

					print_hop(trace, stats);
				}

				printf("  %.3fms", stats->triptime);

				current_hop = stats->addr;

				if (stats->addr.sin_addr.s_addr == trace->dest_addr.sin_addr.s_addr)
					reached = 1;

				// Si should_stop est activé, stop après ce hop
				if (stats->should_stop)
					reached = 1;

				// (option -w here) On met le délai SEULEMENT si on a reçu une réponse && != last probe
				if (probe < trace->nprobes - 1 && trace->here > 0)
					usleep(trace->here * 1000000);
			}
			else if (ret == 0)
				// Timeout : pas de réponse pour ce probe
				printf("  *");
			else {
				// Erreur fatale
				printf("\n");
				return;
			}
		}

		printf("\n");

		// dest ok -> arrêter le traceroute
		if (reached)
			break;

		// Délai entre différents TTL (option -w near)
		if (ttl < trace->max_ttl && trace->near > 0)
			usleep(trace->near * 1000000);
	}
}