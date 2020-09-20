/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: thread.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-04
 *  Description: 
 * ====================================================
 */
#ifndef __YGW_THREAD_H__
#define __YGW_THREAD_H__

#include <functional>
#include <string>
#include <memory>
#include "noncopyable.h"
#include "mutex.h"


namespace ygw {

    //-------------------------------------------------------

    namespace thread {

//#ifdef _MSC_VER
//        struct thread_t;
//#elif __GNUC__
//        using thread_t = unsigned long;
//#endif //


        //---------------------------------------------------

        class Thread : able::Noncopyable {
        public:
            using ptr = std::shared_ptr<Thread>;
            /**
             ** @brief 构造函数
             ** @param[in] cb 线程执行函数
             ** @param[in] name 线程名称
             **/
            Thread(std::function<void()> cb, const std::string& name);

            /**
             ** @brief 析构函数
             **/
            ~Thread();

            /**
             ** @brief 线程ID
             **/
            pid_t GetId() const { return id_; }

            /**
             ** @brief 线程名称
             **/
            const std::string& GetName() const { return name_; }

            /**
             ** @brief 等待线程执行完成
             **/
            void Join();

            /**
             ** @brief 获取当前的线程指针
             **/
            static Thread* GetThisPtr();

            /**
             ** @brief 获取当前的线程名称
             **/
            static const std::string& GetThisName();

            /**
             ** @brief 设置当前线程名称
             ** @param[in] name 线程名称
             **/
            static void SetName(const std::string& name);
        private:
            /**
             ** @brief 线程执行函数
             **/
            static void* Run(void* arg);
        private:
            /// 线程id
            pid_t id_ = -1;
            /// 线程结构
            pthread_t thread_;
            /// 线程执行函数
            std::function<void()> cb_;
            /// 线程名称
            std::string name_;
            ///
            bool is_init_ = false;
            /// 信号量
            Semaphore semaphore_;
        };


    } // namespace thread


    //--------------------------------------------------------

} // namespace ygw

#endif // __YGW_THREAD_H__
