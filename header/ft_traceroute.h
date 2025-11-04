#ifndef FT_TRACEROUTE_H
#define FT_TRACEROUTE_H

// #include <arpa/inet.h> // inet pton & ntop
// #include <ctype.h> // is_*
// #include <errno.h> // errno const
#include <getopt.h> // getopt()
// #include <math.h> // INFINITY
// #include <netdb.h>  // Pour getaddrinfo, struct addrinfo, gai_strerror
#include <netinet/ip.h> // IPPROTO_IP
#include <netinet/ip_icmp.h> // ICMP_ECHO, ICMP_ECHOREPLY
// #include <signal.h>  // Pour sigaction, SIGINT
// #include <stdint.h> // uint8_t, uint16_t, uint32_t
#include <stdio.h> // printf(), fprintf(), perror()
#include <stdlib.h> // exit(), atoi(), strtol()
#include <string.h> // memset(), memcpy(), strcmp(), strlen()
#include <unistd.h> // read(), write(), close(), usleep()
#include <sys/types.h> // id_t, size_t, ssize_t
#include <sys/time.h> //timeval
#include <sys/socket.h> // socket(), setsockopt(), sendto(), recvfrom()

#define PAYLOAD_SIZE 32  // Taille standard

typedef struct s_trace {
	// Mandatory --help -h -? -V
	char *hostname;
	int send_fd;
	int	recv_fd;
	int base_port;

	// Bonus
	int no_dns;		// -n NO DNS (0 par défaut) a faire
	int max_ttl;	// -m MAX_TTL (30 par défaut) a faire
	int nprobes;// -q PROBE PER HOP(3 par défaut) a faire
	char *interface;// -i INTERFACE (0 par défaut) a faire

	// Données internes
	struct sockaddr_in dest_addr;  // Adresse destination
	struct timeval start_time;
	struct timeval end_time;
} t_trace;

typedef struct s_stats {
} t_stats;

typedef struct s_global {
	t_trace *trace;
	t_stats *stats;
	int interrupted;
} t_global;

void	parse_args(int argc, char** argv, t_trace *trace);
void	print_version();
void	print_help();

#endif