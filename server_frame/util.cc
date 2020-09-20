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
#include <arpa/inet.h>
#include <dirent.h>
#include <execinfo.h>
#include <ifaddrs.h>
#include <sys/stat.h>
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

        // 获取协程Id
        int GetFiberId()
        {
            return static_cast<int>(ygw::scheduler::Fiber::GetFiberId());
        }

       
        // 堆栈信息
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


        // 获取当前时间
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


        // 时间 字符串 互转
        std::string Time2Str(time_t ts, const std::string& format) 
        {
            struct tm tm;
            localtime_r(&ts, &tm);
            char buf[64];
            strftime(buf, sizeof(buf), format.c_str(), &tm);
            return buf;
        }

        time_t Str2Time(const char* str, const char* format) 
        {
            struct tm t;
            memset(&t, 0, sizeof(t));
            if(!strptime(str, format, &t)) 
            {
                return 0;
            }
            return mktime(&t);
        }
        
        
        //-----------------------------------------------------------------------------------
        //                  FSUtil Method
        //-----------------------------------------------------------------------------------
        static int __lstat(const char* file, struct stat* st = nullptr) 
        {
            struct stat lst;
            int ret = lstat(file, &lst);
            if(st) {
                *st = lst;
            }
            return ret;
        }
        bool FSUtil::Unlink(const std::string& filename, bool exist) 
        {
            if(!exist && __lstat(filename.c_str())) 
            {
                return true;
            }
            return ::unlink(filename.c_str()) == 0;
        }

        //-----------------------------------------------------------------------------------
        //                  StringUtil Method
        //-----------------------------------------------------------------------------------
        // 大小写转换
        std::string StringUtil::ToUpper(const std::string& name) 
        {
            std::string rt = name;
            std::transform(rt.begin(), rt.end(), rt.begin(), ::toupper);
            return rt;
        }

        std::string StringUtil::ToLower(const std::string& name) 
        {
            std::string rt = name;
            std::transform(rt.begin(), rt.end(), rt.begin(), ::tolower);
            return rt;
        }

        std::string StringUtil::Trim(const std::string& str, const std::string& delimit) 
        {
            auto begin = str.find_first_not_of(delimit);
            if (begin == std::string::npos) 
            {
                return "";
            }
            auto end = str.find_last_not_of(delimit);
            return str.substr(begin, end - begin + 1);
        }

        std::string StringUtil::TrimLeft(const std::string& str, const std::string& delimit) 
        {
            auto begin = str.find_first_not_of(delimit);
            if(begin == std::string::npos) 
            {
                return "";
            }
            return str.substr(begin);
        }

        std::string StringUtil::TrimRight(const std::string& str, const std::string& delimit) 
        {
            auto end = str.find_last_not_of(delimit);
            if(end == std::string::npos) 
            {
                return "";
            }
            return str.substr(0, end);
        }


        static const char uri_chars[256] = {
            /* 0 */
            0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 1, 1, 0,
            1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 0, 0, 0, 1, 0, 0,
            /* 64 */
            0, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 0, 0, 0, 0, 1,
            0, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 0, 0, 0, 1, 0,
            /* 128 */
            0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
            /* 192 */
            0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
        };

        static const char xdigit_chars[256] = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,
            0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        };

#define CHAR_IS_UNRESERVED(c)           \
        (uri_chars[(unsigned char)(c)])

        //-.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz~
        std::string StringUtil::UrlEncode(const std::string& str, bool space_as_plus) {
            static const char *hexdigits = "0123456789ABCDEF";
            std::string* ss = nullptr;
            const char* end = str.c_str() + str.length();
            for(const char* c = str.c_str() ; c < end; ++c) {
                if(!CHAR_IS_UNRESERVED(*c)) {
                    if(!ss) {
                        ss = new std::string;
                        ss->reserve(str.size() * 1.2);
                        ss->append(str.c_str(), c - str.c_str());
                    }
                    if(*c == ' ' && space_as_plus) {
                        ss->append(1, '+');
                    } else {
                        ss->append(1, '%');
                        ss->append(1, hexdigits[(uint8_t)*c >> 4]);
                        ss->append(1, hexdigits[*c & 0xf]);
                    }
                } else if(ss) {
                    ss->append(1, *c);
                }
            }
            if(!ss) {
                return str;
            } else {
                std::string rt = *ss;
                delete ss;
                return rt;
            }
        }

        std::string StringUtil::UrlDecode(const std::string& str, bool space_as_plus) {
            std::string* ss = nullptr;
            const char* end = str.c_str() + str.length();
            for(const char* c = str.c_str(); c < end; ++c) {
                if(*c == '+' && space_as_plus) {
                    if(!ss) {
                        ss = new std::string;
                        ss->append(str.c_str(), c - str.c_str());
                    }
                    ss->append(1, ' ');
                } else if(*c == '%' && (c + 2) < end
                        && isxdigit(*(c + 1)) && isxdigit(*(c + 2))){
                    if(!ss) {
                        ss = new std::string;
                        ss->append(str.c_str(), c - str.c_str());
                    }
                    ss->append(1, (char)(xdigit_chars[(int)*(c + 1)] << 4 | xdigit_chars[(int)*(c + 2)]));
                    c += 2;
                } else if(ss) {
                    ss->append(1, *c);
                }
            }
            if(!ss) {
                return str;
            } else {
                std::string rt = *ss;
                delete ss;
                return rt;
            }
        }

    } // namespace util

    //----------------------------------------------------

} // namespace ygw 
