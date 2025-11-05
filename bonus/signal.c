#include "ft_traceroute.h"

t_global g_data = {NULL, NULL, 0};

void handle_interrupt(int signo) {
	(void)signo;
	g_data.interrupted = 1;
}

void setup_signal(t_trace *trace, t_stats *stats) {
	struct sigaction sa_int;

	g_data.trace = trace;
	g_data.stats = stats;

	// Config structure sigaction
	memset(&sa_int, 0, sizeof(sa_int));
	sa_int.sa_handler = handle_interrupt; 
	sigemptyset(&sa_int.sa_mask);          // Pas de signaux bloqués pendant le handler
	sa_int.sa_flags = 0;                    // Pas de flags spéciaux

	if (sigaction(SIGINT, &sa_int, NULL) < 0) {
		perror("sigaction SIGINT");
		cleanup(trace);
		exit(EXIT_FAILURE);
	}

	if (sigaction(SIGQUIT, &sa_int, NULL) < 0) {
		perror("sigaction SIGQUIT");
		cleanup(trace);
		exit(EXIT_FAILURE);
	}

	if (sigaction(SIGTERM, &sa_int, NULL) < 0) {
		perror("sigaction SIGTERM");
		cleanup(trace);
		exit(EXIT_FAILURE);
	}

	if (sigaction(SIGALRM, &sa_int, NULL) < 0) {
		perror("sigaction SIGALRM");
		cleanup(trace);
		exit(EXIT_FAILURE);
	}
}
