#include <errno.h>
#include <liburing.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <time.h>

#include <tandem/common.h>
#include <tandem/coro.h>
#include <tandem/io.h>
#include <tandem/scheduler.h>

static ssize_t io_res(td_rt *rt) {
  if (rt->current->io_res < 0) {
    errno = -rt->current->io_res;
    return -1;
  }
  return rt->current->io_res;
}

void td_sleep(td_rt *rt, size_t millis) {
  struct __kernel_timespec ts = {
    .tv_sec = millis / 1000,
    .tv_nsec = (millis % 1000) * 1000000ULL
  };
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_timeout(sqe, &ts, 0, 0);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);
  if (rt->current->io_res < 0) {
    errno = -rt->current->io_res;
  }
}

int td_close(td_rt *rt, int fd) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_close(sqe, fd);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_read(td_rt *rt, int fd, void *buf, size_t count) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_read(sqe, fd, buf, count, 0);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_write(td_rt *rt, int fd, const void *buf, size_t count) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_write(sqe, fd, buf, count, 0);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_pread(td_rt *rt, int fd, void *buf, size_t count, off_t offset) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_read(sqe, fd, buf, count, offset);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_pwrite(td_rt *rt, int fd, const void *buf, size_t count, off_t offset) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_write(sqe, fd, buf, count, offset);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_readv(td_rt *rt, int fd, const struct iovec *iov, int iovcnt) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_readv(sqe, fd, iov, iovcnt, -1);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_writev(td_rt *rt, int fd, const struct iovec *iov, int iovcnt) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_writev(sqe, fd, iov, iovcnt, -1);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_preadv(td_rt *rt, int fd, const struct iovec *iov, int iovcnt, off_t offset) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_readv(sqe, fd, iov, iovcnt, offset);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_pwritev(td_rt *rt, int fd, const struct iovec *iov, int iovcnt, off_t offset) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_writev(sqe, fd, iov, iovcnt, offset);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_openat(td_rt *rt, int dfd, const char *path, int flags, mode_t mode) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_openat(sqe, dfd, path, flags, mode);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_fsync(td_rt *rt, int fd, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_fsync(sqe, fd, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_socket(td_rt *rt, int domain, int type, int protocol) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_socket(sqe, domain, type, protocol, 0);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_bind(td_rt *rt, int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_bind(sqe, sockfd, (struct sockaddr *)addr, addrlen);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_listen(td_rt *rt, int sockfd, int backlog) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_listen(sqe, sockfd, backlog);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_accept(td_rt *rt, int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_accept(sqe, sockfd, addr, addrlen, 0);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_connect(td_rt *rt, int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_connect(sqe, sockfd, (struct sockaddr *)addr, addrlen);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_recv(td_rt *rt, int sockfd, void *buf, size_t len, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_recv(sqe, sockfd, buf, len, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_send(td_rt *rt, int sockfd, const void *buf, size_t len, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_send(sqe, sockfd, buf, len, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_recvmsg(td_rt *rt, int sockfd, struct msghdr *msg, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_recvmsg(sqe, sockfd, msg, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_sendmsg(td_rt *rt, int sockfd, const struct msghdr *msg, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_sendmsg(sqe, sockfd, msg, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_shutdown(td_rt *rt, int sockfd, int how) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_shutdown(sqe, sockfd, how);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_poll(td_rt *rt, int fd, unsigned int mask) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_poll_add(sqe, fd, mask);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

