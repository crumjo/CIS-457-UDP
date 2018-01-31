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

	/* Flush stdin. */
	char tmp[16];
	fgets(tmp, 16, stdin);
	fgets(fname, 32, stdin);

	/* Remove trailing newline. */
	int l = (int)strlen(fname);
	if (fname[l - 1] == '\n') {
		fname[l - 1] = '\0';
	}

	printf("\nYou requested the file: %s\n", fname);

	socklen_t len = sizeof(serveraddr);

	/* Send file name request to server. */
	sendto(sockfd, fname, strlen(fname) + 1, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	int packet_info[4];
	int packet_num = 0;
	
	recvfrom(sockfd, packet_info, sizeof(int)*4+1, 0, (struct sockaddr*) &serveraddr, &len);
	printf("%d packet #, %d bytes, %d total packets, %d window size\n",
		packet_info[0], packet_info[1], packet_info[2], packet_info[3]);

	char** packet_storage = (char**)malloc(sizeof(char)*packet_info[2]);
	char temp_buf[1024];
	struct packet msg;
	FILE* file = fopen ("temp.pdf", "wb");
	
	if (packet_info[1] > 1024)
	{
		while (packet_num <= packet_info[2])
		{
			recvfrom(sockfd, &msg, sizeof(struct packet), 0, (struct sockaddr*) &serveraddr, &len);
			
			if (packet_num == msg.p_num)
			{
				packet_num++;
				sendto(sockfd, &msg.p_num, sizeof(int) + 1, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
				fwrite(msg.buffer, sizeof(char), sizeof(msg.buffer), file);
			}
			else
			{
				sendto(sockfd, &packet_num, sizeof(int) + 1, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
			}
				
		}
		sendto(sockfd, &msg.p_num, sizeof(int) + 1, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

	}
	else
	{
		printf("Packet less than 1024\n");
	}
	fclose(file);
	close(sockfd);
	return 0;
}


/*

    printf("Enter a new file name with proper extension: ");
    char new_file[32];
    fgets(new_file, 32, stdin);
    

    int nl = (int)strlen(new_file);
    if (new_file[nl - 1] == '\n') {
        new_file[nl - 1] = '\0';
    }


    FILE *file = fopen(new_file, "wb");
    char *buffer = (char*) malloc (1024 * sizeof(char));
    
    recvfrom(sockfd, buffer, 1000000, 0, (struct sockaddr*) &serveraddr, &len);
    int buf_size = 1000000;
    fwrite(buffer, 1, buf_size, file);
    fclose(file);
    */
