#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define MAXMSG  1024

//error message and kill program
void dieError(char *message, int socket){
	printf("%s\n", message);
	if(socket != 0)
		close(socket);
	exit(1);
}

//create a socket to server
int createServerConnection(int port, struct sockaddr_in6 *sin6){
	int sock;
	int sin6len;

	sock = socket(PF_INET6, SOCK_DGRAM, 0);
	if(sock == -1)
		dieError("socket() failed", sock);

	sin6len = sizeof(struct sockaddr_in6);
	memset(sin6, 0, sin6len);

	sin6->sin6_port = htons(port);
	sin6->sin6_family = AF_INET6;
	sin6->sin6_addr = in6addr_any;

	if(bind(sock, (struct sockaddr *) sin6, sin6len))
		dieError("bind() failed", sock);

	return sock;
}

char *get_data_from_file(FILE* fp, size_t size) {
    char *str;
    int ch;
    size_t len = 0;

    str = realloc(NULL, sizeof(char)*size);

    if(!str)
        return str;

    while(EOF!=(ch=fgetc(fp))){
        str[len++]=ch;

        if(len == size){
            str = realloc(str, sizeof(char)*(size+=16));
            if(!str)
                return str;
        }
    }

    str[len++]='\0';

    return realloc(str, sizeof(char)*len);
}

void generate_data() {
    if(access("output.txt", F_OK ) != -1)
        system("rm -r output.txt");

    system("hostname >> output.txt");
	system("w | head -1 >> output.txt");
	system("ifconfig | grep \"inet6 addr\">> output.txt" );
	system("ifconfig | grep \"inet addr\">> output.txt");
	system("df -kh >> output.txt");

}

char* read_from_client(int sock, struct sockaddr_in6 *client_addr) {
	char* buffer;
	socklen_t clilen = sizeof(*client_addr);
	buffer = (char*) malloc(7);

	if(recvfrom(sock, buffer, 7, 0, (struct sockaddr *) client_addr, &clilen) < 0) {
		dieError("recvfrom() failed", sock);
	}

	return buffer;
}

void send_to_client(int sock, struct sockaddr_in6 *client_addr){
	FILE *fp;
	char *msg;
	int numChunk = 0;
	int count;

	fp = fopen("output.txt", "r");
	msg = get_data_from_file(fp, 10);
	numChunk = (int)strlen(msg) / 1024;
	//number of chunks with added bytes
	//numChunk = ((int)strlen(msg)+numChunk) / 1024;

	for(count = 0; count <= numChunk; count++) {
		char sentMsg[1024];
		int numBytesCpy = ((int)strlen(msg) + numChunk) - (1024*count);
		if(numBytesCpy >= 1024)
			numBytesCpy = 1023;
		memset(sentMsg, 0, 1024);
		sprintf(sentMsg, "%x", (numChunk-count));
		memcpy(sentMsg+1, msg+(1023*count), numBytesCpy);
		sentMsg[1024] = '\0';

		if(sendto(sock, sentMsg, numBytesCpy+1, 0, (struct sockaddr *) client_addr, sizeof(*client_addr)) < 0) {
			dieError("sendto() failed", sock);
		}
	}
}

int main(int argc, char *argv[]) {
	int sock;
	int port = atoi(argv[1]);
	socklen_t clilen;
	struct sockaddr_in6 server_addr, client_addr;
	
	sock = createServerConnection(port, &server_addr);

	while(1){

		if(strcmp(read_from_client(sock, &client_addr), "request") == 0){
			generate_data();
			send_to_client(sock, &client_addr);
		}

		memset(&client_addr, 0, (int)sizeof(client_addr));
	}
	
	return 0;
}

