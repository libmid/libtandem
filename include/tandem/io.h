#ifndef TD_IO_H
#define TD_IO_H

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#include "common.h"

void td_sleep(td_rt *rt, size_t millis);
int td_close(td_rt *rt, int fd);
ssize_t td_read(td_rt *rt, int fd, void *buf, size_t count);
ssize_t td_write(td_rt *rt, int fd, const void *buf, size_t count);
ssize_t td_pread(td_rt *rt, int fd, void *buf, size_t count, off_t offset);
ssize_t td_pwrite(td_rt *rt, int fd, const void *buf, size_t count, off_t offset);
int td_openat(td_rt *rt, int dfd, const char *path, int flags, mode_t mode);
int td_fsync(td_rt *rt, int fd, unsigned int flags);

int td_socket(td_rt *rt, int domain, int type, int protocol);
int td_bind(td_rt *rt, int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int td_listen(td_rt *rt, int sockfd, int backlog);
int td_accept(td_rt *rt, int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int td_connect(td_rt *rt, int sockfd, const struct sockaddr *addr, socklen_t addrlen);
ssize_t td_recv(td_rt *rt, int sockfd, void *buf, size_t len, int flags);
ssize_t td_send(td_rt *rt, int sockfd, const void *buf, size_t len, int flags);
int td_shutdown(td_rt *rt, int sockfd, int how);

int td_poll(td_rt *rt, int fd, unsigned int mask);

#endif

