#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


int main(void) {
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *p;
    int sockfd;
    int status;

    // makesure hints is empty
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo("www.google.com", "http", &hints, &res)) != 0) {
        fprintf(stderr, "gai error %s\n", gai_strerror(status));
        exit(1);
    }

    // print ipv4
    char ip4[INET_ADDRSTRLEN];
    for (p = res; p->ai_next != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            inet_ntop(AF_INET, &(ipv4->sin_addr), ip4, INET_ADDRSTRLEN);

            printf("IPV4: %s\n", ip4);
        }
    }

    freeaddrinfo(res);
}