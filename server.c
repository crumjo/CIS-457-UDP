/**
 * UDP Server that sends files to a client.
 *
 * @author Joshua Crum, Tristan VanFossen, Alex Fountain
 * @version 2/5/2018
 */

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <math.h>

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

	while(port_num < 1023 || port_num > 49152) {
        	if (port_num < 1023 || port_num > 49152) {
        	    printf("Please enter a valid port between 1023 and 49152");
        	    fgets(temp, 5, stdin);
        	    port_num = atoi(temp);
        	}
    	}

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
            int file_check;
			if (access(fname, F_OK) != -1)
			{
                file_check = 1;
                sendto(sockfd, &file_check, sizeof(int) + 1, 0, (struct sockaddr*) &clientaddr, sizeof(clientaddr));
				FILE *file = fopen(fname, "rb");
				struct stat st;
				int fsize = 0;

				if(stat(fname, &st) == 0)
				{
					fsize = st.st_size;
				}
                
                char tmp_num;
                int rem = 0;
                const int window_size = 5;
                int num_packets = (fsize / 1024);
                int buff_l = window_size;
                
                /* Calculate remainder. */
                if (fsize % 1024 != 0)
                {
                    rem = fsize - (num_packets * 1024);
                    num_packets++;
                }
                
                int packets_left = num_packets;
                
				int packet_info [4] = {-1, fsize, num_packets, window_size};
				struct packet msg;
                int x = 0;
				printf("%s contains %d bytes for %d packets\n\n", fname, fsize, num_packets);
				while (x != window_size)
                {
                    sendto(sockfd, packet_info, sizeof(int) * 4 + 1, 0, (struct sockaddr*) &clientaddr, sizeof(clientaddr));
                    recvfrom(sockfd, &x, sizeof(int), 0, (struct sockaddr*) &clientaddr, &clen);

                }

                /* Buffer to hold packets. */
                struct packet *send_buf = (struct packet *) malloc (window_size * sizeof(struct packet));
                
                /* Loops until all packets have been acknowledged. Will resend packets if not acknowledged. */
                while(packets_left > 0)
                {
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
                        buff_l = packets_left;
                        
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
                        printf("Sending packet with sequence number: %d\n", bl);
                        sendto(sockfd, &send_buf[bl], sizeof(struct packet) + 1, 0, (struct sockaddr*) &clientaddr, sizeof(clientaddr));
                    }
                    
                    /* Wait for acknowledgement. */
                    recvfrom(sockfd, &tmp_num, sizeof(char)*1, 0, (struct sockaddr*) &clientaddr, &clen);
                    printf("Ack: %d\n", tmp_num);
                    int packet_num = abs(tmp_num-48);
                    printf("Packet Number: %d \t Buffer Length: %d\n", packet_num, buff_l);
                    if (packet_num < buff_l)
                    {
                        packets_left-=packet_num;
                        fseek(file, (packet_info[2]-packets_left)*1024, SEEK_SET);
                        printf("Packet dropped.\n");
                    }
                    else
                    {
                        printf("All packets made it.\n\n\n");
                        packets_left -= buff_l;
                    }
                    printf("Packets Left: %d\n", packets_left);
                }
                
                fclose(file);
                free(send_buf);
			}
			else
            {
				printf("The file '%s' could not be found.\n", fname);
                file_check = 0;
                sendto(sockfd, &file_check, sizeof(int) + 1, 0, (struct sockaddr*) &clientaddr, sizeof(clientaddr));
            }

		}
	}
	return 0;
}
