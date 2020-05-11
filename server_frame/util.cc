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
#ifdef _MSC_VER
#include <Windows.h>
#elif __GNUC__
#include <execinfo.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/time.h>
#include <unistd.h>
#endif //_MSC_VER

#include "util.h"
#include "log.h"
#include "fiber.h"
namespace ygw {
     
    //--------------------------------------------------
     
    namespace util {

        static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

        int GetThreadId()
        {
#ifdef _MSC_VER
            return ::GetCurrentThreadId();
#elif __GNUC__
            return syscall(SYS_gettid);
#endif //  
        }

        int GetFiberId()
        {
            return static_cast<int>(ygw::scheduler::Fiber::GetFiberId());
        }

        
        bool Backtrace(std::vector<std::string>* bt, int size, int offset)
        {
            if (!bt)
            {
                YGW_LOG_ERROR(g_logger) << "Backtrace's args bt is null";
                return false;
            }
            void **array = (void**)malloc((sizeof(void*)* size));
            size_t s = ::backtrace(array, size);
            char ** strings = backtrace_symbols(array, s);
            if (!strings)
            {
                YGW_LOG_ERROR(g_logger) << "backtrace_symbols error";
                return false;
            }
            for (size_t i = offset; i < s; ++i)
            {
                bt->emplace_back(strings[i]);
            }
            free(strings);
            free(array);
            return true;
        }


        std::string BacktraceToString(int size, int offset, const std::string& prefix)
        {
            std::vector<std::string> bt;
            Backtrace(&bt, size, offset);
            std::stringstream ss;
            for (size_t i = 0; i < bt.size(); ++i)
            {
                ss << prefix << bt[i] << std::endl;
            }
            return ss.str();
        }


        uint64_t GetCurrentMS() 
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return tv.tv_sec * 1000ul  + tv.tv_usec / 1000;
        }

        uint64_t GetCurrentUS() 
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return tv.tv_sec * 1000 * 1000ul  + tv.tv_usec;
        }

    } // namespace util

    //----------------------------------------------------

} // namespace ygw 
