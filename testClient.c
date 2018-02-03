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
    if (argc != 2)
    {
        printf("Please run the program with the new file name as arguments.\n");
        exit(0);
    }

    int port_num;
	char temp[5];
	char client_ip[16];

	printf("Enter an IP address: ");
	fgets(client_ip, 16, stdin);
	printf("Enter a port number: ");
	fgets(temp, 5, stdin);
	port_num = atoi(temp);

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

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port_num);
	serveraddr.sin_addr.s_addr = inet_addr(client_ip);

	printf("\nYou requested the file: %s\n", fname);

	socklen_t len = sizeof(serveraddr);

	/* Send file name request to server. */
	sendto(sockfd, fname, strlen(fname) + 1, 0, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
	int packet_info[4];
	struct packet msg;

	recvfrom(sockfd, packet_info, sizeof(int) * 4 + 1, 0, (struct sockaddr*) &serveraddr, &len);
    int fsize = packet_info[1];
    int num_packets = packet_info[2];
    int window = packet_info[3];
    int rem = 0;
    printf("%d packet #, %d bytes, %d total packets, %d window size\n", packet_info[0], fsize, num_packets, window);

    /* Calculate remainder. */
    if (fsize % 1024 != 0)
    {
        num_packets--;
        rem = fsize - (num_packets * 1024);
        num_packets++;
        printf("Remainder: %d\n", rem);
    }
    
	FILE* file = fopen(argv[1], "wb");
    
    for (int i = 0; i < num_packets; i++)
    {

        if (fsize - i * 1024 > 1024)
        {
            recvfrom(sockfd, &msg, sizeof(struct packet), 0, (struct sockaddr*) &serveraddr, &len);
            fwrite(msg.buffer, sizeof(char), 1024, file);
        }
        else
        {
            printf("Last packet.\n");
            recvfrom(sockfd, &msg, sizeof(struct packet), 0, (struct sockaddr*) &serveraddr, &len);
            fwrite(msg.buffer, sizeof(char), rem, file);
        }
    }
    fclose(file);
	close(sockfd);
	return 0;
}
