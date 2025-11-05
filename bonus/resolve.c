#include "ft_traceroute.h"

void resolve_hostname(t_trace *trace){

	struct addrinfo hints = {0};
	struct addrinfo *result;
	int status;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	status = getaddrinfo(trace->hostname, NULL, &hints, &result);
	if (status != 0) {
		fprintf(stderr, "trace: %s: %s\n", trace->hostname, gai_strerror(status));
		exit(2);
	}

	if (!result){
		fprintf(stderr, "trace: %s: No address found\n", trace->hostname);
		exit(EXIT_FAILURE);
	}

	memcpy(&trace->dest_addr, result->ai_addr, sizeof(struct sockaddr_in));

	freeaddrinfo(result);
}