#include "ft_traceroute.h"

void cleanup (t_trace *trace){
	if (trace->send_fd) close(trace->send_fd);
	if (trace->recv_fd) close(trace->recv_fd);
}

int main(int ac, char **av) {
	t_trace trace = {
		.dest_addr = {0},
		.hostname = NULL,
		.send_fd = 0,
		.recv_fd = 0,
		.max_ttl = 30,
		.nprobes = 3,
		.waittime = 5.0,
		.near = 0.0,
		.here = 0.0,
		.base_port = 33434,
		.interface = NULL,
	};

	t_stats stats = {
		.addr = {0},
		.triptime = 0,
		.icmp_type = 0,
		.icmp_code = 0,
		.should_stop = 0
};

	parse_args(ac, av, &trace);

	// printf("======= DEBUG =======\n");
	// printf("  Hostname: %s\n", trace.hostname);
	// printf("  max_ttl: %d\n", trace.max_ttl);
	// printf("  nprobes: %d\n", trace.nprobes);
	// printf("  base_port: %d\n", trace.base_port);
	// printf("  interface: %s\n", trace.interface);
	// printf("  waittime: %f\n", trace.waittime);
	// printf("  near: %f\n", trace.near);
	// printf("  here: %f\n", trace.here);

	resolve_hostname(&trace);

	print_welcome(trace);

	create_socket(&trace);

	setup_signal(&trace, &stats);

	do_trace(&trace, &stats);

	cleanup(&trace);

	return 0;
}
