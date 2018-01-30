/*
 * UDP Server that echos client input.
 *
 * @author Joshua Crum
 * @author Tristan VanFossen
 */

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "file_utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char **argv)
{
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
		socklen_t len = sizeof(clientaddr);
		char fname[32];
		long n = recvfrom(sockfd, fname, 32, 0, (struct sockaddr*) &clientaddr, &len);

		/* Checks for any recv error, not just timeout. */
		if (n == -1) {
			printf("Timed out while waiting to receive.\n");
		}
		else {
			printf("File request from client: %s\n", fname);

			if (access(fname, F_OK) != -1) {

				FILE *file = fopen(fname, "rb");
				struct stat st;
				int len;

				if(stat(fname, &st) == 0) {
					len = st.st_size;
				}
				char temp_buf [1026];
				int packet_num = 0;
				int scaler = 0;
				int window_size = 5;
				int packet_info [4] = {-1, len, len/1024, window_size};

				//printf("%s contains %d bytes for %d packets\n", fname, len, len/1024+1);
				sendto(sockfd, packet_info, sizeof(int)*4, 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
				
				while (packet_num+(scaler*250) < packet_info[2])
				{
					printf("%d : %d\n", packet_num+(scaler*250), packet_info[2]);
					fseek(file, packet_num*1024, SEEK_SET);
					if (packet_num == 250)
					{
						packet_num = 0;
						scaler++;
					}

					for (int i = packet_num; i < packet_num + window_size && i < 250; i ++)
					{
						printf("On packet %d\n", i+(scaler*250));
						fread(temp_buf, sizeof(temp_buf), 1024, file);
						temp_buf[1024] = i;
						temp_buf[1025] = scaler;
						sendto(sockfd, temp_buf, 1026+1, 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
					}
					recvfrom(sockfd, &packet_num, sizeof(int)+1, 0, (struct sockaddr*) &clientaddr, &len);
			
				}
				
				fclose(file);
			}
			else {
				printf("The file '%s' could not be found.\n", fname);
			}
			
		}
	}
	return 0;
	}



 /*
		FILE *file = fopen(fname, "rb");
                
                fseek(file, 0, SEEK_END);
                long len = ftell(file);
                rewind(file);
                
                char *buffer = (char*) malloc ((len + 1) * sizeof(char));
                fread(buffer, len, 1, file);
                fclose(file);
                
                sendto(sockfd, buffer, len + 1, 0, (struct sockaddr*) &clientaddr, sizeof(clientaddr));
		*/
