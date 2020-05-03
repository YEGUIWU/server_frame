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

namespace ygw {

    //-----------------------------------------------

    namespace util {
        

        int GetThreadId();

        int GetFiberId();

        template<class T>
        const char* TypeToName() 
        {
            static const char* s_name = "Hello";
            return s_name;
        }


    } // namespace util 

    //-----------------------------------------------

} // namespace ygw

#endif //__YGW_UTIL_H__
