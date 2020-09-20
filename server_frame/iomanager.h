/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/iomanager.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-07
 *  Description: 基于Epoll的IO协程调度器
 * ====================================================
 */
#ifndef __YGW_IOMANAGER_H__
#define __YGW_IOMANAGER_H__

#include "scheduler.h"
#include "timer.h"

namespace ygw {

    //--------------------------------------------------------------------

    namespace scheduler {


        //--------------------------------------------------------------------
        /**
         ** @brief 基于Epoll的IO协程调度器
         **/
        class IOManager : public Scheduler , public timer::TimerManager 
        {
        public:
            using ptr = std::shared_ptr<IOManager>;
            using RWMutexType = thread::RWMutex;

            /**
             ** @brief IO事件
             **/
            enum Event {
                /// 无事件
                kNone  = 0x0,
                /// 读事件(EPOLLIN)
                kRead  = 0x1,
                /// 写事件(EPOLLOUT)
                kWrite = 0x4,
            };
        private:
            /**
             ** @brief Socket事件上下文类
             **/
            class FdContext 
            {
            public:
                using MutexType = thread::Mutex;
                /**
                 ** @brief 事件上下文结构
                 **/
                struct EventContext 
                {
                    /// 事件执行的调度器
                    Scheduler* scheduler = nullptr;
                    /// 事件协程
                    Fiber::ptr fiber;
                    /// 事件的回调函数
                    std::function<void()> cb;
                };

                /**
                 ** @brief 获取事件上下文类
                 ** @param[in] event 事件类型
                 ** @return 返回对应事件的上线文
                 **/
                EventContext& GetContext(Event event);

                /**
                 ** @brief 重置事件上下文
                 ** @param[in, out] ctx 待重置的上下文类
                 **/
                void ResetContext(EventContext& ctx);

                /**
                 ** @brief 触发事件
                 ** @param[in] event 事件类型
                 **/
                void TriggerEvent(Event event);

            //private:
            public:
                /// 读事件上下文
                EventContext read_;
                /// 写事件上下文
                EventContext write_;
                /// 事件关联的句柄
                int fd = 0;
                /// 当前的事件
                Event events_ = Event::kNone;
                /// 事件的Mutex
                MutexType mutex_;
            };

        public:
            /**
             ** @brief 构造函数
             ** @param[in] threads 线程数量
             ** @param[in] use_caller 是否将调用线程包含进去
             ** @param[in] name 调度器的名称
             **/
            IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");

            /**
             ** @brief 析构函数
             **/
            ~IOManager();

            /**
             ** @brief 添加事件
             ** @param[in] fd socket句柄
             ** @param[in] event 事件类型
             ** @param[in] cb 事件回调函数
             **/
            int AddEvent(int fd, Event event, std::function<void()> cb = nullptr);

            /**
             ** @brief 删除事件
             ** @param[in] fd socket句柄
             ** @param[in] event 事件类型
             ** @attention 不会触发事件
             **/
            bool DelEvent(int fd, Event event);

            /**
             ** @brief 取消事件
             ** @param[in] fd socket句柄
             ** @param[in] event 事件类型
             ** @attention 如果事件存在则触发事件
             **/
            bool CancelEvent(int fd, Event event);

            /**
             ** @brief 取消所有事件
             ** @param[in] fd socket句柄
             **/
            bool CancelAll(int fd);

            /**
             ** @brief 返回当前的IOManager
             **/
            static IOManager* GetThis();
        protected:
            void Tickle() override;

            bool Stopping() override;

            void Idle() override;

            void OnTimerInsertedAtFront() override;

            /**
             ** @brief 重置socket句柄上下文的容器大小
             ** @param[in] size 容量大小
             **/
            void ContextResize(size_t size);

            /**
             ** @brief 判断是否可以停止
             ** @param[out] timeout 最近要出发的定时器事件间隔
             ** @return 返回是否可以停止
             **/
            bool Stopping(uint64_t* timeout);
        private:
            /// epoll 文件句柄
            int epfd_ = 0;
            /// pipe 文件句柄
            int tickle_fds_[2];
            /// 当前等待执行的事件数量
            std::atomic<size_t> pending_event_count_ = {0};
            /// IOManager的Mutex
            RWMutexType mutex_;
            /// socket事件上下文的容器
            std::vector<FdContext*> fd_contexts_;


        };  // class IOManager

    
        //--------------------------------------------------------------------

    } // namespace scheduler

} // namespace ygw

#endif // __YGW_IOMANAGER_H__
