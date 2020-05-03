/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: singleton.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-02
 *  Description: 
 * ====================================================
 */
#ifndef __YGW_SINGLETON_H__
#define __YGW_SINGLETON_H__
#include <memory>

namespace ygw {

    //--------------------------------------------------
    // 设计模式
    namespace mode {

        // 单例设计模式
        template <class T, class X = void, int N = 0>
        class Singleton {
        public:
            static T* GetInstance()
            {
                static T v;
                return &v;
            }
        }; // class Singleton

        template <class T, class X = void, int N = 0>
        class SingletonPtr {
        public:
            static std::shared_ptr<T> GetInstance() 
            {
                static std::shared_ptr<T> v(new T);
                return v;
            }
        }; // class SingletonPtr

    } // namespace mode

    //--------------------------------------------------

} // namespace ygw



#endif // __YGW_SINGLETON_H__
