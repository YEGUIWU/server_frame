/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/able.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-04
 *  Description: 
 * ====================================================
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
