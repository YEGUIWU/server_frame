/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/scheduler.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-06
 *  Description: 
 * ====================================================
 */

#include "scheduler.h"
#include "log.h"
#include "macro.h"
#include <algorithm>

namespace ygw {
    
    namespace scheduler {

        static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

        static thread_local Scheduler* t_scheduler = nullptr;  //当前协程调度器指针
        static thread_local Fiber* t_scheduler_fiber = nullptr;//主协程

        Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name)
            :name_(name) 
        {
            YGW_ASSERT(threads > 0);

            if (use_caller) 
            {
                Fiber::GetThis();
                --threads;

                YGW_ASSERT(GetThis() == nullptr);
                t_scheduler = this;

                root_fiber_.reset(new Fiber(std::bind(&Scheduler::Run, this), 0, true));
                thread::Thread::SetName(name_);

                t_scheduler_fiber = root_fiber_.get();
                root_thread_ = util::GetThreadId();
                thread_ids_.push_back(root_thread_);
            } 
            else 
            {
                root_thread_ = -1;
            }
            thread_count_ = threads;
        }

        Scheduler::~Scheduler() 
        {
            YGW_ASSERT(stopping_);
            if (GetThis() == this) 
            {
                t_scheduler = nullptr;
            }
        }

        Scheduler* Scheduler::GetThis() 
        {
                return t_scheduler;
        }

        Fiber* Scheduler::GetMainFiber() 
        {
                return t_scheduler_fiber;
        }

        void Scheduler::Start() 
        {
            MutexType::Lock lock(mutex_);
            if (!stopping_) 
            {
                return;
            }
            stopping_ = false;
            YGW_ASSERT(threads_.empty());

            threads_.resize(thread_count_);
            for(size_t i = 0; i < thread_count_; ++i) 
            {
                threads_[i].reset(new thread::Thread(std::bind(&Scheduler::Run, this)
                            , name_ + "_" + std::to_string(i)));
                thread_ids_.push_back(threads_[i]->GetId());
            }
            lock.unlock();
            //-------------------- 
            //不在Start时调用是因为
            //在start时调用会直接切换到run
            //如果想在Start后Stop就没机会stop了，他一直会在Start里面
            //if (root_fiber_) 
            //{
            //    //root_fiber_->SwapIn();
            //    root_fiber_->Call();
            //    YGW_LOG_INFO(g_logger) << "call out " << root_fiber->GetState();
            //}
        }

        void Scheduler::Stop() 
        {
            auto_stop_ = true;
            if (root_fiber_
                    && thread_count_ == 0
                    && (root_fiber_->GetState() == Fiber::kTerm
                        || root_fiber_->GetState() == Fiber::kInit)) 
            {
                YGW_LOG_INFO(g_logger) << this << " stopped";
                stopping_ = true;

                if (Stopping()) 
                {
                    return;
                }
            }

            //bool exit_on_this_fiber = false;
            if (root_thread_ != -1) 
            {
                YGW_ASSERT(GetThis() == this);
            } 
            else 
            {
                YGW_ASSERT(GetThis() != this);
            }

            stopping_ = true;
            for (size_t i = 0; i < thread_count_; ++i) 
            {
                Tickle();
            }

            if (root_fiber_) 
            {
                Tickle();
            }

            //Call调用放在Stop里面
            if (root_fiber_) 
            {
                //while(!stopping()) {
                //    if (root_fiber_->GetState() == Fiber::kTerm
                //            || root_fiber_->GetState() == Fiber::EXCEPT) {
                //        root_fiber_.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
                //        YGW_LOG_INFO(g_logger) << " root fiber is term, reset";
                //        t_fiber = root_fiber_.get();
                //    }
                //    root_fiber_->call();
                //}
                if (!Stopping()) 
                {
                    root_fiber_->Call();
                }
            }

            //
            std::vector<thread::Thread::ptr> threads;
            {
                MutexType::Lock lock(mutex_);
                threads.swap(threads_);
            }
            //
            //for (auto& i : threads) 
            //{
            //    i->Join();
            //}
            std::for_each(threads.begin(), threads.end(), std::mem_fn(&thread::Thread::Join));
            //if (exit_on_this_fiber) {
            //}
        }

        void Scheduler::SetThis() 
        {
            t_scheduler = this;
        }

        void Scheduler::Run() 
        {
            YGW_LOG_DEBUG(g_logger) << name_ << " run";
            //set_hook_enable(true);
            SetThis();// 主线程设置为自己


            //如果当前线程的id，不等于主的线程id
            if (util::GetThreadId() != root_thread_) 
            {
                t_scheduler_fiber = Fiber::GetThis().get();
            }


            //当调度完成时就切换到idle协程
            Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::Idle, this)));
            Fiber::ptr cb_fiber;

            FiberAndThread ft;
            bool tickle_me;
            bool is_active;
            while (true) 
            {
                ft.Reset();
                tickle_me = false;
                is_active = false;
                {
                    MutexType::Lock lock(mutex_);
                    auto it = fibers_.begin();
                    while (it != fibers_.end()) 
                    {
                        if (it->thread_id_ != -1 && it->thread_id_ != util::GetThreadId()) 
                        {
                            ++it;
                            tickle_me = true;
                            continue;
                        }

                        YGW_ASSERT(it->fiber_ || it->cb_);
                        if (it->fiber_ && it->fiber_->GetState() == Fiber::State::kExec) 
                        {//是执行中就不干
                            ++it;
                            continue;
                        }

                        //不是执行中，就从队列中取出来
                        ft = *it;
                        fibers_.erase(it++);
                        ++active_thread_count_;
                        is_active = true;
                        break;
                    }
                    tickle_me |= it != fibers_.end();
                }

                //如果需要tickle_me 就通知一下其他线程
                if (tickle_me) 
                {
                    Tickle();
                }

                //如果是协程,且状态正常
                if (ft.fiber_ && (ft.fiber_->GetState() != Fiber::State::kTerm
                            && ft.fiber_->GetState() != Fiber::State::kExcept)) 
                {
                    ft.fiber_->SwapIn();
                    --active_thread_count_;

                    if (ft.fiber_->GetState() == Fiber::State::kReady) 
                    {
                        Schedule(ft.fiber_);
                    } 
                    else if (ft.fiber_->GetState() != Fiber::State::kTerm
                            && ft.fiber_->GetState() != Fiber::State::kExcept) 
                    {
                        ft.fiber_->state_ = Fiber::State::kHold;
                    }
                    ft.Reset();
                } 
                //如果是回调
                else if (ft.cb_) 
                {
                    if (cb_fiber) 
                    {
                        cb_fiber->Reset(ft.cb_);
                    } 
                    else 
                    {
                        cb_fiber.reset(new Fiber(ft.cb_));
                    }
                    ft.Reset();

                    cb_fiber->SwapIn();
                    --active_thread_count_;
                    if (cb_fiber->GetState() == Fiber::State::kReady) 
                    {
                        Schedule(cb_fiber);
                        cb_fiber.reset();
                    } 
                    else if (cb_fiber->GetState() == Fiber::State::kExcept
                            || cb_fiber->GetState() == Fiber::State::kTerm) 
                    {
                        cb_fiber->Reset(nullptr);
                    } 
                    else 
                    {//if (cb_fiber->GetState() != Fiber::State::kTerm) {
                        cb_fiber->state_ = Fiber::State::kHold;
                        cb_fiber.reset();
                    }
                    } 
                //没有任务做
                else 
                {
                    if (is_active) 
                    {
                        --active_thread_count_;
                        continue;
                    }
                    if (idle_fiber->GetState() == Fiber::State::kTerm) 
                    {
                        YGW_LOG_INFO(g_logger) << "idle fiber term";
                        break;
                    }

                    ++idle_thread_count_;
                    idle_fiber->SwapIn();
                    --idle_thread_count_;
                    if (idle_fiber->GetState() != Fiber::State::kTerm
                            && idle_fiber->GetState() != Fiber::State::kExcept) 
                    {
                        idle_fiber->state_ = Fiber::State::kHold;
                    }
                }
            }
        }

        void Scheduler::Tickle() 
        {
            YGW_LOG_INFO(g_logger) << "tickle";
        }


        bool Scheduler::Stopping()
        {
            MutexType::Lock lock(mutex_);
            return auto_stop_ && stopping_
                && fibers_.empty() && active_thread_count_ == 0;
        }

        void Scheduler::Idle()
        {
            YGW_LOG_INFO(g_logger) << "idle";
            while (!Stopping()) 
            {
                Fiber::YieldToHold();
            }
        }

        void Scheduler::SwitchTo(int thread)
        {
            YGW_ASSERT(Scheduler::GetThis() != nullptr);
            if (Scheduler::GetThis() == this) 
            {
                if (thread == -1 || thread == util::GetThreadId()) 
                {
                    return;
                }
            }
            Schedule(Fiber::GetThis(), thread);
            Fiber::YieldToHold();
        }

        std::ostream& Scheduler::Dump(std::ostream& os)
        {
            os << "[Scheduler name=" << name_
                << " size=" << thread_count_
                << " active_count=" << active_thread_count_
                << " idle_count=" << idle_thread_count_
                << " stopping=" << stopping_
                << " ]" << std::endl << "    ";
            for (size_t i = 0; i < thread_ids_.size(); ++i) 
            {
                if (i) 
                {
                    os << ", ";
                }
                os << thread_ids_[i];
            }
            return os;
        }

        SchedulerSwitcher::SchedulerSwitcher(Scheduler* target)
        {
            caller_ = Scheduler::GetThis();
            if (target)
            {
                target->SwitchTo();
            }
        }

        SchedulerSwitcher::~SchedulerSwitcher()
        {
            if (caller_)
            {
                caller_->SwitchTo();
            }
        }
        
    }  // namespace thread 

} // namespace ygw 
