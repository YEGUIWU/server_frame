/**
 * @file thread.h
 * @brief 基于pthread封装的线程库
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-10-01
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.top
 */
#ifndef __YGW_THREAD_H__
#define __YGW_THREAD_H__

#include <functional>
#include <string>
#include <memory>

#include "mutex.h"
#include "server_frame/noncopyable.h"

namespace ygw {

    //-------------------------------------------------------

    namespace thread {

        /**
         * @brief 线程类封装
         */
        //---------------------------------------------------

        class Thread : able::Noncopyable {
        public:
            using ptr = std::shared_ptr<Thread>;
            /**
             * @brief 构造函数
             * @param[in] cb 线程执行函数
             * @param[in] name 线程名称
             */
            Thread(std::function<void()> cb, const std::string& name);

            /**
             * @brief 析构函数
             */
            ~Thread();

            /**
             * @brief 线程ID
             */
            pid_t GetId() const { return id_; }

            /**
             * @brief 线程名称
             */
            const std::string& GetName() const { return name_; }

            /**
             * @brief 等待线程执行完成
             */
            void Join();

            /**
             * @brief 获取当前的线程指针
             */
            static Thread* GetThisPtr();

            /**
             * @brief 获取当前的线程名称
             */
            static const std::string& GetThisName();

            /**
             * @brief 设置当前线程名称
             * @param[in] name 线程名称
             */
            static void SetName(const std::string& name);
        private:
            /**
             * @brief 线程执行函数
             */
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
            /// 信号量
            Semaphore semaphore_;
        };


    } // namespace thread


    //--------------------------------------------------------

} // namespace ygw

#endif // __YGW_THREAD_H__
