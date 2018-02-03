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
				int fsize = 0;

				if(stat(fname, &st) == 0)
				{
					fsize = st.st_size;
				}
                
                int packet_num = 0;
                int rem = 0;
                const int window_size = 5;
                int num_packets = (fsize / 1024);
                int upper = window_size;
                int lower = 0;
                int recv_num = 0;
                int buff_l = window_size;
                
                /* Calculate remainder. */
                if (fsize % 1024 != 0)
                {
                    rem = fsize - (num_packets * 1024);
                    num_packets++;
                    printf("Remainder: %d\n", rem);
                }
                
                int packets_left = num_packets;
                printf("Num Packets: %d \t Packets left: %d\n", num_packets, packets_left);
                
				int packet_info [4] = {-1, fsize, num_packets, window_size};
				struct packet msg;

				printf("%s contains %d bytes for %d packets\n", fname, fsize, num_packets);
				sendto(sockfd, packet_info, sizeof(int) * 4 + 1, 0, (struct sockaddr*) &clientaddr, sizeof(clientaddr));

                /* Buffer to hold packets. */
                struct packet *send_buf = (struct packet *) malloc (window_size * sizeof(struct packet));
                
                while(packets_left > 0)
                {
                    printf("Packets left: %d\n", packets_left);
                    if (packets_left > window_size)
                    {
                        for (int i = 0; i < window_size; i++)
                        {
                            msg.p_num = i;
                            fread(msg.buffer, sizeof(char), 1024, file);
                            send_buf[i] = msg;
                        }
                    }

                    /* Last array of packets to send. */
                    else
                    {
                        printf("Last buffer to send.\n");
                        buff_l = packets_left;
                        printf("Buffer length: %d\n", buff_l);
                        
                        /* Add what is left. */
                        for (int i = 0; i < buff_l; i++)
                        {
                            if (fsize - i * 1024 > 1024)
                            {
                                msg.p_num = i;
                                fread(msg.buffer, sizeof(char), 1024, file);
                                send_buf[i] = msg;
                            }
                            else
                            {
                                printf("Last packet.\n");
                                msg.p_num = i;
                                fread(msg.buffer, sizeof(char), rem, file);
                                send_buf[i] = msg;
                            }
                        }
                    }
                    
                    for (int bl = 0; bl < buff_l; bl++) {
                        printf("bl: %d\n", bl);
                        sendto(sockfd, &send_buf[bl], sizeof(struct packet) + 1, 0, (struct sockaddr*) &clientaddr, sizeof(clientaddr));
                        packets_left --;
//                        printf("Packets left: %d\n", packets_left);
                    }
                    
                    /* Wait for acknowledgement. */
                    
                    //Receiver sends a 5 if it is ready for the next window of packets.
                }
                
                
                
                    
//                    if (fsize - i * 1024 > 1024)
//                    {
//                        fread(msg.buffer, sizeof(char), 1024, file);
////                        printf("%s", msg.buffer);
////                        printf("\n\n%d\n\n", i);
//                    }
//                    else
//                    {
//                        printf("Last packet.\n");
//                        fread(msg.buffer, sizeof(char), rem, file);
////                        printf("%s", msg.buffer);
////                        printf("\n\n%d\n\n", i);
//                    }
//                    sendto(sockfd, &msg, sizeof(struct packet) + 1, 0, (struct sockaddr*) &clientaddr, sizeof(clientaddr));


                
                
                fclose(file);
                free(send_buf);
			}
			else
            {
				printf("The file '%s' could not be found.\n", fname);
			}

		}
	}
	return 0;
	}
