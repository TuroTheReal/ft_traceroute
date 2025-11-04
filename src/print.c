#include "ft_traceroute.h"

void	print_version(){
	printf("Modern traceroute for Linux, version 1.0.0\nCopyright (c) 2025, Arthur Bernard.\n");
}

void print_help() {
	printf("Usage: ./ft_traceroute [options] <hostname>\n");
	printf("Options:\n");
	printf("  -n,		--no-dns		Do not resolve IP addresses to their domain names\n");
	printf("  -m max_ttl,	--max-hops=max_ttl	Set the max number of hops (max TTL to be reached). Default is 30\n");
	printf("  -q nqueries,	--queries=nqueries	Set the number of probes per each hop. Default is 3\n");
	printf("  -i device,	--interface=device	Specify a network interface to operate with\n");
	printf("  -V,		--version		Print program version\n");
	printf("  -?, -h, --help			Give this help list\n");
}
