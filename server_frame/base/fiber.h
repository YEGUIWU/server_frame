/**
 * @file fiber.h
 * @brief 协程库封装
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#ifndef __YGW_FIBER_H__
#define __YGW_FIBER_H__
#ifdef __GNUC__
#include <ucontext.h>
#endif // __GNUC__

#include <functional>
#include <memory>

namespace ygw {
    
    namespace scheduler {
        
        class Scheduler;

        class Fiber : public std::enable_shared_from_this<Fiber> {
        friend class Scheduler; 
        public:
            using ptr = std::shared_ptr<Fiber>;
            
            /**
             * @brief 协程状态
             */
            enum State {
                /// 初始化状态
                kInit,
                /// 暂停状态
                kHold,
                /// 执行中状态
                kExec,
                /// 结束状态
                kTerm,
                /// 可执行状态
                kReady,
                /// 异常状态
                kExcept 
            };
        private:
            /**
             * @brief 无参构造函数
             * @attention 每个线程第一个协程的构造
             */
            Fiber();
        public:
            /**
             * @brief 构造函数
             * @param[in] cb 协程执行的函数
             * @param[in] stacksize 协程栈大小
             * @param[in] use_caller 是否在MainFiber上调度
             */
            Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);

            /**
             * @brief 析构函数
             */
            ~Fiber();

            /**
             * @brief 重置协程执行函数,并设置状态
             * @pre GetState() 为 INIT, TERM, EXCEPT
             * @post GetState() = INIT
             */
            void Reset(std::function<void()> cb);

            /**
             * @brief 将当前协程切换到运行状态
             * @pre GetState() != EXEC
             * @post GetState() = EXEC
             */
            void SwapIn();

            /**
             * @brief 将当前协程切换到后台
             */
            void SwapOut();

            /**
             * @brief 将当前线程切换到执行状态
             * @pre 执行的为当前线程的主协程
             */
            void Call();

            /**
             * @brief 将当前线程切换到后台
             * @pre 执行的为该协程
             * @post 返回到线程的主协程
             */
            void Back();

            /**
             * @brief 返回协程id
             */
            uint64_t GetId() const { return id_; }

            /**
             * @brief 返回协程状态
             */
            State GetState() const { return state_; }
        public:

            /**
             * @brief 设置当前线程的运行协程
             * @param[in] f 运行协程
             */
            static void SetThis(Fiber* f);

            /**
             * @brief 返回当前所在的协程
             */
            static Fiber::ptr GetThis();

            /**
             * @brief 将当前协程切换到后台,并设置为READY状态
             * @post GetState() = READY
             */
            static void YieldToReady();

            /**
             * @brief 将当前协程切换到后台,并设置为HOLD状态
             * @post GetState() = HOLD
             */
            static void YieldToHold();

            /**
             * @brief 返回当前协程的总数量
             */
            static uint64_t TotalFibers();

            /**
             * @brief 协程执行函数
             * @post 执行完成返回到线程主协程
             */
            static void MainFunc();

            /**
             * @brief 协程执行函数
             * @post 执行完成返回到线程调度协程
             */
            static void CallerMainFunc();

            /**
             * @brief 获取当前协程的id
             */
            static uint64_t GetFiberId();
        private:
            /// 协程id
            uint64_t id_ = 0;
            /// 协程运行栈大小
            uint32_t stack_size_ = 0;
            /// 协程状态
            State state_ = State::kInit;
            /// 协程上下文
            ucontext_t context_;
            /// 协程运行栈指针
            void* stack_ = nullptr;
            /// 协程运行函数
            std::function<void()> cb_;

        };


        //-------------------------------------------------------

    } // namespace thread 

    //-------------------------------------------------------

} // namespace ygw 

#endif // __YGW_FIBER_H__
