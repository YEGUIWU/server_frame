/**
 * @file server_frame/hook.h
 * @brief 协程库
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */

#ifndef __YGW_HOOK_H__
#define __YGW_HOOK_H__

#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>

namespace ygw {

    //---------------------------------------

    namespace hook {

        bool IsHookEnable();

        void SetHookEnable(bool flag);


    } // namespace hook

    //---------------------------------------

} // namespace ygw

extern "C" {

    //----------------------------------------------------------------------------
    //sleep 函数 hook
    //unsigned int sleep(unsigned int seconds);
    typedef unsigned int (*sleep_func)(unsigned int seconds);
    extern sleep_func sleep_f;

    //int usleep(useconds_t usec);
    typedef int (*usleep_func)(useconds_t usec);
    extern usleep_func usleep_f;

    //int nanosleep(const struct timespec *req, struct timespec *rem);
    typedef int (*nanosleep_func)(const struct timespec *req, struct timespec *rem);
    extern nanosleep_func nanosleep_f;

    //----------------------------------------------------------------------------
    //socket 函数族
    //int socket(int domain, int type, int protocol);
    typedef int (*socket_func)(int domain, int type, int protocol);
    extern socket_func socket_f;

    //connect
    typedef int (*connect_func)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    extern connect_func connect_f;

    //accept
    typedef int (*accept_func)(int s, struct sockaddr *addr, socklen_t *addrlen);
    extern accept_func accept_f;

    //read
    typedef ssize_t (*read_func)(int fd, void *buf, size_t count);
    extern read_func read_f;

    //readv
    typedef ssize_t (*readv_func)(int fd, const struct iovec *iov, int iovcnt);
    extern readv_func readv_f;

    //recv
    typedef ssize_t (*recv_func)(int sockfd, void *buf, size_t len, int flags);
    extern recv_func recv_f;

    //recvfrom
    typedef ssize_t (*recvfrom_func)(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
    extern recvfrom_func recvfrom_f;

    //recvmsg
    typedef ssize_t (*recvmsg_func)(int sockfd, struct msghdr *msg, int flags);
    extern recvmsg_func recvmsg_f;

    //write
    typedef ssize_t (*write_func)(int fd, const void *buf, size_t count);
    extern write_func write_f;

    //writev
    typedef ssize_t (*writev_func)(int fd, const struct iovec *iov, int iovcnt);
    extern writev_func writev_f;

    //send_fun
    typedef ssize_t (*send_func)(int s, const void *msg, size_t len, int flags);
    extern send_func send_f;

    //sendto
    typedef ssize_t (*sendto_func)(int s, const void *msg, size_t len, int flags, const struct sockaddr *to, socklen_t tolen);
    extern sendto_func sendto_f;

    //sendmsg
    typedef ssize_t (*sendmsg_func)(int s, const struct msghdr *msg, int flags);
    extern sendmsg_func sendmsg_f;

    //close
    typedef int (*close_func)(int fd);
    extern close_func close_f;

    //fcntl
    typedef int (*fcntl_func)(int fd, int cmd, ... /* arg */ );
    extern fcntl_func fcntl_f;

    //ioctl
    typedef int (*ioctl_func)(int d, unsigned long int request, ...);
    extern ioctl_func ioctl_f;

    //getsockopt
    typedef int (*getsockopt_func)(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
    extern getsockopt_func getsockopt_f;

    //setsockopt
    typedef int (*setsockopt_func)(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
    extern setsockopt_func setsockopt_f;

    extern int connect_with_timeout(int fd, const struct sockaddr* addr, socklen_t addrlen, uint64_t timeout_ms);


} // extern "C" 
#endif // __YGW_HOOK_H__
