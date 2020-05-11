/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: util.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-01
 *  Description: 常用的工具函数
 * ====================================================
 */
#ifndef __YGW_UTIL_H__
#define __YGW_UTIL_H__
#ifdef __GNUC__
#include <cxxabi.h>
#endif 

#include <string>
#include <vector>

namespace ygw {

    //-----------------------------------------------

    namespace util {

        /**
         ** @brief 返回当前线程的ID
         **/
        int GetThreadId();

        /**
         ** @brief 返回当前协程的ID
         **/
        int GetFiberId();

        /**
         ** @brief 获取当前的调用栈
         ** @param[out] bt 保存调用栈
         ** @param[in] size 最多返回层数
         ** @param[in] skip 跳过栈顶的层数
         **/
        void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);

        /**
         ** @brief 获取当前栈信息的字符串
         ** @param[in] size 栈的最大层数
         ** @param[in] skip 跳过栈顶的层数
         ** @param[in] prefix 栈信息前输出的内容
         **/
        std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");

        /**
         ** @brief 获取当前时间的毫秒
         **/
        uint64_t GetCurrentMS();

        /**
         ** @brief 获取当前时间的微秒
         **/
        uint64_t GetCurrentUS();



        /**
         ** @brief 获取类型名
         **/
        template<class T>
        inline const char* TypeToName() 
        {
#ifdef _MSC_VER
            return typeid(T).name();
#elif __GNUC__
            static const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
            return s_name;
#endif //
        }


    } // namespace util 

    //-----------------------------------------------

} // namespace ygw

#endif //__YGW_UTIL_H__
