#ifndef TD_ASYNC_H
#define TD_ASYNC_H

#include "../common.h"

void td_sch_init(td_rt *rt);
void td_sch_loop(td_rt *rt);

void td_sleep(td_rt *rt, size_t millis);
int td_close(td_rt *rt, int fd);
ssize_t td_read(td_rt *rt, int fd, void *buf, size_t count);
ssize_t td_write(td_rt *rt, int fd, void *buf, size_t count);
int td_socket(td_rt *rt, int domain, int type, int protocol);
int td_bind(td_rt *rt, int sockfd, struct sockaddr *addr, socklen_t addrlen);
int td_listen(td_rt *rt, int sockfd, int backlog);
int td_accept(td_rt *rt, int sockfd, struct sockaddr *addr, socklen_t *addrlen);

#endif
