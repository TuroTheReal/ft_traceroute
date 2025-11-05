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
	printf("  -w MAX,HERE,NEAR	--wait=MAX,HERE,NEAR	MAX=Timeout for response, HERE=Time between probes, NEAR=Time between hops\n");
	printf("  -m max_ttl,		--max-hops=max_ttl	Set the max number of hops (max TTL to be reached). Default is 30\n");
	printf("  -q nqueries,		--queries=nqueries	Set the number of probes per each hop. Default is 3\n");
	printf("  -i device,		--interface=device	Specify a network interface to operate with\n");
	printf("  -V,			--version		Print program version\n");
	printf("  -?, -h,		--help			Give this help list\n");
}

const char *get_icmp_code(int icmp_type, int icmp_code) {
	// Type 3 : Destination Unreachable
	if (icmp_type == ICMP_DEST_UNREACH) {
		switch (icmp_code) {
			case ICMP_NET_UNREACH:      // Code 0
				return " !N";  // Network Unreachable
			case ICMP_HOST_UNREACH:     // Code 1
				return " !H";  // Host Unreachable
			case ICMP_PROT_UNREACH:     // Code 2
				return " !P";  // Protocol Unreachable
			case ICMP_PORT_UNREACH:     // Code 3
				return "";     // On n'affiche rien dans ce cas
			case ICMP_FRAG_NEEDED:      // Code 4
				return " !F";  // Fragmentation needed but DF set
			case ICMP_SR_FAILED:        // Code 5
				return " !S";  // Source Route Failed
			case ICMP_NET_UNKNOWN:      // Code 6
				return " !N";  // Destination Network Unknown
			case ICMP_HOST_UNKNOWN:     // Code 7
				return " !H";  // Destination Host Unknown
			case ICMP_HOST_ISOLATED:    // Code 8
				return " !I";  // Source Host Isolated
			case ICMP_NET_ANO:          // Code 9
				return " !N";  // Network Administratively Prohibited
			case ICMP_HOST_ANO:         // Code 10
				return " !H";  // Host Administratively Prohibited
			case ICMP_NET_UNR_TOS:      // Code 11
				return " !T";  // Network Unreachable for TOS
			case ICMP_HOST_UNR_TOS:     // Code 12
				return " !T";  // Host Unreachable for TOS
			case ICMP_PKT_FILTERED:     // Code 13
				return " !X";  // Communication Administratively Prohibited
			case ICMP_PREC_VIOLATION:   // Code 14
				return " !V";  // Precedence Violation
			case ICMP_PREC_CUTOFF:      // Code 15
				return " !C";  // Precedence Cutoff
			default:
				return "";
		}
	}

	// Type 11 : Time Exceeded = hop intermédiaire
	if (icmp_type == ICMP_TIME_EXCEEDED)
		return "";

	// Autres types ICMP non gérés
	return "";
}

void print_hop(t_stats *stats) {
	char hostname[NI_MAXHOST];
	char ip_str[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, &stats->addr.sin_addr, ip_str, sizeof(ip_str));
	int ret = getnameinfo((struct sockaddr *)&stats->addr, sizeof(stats->addr),
							hostname, sizeof(hostname),
							NULL, 0,  // On ne veut pas le port
							0);       // Flags : 0 = comportement par défaut

	if (ret == 0)
		// Format : hostname (IP) [code_icmp]
		printf("%s (%s)%s", hostname, ip_str, get_icmp_code(stats->icmp_type, stats->icmp_code));
	else
		// Format : IP [code_icmp]
		printf("%s%s", ip_str, get_icmp_code(stats->icmp_type, stats->icmp_code));
}
