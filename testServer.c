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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

	while (1)
	{
		socklen_t clen = sizeof(clientaddr);
		char fname[32];
		long n = recvfrom(sockfd, fname, 32, 0, (struct sockaddr*) &clientaddr, &clen);

		/* Checks for any recv error, not just timeout. */
		if (n == -1)
		{
			printf("Timed out while waiting to receive.\n");
		}
		else
		{
			printf("File request from client: %s\n", fname);

			if (access(fname, F_OK) != -1)
			{

				FILE *file = fopen(fname, "rb");
				struct stat st;
				int len;

				if(stat(fname, &st) == 0)
				{
					len = st.st_size;
				}
                int packet_num;
				int window_size = 5;
				int packet_info [4] = {-1, len, len/1024+1, window_size};
				struct packet msg;

				printf("%s contains %d bytes for %d packets\n", fname, packet_info[1], packet_info[2]);
				sendto(sockfd, packet_info, sizeof(int)*4+1, 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr));



                for (int i = 0; i < packet_info[2]; i++)
                {
                    if (packet_info[1] - i*1024 > 1024)
                    {
                        fread(msg.buffer, sizeof(char), 1024, file);
//                        printf("%s", msg.buffer);
//                        printf("\n\n%d\n\n", i);
                    }
                    else
                    {
                        printf("last packet");
                        fread(msg.buffer, sizeof(char), packet_info[1]-i*1024, file);
//                        printf("%s", msg.buffer);
//                        printf("\n\n%d\n\n", i);
                    }
                    sendto(sockfd, &msg, sizeof(struct packet)+1, 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
                }





				//END FREE

                fclose(file);
			}
			else
            {
				printf("The file '%s' could not be found.\n", fname);
			}

		}
	}
	return 0;
	}
