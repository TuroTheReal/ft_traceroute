#ifndef FT_TRACEROUTE_H
#define FT_TRACEROUTE_H

#define _GNU_SOURCE        // Active les extensions GNU (dont SO_BINDTODEVICE)
#define _DEFAULT_SOURCE    // Active les définitions par défaut POSIX

#include <arpa/inet.h> // inet pton & ntop
#include <errno.h> // errno const
#include <getopt.h> // getopt()
#include <netdb.h>  // Pour getaddrinfo, struct addrinfo, gai_strerror
#include <netinet/ip.h> // IPPROTO_IP
#include <netinet/ip_icmp.h> // ICMP_ECHO, ICMP_ECHOREPLY
#include <netinet/udp.h>  // Pour struct udphdr
#include <signal.h>  // Pour sigaction, SIGINT
#include <stdint.h> // uint8_t, uint16_t, uint32_t
#include <stdio.h> // printf(), fprintf(), perror()
#include <stdlib.h> // exit(), atoi(), strtol()
#include <string.h> // memset(), memcpy(), strcmp(), strlen()
#include <unistd.h> // read(), write(), close(), usleep()
#include <sys/types.h> // id_t, size_t, ssize_t
#include <sys/time.h> //timeval
#include <sys/socket.h> // socket(), setsockopt(), sendto(), recvfrom()

#define PAYLOAD_SIZE 32		// Données à envoyer
#define IP_HEADER_SIZE 20	// En-tête IP standard (sans options)
#define UDP_HEADER_SIZE 8	// En-tête UDP

typedef struct s_trace {
	// Mandatory --help -h -? -V
	char	*hostname;
	int		send_fd;
	int		recv_fd;

	// Bonus
	char	*interface;// -i INTERFACE (0 par défaut) a faire
	int		max_ttl;	// -m MAX_TTL (30 par défaut) a faire
	int		base_port;	// -p PORT (33434 par défaut) a faire
	int		nprobes;	// -q PROBE PER HOP(3 par défaut) a faire
	int		waittime;	// -w MAX TIME to wait(5 par défaut) a faire
	double	here;       // -w deuxième param (secondes)a faire
	double	near;       // -w troisième param (secondes)a faire

	struct	sockaddr_in dest_addr;  // Adresse destination
	struct	timeval start_time;
	struct	timeval end_time;
} t_trace;

typedef struct s_stats {
	struct		sockaddr_in  addr;	   // Adresse du routeur
	double		triptime;
	int			timeout;
	uint8_t		icmp_type;
} t_stats;

typedef struct s_global {
	t_trace	*trace;
	t_stats	*stats;
	int		interrupted;
} t_global;

void	parse_args(int argc, char** argv, t_trace *trace);

void	print_version();
void	print_help();
void	print_welcome(t_trace trace);
void	print_hop(t_trace *trace, t_stats *stats);

void	create_socket(t_trace *trace);
void	set_ttl(t_trace *trace, int ttl);

void	resolve_hostname(t_trace *trace);

void	do_trace(t_trace *trace, t_stats *stats);

void	cleanup (t_trace *trace);

#endif