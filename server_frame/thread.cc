/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: thread.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-04
 *  Description: 
 * ====================================================
 */

#include <pthread.h>

#include "thread.h"
#include "log.h"
#include "util.h"

namespace ygw {

    //----------------------------------------------

    namespace thread {
        
        static thread_local Thread* t_thread = nullptr;
        static thread_local std::string t_thread_name = "UNKNOW";

        static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

        Thread* Thread::GetThisPtr() 
        {
            return t_thread;
        }

        const std::string& Thread::GetThisName() 
        {
                return t_thread_name;
        }

        void Thread::SetName(const std::string& name) 
        {
            if (name.empty()) 
            {
                return;
            }
            if (t_thread) 
            {
                t_thread->name_ = name;
            }
            t_thread_name = name;
        }

        Thread::Thread(std::function<void()> cb, const std::string& name)
            : cb_(cb)
              , name_(name) 
        {
            if (name.empty()) 
            {
                name_ = "UNKNOW";
            }
            int rt = pthread_create(&thread_, nullptr, &Thread::Run, this);
            if (rt) 
            {
                YGW_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << rt
                    << " name=" << name;
                throw std::logic_error("pthread_create error");
            }
            is_init_ = true;
            //semaphore_.Wait();
        }
        
        Thread::~Thread() 
        {
            //if (thread_) 
            if (is_init_)
            {
                pthread_detach(thread_);
            }
        }
        
        void Thread::Join() 
        {
            //if (thread_) 
            if (is_init_)
            {
                int rt = pthread_join(thread_, nullptr);
                if(rt) 
                {
                    YGW_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << rt
                        << " name=" << name_;
                    throw std::logic_error("pthread_join error");
                }
                is_init_ = false;
                //thread_ = 0;
            }
        }

        void* Thread::Run(void* arg) 
        {

            Thread* thread = (Thread*)arg;
            t_thread = thread;
            t_thread_name = thread->name_;
            //获取线程ID
            thread->id_ = ygw::util::GetThreadId();
            //设置线程名: 最大支持16未
#ifdef _MSC_VER

#elif __GNUC__
            pthread_setname_np(pthread_self(), thread->name_.substr(0, 15).c_str());
#endif //
            //对换一下
            std::function<void()> cb;
            cb.swap(thread->cb_);
            //唤醒
            //thread->semaphore_.Notify();

            cb();
            return 0;
        }


    } // namespace thread 

    //----------------------------------------------

} // namespace ygw 
