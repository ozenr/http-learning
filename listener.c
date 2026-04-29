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
  // server settings
  struct addrinfo hints;
  struct addrinfo *res, *p;
  int status;
  int sockfd;
  int yes = 1;

  // client settings
  struct sockaddr_storage conn_addr;
  socklen_t conn_addrlen;
  int conn_fd;

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

  // get server addrinfo
  if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
    fprintf(stderr, "gai error %s\n", gai_strerror(status));
    exit(1);
  }

  // get server socket
  for (p = res; p != NULL; p = p->ai_next) {
    // get socket file descriptor
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
        -1) {
      perror("socket error");
      exit(1);
    }

    // release port immediately -> if re-opening server
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt error");
      exit(1);
    }

    // ----- BIND To Machine -----
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("bind error");
      continue;
    }

    break;
  }

  freeaddrinfo(res);

  // validate server info
  if (p == NULL) {
    fprintf(stderr, "server failed to bind\n");
    exit(1);
  }

  // Listen and Accept Incoming Connections
  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen error");
    exit(1);
  }

  // accept and persist loop
  char m_buf[MAXBUFSIZE + 1];
  int running = 1;
  while (running) {
    conn_addrlen = sizeof(conn_addr);
    if ((conn_fd = accept(sockfd, (struct sockaddr *)&conn_addr,
                          &conn_addrlen)) == -1) {
      perror("accept error");
      continue;
    }

    // get client machine name
    char *peername = get_client_hostname(&conn_addr, conn_addrlen);
    if (!peername) {
      peername = "Unknown Host";
    }

    printf("server: got connection from %s\n", peername);

    // Receive from Client
    int bytes_sent;
    while (1) {
      bytes_sent = recv(conn_fd, m_buf, MAXBUFSIZE+1, 0);
      if (bytes_sent == -1) {
        perror("recv error");
        break;
      }

      else if (bytes_sent == 0) {
        running = 0;
        printf("Client disconnnected\n");
        break;
      }

      m_buf[bytes_sent] = '\0';
      printf("client %s, received: %s", peername, m_buf);
    }

    // free client connection
    close(conn_fd);
    free(peername);
  }

  close(sockfd);
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