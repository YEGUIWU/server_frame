/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/iomanager.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-07
 *  Description: 
 * ====================================================
 */

#ifdef __GNUC__
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#endif // __GNUC__
#include <cerrno>
#include <cstring>

#include "iomanager.h"
#include "macro.h"
#include "log.h"

namespace ygw {

    //------------------------------------------------------------------- 

    namespace scheduler {

        static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

        enum EpollCtlOp {
        };
        /*
        static std::ostream& operator<< (std::ostream& os, const EpollCtlOp& op) 
        {
            switch((int)op) 
            {
#define XX(ctl) \
                case ctl: \
                          return os << #ctl;
                XX(EPOLL_CTL_ADD);
                XX(EPOLL_CTL_MOD);
                XX(EPOLL_CTL_DEL);
                default:
                return os << (int)op;
            }
#undef XX
        }

        static std::ostream& operator<< (std::ostream& os, EPOLL_EVENTS events) 
        {
            if (!events) 
            {
                return os << "0";
            }
            bool first = true;
#define XX(E) \
            if (events & E) { \
                if (!first) { \
                    os << "|"; \
                } \
                os << #E; \
                first = false; \
            }
            XX(EPOLLIN);
            XX(EPOLLPRI);
            XX(EPOLLOUT);
            XX(EPOLLRDNORM);
            XX(EPOLLRDBAND);
            XX(EPOLLWRNORM);
            XX(EPOLLWRBAND);
            XX(EPOLLMSG);
            XX(EPOLLERR);
            XX(EPOLLHUP);
            XX(EPOLLRDHUP);
            XX(EPOLLONESHOT);
            XX(EPOLLET);
#undef XX
            return os;
        }
        */

    
        IOManager::FdContext::EventContext& 
        IOManager::FdContext::GetContext(IOManager::Event event) 
        {
            switch(event) 
            {
                case IOManager::Event::kRead:
                    return read_;
                case IOManager::Event::kWrite:
                    return write_;
                default:
                    YGW_MSG_ASSERT(false, "GetContext");
            }
            throw std::invalid_argument("GetContext invalid event");
        }

        void IOManager::FdContext::ResetContext(EventContext& ctx) 
        {
            ctx.scheduler = nullptr;
            ctx.fiber.reset();
            ctx.cb = nullptr;
        }

        void IOManager::FdContext::TriggerEvent(IOManager::Event event) 
        {
            //----- use for debug -----//
            //YGW_LOG_INFO(g_logger) << "fd=" << fd
            //    << " TriggerEvent event=" << event
            //    << " events=" << events;
            YGW_ASSERT(events & event);
            //if (YGW_UNLIKELY(!(event & event))) 
            //{
            //    return;
            //}
            events = (Event)(events & ~event);
            EventContext& ctx = GetContext(event);
            if (ctx.cb) 
            {
                ctx.scheduler->Schedule(&ctx.cb);
            } 
            else 
            {
                ctx.scheduler->Schedule(&ctx.fiber);
            }
            ctx.scheduler = nullptr;
            return;
        }



        //----------------------------------------------------------
        // class Scheduler method
        IOManager::IOManager(size_t threads, bool use_caller, const std::string& name)
            :Scheduler(threads, use_caller, name) 
        {
            epfd_ = epoll_create(5000);
            YGW_ASSERT(epfd_ > 0);

            int rt = pipe(tickle_fds_);
            YGW_ASSERT(!rt);

            //init epoll
            epoll_event event;
            memset(&event, 0, sizeof(epoll_event));
            event.events = EPOLLIN | EPOLLET;
            event.data.fd = tickle_fds_[0];

            rt = fcntl(tickle_fds_[0], F_SETFL, O_NONBLOCK);
            YGW_ASSERT(!rt);

            rt = epoll_ctl(epfd_, EPOLL_CTL_ADD, tickle_fds_[0], &event);
            YGW_ASSERT(!rt);

            ContextResize(32);

            Start();
        }

        //destruct
        IOManager::~IOManager() 
        {
            Stop();
            close(epfd_);
            close(tickle_fds_[0]);
            close(tickle_fds_[1]);

            for (size_t i = 0; i < fd_contexts_.size(); ++i) 
            {
                if (fd_contexts_[i]) 
                {
                    delete fd_contexts_[i];
                }
            }
        }


        //扩容
        void IOManager::ContextResize(size_t size) 
        {
            fd_contexts_.resize(size);

            for (size_t i = 0; i < fd_contexts_.size(); ++i) 
            {
                if (!fd_contexts_[i]) {
                    fd_contexts_[i] = new FdContext;
                    fd_contexts_[i]->fd = i;
                }
            }
        }

        //添加事件
        int IOManager::AddEvent(int fd, Event event, std::function<void()> cb) 
        {
            FdContext* fd_ctx = nullptr;
            {//取出fd上下文
                RWMutexType::ReadLock lock(mutex_);
                if (static_cast<int>(fd_contexts_.size()) > fd) //fd在已创建范围内
                {
                    fd_ctx = fd_contexts_[fd];
                    lock.unlock();
                } 
                else  //fd 超出范围 写锁
                {
                    lock.unlock();
                    RWMutexType::WriteLock lock2(mutex_);
                    ContextResize(fd * 1.5);// 扩大1.5倍
                    fd_ctx = fd_contexts_[fd];
                }
            }

            FdContext::MutexType::Lock lock(fd_ctx->mutex);
            if (YGW_UNLIKELY(fd_ctx->events & event)) 
            {
                YGW_LOG_ERROR(g_logger) << "AddEvent assert fd=" << fd
                    << " event=" << (EPOLL_EVENTS)event
                    << " fd_ctx.event=" << (EPOLL_EVENTS)fd_ctx->events;
                YGW_ASSERT(!(fd_ctx->events & event));
            }

            int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
            epoll_event epevent;
            epevent.events = EPOLLET | fd_ctx->events | event;
            epevent.data.ptr = fd_ctx;

            int rt = epoll_ctl(epfd_, op, fd, &epevent);
            if (rt) 
            {
                YGW_LOG_ERROR(g_logger) << "epoll_ctl(" << epfd_ << ", "
                    << (EpollCtlOp)op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "):"
                    << rt << " (" << errno << ") (" << strerror(errno) << ") fd_ctx->events="
                    << (EPOLL_EVENTS)fd_ctx->events;
                return -1;
            }

            ++pending_event_count_;
            fd_ctx->events = (Event)(fd_ctx->events | event);
            FdContext::EventContext& event_ctx = fd_ctx->GetContext(event);
            YGW_ASSERT(!event_ctx.scheduler
                    && !event_ctx.fiber
                    && !event_ctx.cb);

            event_ctx.scheduler = Scheduler::GetThis();
            if (cb) 
            {
                event_ctx.cb.swap(cb);
            } 
            else 
            {
                event_ctx.fiber = Fiber::GetThis();
                YGW_MSG_ASSERT(event_ctx.fiber->GetState() == Fiber::State::kExec
                        ,"state = " << event_ctx.fiber->GetState());
            }
            return 0;
        }

        //delete event
        bool IOManager::DelEvent(int fd, Event event) 
        {
            RWMutexType::ReadLock lock(mutex_);
            if (static_cast<int>(fd_contexts_.size()) <= fd) 
            {
                return false;
            }
            FdContext* fd_ctx = fd_contexts_[fd];
            lock.unlock();

            FdContext::MutexType::Lock fd_lock(fd_ctx->mutex);
            if (YGW_UNLIKELY(!(fd_ctx->events & event))) 
            {
                return false;
            }

            Event new_events = (Event)(fd_ctx->events & ~event);
            int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            epoll_event epevent;
            epevent.events = EPOLLET | new_events;
            epevent.data.ptr = fd_ctx;

            int rt = epoll_ctl(epfd_, op, fd, &epevent);
            if (rt) 
            {
                YGW_LOG_ERROR(g_logger) << "epoll_ctl(" << epfd_ << ", "
                    << (EpollCtlOp)op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "):"
                    << rt << " (" << errno << ") (" << strerror(errno) << ")";
                return false;
            }

            --pending_event_count_;
            fd_ctx->events = new_events;
            FdContext::EventContext& event_ctx = fd_ctx->GetContext(event);
            fd_ctx->ResetContext(event_ctx);
            return true;
        }

        
        //cancel event
        bool IOManager::CancelEvent(int fd, Event event) 
        {
            RWMutexType::ReadLock lock(mutex_);
            if ((int)fd_contexts_.size() <= fd) 
            {
                return false;
            }
            FdContext* fd_ctx = fd_contexts_[fd];
            lock.unlock();

            FdContext::MutexType::Lock fd_lock(fd_ctx->mutex);
            if (YGW_UNLIKELY(!(fd_ctx->events & event))) 
            {
                return false;
            }

            Event new_events = (Event)(fd_ctx->events & ~event);
            int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            epoll_event epevent;
            epevent.events = EPOLLET | new_events;
            epevent.data.ptr = fd_ctx;

            int rt = epoll_ctl(epfd_, op, fd, &epevent);
            if (rt) 
            {
                YGW_LOG_ERROR(g_logger) << "epoll_ctl(" << epfd_ << ", "
                    << (EpollCtlOp)op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "):"
                    << rt << " (" << errno << ") (" << strerror(errno) << ")";
                return false;
            }

            fd_ctx->TriggerEvent(event);
            --pending_event_count_;
            return true;
        }


        //cancel all
        bool IOManager::CancelAll(int fd) 
        {
            RWMutexType::ReadLock lock(mutex_);
            if ((int)fd_contexts_.size() <= fd) 
            {
                return false;
            }
            FdContext* fd_ctx = fd_contexts_[fd];
            lock.unlock();

            FdContext::MutexType::Lock fd_lock(fd_ctx->mutex);
            //没有事件就不需要操作
            if (!fd_ctx->events) 
            {
                return false;
            }

            int op = EPOLL_CTL_DEL;
            epoll_event epevent;
            epevent.events = 0;
            epevent.data.ptr = fd_ctx;

            int rt = epoll_ctl(epfd_, op, fd, &epevent);
            if (rt) 
            {
                YGW_LOG_ERROR(g_logger) << "epoll_ctl(" << epfd_ << ", "
                    << (EpollCtlOp)op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "):"
                    << rt << " (" << errno << ") (" << strerror(errno) << ")";
                return false;
            }

            if (fd_ctx->events & Event::kRead) 
            {
                fd_ctx->TriggerEvent(Event::kRead);
                --pending_event_count_;
            }
            if (fd_ctx->events & Event::kWrite) 
            {
                fd_ctx->TriggerEvent(Event::kWrite);
                --pending_event_count_;
            }

            YGW_ASSERT(fd_ctx->events == 0);
            return true;
        }

        
        IOManager* IOManager::GetThis() 
        {
            return dynamic_cast<IOManager*>(Scheduler::GetThis());
        }


        //Tickle overwrite
        void IOManager::Tickle() 
        {
            if (!HasIdleThreads()) 
            {
                return;
            }
            //写一个字节表示敬意
            int rt = write(tickle_fds_[1], "T", 1);
            YGW_ASSERT(rt == 1);
        }

        //--------//
        //Stopping// 
        //--------//
        /*
        bool IOManager::Stopping(uint64_t& timeout) 
        {
            timeout = getNextTimer();
            return timeout == ~0ull
                && pending_event_count_ == 0
                && Scheduler::Stopping();
        }
        */


        bool IOManager::Stopping() 
        {
            //uint64_t timeout = 0;
            //return Stopping(timeout);
            return Scheduler::Stopping() && pending_event_count_ == 0;
        }

        void IOManager::Idle() 
        {
            YGW_LOG_DEBUG(g_logger) << "idle";
            const uint64_t MAX_EVNETS = 256;
            epoll_event* events = new epoll_event[MAX_EVNETS]();
            std::shared_ptr<epoll_event> shared_events(events, [](epoll_event* ptr){
                    delete[] ptr;
            });

            while (true) 
            {
                uint64_t next_timeout = 0;
                //if (YGW_UNLIKELY(Stopping(next_timeout))) 
                if (Stopping()) 
                {
                    YGW_LOG_INFO(g_logger) << "name=" << GetName()
                        << " idle stopping exit";
                    break;
                }

                int rt = 0;
                do 
                {
                    static const int MAX_TIMEOUT = 3000;
                    if (next_timeout != ~0ull) 
                    {
                        next_timeout = (int)next_timeout > MAX_TIMEOUT
                            ? MAX_TIMEOUT : next_timeout;
                    } 
                    else 
                    {
                        next_timeout = MAX_TIMEOUT;
                    }
                    rt = epoll_wait(epfd_, events, MAX_EVNETS, (int)next_timeout);
                    if (rt < 0 && errno == EINTR) 
                    {

                    } 
                    else 
                    {
                        break;
                    }
                } while (true);

                std::vector<std::function<void()> > cbs;
                //ListExpiredCb(cbs);
                if (!cbs.empty()) 
                {
                    //YGW_LOG_DEBUG(g_logger) << "on timer cbs.size=" << cbs.size();
                    Schedule(cbs.begin(), cbs.end());
                    cbs.clear();
                }

                //if (YGW_UNLIKELY(rt == MAX_EVNETS)) {
                //    YGW_LOG_INFO(g_logger) << "epoll wait events=" << rt;
                //}

                //
                for (int i = 0; i < rt; ++i) 
                {
                    epoll_event& event = events[i];
                    if (event.data.fd == tickle_fds_[0]) 
                    {
                        uint8_t dummy[256];
                        while (read(tickle_fds_[0], dummy, sizeof(dummy)) > 0)
                            ;//do noting 
                        continue;
                    }

                    FdContext* fd_ctx = (FdContext*)event.data.ptr;
                    FdContext::MutexType::Lock lock(fd_ctx->mutex);
                    if (event.events & (EPOLLERR | EPOLLHUP)) 
                    {
                        event.events |= (EPOLLIN | EPOLLOUT) & fd_ctx->events;
                    }
                    int real_events = Event::kNone;

                    if (event.events & EPOLLIN) //读事件
                    {
                        real_events |= Event::kRead;
                    }
                    if (event.events & EPOLLOUT) //写事件
                    {
                        real_events |= Event::kWrite;
                    }

                    if ((fd_ctx->events & real_events) == Event::kNone) //没有事件
                    {
                        continue;
                    }

                    int left_events = (fd_ctx->events & ~real_events);   // 剩余事件
                    int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
                    event.events = EPOLLET | left_events; // 边缘触发剩余事件


                    int rt2 = epoll_ctl(epfd_, op, fd_ctx->fd, &event);//添加到epoll树
                    if (rt2) 
                    {
                        YGW_LOG_ERROR(g_logger) << "epoll_ctl(" << epfd_ << ", "
                            << (EpollCtlOp)op << ", " << fd_ctx->fd << ", " << (EPOLL_EVENTS)event.events << "):"
                            << rt2 << " (" << errno << ") (" << strerror(errno) << ")";
                        continue; //放弃本次操作
                    }

                    //YGW_LOG_INFO(g_logger) << " fd=" << fd_ctx->fd << " events=" << fd_ctx->events
                    //                         << " real_events=" << real_events;

                    if (real_events & Event::kRead)  // 读事件
                    {
                        fd_ctx->TriggerEvent(Event::kRead);
                        --pending_event_count_;
                    } 
                    if (real_events & Event::kWrite)  // 写事件
                    {
                        fd_ctx->TriggerEvent(Event::kWrite);
                        --pending_event_count_;
                    }
                }

                //让出执行权，回到main fiber
                Fiber::ptr cur = Fiber::GetThis();
                auto raw_ptr = cur.get();
                cur.reset();
                raw_ptr->SwapOut();
            }
        }

         
       //void IOManager::OnTimerInsertedAtFront() 
       //{
       //    Tickle();
       //}
                

    } // namespace scheduler

    //------------------------------------------------------------------- 

} // namespace ygw 
