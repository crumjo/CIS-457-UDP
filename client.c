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

    while(tmp_port < 1023 || tmp_port > 49152) {
        if (tmp_port < 1023 || tmp_port > 49152) {
            printf("Please enter a valid port between 1023 and 49152");
            fgets(tmp_port, 5, stdin);
            port_num = atoi(tmp_port);
        }
    }

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

//    printf("You requested the file :%s:\n", fname);

	socklen_t len = sizeof(serveraddr);

	/* Send file name request to server. */
	sendto(sockfd, fname, strlen(fname) + 1, 0, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
    
    /* Receive packet information. */
    int packet_info[4];
//    int packet_num = 0;
    recvfrom(sockfd, packet_info, sizeof(int) * 4 + 1, 0, (struct sockaddr*) &serveraddr, &len);
    printf("%d packet #, %d bytes, %d total packets, %d window size\n",
           packet_info[0], packet_info[1], packet_info[2], packet_info[3]);
    
    /* Get size of window. */
//    int window_size = packet_info[3];
    
    printf("Enter a new file name with proper extension: ");
    char nfname[32];
    fgets(nfname, 32, stdin);
    
    int nf_len = (int)strlen(nfname);
    if (nfname[nf_len - 1] == '\n') {
        nfname[nf_len - 1] = '\0';
    }

    FILE *out_file = fopen(nfname, "wb");
    char *buffer = (char *) malloc (packet_info[1] + 1);
    char *tmp_buffer = (char *) malloc (1025);
    char *index = (char *) malloc (1);
    
    for (int i = 0; i < packet_info[2]; i++) {
        recvfrom(sockfd, tmp_buffer, 1025, 0, (struct sockaddr*) &serveraddr, &len);
        strcat(buffer, tmp_buffer);
//        printf("Temp buf :%s:\n", tmp_buffer);
        sprintf(index, "%d", i);
//        printf("Index :%s:\n", index);
        sendto(sockfd, index, strlen(index) + 1, 0, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
    }
    
    fwrite(buffer, packet_info[1], 1, out_file);
    
    free(index);
    free(tmp_buffer);
    free(buffer);
    fclose(out_file);
    close(sockfd);
	return 0;
}
