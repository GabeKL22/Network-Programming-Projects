/*
Author:            Gabriel Leffew
Major:             Computer Science
Creation Date:     11/01/22
Due Date:          11/08/22
Course:            CSC328
Professor Name:    Dr. Frye
Assignment:        TCP_UDP QOTD HW#9
Filename:          qotd_GLeffew.c
Purpose:           This program utilizes sockets
                   to connect to a server and recieve 
                   a Quote of the day, users can choose 
                   whether they would like to use UDP or
                   TCP.
*/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 17

/*
    Function name: TCP
    Description:   Create a TCP socket to recieve
                   a Quote of the Day from the server.

                   **I Got a lot of this from Dr. Frye's
                     csc328/sockets/sum/client.c **

    Parameters:    char *hostname - pointer to the command line argument 
                   specifying the hostname to use.
    Return Value:  none, void function 
*/
void TCP(char *hostname) {
    printf("\nGetting QOTD using TCP:\n\n");
    int   sockfd, rv;
    struct sockaddr_in *servaddr;
    struct addrinfo *servaddrinfo = (struct addrinfo *)malloc(sizeof(struct addrinfo));
    struct addrinfo *hintsinfo = (struct addrinfo *)malloc(sizeof(struct addrinfo));
    char result[512]; //result from server (buff)

    // Setup servaddrinfo structure for hints parameter to getaddrinfo.
    // Used to provide additional information to getaddrinfo for specific
    // address needed for this application. 
    // Got this from Dr. Frye's /sum/client.c 
    hintsinfo->ai_family = AF_INET;
    hintsinfo->ai_flags = 0;
    hintsinfo->ai_protocol = 0;
    hintsinfo->ai_socktype = SOCK_STREAM;

    //get IP from hostname 
    if ((rv = getaddrinfo(hostname, NULL, hintsinfo, &servaddrinfo)) < 0) {
        printf("Error in getaddrinfo call: %s\n\nCheck that you have the right hostname.\n\n", gai_strerror(rv));
                exit(1);
    }
    //create TCP socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Error creating client socket");
            exit(1);
    }

    servaddr = (struct sockaddr_in *)servaddrinfo->ai_addr;
    servaddr->sin_port = htons(PORT); // asign port number 17

    //connect to server
    if ((rv = connect(sockfd, (struct sockaddr *) servaddr, sizeof(struct sockaddr))) == -1)  {  // error
        perror("Error connecting to server");
        exit(1);
    }  // end if

    if (recv(sockfd, result, sizeof(result), 0) == -1) {
        perror("Client: recieving");
    }

    printf("%s\n\n", result);
    //free memory malloc()
    free(servaddrinfo);
    free(hintsinfo);
}

/*
    Function name: UDP
    Description:   Create a UDP socket to recieve
                   a Quote of the Day from the server.

                   **I Got a lot of this from Dr. Frye's
                     csc328/sockets/UDP/client.c **

    Parameters:    char *hostname - pointer to the command line argument 
                   specifying the hostname to use.
    Return Value:  none, void function 
*/
void UDP(char *hostname) {
    printf("\nGetting QOTD using UDP:\n\n");
    char result[512];
    int sockfd, rv;
    // remote address of the server
    struct sockaddr_in server = {AF_INET, htons(PORT)};

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
	    perror("Error creating client socket");
        exit(1);
    }   // end if socket
      // send the character to the server
    if (sendto(sockfd, &result, 1, 0, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1) {
	    perror("Client: sending");
	    exit(1);
	}  // end if sendto

      // receive the message back
    if (recv(sockfd, &result, sizeof(result), 0) == -1) {
	  perror("Client: receiving");
	  exit(1);
	}  // end recv

    printf("%s\n\n", result);
}

/*
    Function name: main
    Description:   Take two command line arguments, one for
                   the host name, and one for the protocol (tcp or udp)
    Parameters:    int argc: amount of command line arguments
                   char *argv[1]: protocol
                   char *argv[2]: hostname
    Return Value:  0 
*/
int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("\nusage: %s <hostname> <tcp or udp>\n\n", argv[0]);
        exit(-1);
    }
    if (strcmp(argv[1], "udp") == 0) {
        UDP(argv[2]);
    }
    else if (strcmp(argv[1], "tcp") == 0) {
        TCP(argv[2]);
    }
    else {
        printf("Please enter a protocol: \n     ./filename <hostname> <tcp or udp>\n");
    }

}
