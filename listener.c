#define _POSIX_C_SOURCE 200112L

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "8080"
#define BACKLOG 10
#define MAXBUFSIZE 100

// PURPOSE: LEARN SOCKEt IMPLEMENTATION
int main(void) {
  // -----ADDRINFO AND SOCKET SETUP-----
  struct addrinfo hints;
  struct addrinfo *res;
  // struct addrinfo *p;
  int status;

  // makesure hints is empty
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // check if args were inputted
  char host[255];
  const size_t HOSTNAME_MAX = 255;
  gethostname(host, HOSTNAME_MAX);
  printf("%s\n", host);

  if ((status = getaddrinfo(host, PORT, &hints, &res)) != 0) {
    fprintf(stderr, "gai error %s\n", gai_strerror(status));
    exit(1);
  }

  // get socket file descriptor
  int sockfd;
  if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) ==
      -1) {
    perror("socket error");
    exit(1);
  }

  // ----- BIND To Machine -----
  if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("bind error");
    exit(1);
  }

  // ----- Listen and Accept Incoming Connections-----
  while (1) {
    if (listen(sockfd, BACKLOG) == -1) {
      perror("listen error");
      exit(1);
    }

    struct sockaddr conn_addr;
    struct sockaddr_storage conn_adds;
    socklen_t conn_addrlen = sizeof(conn_adds);
    int conn_fd;
    if ((conn_fd = accept(sockfd, &conn_addr, &conn_addrlen)) == -1) {
      perror("accept error");
      exit(1);
    }

    // Receive from Client
    int bytes_sent;
    char m_buf[MAXBUFSIZE];
    if ((bytes_sent = recv(conn_fd, m_buf, MAXBUFSIZE, 0)) == -1) {
      perror("recv error");
      exit(1);
    }

    m_buf[bytes_sent] = '\0';
    printf("client: received '%s'\n", m_buf);
  }
  
  close(sockfd);
  freeaddrinfo(res);
  return 0;
}