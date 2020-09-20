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
#include <cstdio>
#include <cstdint>

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>
#include <json/json.h>
#include <boost/lexical_cast.hpp>

#include <sys/types.h>
#include <sys/syscall.h>

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


        std::string Time2Str(time_t ts = time(0), const std::string& format = "%Y-%m-%d %H:%M:%S");
        time_t Str2Time(const char* str, const char* format = "%Y-%m-%d %H:%M:%S");



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

        // 文件操作工具
        class FSUtil {
        public:
            static void ListAllFile(std::vector<std::string>& files
                    ,const std::string& path
                    ,const std::string& subfix);
            static bool Mkdir(const std::string& dirname);
            static bool IsRunningPidfile(const std::string& pidfile);
            static bool Rm(const std::string& path);
            static bool Mv(const std::string& from, const std::string& to);
            static bool Realpath(const std::string& path, std::string& rpath);
            static bool Symlink(const std::string& frm, const std::string& to);
            static bool Unlink(const std::string& filename, bool exist = false);
            static std::string Dirname(const std::string& filename);
            static std::string Basename(const std::string& filename);
            static bool OpenForRead(std::ifstream& ifs, const std::string& filename
                    ,std::ios_base::openmode mode);
            static bool OpenForWrite(std::ofstream& ofs, const std::string& filename
                    ,std::ios_base::openmode mode);
        };


        // 字符串工具
        class StringUtil {
        public:
            static std::string Format(const char* fmt, ...);
            static std::string Formatv(const char* fmt, va_list ap);

            static std::string UrlEncode(const std::string& str, bool space_as_plus = true);
            static std::string UrlDecode(const std::string& str, bool space_as_plus = true);

            static std::string Trim(const std::string& str, const std::string& delimit = " \t\r\n");
            static std::string TrimLeft(const std::string& str, const std::string& delimit = " \t\r\n");
            static std::string TrimRight(const std::string& str, const std::string& delimit = " \t\r\n");


            static std::string ToUpper(const std::string& name);
            static std::string ToLower(const std::string& name);

            static std::string WStringToString(const std::wstring& ws);
            static std::wstring StringToWString(const std::string& s);
        };

    } // namespace util 

    //-----------------------------------------------

} // namespace ygw

#endif //__YGW_UTIL_H__
