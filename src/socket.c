#include "ft_traceroute.h"

void set_ttl(t_trace *trace, int ttl){
	if (setsockopt(trace->send_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
		perror("setsockopt (timeout)");
		close(trace->send_fd);
		close(trace->recv_fd);
		exit(EXIT_FAILURE);
	}
}

void create_socket(t_trace *trace)
{
	int send_sock;
	int recv_sock;

	send_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (send_sock < 0) {
		perror("socket (UDP)");
		exit(EXIT_FAILURE);
	}

	if (trace->interface){
		if (setsockopt(send_sock, SOL_SOCKET, SO_BINDTODEVICE, trace->interface, strlen(trace->interface)) < 0) {
			perror("setsockopt SO_BINDTODEVICE");
			close(send_sock);
			exit(EXIT_FAILURE);
		}
	}
	trace->send_fd = send_sock;


	recv_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (recv_sock < 0) {
		perror("socket (ICMP)");
		close(send_sock);
		exit(EXIT_FAILURE);
	}

	struct timeval tv;
	tv.tv_sec = trace->waittime;
	tv.tv_usec = 0;

	if (setsockopt(recv_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
		perror("setsockopt (timeout)");
		close(send_sock);
		close(recv_sock);
		exit(EXIT_FAILURE);
	}
	trace->recv_fd = recv_sock;
}
