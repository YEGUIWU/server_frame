/**
 * @file noncopyable.h
 * @brief able接口封装
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */

#ifndef __YGW_ABLE_H__
#define __YGW_ABLE_H__

namespace ygw {

    namespace able {


        //-------------------------------------------------------
        /**
         ** @brief 对象无法拷贝,赋值
         **/
        class Noncopyable {
        public:
            /**
             ** @brief 默认构造函数
             **/
            Noncopyable() = default;

            /**
             ** @brief 默认析构函数
             **/
            ~Noncopyable() = default;

            /**
             ** @brief 拷贝构造函数(禁用)
             **/
            Noncopyable(const Noncopyable&) = delete;

            /**
             ** @brief 赋值函数(禁用)
             **/
            Noncopyable& operator=(const Noncopyable&) = delete;
        };

        //------------------------------------------------------

    } // namespace interface 

} // namespace ygw

#endif // __YGW_ABLE_H__
