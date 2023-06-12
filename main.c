#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>


#define PORT 8001
#define IP "127.0.0.1"
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

int
main() {
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int sfd;
    int clientfd;
    int clientlen;
    char readbuff[1024];
 
	
    // Create socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        handle_error("Socket creation failed.")
    }

    // Setup address to bind
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Binding address
    if (bind(sfd, (struct sockaddr *) &server_addr,
                sizeof(server_addr)) != 0) {
        handle_error("Binding socket to address failed.");
    }

    // Listening for new connection
    if (listen(sfd, 10) != 0) {
        handle_error("Listening for new connection failed.");
    }

    // Accepting connection
    clientlen = sizeof(client_addr);
    clientfd = accept(sfd, (struct sockaddr *) &client_addr, &clientlen);
    if (clientfd == -1) {
        handle_error("Accepting connection failed.");
    }

    while(1) {
        bzero(readbuff, 1024);
        read(clientfd, readbuff, 1024);
        write(clientfd, readbuff, 1024);
    }

    close(sfd);
}
