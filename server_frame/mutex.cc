/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/mutex.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-04
 *  Description: 
 * ====================================================
 */
#include "mutex.h" 
#include <stdexcept>
namespace ygw {


    namespace thread {

        //----------------------------------------------------
        //Semaphore

        Semaphore::Semaphore(uint32_t count) 
        {
            if (sem_init(&semaphore_, 0, count)) 
            {
                throw std::logic_error("sem_init error");
            }
        }

        Semaphore::~Semaphore() 
        {
            sem_destroy(&semaphore_);
        }

        void Semaphore::Wait() 
        {
            if(sem_wait(&semaphore_)) 
            {
                throw std::logic_error("sem_wait error");
            }
        }

        void Semaphore::Notify() 
        {
            if(sem_post(&semaphore_)) 
            {
                throw std::logic_error("sem_post error");
            }
        }

        //----------------------------------------------------
        // class Mutex method
        Mutex::Mutex()
        {
            pthread_mutex_init(&mutex_, nullptr);
        }

        Mutex::~Mutex()
        {
            pthread_mutex_destroy(&mutex_);
        }

        void Mutex::lock() 
        {
            pthread_mutex_lock(&mutex_);
        }

        void Mutex::unlock() 
        {
            pthread_mutex_unlock(&mutex_);
        }
        //----------------------------------------------------
        // class RWMutex method

        RWMutex::RWMutex() 
        {
            pthread_rwlock_init(&lock_, nullptr);
        }

        RWMutex::~RWMutex() 
        {
            pthread_rwlock_destroy(&lock_);
        }

        void RWMutex::rdlock() 
        {
            pthread_rwlock_rdlock(&lock_);
        }

        void RWMutex::lock_shared() 
        {
            pthread_rwlock_rdlock(&lock_);
        }

        void RWMutex::lock()
        {
            pthread_rwlock_wrlock(&lock_);
        }

        void RWMutex::wrlock() 
        {
            pthread_rwlock_wrlock(&lock_);
        }

        void RWMutex::unlock() 
        {
            pthread_rwlock_unlock(&lock_);
        }

        void RWMutex::unlock_shared() 
        {
            pthread_rwlock_unlock(&lock_);
        }

        //----------------------------------------------------
        // class Spinlock
        Spinlock::Spinlock() 
        {
            pthread_spin_init(&mutex_, 0);
        }

        Spinlock::~Spinlock() 
        {
            pthread_spin_destroy(&mutex_);
        }

        void Spinlock::lock() 
        {
            pthread_spin_lock(&mutex_);
        }

        void Spinlock::unlock() 
        {
            pthread_spin_unlock(&mutex_);
        }

        //----------------------------------------------------
        // class CASLock method
        CASLock::CASLock()
        {
            mutex_.clear();
        }

        void CASLock::lock() 
        {
            while(std::atomic_flag_test_and_set_explicit(&mutex_, 
                        std::memory_order_acquire));
        }

        void CASLock::unlock() 
        {
            std::atomic_flag_clear_explicit(&mutex_, std::memory_order_release);
        }


        //----------------------------------------------------
    } // namespace thread 

    //---------------------------------------------------------

} // namespace ygw 
