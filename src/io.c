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


ssize_t td_splice(td_rt *rt, int fd_in, int64_t off_in, int fd_out, int64_t off_out, unsigned int nbytes, unsigned int splice_flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_splice(sqe, fd_in, off_in, fd_out, off_out, nbytes, splice_flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_tee(td_rt *rt, int fd_in, int fd_out, unsigned int nbytes, unsigned int splice_flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_tee(sqe, fd_in, fd_out, nbytes, splice_flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_readv2(td_rt *rt, int fd, const struct iovec *iovecs, unsigned nr_vecs, __u64 offset, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_readv2(sqe, fd, iovecs, nr_vecs, offset, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_read_fixed(td_rt *rt, int fd, void *buf, unsigned nbytes, __u64 offset, int buf_index) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_read_fixed(sqe, fd, buf, nbytes, offset, buf_index);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_readv_fixed(td_rt *rt, int fd, const struct iovec *iovecs, unsigned nr_vecs, __u64 offset, int flags, int buf_index) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_readv_fixed(sqe, fd, iovecs, nr_vecs, offset, flags, buf_index);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_writev2(td_rt *rt, int fd, const struct iovec *iovecs, unsigned nr_vecs, __u64 offset, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_writev2(sqe, fd, iovecs, nr_vecs, offset, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_write_fixed(td_rt *rt, int fd, const void *buf, unsigned nbytes, __u64 offset, int buf_index) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_write_fixed(sqe, fd, buf, nbytes, offset, buf_index);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_writev_fixed(td_rt *rt, int fd, const struct iovec *iovecs, unsigned nr_vecs, __u64 offset, int flags, int buf_index) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_writev_fixed(sqe, fd, iovecs, nr_vecs, offset, flags, buf_index);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_recvmsg_multishot(td_rt *rt, int fd, struct msghdr *msg, unsigned flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_recvmsg_multishot(sqe, fd, msg, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_poll_add(td_rt *rt, int fd, unsigned poll_mask) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_poll_add(sqe, fd, poll_mask);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_poll_multishot(td_rt *rt, int fd, unsigned poll_mask) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_poll_multishot(sqe, fd, poll_mask);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_poll_remove(td_rt *rt, __u64 user_data) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_poll_remove(sqe, user_data);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_poll_update(td_rt *rt, __u64 old_user_data, __u64 new_user_data, unsigned poll_mask, unsigned flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_poll_update(sqe, old_user_data, new_user_data, poll_mask, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_nop(td_rt *rt) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_nop(sqe);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_nop128(td_rt *rt) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_nop128(sqe);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_timeout(td_rt *rt, const struct __kernel_timespec *ts, unsigned count, unsigned flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_timeout(sqe, ts, count, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_timeout_remove(td_rt *rt, __u64 user_data, unsigned flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_timeout_remove(sqe, user_data, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_timeout_update(td_rt *rt, const struct __kernel_timespec *ts, __u64 user_data, unsigned flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_timeout_update(sqe, ts, user_data, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_accept_direct(td_rt *rt, int fd, struct sockaddr *addr, socklen_t *addrlen, int flags, unsigned int file_index) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_accept_direct(sqe, fd, addr, addrlen, flags, file_index);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_multishot_accept(td_rt *rt, int fd, struct sockaddr *addr, socklen_t *addrlen, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_multishot_accept(sqe, fd, addr, addrlen, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_multishot_accept_direct(td_rt *rt, int fd, struct sockaddr *addr, socklen_t *addrlen, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_multishot_accept_direct(sqe, fd, addr, addrlen, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_cancel64(td_rt *rt, __u64 user_data, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_cancel64(sqe, user_data, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_cancel(td_rt *rt, const void *user_data, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_cancel(sqe, user_data, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_cancel_fd(td_rt *rt, int fd, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_cancel_fd(sqe, fd, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_link_timeout(td_rt *rt, const struct __kernel_timespec *ts, unsigned flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_link_timeout(sqe, ts, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_epoll_wait(td_rt *rt, int fd, struct epoll_event *events, int maxevents, unsigned flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_epoll_wait(sqe, fd, events, maxevents, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_files_update(td_rt *rt, int *fds, unsigned nr_fds, int offset) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_files_update(sqe, fds, nr_fds, offset);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_fallocate(td_rt *rt, int fd, int mode, __u64 offset, __u64 len) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_fallocate(sqe, fd, mode, offset, len);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_openat_direct(td_rt *rt, int dfd, const char *path, int flags, mode_t mode, unsigned file_index) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_openat_direct(sqe, dfd, path, flags, mode, file_index);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_open(td_rt *rt, const char *path, int flags, mode_t mode) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_open(sqe, path, flags, mode);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_open_direct(td_rt *rt, const char *path, int flags, mode_t mode, unsigned file_index) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_open_direct(sqe, path, flags, mode, file_index);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_close_direct(td_rt *rt, unsigned file_index) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_close_direct(sqe, file_index);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_read_multishot(td_rt *rt, int fd, unsigned nbytes, __u64 offset, int buf_group) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_read_multishot(sqe, fd, nbytes, offset, buf_group);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_statx(td_rt *rt, int dfd, const char *path, int flags, unsigned mask, struct statx *statxbuf) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_statx(sqe, dfd, path, flags, mask, statxbuf);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_fadvise(td_rt *rt, int fd, __u64 offset, __u32 len, int advice) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_fadvise(sqe, fd, offset, len, advice);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_madvise(td_rt *rt, void *addr, __u32 length, int advice) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_madvise(sqe, addr, length, advice);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_fadvise64(td_rt *rt, int fd, __u64 offset, off_t len, int advice) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_fadvise64(sqe, fd, offset, len, advice);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_madvise64(td_rt *rt, void *addr, off_t length, int advice) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_madvise64(sqe, addr, length, advice);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_send_bundle(td_rt *rt, int sockfd, size_t len, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_send_bundle(sqe, sockfd, len, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_send_set_addr(td_rt *rt, const struct sockaddr *dest_addr, __u16 addr_len) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_send_set_addr(sqe, dest_addr, addr_len);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_sendto(td_rt *rt, int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *addr, socklen_t addrlen) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_sendto(sqe, sockfd, buf, len, flags, addr, addrlen);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_send_zc(td_rt *rt, int sockfd, const void *buf, size_t len, int flags, unsigned zc_flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_send_zc(sqe, sockfd, buf, len, flags, zc_flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_send_zc_fixed(td_rt *rt, int sockfd, const void *buf, size_t len, int flags, unsigned zc_flags, unsigned buf_index) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_send_zc_fixed(sqe, sockfd, buf, len, flags, zc_flags, buf_index);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_sendmsg_zc(td_rt *rt, int fd, const struct msghdr *msg, unsigned flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_sendmsg_zc(sqe, fd, msg, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_sendmsg_zc_fixed(td_rt *rt, int fd, const struct msghdr *msg, unsigned flags, unsigned buf_index) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_sendmsg_zc_fixed(sqe, fd, msg, flags, buf_index);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_recv_multishot(td_rt *rt, int sockfd, void *buf, size_t len, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_recv_multishot(sqe, sockfd, buf, len, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_openat2(td_rt *rt, int dfd, const char *path, const struct open_how *how) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_openat2(sqe, dfd, path, how);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_openat2_direct(td_rt *rt, int dfd, const char *path, const struct open_how *how, unsigned file_index) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_openat2_direct(sqe, dfd, path, how, file_index);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_epoll_ctl(td_rt *rt, int epfd, int fd, int op, const struct epoll_event *ev) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_epoll_ctl(sqe, epfd, fd, op, ev);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_provide_buffers(td_rt *rt, void *addr, int len, int nr, int bgid, int bid) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_provide_buffers(sqe, addr, len, nr, bgid, bid);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_remove_buffers(td_rt *rt, int nr, int bgid) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_remove_buffers(sqe, nr, bgid);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_unlinkat(td_rt *rt, int dfd, const char *path, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_unlinkat(sqe, dfd, path, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_unlink(td_rt *rt, const char *path, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_unlink(sqe, path, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_renameat(td_rt *rt, int olddfd, const char *oldpath, int newdfd, const char *newpath, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_renameat(sqe, olddfd, oldpath, newdfd, newpath, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_rename(td_rt *rt, const char *oldpath, const char *newpath) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_rename(sqe, oldpath, newpath);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_sync_file_range(td_rt *rt, int fd, unsigned len, __u64 offset, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_sync_file_range(sqe, fd, len, offset, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_mkdirat(td_rt *rt, int dfd, const char *path, mode_t mode) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_mkdirat(sqe, dfd, path, mode);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_mkdir(td_rt *rt, const char *path, mode_t mode) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_mkdir(sqe, path, mode);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_symlinkat(td_rt *rt, const char *target, int newdirfd, const char *linkpath) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_symlinkat(sqe, target, newdirfd, linkpath);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_symlink(td_rt *rt, const char *target, const char *linkpath) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_symlink(sqe, target, linkpath);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_linkat(td_rt *rt, int olddfd, const char *oldpath, int newdfd, const char *newpath, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_linkat(sqe, olddfd, oldpath, newdfd, newpath, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_link(td_rt *rt, const char *oldpath, const char *newpath, int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_link(sqe, oldpath, newpath, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_msg_ring_cqe_flags(td_rt *rt, int fd, unsigned int len, __u64 data, unsigned int flags, unsigned int cqe_flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_msg_ring_cqe_flags(sqe, fd, len, data, flags, cqe_flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_msg_ring(td_rt *rt, int fd, unsigned int len, __u64 data, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_msg_ring(sqe, fd, len, data, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_msg_ring_fd(td_rt *rt, int fd, int source_fd, int target_fd, __u64 data, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_msg_ring_fd(sqe, fd, source_fd, target_fd, data, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_msg_ring_fd_alloc(td_rt *rt, int fd, int source_fd, __u64 data, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_msg_ring_fd_alloc(sqe, fd, source_fd, data, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_getxattr(td_rt *rt, const char *name, char *value, const char *path, unsigned int len) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_getxattr(sqe, name, value, path, len);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_setxattr(td_rt *rt, const char *name, const char *value, const char *path, int flags, unsigned int len) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_setxattr(sqe, name, value, path, flags, len);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

ssize_t td_fgetxattr(td_rt *rt, int fd, const char *name, char *value, unsigned int len) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_fgetxattr(sqe, fd, name, value, len);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_fsetxattr(td_rt *rt, int fd, const char *name, const char *value, int flags, unsigned int len) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_fsetxattr(sqe, fd, name, value, flags, len);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_socket_direct(td_rt *rt, int domain, int type, int protocol, unsigned file_index, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_socket_direct(sqe, domain, type, protocol, file_index, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_socket_direct_alloc(td_rt *rt, int domain, int type, int protocol, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_socket_direct_alloc(sqe, domain, type, protocol, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_uring_cmd(td_rt *rt, int cmd_op, int fd) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_uring_cmd(sqe, cmd_op, fd);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_uring_cmd128(td_rt *rt, int cmd_op, int fd) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_uring_cmd128(sqe, cmd_op, fd);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_cmd_sock(td_rt *rt, int cmd_op, int fd, int level, int optname, void *optval, int optlen) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_cmd_sock(sqe, cmd_op, fd, level, optname, optval, optlen);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_cmd_getsockname(td_rt *rt, int fd, struct sockaddr *sockaddr, socklen_t *sockaddr_len, int peer) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_cmd_getsockname(sqe, fd, sockaddr, sockaddr_len, peer);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_waitid(td_rt *rt, idtype_t idtype, id_t id, siginfo_t *infop, int options, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_waitid(sqe, idtype, id, infop, options, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_futex_wake(td_rt *rt, const uint32_t *futex, uint64_t val, uint64_t mask, uint32_t futex_flags, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_futex_wake(sqe, futex, val, mask, futex_flags, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_futex_wait(td_rt *rt, const uint32_t *futex, uint64_t val, uint64_t mask, uint32_t futex_flags, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_futex_wait(sqe, futex, val, mask, futex_flags, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_futex_waitv(td_rt *rt, const struct futex_waitv *futex, uint32_t nr_futex, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_futex_waitv(sqe, futex, nr_futex, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_fixed_fd_install(td_rt *rt, int fd, unsigned int flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_fixed_fd_install(sqe, fd, flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_ftruncate(td_rt *rt, int fd, loff_t len) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_ftruncate(sqe, fd, len);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_cmd_discard(td_rt *rt, int fd, uint64_t offset, uint64_t nbytes) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_cmd_discard(sqe, fd, offset, nbytes);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_pipe(td_rt *rt, int *fds, int pipe_flags) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_pipe(sqe, fds, pipe_flags);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}

int td_pipe_direct(td_rt *rt, int *fds, int pipe_flags, unsigned int file_index) {
  struct io_uring_sqe *sqe = td_sch_get_sqe(rt);

  io_uring_prep_pipe_direct(sqe, fds, pipe_flags, file_index);
  io_uring_sqe_set_data(sqe, rt->current);

  td_sch_wait(rt);

  return io_res(rt);
}
