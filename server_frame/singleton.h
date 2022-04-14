/**
 * @file singleton.h
 * @brief 设计模式
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
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
