#define _POSIX_C_SOURCE 200112L

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Macros
#define PORT "8080"

int main(int argc, char **argv) {
  // get message and host info of listener
  char *host;
  char *port;
  char *message;

  if (argc < 4) {
    fprintf(stderr, "invalid args. usage: ./client [host] [port] [message]");
    exit(1);
  } 

  host = argv[1];
  port = argv[2];
  message = argv[3];

  // getaddrinfo and socket setup
  struct addrinfo hints, *res;
  int sockfd;
  int status;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo(host, port, &hints, &res)) != 0) {
    fprintf(stderr, "gai error %s\n", gai_strerror(status));
    exit(1);
  }

  if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) ==
      -1) {
    perror("socket error");
    exit(1);
  }

  // Connect to Listener
  if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("connect error");
    exit(1);
  }

  // Echo to Listener
  int msg_len = strlen(message);
  int bytes_sent;

  if ( (bytes_sent = send(sockfd, message, msg_len, 0)) == -1) {
    perror("send error");
    exit(1);
  }

  close(sockfd);
  freeaddrinfo(res);

  return 0;
}