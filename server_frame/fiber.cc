/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: /mnt/c/Users/Administrator/ubuntu/server_frame/server_frame/fiber.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-05
 *  Description: 
 * ====================================================
 */

#include <cstdlib>

#include <atomic>

#include "config.h"  // 里面有log.h util.h
#include "fiber.h"
#include "macro.h"
#include "scheduler.h"

namespace ygw {

    //---------------------------------------------------

    namespace scheduler {

        //------------------------------------------------
        //static var
        static log::Logger::ptr g_logger = YGW_LOG_NAME("system");

        static std::atomic<uint64_t> s_fiber_id {0};
        static std::atomic<uint64_t> s_fiber_count {0};

        static thread_local Fiber* t_fiber = nullptr;
        static thread_local Fiber::ptr t_thread_fiber = nullptr;

        static config::ConfigVar<uint32_t>::ptr g_fiber_stack_size = 
            config::Config::Lookup<uint32_t>("fiber.stack_size", 
                    128 * 1024, 
                    "fiber stack size");

        static inline bool SetContext(ucontext_t* pcontext, ucontext_t *uc_link, void *stack, size_t size)
        {
            if (!pcontext)
            {
                return false;
            }
            pcontext->uc_link = uc_link;
            pcontext->uc_stack.ss_sp = stack;
            pcontext->uc_stack.ss_size = size;

            return true;
        }

        //-----------------------------------------------
        // class StackAllocator
        class StackAllocator {
        public:
            static void * Alloc(size_t size)
            {
                return std::malloc(size);
            }

            static void Dealloc(void* vp, size_t size)
            {
                return free(vp);
            }
        };
        

        //-----------------------------------------------
        //              class Fiber methods
        //-----------------------------------------------

        //----------------------//
        //static member function// 
        //----------------------//
        uint64_t Fiber::GetFiberId()
        {
            return t_fiber ? t_fiber->GetId() : 0;
        }


        //---------------//
        //member function// 
        //---------------//
        Fiber::Fiber()
        {
            state_ = State::kExec;
            SetThis(this);

            YGW_MSG_ASSERT(!getcontext(&context_), "getcontext");

            ++s_fiber_count;

            YGW_LOG_DEBUG(g_logger) << "Fiber::Fiber main";
        }

        
        Fiber::Fiber(std::function<void()> cb, size_t stack_size, bool use_caller)
            :id_(++s_fiber_id)
            ,cb_(cb)
        {
            ++s_fiber_count;
            stack_size_ = stack_size ? stack_size : g_fiber_stack_size->GetValue();

            stack_ = StackAllocator::Alloc(stack_size_);

            YGW_MSG_ASSERT(!getcontext(&context_), "getcontext");

            SetContext(&context_, nullptr, stack_, stack_size_);
            //context_.uc_link = nullptr;
            //context_.uc_stack.ss_sp = stack_;
            //context_.uc_stack.ss_size = stack_size_;
            
            if (!use_caller)
            {
                makecontext(&context_, &Fiber::MainFunc, 0);
            }
            else 
            {
                makecontext(&context_, &Fiber::CallerMainFunc, 0);
            }

            YGW_LOG_DEBUG(g_logger) << "Fiber::Fiber id = " << id_;
        }
        Fiber::~Fiber()
        {
            --s_fiber_count;
            if (stack_)
            {
                YGW_ASSERT(state_ == State::kTerm 
                        || state_ == State::kExcept
                        || state_ == State::kInit);
                StackAllocator::Dealloc(stack_, stack_size_);                
            }
            else
            {
                YGW_ASSERT(!cb_);
                YGW_ASSERT(state_ == State::kExec);

                Fiber* cur = t_fiber;
                if (cur == this)
                {
                    SetThis(nullptr);
                }
            }
            YGW_LOG_DEBUG(g_logger) << "Fiber::~Fiber id = " << id_
                << " total = " << s_fiber_count;
        }
        void Fiber::Reset(std::function<void()> cb)
        {
            YGW_ASSERT(stack_);
            YGW_ASSERT(state_ == State::kTerm 
                    || state_ == State::kExcept
                    || state_ == State::kInit);
            cb_ = cb;
            
            //获取上下文
            YGW_MSG_ASSERT(!getcontext(&context_), "getcontext");
           
            //设置栈

            SetContext(&context_, nullptr, stack_, stack_size_);
            //context_.uc_link = nullptr;
            //context_.uc_stack.ss_sp = stack_;
            //context_.uc_stack.ss_size = stack_size_;

            //设置协程回调
            makecontext(&context_, &Fiber::MainFunc, 0);


            state_ = State::kInit;
        }

        //当前的协程切换到后台，自己到前台
        void Fiber::SwapIn()
        {
            SetThis(this);
            YGW_ASSERT(state_ != State::kExec);
            state_ = State::kExec;
            //YGW_MSG_ASSERT(!swapcontext(&t_thread_fiber->context_, &context_),
            //        "swapcontext");
            YGW_MSG_ASSERT(!swapcontext(&Scheduler::GetMainFiber()->context_, &context_), 
                    "swapcontext");

        }

        //自己切换到后台
        void Fiber::SwapOut()
        {
            //SetThis(Scheduler::GetMainFiber());
            SetThis(t_thread_fiber.get());
            YGW_MSG_ASSERT(!swapcontext(&context_, &Scheduler::GetMainFiber()->context_), 
                    "swapcontext");
            //YGW_MSG_ASSERT(!swapcontext(&context_, &t_thread_fiber->context_), 
            //        "swapcontext");
        }

        void Fiber::Call()
        {
            SetThis(this);
            state_ = State::kExec;
            YGW_MSG_ASSERT(!swapcontext(&t_thread_fiber->context_, &context_),
                    "swapcontext"); 
        }

        void Fiber::Back()
        {
            SetThis(t_thread_fiber.get());

            YGW_MSG_ASSERT(!swapcontext(&context_, &t_thread_fiber->context_), "swapcontext");
        }


        //设置当前协程
        void Fiber::SetThis(Fiber* f)
        {
            t_fiber = f;
        }

        //获取当前协程
        Fiber::ptr Fiber::GetThis()
        {
            if (t_fiber)
            {
                return t_fiber->shared_from_this();
            }
            Fiber::ptr main_fiber(new Fiber);
            YGW_ASSERT(t_fiber == main_fiber.get());
            t_thread_fiber = main_fiber;

            return  t_fiber->shared_from_this();
        }

        //协程切换后台
        void Fiber::YieldToReady()
        {
            Fiber::ptr cur = GetThis();
            YGW_ASSERT(cur->state_ == State::kExec);
            cur->state_ = State::kReady;
            cur->SwapOut();
        }

        void Fiber::YieldToHold()
        {
            Fiber::ptr cur = GetThis();
            YGW_ASSERT(cur->state_ == State::kExec);
            cur->SwapOut();
        }

        uint64_t Fiber::TotalFibers()
        {
            return s_fiber_count;
        }

        void Fiber::MainFunc()
        {
            Fiber::ptr cur = GetThis();
            YGW_ASSERT(cur);
            try 
            {
                cur->cb_();
                cur->cb_ = nullptr;
                cur->state_ = State::kTerm;
            }
            catch (std::exception& e) 
            {
                cur->state_ = State::kExcept;
                YGW_LOG_ERROR(g_logger) << "Fiber Execpt: " << e.what()
                    << " ifber_id = " << cur->GetId()
                    << std::endl
                    << ygw::util::BacktraceToString(10);
            }
            catch (...)
            {
                cur->state_ = State::kExcept;
                YGW_LOG_ERROR(g_logger) << "Fiber Execpt"
                    << " fiber_id = " << cur->GetId()
                    << std::endl
                    << ygw::util::BacktraceToString(10);
            }

            auto raw_ptr = cur.get();
            cur.reset();
            raw_ptr->SwapOut();
            YGW_MSG_ASSERT(false, "never reach fiber_id = " + std::to_string(raw_ptr->GetId()));
        }

        void Fiber::CallerMainFunc() 
        {
            Fiber::ptr cur = GetThis();
            YGW_ASSERT(cur);
            try 
            {
                cur->cb_();
                cur->cb_ = nullptr;
                cur->state_ = State::kTerm;
            } 
            catch (std::exception& ex) 
            {
                cur->state_ = State::kExcept;
                YGW_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
                    << " fiber_id=" << cur->GetId()
                    << std::endl
                    << util::BacktraceToString();
            } 
            catch (...) 
            {
                cur->state_ = State::kExcept;
                YGW_LOG_ERROR(g_logger) << "Fiber Except"
                    << " fiber_id=" << cur->GetId()
                    << std::endl
                    << util::BacktraceToString();
            }

            auto raw_ptr = cur.get();
            cur.reset();
            raw_ptr->Back();
            YGW_MSG_ASSERT(false, "never reach fiber_id=" + std::to_string(raw_ptr->GetId()));

        }


    } // namespace thread  

    //---------------------------------------------------

} // namespace ygw
