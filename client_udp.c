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



//recievePackets(char buffer, int lower, int upper, array packetRecieved[]){
//    //Check the array to see if the packet has already been recieved.
//
//    //If the packet has been recieved, drop the packet and send a confirmation.  Return();
//
//    //If the packet hasn't been recieved, continue with steps below.
//
//    //Convert buffer to string here
//
//    //Remove first character from string to get packet number
//
//    //check if it is the lower
//
//    //If it is the lower, increment lower and increment upper, add 1 to array
//
//    //Check if the next slot of the array is a 1
//
//    //If it is, increment the lower and upper.  Then check the next value in the array (Use a loop)
//
//    //If not, move on.
//
//    //If it isn't the lower, add a 1 to the array
//
//    //Write buffer to file
//
//    //Send confirmation to server
//}


int main(int argc, char **argv)
{
	int port_num;
	char temp[5];
	char client_ip[16];
	int lower = 0;
	int upper = 4;

	char packets[];
    
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
    char *buffer = (char*) malloc (1042 * sizeof(char));
    
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
    
	socklen_t len = sizeof serveraddr;
    
    /* Send file name request to server. */
	sendto(sockfd, fname, strlen(fname) + 1, 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    
    /* Receive contents of file (if it exists) from server. */
    //int n = recvfrom(sockfd, buffer, 5000, 0, (struct sockaddr*)&serveraddr, &len);

//    while(buffer != NULL){
//        recievePackets(buffer, lower, upper, packets);
//    }
    
    long n = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)&serveraddr, &len);
    
    printf("Got from server: \n :%s: \n", buffer);
    free(buffer);
	close(sockfd);
	return 0;
}

	recievePackets(char buffer, int lower, int upper, char packetRecieved[]){
		//Check the array to see if the packet has already been recieved.

		//If the packet has been recieved, drop the packet and send a confirmation.  Return();

		//If the packet hasn't been recieved, continue with steps below.

		//Convert buffer to string here

		//Remove first character from string to get packet number

		//check if it is the lower

		//If it is the lower, increment lower and increment upper, add 1 to array

		//Check if the next slot of the array is a 1

		//If it is, increment the lower and upper.  Then check the next value in the array (Use a loop)

		//If not, move on.

		//If it isn't the lower, add a 1 to the array

		//Write buffer to file

		//Send confirmation to server
	}
