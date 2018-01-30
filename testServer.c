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
    const int window_size = 5;
    const int max_packet_size = 1024;
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
            /* Check if file exists and then send its contents. */
            if (access(fname, F_OK) != -1) {
                FILE *file = fopen(fname, "rb");
                
                /* Get file length. */
                fseek(file, 0, SEEK_END);
                long file_len = ftell(file);
                rewind(file);
                
                double r = ((double) file_len / 1024) + 1;
                char tmp[1024];
                char ack[3];
                
                /* Send initial size packet. */
                double num_packets = file_len / max_packet_size;
                char *initial = (char*) malloc (file_len + sizeof(num_packets));
                sprintf(initial, "%d", (int)num_packets);
                strcat(initial, "/");
                sprintf(initial + strlen(initial), "%ld", file_len);
                sendto(sockfd, initial, (strlen(initial) * sizeof(char)), 0, (struct sockaddr*) & clientaddr, sizeof(clientaddr));
                printf("Initial :%s:\n", initial);
                free(initial);
                
                while (fread(tmp, 1, 1024, file) == 1024) {
                    sendto(sockfd, tmp, 1024, 0, (struct sockaddr*) &clientaddr, sizeof(clientaddr));
                    
                    /* Wait until ack. */
                    recvfrom(sockfd, ack, 3, 0, (struct sockaddr*) &clientaddr, &len);
                }
                
                sendto(sockfd, tmp, r, 0, (struct sockaddr*) &clientaddr, sizeof(clientaddr));
                
            }
            else {
                printf("The file '%s' could not be found.\n", fname);
            }
        }
    }
    return 0;
}
