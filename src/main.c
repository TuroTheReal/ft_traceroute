#include "ft_traceroute.h"

int main(int ac, char **av) {
	t_trace trace = {
		.dest_addr = {0},
		.hostname = NULL,
		.send_fd = 0,
		.recv_fd = 0,
		.no_dns = 0,
		.max_ttl = 30,
		.nprobes = 3,
		.base_port = 33434,
		.interface = NULL
	};

	t_stats stats = {
	};

	(void)stats;
	parse_args(ac, av, &trace);

	printf("======= DEBUG =======\n");
	printf("  Hostname: %s\n", trace.hostname);
	printf("  no-dns: %d\n", trace.no_dns);
	printf("  max_ttl: %d\n", trace.max_ttl);
	printf("  nprobes: %d\n", trace.nprobes);
	printf("  base_port: %d\n", trace.base_port);
	printf("  interface: %s\n", trace.interface);

	// trace.sockfd = create_socket(&trace);
	// resolve_hostname(&trace);

	// setup_signal(&trace, &stats);

	// gettimeofday(&trace.start_time, NULL);
	// trace.tv = trace.start_time;

	// do_trace(&trace, &stats);

	// print_stats(&trace, &stats);

	// cleanup(&trace);

	return 0;
}
