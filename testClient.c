/*
 * Client that sends data to a server and then receives it back.
 *
 * @author Joshua Crum
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>



int main(int argc, char **argv)
{
    const int window_size = 5;
    int port_num;
    char temp[5];
    char client_ip[16];
    
    printf("Enter an IP address: ");
    fgets(client_ip, 16, stdin);
    printf("Enter a port number: ");
    fgets(temp, 5, stdin);
    port_num = atoi(temp);
    
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port_num);
    serveraddr.sin_addr.s_addr = inet_addr(client_ip);
    
    printf("Enter a file name: ");
    char fname[32];
    
    /* Flush stdin. */
    char tmp_name[16];
    fgets(tmp_name, 16, stdin);
    fgets(fname, 32, stdin);
    
    /* Remove trailing newline. */
    int l = (int)strlen(fname);
    if (fname[l - 1] == '\n') {
        fname[l - 1] = '\0';
    }
    
    socklen_t len = sizeof(serveraddr);
    
    /* Send file name request to server. */
    sendto(sockfd, fname, strlen(fname) + 1, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    
    /* Receive contents of file (if it exists) from server. */
    printf("Enter a new file name with proper extension: ");
    char new_file[32];
    fgets(new_file, 32, stdin);
    
    /* Remove trailing newline. */
    int nl = (int)strlen(new_file);
    if (new_file[nl - 1] == '\n') {
        new_file[nl - 1] = '\0';
    }

    /* Create a new file with user input. */
    FILE *file = fopen(new_file, "wb");
    char tmp[1024];
        
    /* Read byte array from server to buffer. */
    int i = 1;
    int i_size = 32 * sizeof(char);
    char *ack = (char*) malloc(3 * sizeof(char));
    char *initial = (char*) malloc(i_size);
    recvfrom(sockfd, initial, i_size, 0, (struct sockaddr*) &serveraddr, &len);
    
    char *s_num_packets, *s_file_length;
    s_num_packets = strtok(initial, "/");
    s_file_length = strtok(NULL, "/");
    int num_packets = atoi(s_num_packets);
    int file_length = atoi(s_file_length);
        
    free(initial);
    
    ssize_t bytes_recv;
    
    while((bytes_recv = recvfrom(sockfd, tmp, 1024, 0, (struct sockaddr*) &serveraddr, &len)) != -1) {
        sprintf(ack,"%d", i);
        fwrite(tmp, 1, bytes_recv, file);
        i++;
        sendto(sockfd, ack, 4, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    }
    
    free(ack);
    fclose(file);
    close(sockfd);
    return 0;
}
