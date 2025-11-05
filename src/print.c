#include "ft_traceroute.h"

void	print_welcome(t_trace trace){
	char ip_str[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, &trace.dest_addr.sin_addr, ip_str, INET_ADDRSTRLEN);

	printf("traceroute to %s (%s), %d hops max, %d byte packets\n" , trace.hostname, ip_str, trace.max_ttl,
			PAYLOAD_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE);
}

void	print_version(){
	printf("Modern traceroute for Linux, version 1.0.0\nCopyright (c) 2025, Arthur Bernard.\n");
}

void	print_help() {
	printf("Usage: ./ft_traceroute [options] <hostname>\n");
	printf("Options:\n");
	printf("  -n,		--no-dns		Do not resolve IP addresses to their domain names\n");
	printf("  -m max_ttl,	--max-hops=max_ttl	Set the max number of hops (max TTL to be reached). Default is 30\n");
	printf("  -q nqueries,	--queries=nqueries	Set the number of probes per each hop. Default is 3\n");
	printf("  -i device,	--interface=device	Specify a network interface to operate with\n");
	printf("  -V,		--version		Print program version\n");
	printf("  -?, -h, --help			Give this help list\n");
}

void	print_hop(t_stats *stats) {
	char ip_str[INET_ADDRSTRLEN];
	char hostname[NI_MAXHOST];

	// Convertir l'IP en string
	inet_ntop(AF_INET, &stats->addr.sin_addr, ip_str, INET_ADDRSTRLEN);

	// Essayer de résoudre le hostname
	int ret = getnameinfo((struct sockaddr *)&stats->addr, sizeof(stats->addr),
							hostname, sizeof(hostname), NULL, 0, 0);

	// On a un hostname différent de l'IP
	if (ret == 0 && strcmp(hostname, ip_str) != 0) {
		printf("%s (%s)", hostname, ip_str);
	}
	else
		printf("%s", ip_str);
}