/*
 * Client that sends data to a server and then receives it back.
 *
 * @author Joshua Crum
 * @author Tristan VanFossen
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/uio.h>

#pragma pack(1)
struct packet {
	int p_num;
	char buffer[1024];
};
#pragma pack(0)


int main(int argc, char **argv)
{
	int port_num;
	char tmp_port[5];
	char client_ip[16];

	printf("Enter an IP address: ");
	fgets(client_ip, 16, stdin);
    
	printf("Enter a port number: ");
	fgets(tmp_port, 5, stdin);
	port_num = atoi(tmp_port);

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port_num);
	serveraddr.sin_addr.s_addr = inet_addr(client_ip);

	printf("Enter a file name: ");
	char fname[32];
    
    /* Flush stdin. */
    char tmp[16];
    fgets(tmp, 16, stdin);
    fgets(fname, 32, stdin);

	/* Remove trailing newline. */
	int l = (int)strlen(fname);
	if (fname[l - 1] == '\n') {
		fname[l - 1] = '\0';
	}

    printf("You requested the file :%s:\n", fname);

	socklen_t len = sizeof(serveraddr);

	/* Send file name request to server. */
	sendto(sockfd, fname, strlen(fname) + 1, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    
    printf("Enter a new file name with proper extension: ");
    char nfname[32];
    fgets(nfname, 32, stdin);
    
    int nf_len = (int)strlen(nfname);
    if (nfname[nf_len - 1] == '\n') {
        nfname[nf_len - 1] = '\0';
    }

    FILE *out_file = fopen(nfname, "wb");
    
    fclose(out_file);
    close(sockfd);
	return 0;
}
