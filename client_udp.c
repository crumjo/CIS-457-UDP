/* 
 * Client that sends data to a server and then receives it back.
 * 
 * @author Joshua Crum
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>



int main(int argc, char **argv)
{
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
    
    printf("Enter a file name: ");
	char fname[32];
	char buffer[5000];
    
    /* Flush stdin. */
    char tmp[16];
    fgets(tmp, 16, stdin);
    fgets(fname, 32, stdin);
    
    /* Remove trailing newline. */
    int l = strlen(fname);
    if (fname[l - 1] == '\n') {
        fname[l - 1] = '\0';
    }
    
    printf("\nYou requested the file: %s\n", fname);
    
	int len = sizeof serveraddr;
    
    /* Send file name request to server. */
	sendto(sockfd, fname, strlen(fname) + 1, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    
    /* Receive contents of file (if it exists) from server. */
    //int n = recvfrom(sockfd, buffer, 5000, 0, (struct sockaddr*)&serveraddr, &len);
    
    printf("Got from server: \n :%s: \n", buffer);
    
	close(sockfd);
	return 0;
}
