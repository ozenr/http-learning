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
#define MAXBUFSIZE 65535

int main(int argc, char **argv) {
  // get message and host info of listener
  char *host;
  char *port;
  if (argc != 3) {
    fprintf(stderr, "invalid args. usage: ./client [host] [port]\n");
    exit(1);
  }
  host = argv[1];
  port = argv[2];

  // client info setup
  struct addrinfo hints, *res, *p;
  int sockfd;
  int status;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo(host, port, &hints, &res)) != 0) {
    fprintf(stderr, "gai error %s\n", gai_strerror(status));
    exit(1);
  }

  for (p = res; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
      -1) {
      perror("socket error");
      continue;
    }

    // Connect to Listener
    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("connect error");
      close(sockfd);
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "getting addrinfo failed\n");
    exit(1);
  }

  // validate getaddrinfo 
  freeaddrinfo(res);

  // send loop
  while(1) {
    // Echo to Listener
    char buf[MAXBUFSIZE];
    int bytes_sent;

    printf("client message: ");
    if (fgets(buf, MAXBUFSIZE, stdin) == NULL) {
      printf("Closing connection\n");
      break;
    } else if (strcmp(buf, "exit\n") == 0 || strcmp(buf, "q\n") == 0) {
      printf("Closing connection\n");
      break;
    }

    if ((bytes_sent = send(sockfd, buf, strlen(buf), 0)) == -1) {
      perror("send error");
      break;
    } 
  }

  close(sockfd);
  return 0;
}