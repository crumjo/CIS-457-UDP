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

				FILE *file = fopen(fname, "rb");
				struct stat st;
				int len;

				if(stat(fname, &st) == 0) {
                    
                    int packet_num = 0;
                    int window_size = 5;
                    int packet_info [4] = {-1, len, len/1024, window_size};
                    struct packet msg;
                    
                    printf("File found: %s\n", fname);
                    len = st.st_size;
                    FILE *in_file = fopen(fname, "rb");
                    
                    /* Calculate file size. */
                    fseek(in_file, 0, SEEK_END);
                    long fsize = ftell(in_file);
                    rewind(in_file);
                    
                    /* Get file length. */
                    if(stat(fname, &st) == 0) {
                        len = st.st_size;
                    }
                    
                    /* Read file into buffer. */
                    char *buffer = (char *) malloc (fsize + 1);
                    fread(buffer, fsize, 1, in_file);
                    fclose(in_file);
                    
                    /* Buffer that holds all packets. */
                    struct packet *packet_buf = (struct packet *) malloc (packet_info[2] * sizeof(packet_info));
                    
                    /* Split buffer into packet structs and put in packet_buf. */
                    
                    
                    free(packet_buf);
                    free(buffer);
                    
                    /* Send packet_info. */
                    
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
