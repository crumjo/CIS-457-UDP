/* 
 * Client that sends data to a server and then receives it back.
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
	char client_ip[16];
	printf("Enter an IP address: ");
	fgets(client_ip, 16, stdin);
	printf("Enter a port number: ");
	fgets(temp, 5, stdin);
	port_num = atoi(temp);
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port_num);
	serveraddr.sin_addr.s_addr = inet_addr(client_ip);
	printf("Enter a message: ");
	char line[5000];
	char line2[5000];
	char tmp[10];
	fgets(tmp, 10, stdin);
	fgets(line, 5000, stdin);
	int len = sizeof serveraddr;
	sendto(sockfd, line, strlen(line) + 1, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	int n = recvfrom(sockfd, line2, 5000, 0, (struct sockaddr*)&serveraddr, &len);
	printf("Got from server: %s\n", line2);
	close(sockfd);
	return 0;
}
