/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: util.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-01
 *  Description: 
 * ====================================================
 */
#ifndef __YGW_UTIL_H__
#define __YGW_UTIL_H__
#include <cxxabi.h>

#include <string>
#include <vector>

namespace ygw {

    //-----------------------------------------------

    namespace util {
        

        int GetThreadId();

        int GetFiberId();

        template<class T>
        const char* TypeToName() 
        {
            static const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
                return s_name;
        }

        void Backtrace(std::vector<std::string>& bt, int size, int offset = 1);

        std::string BacktraceToString(int size = 64, int offset = 2, const std::string& prefix="");


    } // namespace util 

    //-----------------------------------------------

} // namespace ygw

#endif //__YGW_UTIL_H__
