#define _POSIX_C_SOURCE 200112L

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

// Macros
#define PORT "8080"

int main(int argc, char **argv) {
  struct addrinfo hints, *res;
  int sockfd;
  int status;

  // make sure struct is empty
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  // addrinfo and socket setup
  if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
    fprintf(stderr, "gai error %s\n", gai_strerror(status));
    exit(1);
  }

  if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) ==
      -1) {
    perror("socket error");
    exit(2);
  }

  // Connect to Listener
  if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("connect error");
    exit(3);
  }
  return 0;
}