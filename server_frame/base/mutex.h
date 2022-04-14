/**
 * @file mutex.h
 * @brief 常用互斥量、信号量的封装
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#define __YGW_MUTEX_H__
#include <cstdint>
#include <atomic>
#include <semaphore.h>
#include <pthread.h>
#include "server_frame/noncopyable.h"

namespace ygw {

    //----------------------------------------------------

    namespace thread {

        /**
         * @brief 信号量
         */
        class Semaphore : able::Noncopyable {
        public:
            /**
             * @brief 构造函数
             * @param[in] count 信号量值的大小
             */
            Semaphore(uint32_t count = 0);

            /**
             * @brief 析构函数
             */
            ~Semaphore();

            /**
             * @brief 获取信号量
             */
            void Wait();

            /**
             * @brief 释放信号量
             */
            void Notify();
        private:
            sem_t semaphore_;
        };

        
        /**
         * @brief 局部锁的模板实现
         */
        template<class T>
        struct ScopedLockImpl {
        public:
            /**
             * @brief 构造函数
             * @param[in] mutex Mutex
             */
            ScopedLockImpl(T& mutex)
                :mutex_(mutex) 
            {
                mutex_.lock();
                locked_ = true;
            }

            /**
             * @brief 析构函数,自动释放锁
             */
            ~ScopedLockImpl() 
            {
                unlock();
            }

            /**
             * @brief 加锁
             */
            void lock() 
            {
                if (!locked_) 
                {
                    mutex_.lock();
                    locked_ = true;
                }
            }

            /**
             * @brief 解锁
             */
            void unlock() 
            {
                if (locked_) 
                {
                    mutex_.unlock();
                    locked_ = false;
                }
            }
        private:
            /// mutex
            T& mutex_;
            /// 是否已上锁
            bool locked_;
        };

        /**
         *  * @brief 局部读锁模板实现
         *   */
        template<class T>
        struct ReadScopedLockImpl {
        public:
            /**
             * @brief 构造函数
             * @param[in] mutex 读写锁
             */
            ReadScopedLockImpl(T& mutex)
                :mutex_(mutex) 
            {
                mutex_.rdlock();
                locked_ = true;
            }

            /**
             * @brief 析构函数,自动释放锁
             */
            ~ReadScopedLockImpl() 
            {
                unlock();
            }

            /**
             * @brief 上读锁
             */
            void lock() 
            {
                if (!locked_) 
                {
                    mutex_.rdlock();
                    locked_ = true;
                }
            }

            /**
             * @brief 释放锁
             */
            void unlock() 
            {
                if (locked_) 
                {
                    mutex_.unlock();
                    locked_ = false;
                }
            }
        private:
            /// mutex
            T& mutex_;
            /// 是否已上锁
            bool locked_;
        };

        /**
         * @brief 局部写锁模板实现
         */
        template<class T>
        struct WriteScopedLockImpl {
        public:
            /**
             * @brief 构造函数
             * @param[in] mutex 读写锁
             */
            WriteScopedLockImpl(T& mutex)
                :mutex_(mutex) 
            {
                    mutex_.wrlock();
                    locked_ = true;
            }

            /**
             * @brief 析构函数
             */
            ~WriteScopedLockImpl() 
            {
                unlock();
            }

            /**
             * @brief 上写锁
             */
            void lock() 
            {
                if (!locked_) 
                {
                    mutex_.wrlock();
                    locked_ = true;
                }
            }

            /**
             * @brief 解锁
             */
            void unlock() 
            {
                if (locked_) 
                {
                    mutex_.unlock();
                    locked_ = false;
                }
            }
        private:
            /// Mutex
            T& mutex_;
            /// 是否已上锁
            bool locked_;
        };

        /**
         * @brief 互斥量
         */
        class Mutex : able::Noncopyable 
        {
        public: 
            /// 局部锁
            using Lock = ScopedLockImpl<Mutex>; 

            /**
             * @brief 构造函数
             */
            Mutex();

            /**
             * @brief 析构函数
             */
            ~Mutex();

            /**
             * @brief 加锁
             */
            void lock();

            /**
             * @brief 解锁
             */
            void unlock();
        private:
            /// mutex
            pthread_mutex_t mutex_;
        };

        /**
         *  * @brief 空锁(用于调试)
         *   */
        class NullMutex : able::Noncopyable{
        public:
        /// 局部锁
        using Lock = ScopedLockImpl<NullMutex>;

        /**
         * @brief 构造函数
         */
        NullMutex() {}

        /**
         * @brief 析构函数
         */
        ~NullMutex() {}

        /**
         * @brief 加锁
         */
        void lock() {}

        /**
         * @brief 解锁
         */
        void unlock() {}
        };

        /**
         *  * @brief 读写互斥量
         *   */
        class RWMutex : able::Noncopyable{
        public:
            /// 局部读锁
            typedef ReadScopedLockImpl<RWMutex> ReadLock;
            /// 局部写锁
            typedef WriteScopedLockImpl<RWMutex> WriteLock;

            /**
             * @brief 构造函数
             */
            RWMutex();

            /**
             * @brief 析构函数
             */
            ~RWMutex();

            /**
             * @brief 上读锁
             */
            void rdlock();
            void lock();

            /**
             * @brief 上写锁
             */
            void wrlock();
            void lock_shared();

            /**
             * @brief 解锁
             */
            void unlock();
            void unlock_shared();
        private:
            /// 读写锁
            pthread_rwlock_t lock_;
        };

        /**
         *  * @brief 空读写锁(用于调试)
         *   */
        class NullRWMutex : able::Noncopyable {
        public:
            /// 局部读锁
            using ReadLock  = ReadScopedLockImpl<NullMutex>; 
            /// 局部写锁
            using WriteLock = WriteScopedLockImpl<NullMutex>;

            /**
             * @brief 构造函数
             */
            NullRWMutex() {}
            /**
             * @brief 析构函数
             */
            ~NullRWMutex() {}

            /**
             * @brief 上读锁
             */
            void rdlock() {}

            /**
             * @brief 上写锁
             */
            void wrlock() {}
            /**
             * @brief 解锁
             */
            void unlock() {}
        };

        /**
         * @brief 自旋锁
         */
        class Spinlock : able::Noncopyable {
        public:
        /// 局部锁
            using Lock = ScopedLockImpl<Spinlock>; 

            /**
             * @brief 构造函数
             */
            Spinlock(); 

            /**
             * @brief 析构函数
             */
            ~Spinlock(); 

            /**
             * @brief 上锁
             */
            void lock();

            /**
             * @brief 解锁
             */
            void unlock();
        private:
            /// 自旋锁
            pthread_spinlock_t mutex_;
        };

        /**
         * @brief 原子锁
         */
        class CASLock : able::Noncopyable {
        public:
            /// 局部锁
            using Lock = ScopedLockImpl<CASLock>; 

            /**
             * @brief 构造函数
             */
            CASLock();

            /**
             * @brief 析构函数
             */
            ~CASLock() {
            }

            /**
             * @brief 上锁
             */
            void lock();

            /**
             * @brief 解锁
             */
            void unlock();
        private:
            /// 原子状态
            volatile std::atomic_flag mutex_;
        };


    } // namespace thread 

    //----------------------------------------------------

} // namespace ygw 

#endif // __YGW_MUTEX_H__
