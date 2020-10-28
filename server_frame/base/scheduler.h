/**
 * @file scheduler.h
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-09-27
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.top
 */

#ifndef __YGW_SCHEDULER_H__
#define __YGW_SCHEDULER_H__

#include <list>
#include <memory>
#include <vector>

#include "fiber.h"
#include "server_frame/noncopyable.h"
#include "thread.h"

namespace ygw {

    namespace scheduler {


        /**
         * @brief 协程调度器
         * @details 封装的是N-M的协程调度器
         *          内部有一个线程池,支持协程在线程池里面切换
         */
        class Scheduler 
        {
        public:
            using ptr = std::shared_ptr<Scheduler>;
            using MutexType = thread::Mutex;

            
            /**
             * @brief 构造函数
             * @param[in] threads 线程数量
             * @param[in] use_caller 是否使用当前调用线程
             * @param[in] name 协程调度器名称
             */
            Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");

            /**
             * @brief 析构函数
             */
            virtual ~Scheduler();


            /**
             * @brief 返回协程调度器名称
             */
            const std::string& GetName() const { return name_;}

            /**
             * @brief 返回当前协程调度器
             */
            static Scheduler* GetThis();

            /**
             * @brief 返回当前协程调度器的调度协程
             */
            static Fiber* GetMainFiber();

            /**
             * @brief 启动协程调度器
             */
            void Start();

            /**
             * @brief 停止协程调度器
             */
            void Stop();

            /**
             * @brief 调度协程
             * @param[in] fc 协程或函数
             * @param[in] thread 协程执行的线程id,-1标识任意线程
             */
            template<class FiberOrCb>
            void Schedule(FiberOrCb fc, int thread = -1) 
            {
                bool need_tickle = false;
                {
                    MutexType::Lock lock(mutex_);
                    need_tickle = ScheduleNoLock(fc, thread);
                }

                if (need_tickle) 
                {
                    Tickle();
                }
            }

            /**
             * @brief 批量调度协程
             * @param[in] begin 协程数组的开始
             * @param[in] end 协程数组的结束
             */
            template<class InputIterator>
            void Schedule(InputIterator begin, InputIterator end) 
            {
                bool need_tickle = false;
                {
                    MutexType::Lock lock(mutex_);
                    while (begin != end) 
                    {
                        need_tickle = ScheduleNoLock(&*begin, -1) || need_tickle;
                        ++begin;
                    }
                }
                if (need_tickle) 
                {
                    Tickle();
                }
            }

            void SwitchTo(int thread = -1);

            std::ostream& Dump(std::ostream& os);
        protected:
            /**
             * @brief 通知协程调度器有任务了
             */
            virtual void Tickle();

            /**
             * @brief 协程调度函数
             */
            void Run();

            /**
             * @brief 返回是否可以停止
             */
            virtual bool Stopping();

            /**
             * @brief 协程无任务可调度时执行idle协程
             */
            virtual void Idle();

            /**
             * @brief 设置当前的协程调度器
             */
            void SetThis();

            /**
             * @brief 是否有空闲线程
             */
            bool HasIdleThreads() { return idle_thread_count_ > 0;}
        private:
            /**
             * @brief 协程调度启动(无锁)
             */
            template<class FiberOrCb>
            bool ScheduleNoLock(FiberOrCb fc, int thread) 
            {
                bool need_tickle = fibers_.empty(); // 有协程任务就need_tickle
                FiberAndThread ft(fc, thread);
                if (ft.fiber_ || ft.cb_) 
                {
                    fibers_.push_back(ft);
                }
                return need_tickle;
            }
        private:
            //-----------------------------------------
            //
            /**
             * @brief 协程/函数/线程
             */
            class FiberAndThread {
            public:
                /// 协程
                Fiber::ptr fiber_;
                /// 协程执行函数
                std::function<void()> cb_;
                /// 线程id
                int thread_id_;

                /**
                 * @brief 构造函数
                 * @param[in] f 协程
                 * @param[in] thr 线程id
                 */
                FiberAndThread(Fiber::ptr f, int th_id)
                    : fiber_(f), thread_id_(th_id)
                {

                }
                /**
                 * @brief 构造函数
                 * @param[in] f 协程指针
                 * @param[in] thr 线程id
                 * @post *f = nullptr
                 */
                FiberAndThread(Fiber::ptr *f, int th_id)
                    : thread_id_(th_id)
                {
                   fiber_.swap(*f);
                }
                /**
                 * @brief 构造函数
                 * @param[in] f 协程执行函数
                 * @param[in] thr 线程id
                 */
                FiberAndThread(std::function<void()> f, int th_id)
                    : cb_(f), thread_id_(th_id)
                {

                }

                /**
                 * @brief 构造函数
                 * @param[in] f 协程执行函数指针
                 * @param[in] thr 线程id
                 * @post *f = nullptr
                 */
                FiberAndThread(std::function<void()> *f, int th_id)
                    : thread_id_(th_id)
                {
                    cb_.swap(*f);
                }
                /**
                 * @brief 无参构造函数
                 */
                FiberAndThread()
                    : thread_id_(-1)
                {

                }
                /**
                 * @brief 重置数据
                 */
                void Reset() 
                {
                    fiber_ = nullptr;
                    cb_ = nullptr;
                    thread_id_ = -1;
                }
            }; // class FiberAndThread
            //-----------------------------------------
            
        private:
            /// Mutex
            MutexType mutex_;
            /// 线程池
            std::vector<thread::Thread::ptr> threads_;
            /// 待执行的协程队列
            std::list<FiberAndThread> fibers_;
            /// use_caller为true时有效, 调度协程
            Fiber::ptr root_fiber_;
            /// 协程调度器名称
            std::string name_;
        protected:
            /// 协程下的线程id数组
            std::vector<int> thread_ids_;
            /// 线程数量
            size_t thread_count_ = 0;
            /// 工作线程数量
            std::atomic<size_t> active_thread_count_ = {0};
            /// 空闲线程数量
            std::atomic<size_t> idle_thread_count_ = {0};
            /// 是否正在停止
            bool stopping_ = true;
            /// 是否自动停止
            bool auto_stop_ = false;
            /// 主线程id(use_caller)
            int root_thread_ = 0;
        }; // class Scheduler 
    
        //---------------------------------------------------------

        class SchedulerSwitcher : public able::Noncopyable  
        {
        public:
            SchedulerSwitcher(Scheduler* target = nullptr);
            ~SchedulerSwitcher();
        private:
            Scheduler* caller_;
        };

    } // namespace thread 

    //---------------------------------------------------------

} // namespace ygw

#endif //__YGW_SCHEDULER_H__
