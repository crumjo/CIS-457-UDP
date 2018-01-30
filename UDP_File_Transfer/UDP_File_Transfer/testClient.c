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
    char *buffer = (char*) malloc (1024 * sizeof(char));
    
    /* Read byte array from server to buffer. */
    recvfrom(sockfd, buffer, 1000000, 0, (struct sockaddr*) &serveraddr, &len);
    int buf_size = 1000000;
    fwrite(buffer, 1, buf_size, file);
    fclose(file);
    
    close(sockfd);
    return 0;
}
