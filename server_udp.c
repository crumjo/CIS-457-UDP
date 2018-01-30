/*
 * UDP Server that echos client input.
 *
 * @author Joshua Crum
 */

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "file_utils.h"



int main(int argc, char **argv)
{

    static int window_size = 5;
    int *send_buffer = (int*) malloc (5000 * sizeof(int));

	char packetTrack[];
	char confirmedTrack[];
	int lower = 0;
	int upper = 4;

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
		long n = recvfrom(sockfd, fname, 32, 0, (struct sockaddr*)&clientaddr, &len);

		/* Checks for any recv error, not just timeout. */
		if (n == -1) {
			printf("Timed out while waiting to receive.\n");
		}
        else {
			printf("File request from client: %s\n", fname);
            
            /* Check if file exists and then send its contents. */
            if (access(fname, F_OK) != -1) {
                /* File exists. */
                char *buffer = (char*) malloc (1024 * sizeof(char));
                int size = read_file(fname, &buffer);
                sendto(sockfd, buffer, size, 0, (struct sockaddr*) &clientaddr, sizeof(clientaddr));
            }
            else {
                /* Does not exist. */
                printf("File '%s' does not exist.\n", fname);
            }
		}
	}
    return 0;
}

	int packetTrack(char packetArray[], char confirmedPacketArray[]){
		//Place a 1 in packetArray for the packet being sent

		//Place the int value of the packet at the beginning of the packet

		//Send the packet

		//If a reply hasn't been recieved in 5 seconds, resend packet

		//If a confirmation comes back for a packet, place a 1 in the confirmedPacketArray,
		//check if the confirmed packet is equal to the lower bound of the window,
		//if it is, increment lower and check if there is a 1 in the confirmedPacketArray,
		//if there is, incrment lower again and loop till confirmedPacketArray has a zero

		return 0;
	}
