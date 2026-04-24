#define _POSIX_C_SOURCE 200112L

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char **argv) {
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

  // check if args were inputted
  char *host;
  if (argc < 2) {
    host = "www.google.com";
  } else {
    host = argv[1];
  }

  if ((status = getaddrinfo(host, "http", &hints, &res)) != 0) {
    fprintf(stderr, "gai error %s\n", gai_strerror(status));
    exit(1);
  }

  // print ip addresses
  char ip[INET_ADDRSTRLEN];
  printf("HOST: %s\n", host);
  int count = 0;
  for (p = res; p->ai_next != NULL; p = p->ai_next) {
    if (p->ai_family == AF_INET) {
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      inet_ntop(AF_INET, &(ipv4->sin_addr), ip, INET_ADDRSTRLEN);

      printf("IPV4: %s\n", ip);
    } else {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      inet_ntop(AF_INET6, &(ipv6->sin6_addr), ip, INET6_ADDRSTRLEN);

      printf("IPV6: %s\n", ip);
    }
  }
  freeaddrinfo(res);
}