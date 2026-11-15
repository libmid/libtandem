#ifndef TD_IO_H
#define TD_IO_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <fcntl.h>
#include <linux/futex.h>
#include <linux/openat2.h>
#include <linux/time_types.h>
#include <linux/types.h>
#include <signal.h>
#include <stdint.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <time.h>

#include "common.h"

/* Standard libc IO functions, first argument is the tandem runtime.
    Appart from that everything is exactly like you would expect. */

TD_API void td_sleep(td_rt *rt, size_t millis);
TD_API int td_close(td_rt *rt, int fd);
TD_API ssize_t td_read(td_rt *rt, int fd, void *buf, size_t count);
TD_API ssize_t td_write(td_rt *rt, int fd, const void *buf, size_t count);
TD_API ssize_t td_pread(td_rt *rt, int fd, void *buf, size_t count, off_t offset);
TD_API ssize_t td_pwrite(td_rt *rt, int fd, const void *buf, size_t count, off_t offset);
TD_API ssize_t td_readv(td_rt *rt, int fd, const struct iovec *iov, int iovcnt);
TD_API ssize_t td_writev(td_rt *rt, int fd, const struct iovec *iov, int iovcnt);
TD_API ssize_t td_preadv(td_rt *rt, int fd, const struct iovec *iov, int iovcnt, off_t offset);
TD_API ssize_t td_pwritev(td_rt *rt, int fd, const struct iovec *iov, int iovcnt, off_t offset);
TD_API int td_openat(td_rt *rt, int dfd, const char *path, int flags, mode_t mode);
TD_API int td_fsync(td_rt *rt, int fd, unsigned int flags);

TD_API int td_socket(td_rt *rt, int domain, int type, int protocol);
TD_API int td_bind(td_rt *rt, int sockfd, const struct sockaddr *addr, socklen_t addrlen);
TD_API int td_listen(td_rt *rt, int sockfd, int backlog);
TD_API int td_accept(td_rt *rt, int sockfd, struct sockaddr *addr, socklen_t *addrlen);
TD_API int td_connect(td_rt *rt, int sockfd, const struct sockaddr *addr, socklen_t addrlen);
TD_API ssize_t td_recv(td_rt *rt, int sockfd, void *buf, size_t len, int flags);
TD_API ssize_t td_send(td_rt *rt, int sockfd, const void *buf, size_t len, int flags);
TD_API ssize_t td_recvmsg(td_rt *rt, int sockfd, struct msghdr *msg, unsigned int flags);
TD_API ssize_t td_sendmsg(td_rt *rt, int sockfd, const struct msghdr *msg, unsigned int flags);
TD_API int td_shutdown(td_rt *rt, int sockfd, int how);

TD_API int td_poll(td_rt *rt, int fd, unsigned int mask);

TD_API ssize_t td_splice(td_rt *rt, int fd_in, int64_t off_in, int fd_out, int64_t off_out, unsigned int nbytes, unsigned int splice_flags);
TD_API ssize_t td_tee(td_rt *rt, int fd_in, int fd_out, unsigned int nbytes, unsigned int splice_flags);
TD_API ssize_t td_readv2(td_rt *rt, int fd, const struct iovec *iovecs, unsigned nr_vecs, __u64 offset, int flags);
TD_API ssize_t td_read_fixed(td_rt *rt, int fd, void *buf, unsigned nbytes, __u64 offset, int buf_index);
TD_API ssize_t td_readv_fixed(td_rt *rt, int fd, const struct iovec *iovecs, unsigned nr_vecs, __u64 offset, int flags, int buf_index);
TD_API ssize_t td_writev2(td_rt *rt, int fd, const struct iovec *iovecs, unsigned nr_vecs, __u64 offset, int flags);
TD_API ssize_t td_write_fixed(td_rt *rt, int fd, const void *buf, unsigned nbytes, __u64 offset, int buf_index);
TD_API ssize_t td_writev_fixed(td_rt *rt, int fd, const struct iovec *iovecs, unsigned nr_vecs, __u64 offset, int flags, int buf_index);
TD_API int td_recvmsg_multishot(td_rt *rt, int fd, struct msghdr *msg, unsigned flags);
TD_API int td_poll_add(td_rt *rt, int fd, unsigned poll_mask);
TD_API int td_poll_multishot(td_rt *rt, int fd, unsigned poll_mask);
TD_API int td_poll_remove(td_rt *rt, __u64 user_data);
TD_API int td_poll_update(td_rt *rt, __u64 old_user_data, __u64 new_user_data, unsigned poll_mask, unsigned flags);
TD_API int td_nop(td_rt *rt);
TD_API int td_nop128(td_rt *rt);
TD_API int td_timeout(td_rt *rt, const struct __kernel_timespec *ts, unsigned count, unsigned flags);
TD_API int td_timeout_remove(td_rt *rt, __u64 user_data, unsigned flags);
TD_API int td_timeout_update(td_rt *rt, const struct __kernel_timespec *ts, __u64 user_data, unsigned flags);
TD_API int td_accept_direct(td_rt *rt, int fd, struct sockaddr *addr, socklen_t *addrlen, int flags, unsigned int file_index);
TD_API int td_multishot_accept(td_rt *rt, int fd, struct sockaddr *addr, socklen_t *addrlen, int flags);
TD_API int td_multishot_accept_direct(td_rt *rt, int fd, struct sockaddr *addr, socklen_t *addrlen, int flags);
TD_API int td_cancel64(td_rt *rt, __u64 user_data, int flags);
TD_API int td_cancel(td_rt *rt, const void *user_data, int flags);
TD_API int td_cancel_fd(td_rt *rt, int fd, unsigned int flags);
TD_API int td_link_timeout(td_rt *rt, const struct __kernel_timespec *ts, unsigned flags);
TD_API int td_epoll_wait(td_rt *rt, int fd, struct epoll_event *events, int maxevents, unsigned flags);
TD_API int td_files_update(td_rt *rt, int *fds, unsigned nr_fds, int offset);
TD_API int td_fallocate(td_rt *rt, int fd, int mode, __u64 offset, __u64 len);
TD_API int td_openat_direct(td_rt *rt, int dfd, const char *path, int flags, mode_t mode, unsigned file_index);
TD_API int td_open(td_rt *rt, const char *path, int flags, mode_t mode);
TD_API int td_open_direct(td_rt *rt, const char *path, int flags, mode_t mode, unsigned file_index);
TD_API int td_close_direct(td_rt *rt, unsigned file_index);
TD_API ssize_t td_read_multishot(td_rt *rt, int fd, unsigned nbytes, __u64 offset, int buf_group);
TD_API int td_statx(td_rt *rt, int dfd, const char *path, int flags, unsigned mask, struct statx *statxbuf);
TD_API int td_fadvise(td_rt *rt, int fd, __u64 offset, __u32 len, int advice);
TD_API int td_madvise(td_rt *rt, void *addr, __u32 length, int advice);
TD_API int td_fadvise64(td_rt *rt, int fd, __u64 offset, off_t len, int advice);
TD_API int td_madvise64(td_rt *rt, void *addr, off_t length, int advice);
TD_API ssize_t td_send_bundle(td_rt *rt, int sockfd, size_t len, int flags);
TD_API ssize_t td_send_set_addr(td_rt *rt, const struct sockaddr *dest_addr, __u16 addr_len);
TD_API ssize_t td_sendto(td_rt *rt, int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *addr, socklen_t addrlen);
TD_API ssize_t td_send_zc(td_rt *rt, int sockfd, const void *buf, size_t len, int flags, unsigned zc_flags);
TD_API ssize_t td_send_zc_fixed(td_rt *rt, int sockfd, const void *buf, size_t len, int flags, unsigned zc_flags, unsigned buf_index);
TD_API int td_sendmsg_zc(td_rt *rt, int fd, const struct msghdr *msg, unsigned flags);
TD_API int td_sendmsg_zc_fixed(td_rt *rt, int fd, const struct msghdr *msg, unsigned flags, unsigned buf_index);
TD_API ssize_t td_recv_multishot(td_rt *rt, int sockfd, void *buf, size_t len, int flags);
TD_API int td_openat2(td_rt *rt, int dfd, const char *path, const struct open_how *how);
TD_API int td_openat2_direct(td_rt *rt, int dfd, const char *path, const struct open_how *how, unsigned file_index);
TD_API int td_epoll_ctl(td_rt *rt, int epfd, int fd, int op, const struct epoll_event *ev);
TD_API int td_provide_buffers(td_rt *rt, void *addr, int len, int nr, int bgid, int bid);
TD_API int td_remove_buffers(td_rt *rt, int nr, int bgid);
TD_API int td_unlinkat(td_rt *rt, int dfd, const char *path, int flags);
TD_API int td_unlink(td_rt *rt, const char *path, int flags);
TD_API int td_renameat(td_rt *rt, int olddfd, const char *oldpath, int newdfd, const char *newpath, unsigned int flags);
TD_API int td_rename(td_rt *rt, const char *oldpath, const char *newpath);
TD_API int td_sync_file_range(td_rt *rt, int fd, unsigned len, __u64 offset, int flags);
TD_API int td_mkdirat(td_rt *rt, int dfd, const char *path, mode_t mode);
TD_API int td_mkdir(td_rt *rt, const char *path, mode_t mode);
TD_API int td_symlinkat(td_rt *rt, const char *target, int newdirfd, const char *linkpath);
TD_API int td_symlink(td_rt *rt, const char *target, const char *linkpath);
TD_API int td_linkat(td_rt *rt, int olddfd, const char *oldpath, int newdfd, const char *newpath, int flags);
TD_API int td_link(td_rt *rt, const char *oldpath, const char *newpath, int flags);
TD_API int td_msg_ring_cqe_flags(td_rt *rt, int fd, unsigned int len, __u64 data, unsigned int flags, unsigned int cqe_flags);
TD_API int td_msg_ring(td_rt *rt, int fd, unsigned int len, __u64 data, unsigned int flags);
TD_API int td_msg_ring_fd(td_rt *rt, int fd, int source_fd, int target_fd, __u64 data, unsigned int flags);
TD_API int td_msg_ring_fd_alloc(td_rt *rt, int fd, int source_fd, __u64 data, unsigned int flags);
TD_API ssize_t td_getxattr(td_rt *rt, const char *name, char *value, const char *path, unsigned int len);
TD_API int td_setxattr(td_rt *rt, const char *name, const char *value, const char *path, int flags, unsigned int len);
TD_API ssize_t td_fgetxattr(td_rt *rt, int fd, const char *name, char *value, unsigned int len);
TD_API int td_fsetxattr(td_rt *rt, int fd, const char *name, const char *value, int flags, unsigned int len);
TD_API int td_socket_direct(td_rt *rt, int domain, int type, int protocol, unsigned file_index, unsigned int flags);
TD_API int td_socket_direct_alloc(td_rt *rt, int domain, int type, int protocol, unsigned int flags);
TD_API int td_uring_cmd(td_rt *rt, int cmd_op, int fd);
TD_API int td_uring_cmd128(td_rt *rt, int cmd_op, int fd);
TD_API int td_cmd_sock(td_rt *rt, int cmd_op, int fd, int level, int optname, void *optval, int optlen);
TD_API int td_cmd_getsockname(td_rt *rt, int fd, struct sockaddr *sockaddr, socklen_t *sockaddr_len, int peer);
TD_API int td_waitid(td_rt *rt, idtype_t idtype, id_t id, siginfo_t *infop, int options, unsigned int flags);
TD_API int td_futex_wake(td_rt *rt, const uint32_t *futex, uint64_t val, uint64_t mask, uint32_t futex_flags, unsigned int flags);
TD_API int td_futex_wait(td_rt *rt, const uint32_t *futex, uint64_t val, uint64_t mask, uint32_t futex_flags, unsigned int flags);
TD_API int td_futex_waitv(td_rt *rt, const struct futex_waitv *futex, uint32_t nr_futex, unsigned int flags);
TD_API int td_fixed_fd_install(td_rt *rt, int fd, unsigned int flags);
TD_API int td_ftruncate(td_rt *rt, int fd, loff_t len);
TD_API int td_cmd_discard(td_rt *rt, int fd, uint64_t offset, uint64_t nbytes);
TD_API int td_pipe(td_rt *rt, int *fds, int pipe_flags);
TD_API int td_pipe_direct(td_rt *rt, int *fds, int pipe_flags, unsigned int file_index);

#endif

