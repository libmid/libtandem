#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/tandem/coro.h"
#include "../include/tandem/scheduler.h"

#define STACK_SIZE 32 * 1024
#define PORT 6968

void handle_connection(td_rt *rt) {
  int clientfd = *(int *)td_argv(rt);
  free(rt->current->argv);
  char buf[1024];
  while (true) {
    int size = td_read(rt, clientfd, &buf, sizeof(buf));

    if (size <= 0) {
      printf("ERROR: Read from client / Connection closed\n");
      break;
    }

    printf("[CLIENT]: ");
    for (int i = 0; i < size; i++) {
      printf("%c", buf[i]);
    }

    size = td_write(rt, clientfd, &buf, size);
    if (size <= 0) {
      printf("ERROR: Write to client\n");
      break;
    }
  }

  td_close(rt, clientfd);
}

void server(td_rt *rt) {
  int sockfd, clientfd;
  struct sockaddr_in server;

  sockfd = td_socket(rt, AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    printf("ERROR: Can not create socket\n");
    return;
  }
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(PORT);

  if (td_bind(rt, sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    printf("ERROR: Can not bind socket\n");
    return;
  }

  if (td_listen(rt, sockfd, 10) < 0) {
    printf("ERROR: Can not listen to socket\n");
    return;
  }

  char *server_ip = inet_ntoa(server.sin_addr);
  printf("[SERVER] Stared at %s:%d\n", server_ip, ntohs(server.sin_port));

  while (1) {
    printf("[SERVER] Waiting for connection\n");
    struct sockaddr_in *client =
        (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    socklen_t len = sizeof(struct sockaddr);
    if ((clientfd = td_accept(rt, sockfd, (struct sockaddr *)client, &len)) <
        0) {
      printf("ERROR: Can not accept connection\n");
      break;
    }
    char *client_ip = inet_ntoa(client->sin_addr);
    printf("[SERVER] Accepted new connection from client %s:%d\n", client_ip,
           ntohs(client->sin_port));

    int *cfd = (int *)malloc(sizeof(int));
    *cfd = clientfd;
    td_spawn(rt, &handle_connection, 1, cfd, STACK_SIZE);

    free(client);
  }

  td_close(rt, sockfd);
}

int main() {
  td_rt *rt = td_init();
  td_sch_init(rt);

  td_spawn(rt, &server, 0, NULL, STACK_SIZE);

  td_sch_run(rt);

  td_free(rt);
  return 0;
}
