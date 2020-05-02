/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: util.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-01
 *  Description: 
 * ====================================================
 */
#include "util.h"
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <pthread.h>
namespace ygw {
     
    //--------------------------------------------------
     
    namespace util {

        int GetThreadId()
        {
            return syscall(SYS_gettid);            
        }

        int GetFiberId()
        {
            return 0;
        }
    } // namespace util

    //----------------------------------------------------

} // namespace ygw 
