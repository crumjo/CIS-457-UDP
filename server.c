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
	char *buffer;
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

	while (1) {
		socklen_t clen = sizeof(clientaddr);
		char fname[32];
		long n = recvfrom(sockfd, fname, 32, 0, (struct sockaddr*) &clientaddr, &clen);

		/* Checks for any recv error, not just timeout. */
		if (n == -1) {
			printf("Timed out while waiting to receive.\n");
		}
		else {
			printf("File request from client: %s\n", fname);

			if (access(fname, F_OK) != -1) {

				FILE *in_file = fopen(fname, "rb");
				struct stat st;
				int len;

				if(stat(fname, &st) == 0) {
                    
                    const int data_size = 1024;
//                    int packet_num = 0;
                    int num_packets = 0;
                    int rem = 0;
                    int window_size = 5;
                    struct packet tmp_packet;
                    
                    printf("File found: %s\n", fname);
                    len = st.st_size;
                    
                    /* Calculate file size. */
                    fseek(in_file, 0, SEEK_END);
                    long fsize = ftell(in_file);
                    rewind(in_file);
                    
                    /* Get file length. */
                    if(stat(fname, &st) == 0) {
                        len = st.st_size;
                    }
                    
                    /* Create initial packet. Considers packet with less than 1024 size at end. */
                    if (fsize % data_size != 0) {
                        num_packets = fsize / data_size;
                        rem = fsize - (num_packets * data_size);
                        num_packets++;
                    }
                    else {
                        num_packets = fsize / data_size;
                    }
                    
                    int packet_info [4] = {-1, fsize, num_packets, window_size};
                    
                    /* Read file into buffer. */
                    char *buffer = (char *) malloc (fsize + 1);
                    fread(buffer, fsize, 1, in_file);
                    fclose(in_file);
                    
                    /* Buffer that holds five packets. */
//                    struct packet *tmp_buf = (struct packet *) malloc (5 * sizeof(struct packet));
                    
                    /* Send initial packet information. */
                    printf("%d packet #, %d bytes, %d total packets, %d window size\n",
                           packet_info[0], packet_info[1], packet_info[2], packet_info[3]);
                    sendto(sockfd, packet_info, sizeof(int) * 4, 0, (struct sockaddr*) &clientaddr, sizeof(clientaddr));
                    
                    /* Split buffer into packet structs and put in packet_buf. */
                    char *c_packet_num = (char *) malloc (sizeof(int));
                    
                    /* Make sure server is sending correctly. */
                    FILE *test = fopen("test.pdf", "wb");
                    
                    for (int i = 0; i < num_packets; i++) {
                        
                        /* Check if last packet with remainder. */
                        if (rem != 0 && i == (num_packets)) {
                            printf("Remainder: %d\n", rem);
                            
                            tmp_packet.p_num = i;
                            tmp_packet.buffer = (char *) malloc (rem * sizeof(char) + 1);
                            
//                            strncpy(tmp_packet.buffer, buffer + (i * data_size), rem);
                            memcpy(tmp_packet.buffer, &buffer[i * data_size], rem * sizeof(char));
                            
//                            printf("Temp buf :%s:\n", tmp_packet.buffer);
                            
                            sendto(sockfd, tmp_packet.buffer, strlen(tmp_packet.buffer) + 1, 0, (struct sockaddr*) &clientaddr, sizeof(clientaddr));
                            
                            fwrite(tmp_packet.buffer, strlen(tmp_packet.buffer), 1, test);
                        }
                        else {
    
                            tmp_packet.p_num = i;
                            tmp_packet.buffer = (char *) malloc (1025 * sizeof(char));
                            
//                            strncpy(tmp_packet.buffer, buffer + (i * data_size), data_size);
                            memcpy(tmp_packet.buffer, &buffer[i * data_size], data_size * sizeof(char));
                            
//                            printf("Temp buf :%s:\n", tmp_packet.buffer);
                            
                            sendto(sockfd, tmp_packet.buffer, strlen(tmp_packet.buffer) + 1, 0, (struct sockaddr*) &clientaddr, sizeof(clientaddr));
                            
                            fwrite(tmp_packet.buffer, strlen(tmp_packet.buffer), 1, test);
                        }
//                        printf("Temp buf :%s:\n", tmp_packet.buffer);
                        /* Wait for packet number confirmation. */
                        recvfrom(sockfd, c_packet_num, sizeof(int), 0, (struct sockaddr*) &clientaddr, &clen);
                        printf("c_packet_num :%s:\n", c_packet_num);
                        int curr_packet_num = atoi(c_packet_num);
                        printf("Got packet: %d\n", curr_packet_num);
                        free(tmp_packet.buffer);
                    }
                    
                    fclose(test);
//                    free(tmp_buf);
                    free(buffer);
                    
                    /* Sliding window here. */
				}
				
			}
			else {
				printf("The file '%s' could not be found.\n", fname);
			}
		}
	}
	return 0;
}
