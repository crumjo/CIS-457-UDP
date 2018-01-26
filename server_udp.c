/*
 * UDP Server that echos client input.
 * 
 * @author Joshua Crum
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int port_num;
	char temp[5];
	printf("Enter a port number: ");
	fgets(temp, 5, stdin);
	port_num = atoi(temp);
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	/* Set socket options. */
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	struct sockaddr_in serveraddr, clientaddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port_num);
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

	while (1) {
		int len = sizeof(clientaddr);
		char line[5000];
		int n = recvfrom(sockfd, line, 5000, 0, (struct sockaddr*)&clientaddr, &len);

		/* Checks for any recv error, not just timeout. */
		if (n == -1) {
			printf("Timed out while waiting to receive.\n");
		} else {
			printf("Got from client: %s\n", line);
			sendto(sockfd, line, strlen(line) + 1, 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
		}
	}

	return 0;
}
