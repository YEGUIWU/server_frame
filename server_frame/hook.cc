/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/hook.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-11
 *  Description: 
 * ====================================================
 */


#include <dlfcn.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#include <memory>

#include "config.h" // log.h 在里面包含了 
#include "base/fd_manager.h"
#include "hook.h"
#include "iomanager.h"
#include "macro.h"

ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

namespace ygw {

    //----------------------------------------------------------

    namespace hook {

        static ygw::config::ConfigVar<int>::ptr g_tcp_connect_timeout =
                ygw::config::Config::Lookup("tcp.connect.timeout", 5000, "tcp connect timeout");

        static thread_local bool t_hook_enable = false;

#define HOOK_FUNC(XX) \
        XX(sleep) \
        XX(usleep) \
        XX(nanosleep) \
        XX(socket) \
        XX(connect) \
        XX(accept) \
        XX(read) \
        XX(readv) \
        XX(recv) \
        XX(recvfrom) \
        XX(recvmsg) \
        XX(write) \
        XX(writev) \
        XX(send) \
        XX(sendto) \
        XX(sendmsg) \
        XX(close) \
        XX(fcntl) \
        XX(ioctl) \
        XX(getsockopt) \
        XX(setsockopt)

        void HookInit()
        {
            static bool is_inited = false;
            if (is_inited)
            {
                return;
            }
#define XX(name) name ## _f = (name ## _func)dlsym(RTLD_NEXT, #name);
                HOOK_FUNC(XX);
#undef XX
        }

        //---------------------------------
        static uint64_t s_connect_timeout = -1;
        class _HookIniter 
        {
        public:
            _HookIniter() 
            {
                HookInit();
                s_connect_timeout = g_tcp_connect_timeout->GetValue();

                g_tcp_connect_timeout->AddListener( [] (const int& old_value, const int& new_value) {
                    YGW_LOG_INFO(g_logger) << "tcp connect timeout changed from "
                    << old_value << " to " << new_value;
                    s_connect_timeout = new_value;
                });
            }
        }; // class _HookIniter
        static _HookIniter s_hook_initer;
        //---------------------------------

        bool IsHookEnable()
        {
            return t_hook_enable;
        }

        void SetHookEnable(bool flag)
        {
            t_hook_enable = flag;
        }
        
//#undef HOOK_FUNC



    } // namespace hook

    //----------------------------------------------------------

} // namespace ygw



//-----------------------------------------------------------------
//                      io操作模板函数
//-----------------------------------------------------------------
struct TimerInfo
{
    int cancelled = 0;;
};

template<typename OriginFunc, typename ... Args>
static ssize_t DoIo(
        int fd,         //文件描述符 
        OriginFunc func,            //原函数
        const char* hook_func_name, //要hook的函数名
        uint32_t event,             //事件 
        int timeout_so,             //超时类型
        Args&&... args)             //函数参数
{
    if (!ygw::hook::t_hook_enable)//没有设置hook就调用原来函数
    {
        return func(fd, std::forward<Args>(args)...);
    }

    //获取对应fd的文件上下文
    ygw::handle::FdContext::ptr ctx = ygw::handle::FdManager::GetInstance()->Get(fd);
    if (!ctx)//获取失败就调用原来的函数
    {
        return func(fd, std::forward<Args>(args)...);
    }

    //获取成功
    if (ctx->IsClose())//但是文件已经关闭
    {
        errno = EBADF;
        return -1;
    }

    //如果文件不是套接字文件或者 是用户设置了非阻塞，就调用原来的函数
    if (!ctx->IsSocket() || ctx->IsUserNonblock())
    {
        return func(fd, std::forward<Args>(args)...);
    }

    //获取超时毫秒
    uint64_t to = ctx->GetTimeout(timeout_so);
    std::shared_ptr<TimerInfo> tinfo(new TimerInfo);


retry:
    ssize_t n = func(fd, std::forward<Args>(args)...); // 调用一次

    while(n == -1 && errno == EINTR) //执行失败且被系统中断
    {
        n = func(fd, std::forward<Args>(args)...); // 再尝试执行
    }


    if (n == -1 && errno == EAGAIN) //执行失败且，需要再来一次
    {
        ygw::scheduler::IOManager* iom = ygw::scheduler::IOManager::GetThis();
        ygw::timer::Timer::ptr timer;
        std::weak_ptr<TimerInfo> winfo(tinfo);

        if (to != static_cast<uint64_t>(-1))//如果获取超时毫秒成功
        {
            timer = iom->AddConditionTimer(to, [winfo, fd, iom, event]() {
                auto t = winfo.lock();      //拿出条件尝试唤醒
                if (!t || t->cancelled)     //如果没有条件 或者 设置了错误
                {
                    return;
                }
                t->cancelled = ETIMEDOUT;   //将s错误设置为超时
                iom->CancelEvent(fd, (ygw::scheduler::IOManager::Event)(event)); //取消事件
            }, winfo);//添加条件定时器
        }

        //添加事件， 不传回调函数，就是把当前协程作为事件唤醒对象
        int rt = iom->AddEvent(fd, (ygw::scheduler::IOManager::Event)(event));
        if (YGW_UNLIKELY(rt)) //添加失败
        {
            YGW_LOG_ERROR(g_logger) << hook_func_name << " AddEvent("
                << fd << ", " << event << ")";
            if (timer)//有设定定时器就取消掉
            {
                timer->Cancel();
            }
            return -1;
        }
        else                //添加成功 
        {
            ygw::scheduler::Fiber::YieldToHold();//让出资源

            if (timer)              //如果有设定定时器就取消掉
            {
                timer->Cancel();
            }
            if (tinfo->cancelled)  //cancelled有值就说明超时了
            {
                errno = tinfo->cancelled;
                return -1;         //直接返回 -1
            }

            goto retry;             //没操作完或者出错了就回到retry再次操作
        }
    }

    return n;
}
//---------------------------------------------------------------------------------------------


extern "C" { 

#define XX(name) name ## _func name ## _f = nullptr;
        HOOK_FUNC(XX);
#undef XX



    //-----------------------------------------------------------------------
    //                          Sleep
    //-----------------------------------------------------------------------
    unsigned int sleep(unsigned int seconds) 
    {
        if (!ygw::hook::t_hook_enable) 
        {
            return sleep_f(seconds);
        }
        ygw::scheduler::Fiber::ptr fiber = ygw::scheduler::Fiber::GetThis();
        ygw::scheduler::IOManager* iom = ygw::scheduler::IOManager::GetThis(); 
        //iom->AddTimer(seconds * 1000, [iom, fiber](){
        //    iom->Schedule(fiber);
        //});
        iom->AddTimer(seconds * 1000, std::bind((void(ygw::scheduler::Scheduler::*)
                        (ygw::scheduler::Fiber::ptr, int thread))&ygw::scheduler::IOManager::Schedule
                    , iom, fiber, -1));
        ygw::scheduler::Fiber::YieldToHold();
        return 0;
    }



    int usleep(useconds_t usec)
    {
        if (!ygw::hook::t_hook_enable) 
        {
            return usleep_f(usec);
        }

        ygw::scheduler::Fiber::ptr fiber = ygw::scheduler::Fiber::GetThis();
        ygw::scheduler::IOManager* iom = ygw::scheduler::IOManager::GetThis(); 
        //iom->AddTimer(usec / 1000, [iom, fiber](){
        //    iom->Schedule(fiber);
        //});
        iom->AddTimer(usec / 1000, std::bind((void(ygw::scheduler::Scheduler::*)
                        (ygw::scheduler::Fiber::ptr, int thread))&ygw::scheduler::IOManager::Schedule
                    , iom, fiber, -1));
        ygw::scheduler::Fiber::YieldToHold();
        return 0;
    }

    int nanosleep(const struct timespec *req, struct timespec *rem)
    {
        if (!ygw::hook::t_hook_enable)
        {
            return nanosleep_f(req, rem);
        }

        int timeout_ms = req->tv_sec * 1000 + req->tv_nsec / 1000 / 1000;
        ygw::scheduler::Fiber::ptr fiber = ygw::scheduler::Fiber::GetThis();
        ygw::scheduler::IOManager* iom = ygw::scheduler::IOManager::GetThis(); 
        //iom->AddTimer(timeout_ms, [iom, fiber](){
        //    iom->Schedule(fiber);
        //});
        iom->AddTimer(timeout_ms, std::bind((void(ygw::scheduler::Scheduler::*)
                        (ygw::scheduler::Fiber::ptr, int thread))&ygw::scheduler::IOManager::Schedule
                    , iom, fiber, -1));
        ygw::scheduler::Fiber::YieldToHold();
        return 0;
    }

    //-----------------------------------------------------------------------
    //                          Socket
    //-----------------------------------------------------------------------
    int socket(int domain, int type, int protocol)
    {
        if (!ygw::hook::t_hook_enable)
        {
            return socket_f(domain, type, protocol);
        }
        int fd = socket_f(domain, type, protocol);
        if (fd == -1)
        {
            return fd;
        }
        ygw::handle::FdManager::GetInstance()->Get(fd, true);
        return fd;
    }

    int connect_with_timeout(int fd, const struct sockaddr* addr, socklen_t addrlen, uint64_t timeout_ms)
    {
        if (!ygw::hook::t_hook_enable) 
        {
            return connect_f(fd, addr, addrlen);
        }
        ygw::handle::FdContext::ptr ctx = ygw::handle::FdManager::GetInstance()->Get(fd);
        if (!ctx || ctx->IsClose()) 
        {
            errno = EBADF;
            return -1;
        }

        if (!ctx->IsSocket()) 
        {
            return connect_f(fd, addr, addrlen);
        }

        if (ctx->IsUserNonblock()) 
        {
            return connect_f(fd, addr, addrlen);
        }

        int n = connect_f(fd, addr, addrlen);
        if (n == 0) 
        {
            return 0;
        } 
        else if (n != -1 || errno != EINPROGRESS) 
        {
            return n;
        }

        ygw::scheduler::IOManager* iom = ygw::scheduler::IOManager::GetThis();
        ygw::timer::Timer::ptr timer;
        std::shared_ptr<TimerInfo> tinfo(new TimerInfo);
        std::weak_ptr<TimerInfo> winfo(tinfo);

        if (timeout_ms != (uint64_t)-1) 
        {
            timer = iom->AddConditionTimer(timeout_ms, [winfo, fd, iom]() {
                auto t = winfo.lock();
                if (!t || t->cancelled) 
                {
                    return;
                }
                t->cancelled = ETIMEDOUT;
                iom->CancelEvent(fd, ygw::scheduler::IOManager::Event::kWrite);
            }, winfo);
        }

        int rt = iom->AddEvent(fd, ygw::scheduler::IOManager::Event::kWrite);
        if (rt == 0) 
        {
            ygw::scheduler::Fiber::YieldToHold();
            if (timer) 
            {
                timer->Cancel();
            }
            if (tinfo->cancelled) 
            {
                errno = tinfo->cancelled;
                return -1;
            }
        } 
        else 
        {
            if (timer) 
            {
                timer->Cancel();
            }
            YGW_LOG_ERROR(g_logger) << "connect addEvent(" << fd << ", WRITE) error";
        }

        int error = 0;
        socklen_t len = sizeof(int);
        if (-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len)) 
        {
            return -1;
        }
        if (!error) 
        {
            return 0;
        } 
        else 
        {
            errno = error;
            return -1;
        }
    }

    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) 
    {
        return connect_with_timeout(sockfd, addr, addrlen, ygw::hook::s_connect_timeout);
    }

    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
    {
        int fd = DoIo(sockfd, accept_f, "accept", ygw::scheduler::IOManager::Event::kRead, SO_RCVTIMEO, addr, addrlen);
        if (fd >= 0)
        {
            ygw::handle::FdManager::GetInstance()->Get(fd, true);
        }
        return fd;
    }



    ssize_t read(int fd, void *buf, size_t count) 
    {
        return DoIo(fd, read_f, "read", ygw::scheduler::IOManager::Event::kRead, SO_RCVTIMEO, buf, count);
    }

    ssize_t readv(int fd, const struct iovec *iov, int iovcnt) 
    {
        return DoIo(fd, readv_f, "readv", ygw::scheduler::IOManager::Event::kRead, SO_RCVTIMEO, iov, iovcnt);
    }

    ssize_t recv(int sockfd, void *buf, size_t len, int flags) 
    {
        return DoIo(sockfd, recv_f, "recv", ygw::scheduler::IOManager::Event::kRead, SO_RCVTIMEO, buf, len, flags);
    }

    ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) 
    {
        return DoIo(sockfd, recvfrom_f, "recvfrom", 
                ygw::scheduler::IOManager::Event::kRead, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
    }

    ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) 
    {
        return DoIo(sockfd, recvmsg_f, "recvmsg", ygw::scheduler::IOManager::Event::kRead, SO_RCVTIMEO, msg, flags);
    }

    ssize_t write(int fd, const void *buf, size_t count) 
    {
        return DoIo(fd, write_f, "write", ygw::scheduler::IOManager::Event::kWrite, SO_SNDTIMEO, buf, count);
    }

    ssize_t writev(int fd, const struct iovec *iov, int iovcnt) 
    {
        return DoIo(fd, writev_f, "writev", ygw::scheduler::IOManager::Event::kWrite, SO_SNDTIMEO, iov, iovcnt);
    }

    ssize_t send(int s, const void *msg, size_t len, int flags) 
    {
        return DoIo(s, send_f, "send", ygw::scheduler::IOManager::Event::kWrite, SO_SNDTIMEO, msg, len, flags);
    }

    ssize_t sendto(int s, const void *msg, size_t len, int flags, const struct sockaddr *to, socklen_t tolen) 
    {
        return DoIo(s, sendto_f, "sendto", ygw::scheduler::IOManager::Event::kWrite, SO_SNDTIMEO, msg, len, flags, to, tolen);
    }

    ssize_t sendmsg(int s, const struct msghdr *msg, int flags) 
    {
        return DoIo(s, sendmsg_f, "sendmsg", ygw::scheduler::IOManager::Event::kWrite, SO_SNDTIMEO, msg, flags);
    }

    int close(int fd)
    {
        if (!ygw::hook::t_hook_enable)
        {
            return close_f(fd);
        }
        ygw::handle::FdContext::ptr ctx = ygw::handle::FdManager::GetInstance()->Get(fd);
        if (ctx)
        {
            auto iom = ygw::scheduler::IOManager::GetThis();
            if (iom)
            {
                iom->CancelAll(fd); 
            }
            ygw::handle::FdManager::GetInstance()->Del(fd);
        }
        return close_f(fd);
    }

    int fcntl(int fd, int cmd, ... /* arg */ )
    {
        va_list va;
        va_start(va, cmd);
        switch(cmd) 
        {
        case F_SETFL:
            {
                int arg = va_arg(va, int);
                va_end(va);
                ygw::handle::FdContext::ptr ctx = ygw::handle::FdManager::GetInstance()->Get(fd);
                if (!ctx || ctx->IsClose() || !ctx->IsSocket()) //获取失败 或 关闭了 或 不是套接字
                {
                    return fcntl_f(fd, cmd, arg);
                }
                ctx->SetUserNonblock(arg & O_NONBLOCK); //设置为用户非阻塞
                if (ctx->IsSysNonblock()) //有设置为系统非阻塞
                {
                    arg |= O_NONBLOCK; //arg + 非阻塞
                }
                else //没有设置就 
                {
                    arg &= ~O_NONBLOCK; //arg - 非阻塞
                }
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GETFL:
            {
                va_end(va);
                int arg = fcntl_f(fd, cmd);
                ygw::handle::FdContext::ptr ctx = ygw::handle::FdManager::GetInstance()->Get(fd);
                if (!ctx || ctx->IsClose() || !ctx->IsSocket())
                {
                    return arg;
                }
                if (ctx->IsUserNonblock())
                {
                    return arg | O_NONBLOCK;
                }
                else 
                {
                    return arg & ~O_NONBLOCK;
                }
            }
            break;
        //-------------------------
        // int type
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
#ifdef F_SETPIPE_SZ
        case F_SETPIPE_SZ:
#endif // F_SETPIPE_SZ
            {
                int arg = va_arg(va, int);
                va_end(va);
                return fcntl_f(fd, cmd, arg); 
            }
            break;
        //-------------------------
        // void type
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
#ifdef F_GETPIPE_SZ
        case F_GETPIPE_SZ:
#endif // F_SETPIPE_SZ
            {
                va_end(va);
                return fcntl_f(fd, cmd);
            }
            break;
        //-------------------------
        // flock
        case F_SETLK:
        case F_SETLKW:
        case F_GETLK:
            {
                struct flock* arg = va_arg(va, struct flock*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        //-------------------------
        // f_owner_lock
        case F_GETOWN_EX:
        case F_SETOWN_EX:
            {
                struct f_owner_exlock* arg = va_arg(va, struct f_owner_exlock*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        default:
            va_end(va);
            return fcntl_f(fd, cmd);
        }
    }

    int ioctl(int d, unsigned long int request, ...) 
    {
        va_list va;
        va_start(va, request);
        void* arg = va_arg(va, void*);
        va_end(va);

        if (FIONBIO == request) 
        {
            bool user_nonblock = !!*(int*)arg;
            ygw::handle::FdContext::ptr ctx = ygw::handle::FdManager::GetInstance()->Get(d);
            if (!ctx || ctx->IsClose() || !ctx->IsSocket()) 
            {
                return ioctl_f(d, request, arg);
            }
            ctx->SetUserNonblock(user_nonblock);
        }
        return ioctl_f(d, request, arg);
    }


    int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) 
    {
        return getsockopt_f(sockfd, level, optname, optval, optlen);
    }

   
    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) 
    {
        if (!ygw::hook::t_hook_enable) 
        {
            return setsockopt_f(sockfd, level, optname, optval, optlen);
        }
        if (level == SOL_SOCKET) 
        {
            if (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) 
            {
                ygw::handle::FdContext::ptr ctx = ygw::handle::FdManager::GetInstance()->Get(sockfd);
                if (ctx) 
                {
                    const timeval* v = (const timeval*)optval;
                    ctx->SetTimeout(optname, v->tv_sec * 1000 + v->tv_usec / 1000);
                }
            }
        }
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    }

} // extern "C" 
