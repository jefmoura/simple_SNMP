#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define MAXMSG 1024

void dieError(char *message, int socket){
	printf("%s\n", message);
	if(socket != 0)
		close(socket);
	exit(1);
}

int createClientConnection(int port, char* address, struct sockaddr_in6* server_addr){
	int sockid;

	sockid = socket(PF_INET6, SOCK_DGRAM, 0);
	if(sockid == -1)
		dieError("socket() failed", sockid);

	memset(server_addr, 0, sizeof(*server_addr));
	/* it is an INET address */
	server_addr->sin6_family = AF_INET6;

	/* the server IP address, in network byte order */
	inet_pton(AF_INET6, address, &(server_addr->sin6_addr));

	/* the port we are going to send to, in network byte order */
	server_addr->sin6_port = htons(port);

	return sockid;
}

void sendRequest(int sock, struct sockaddr_in6 server_addr){

	if(sendto(sock, "request", 7, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		dieError("sendto() failed", sock);
	}
}

void getAnswer(int sock, struct sockaddr_in6* client_addr){
	socklen_t clilen = sizeof(*client_addr);
	char buffer[1024];
	int lenAns = 0;

	do {
		memset(buffer, 0, 1024);
		if(recvfrom(sock, buffer, 1024, 0, (struct sockaddr *) client_addr, &clilen) < 0) {
			dieError("recvfrom() failed", sock);
		} else
			if(strlen(buffer) > 0) {
				lenAns = (int)strtol(&buffer[0], NULL, 8);
				buffer[1024] = '\0';
				if(lenAns >= 0){
					printf("%s", buffer+1);
					if(lenAns == 0)
						break;
				}
			}
	} while(1);
}

int main(int argc, char **argv){
	struct sockaddr_in6 server_addr, client_addr;
	int port;
	sscanf(argv[2], "%d", &port);

	int sock = createClientConnection(port, argv[1], &server_addr);

	sendRequest(sock, server_addr);
	getAnswer(sock, &client_addr);
	close(sock);

	return 0;
}
