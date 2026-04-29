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
#define MAXBUFSIZE 65535

char *get_client_hostname(struct sockaddr_storage *addr, socklen_t addr_len);

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

  if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
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
  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen error");
    exit(1);
  }

  char m_buf[MAXBUFSIZE];
  while (strcmp(m_buf, "exit") != 0) {
    // client settings
    struct sockaddr_storage conn_addr;
    socklen_t conn_addrlen = sizeof(conn_addr);
    int conn_fd;
    if ((conn_fd = accept(sockfd, (struct sockaddr *)&conn_addr,
                          &conn_addrlen)) == -1) {
      perror("accept error");
      exit(1);
    }

    // get client machine name
    char *peername = get_client_hostname(&conn_addr, conn_addrlen);

    // Receive from Client
    int bytes_sent;
    if ((bytes_sent = recv(conn_fd, m_buf, MAXBUFSIZE, 0)) == -1) {
      perror("recv error");
      exit(1);
    }

    m_buf[bytes_sent] = '\0';
    printf("client %s: received '%s'\n", peername, m_buf);
    free(peername);
  }

  close(sockfd);
  freeaddrinfo(res);

  printf("exiting\n");
  return 0;
}

// get_client_hostname(addr, addr_len) returns a heap-allocated string of the
// client's hostname;
//  otherwise returns NULL. heap-allocated string must be freed.
char *get_client_hostname(struct sockaddr_storage *addr, socklen_t addr_len) {
  const size_t BUF_SIZE = 1025;
  char *hostname = malloc(BUF_SIZE);

  if (getnameinfo((struct sockaddr *)addr, addr_len, hostname, BUF_SIZE, NULL,
                  0, NI_NAMEREQD) != 0) {
    free(hostname);
    return NULL;
  } else {
    return hostname;
  }
}