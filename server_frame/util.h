/**
 * @file util.h
 * @brief 常用的工具函数
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#ifndef __YGW_UTIL_H__
#define __YGW_UTIL_H__
#ifdef __GNUC__
#include <cxxabi.h>
#endif 

#include <sys/types.h>
#include <sys/syscall.h>

#include <cstdio>
#include <cstdint>

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <google/protobuf/message.h>
#include <json/json.h>
#include <yaml-cpp/yaml.h>

#include "util/json_util.h"



namespace ygw {

    //-----------------------------------------------

    namespace util {

        /**
         * @brief 返回当前线程的ID
         */
        int GetThreadId();

        /**
         * @brief 返回当前协程的ID
         */
        int GetFiberId();

        /**
         * @brief 获取当前的调用栈
         * @param[out] bt 保存调用栈
         * @param[in] size 最多返回层数
         * @param[in] skip 跳过栈顶的层数
         */
        void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);

        /**
         * @brief 获取当前栈信息的字符串
         * @param[in] size 栈的最大层数
         * @param[in] skip 跳过栈顶的层数
         * @param[in] prefix 栈信息前输出的内容
         */
        std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");


        std::string GetHostName();

	    std::string GetIPv4();

        /**
         * @brief 时间工具
         */
        class TimeUtil {
        public:
            /**
             * @brief 获取当前时间的毫秒
             */
            static uint64_t GetCurrentMS();

            /**
             * @brief 获取当前时间的微秒
             */
            static uint64_t GetCurrentUS();


            /**
             * @brief 时间转字符串
             * @param[in] ts time_t类型的时间值
             * @param[in] format 格式
             * @return std::string 时间字符串
             */
            static std::string Time2Str(time_t ts = time(0), const std::string& format = "%Y-%m-%d %H:%M:%S");

            /**
             * @brief 字符串转时间
             * @param[in] str C字符串类型的时间
             * @param[in] format 时间格式
             */
            static time_t Str2Time(const char* str, const char* format = "%Y-%m-%d %H:%M:%S");
        };
        

        /**
         * @brief 获取类型名
         */
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
            static std::shared_ptr<FILE> OpenFile(const std::string& filename, const char* mode);
            static std::string GetCurDir();
            static int GetFileSize(FILE* pf);
        };


        template<class V, class Map, class K>
        V GetParamValue(const Map& m, const K& k, const V& def = V()) 
        {
            auto it = m.find(k);
            if (it == m.end()) 
            {
                return def;
            }
            try 
            {
                return boost::lexical_cast<V>(it->second);
            } 
            catch (...) 
            {
            }
            return def;
        }

        template<class V, class Map, class K>
        bool CheckGetParamValue(const Map& m, const K& k, V& v) 
        {
            auto it = m.find(k);
            if (it == m.end())
            {
                return false;
            }
            try 
            {
                v = boost::lexical_cast<V>(it->second);
                return true;
            } 
            catch (...) 
            {
            }
            return false;
        }


        // 类型转换工具
        class TypeUtil {
        public:
            static int8_t ToChar(const std::string& str);
            static int64_t Atoi(const std::string& str);
            static double Atof(const std::string& str);
            static int8_t ToChar(const char* str);
            static int64_t Atoi(const char* str);
            static double Atof(const char* str);
        };

        class Atomic {
        public:
            // +
            template<class T, class S = T>
            static T AddFetch(volatile T& t, S v = 1) 
            {
                return __sync_add_and_fetch(&t, (T)v);
            }
            // -
            template<class T, class S = T>
            static T SubFetch(volatile T& t, S v = 1) 
            {
                    return __sync_sub_and_fetch(&t, (T)v);
            }
            // | 
            template<class T, class S>
            static T OrFetch(volatile T& t, S v) 
            {
                return __sync_or_and_fetch(&t, (T)v);
            }
            // &
            template<class T, class S>
            static T AndFetch(volatile T& t, S v) 
            {
                return __sync_and_and_fetch(&t, (T)v);
            }
            // ^
            template<class T, class S>
            static T XorFetch(volatile T& t, S v) 
            {
                return __sync_xor_and_fetch(&t, (T)v);
            }
            // 
            template<class T, class S>
            static T NandFetch(volatile T& t, S v) 
            {
                return __sync_nand_and_fetch(&t, (T)v);
            }

            template<class T, class S>
            static T FetchAdd(volatile T& t, S v = 1) 
            {
                return __sync_fetch_and_add(&t, (T)v);
            }

            template<class T, class S>
            static T FetchSub(volatile T& t, S v = 1)
            {
                return __sync_fetch_and_sub(&t, (T)v);
            }

            template<class T, class S>
            static T FetchOr(volatile T& t, S v)
            {
                return __sync_fetch_and_or(&t, (T)v);
            }

            template<class T, class S>
            static T FetchAnd(volatile T& t, S v) 
            {
                return __sync_fetch_and_and(&t, (T)v);
            }

            template<class T, class S>
            static T FetchXor(volatile T& t, S v) 
            {
                return __sync_fetch_and_xor(&t, (T)v);
            }

            template<class T, class S>
            static T FetchNand(volatile T& t, S v) 
            {
                return __sync_fetch_and_nand(&t, (T)v);
            }

            template<class T, class S>
            static T CompareAndSwap(volatile T& t, S old_val, S new_val) 
            {
                return __sync_val_compare_and_swap(&t, (T)old_val, (T)new_val);
            }

            template<class T, class S>
            static bool CompareAndSwapBool(volatile T& t, S old_val, S new_val) 
            {
                return __sync_bool_compare_and_swap(&t, (T)old_val, (T)new_val);
            }
        }; // class Atomic



        // 字符串工具
        class StringUtil 
        {
        public:
            static std::string Format(const char* fmt, ...);
            static std::string Formatv(const char* fmt, va_list ap);

            static std::string UrlEncode(const std::string& str, bool space_as_plus = true);
            static std::string UrlDecode(const std::string& str, bool space_as_plus = true);

            static const char* GuessContentType(const std::string& path);

            static std::string Trim(const std::string& str, const std::string& delimit = " \t\r\n");
            static std::string TrimLeft(const std::string& str, const std::string& delimit = " \t\r\n");
            static std::string TrimRight(const std::string& str, const std::string& delimit = " \t\r\n");


            static std::string ToUpper(const std::string& name);
            static std::string ToLower(const std::string& name);

            static std::string WStringToString(const std::wstring& ws);
            static std::wstring StringToWString(const std::string& s);
        };



        // Yaml Json 互转
	    bool YamlToJson(const YAML::Node& ynode, Json::Value& jnode);
	    bool JsonToYaml(const Json::Value& jnode, YAML::Node& ynode);

		//template<class T>
		//const char* TypeToName() 
        //{
		//    static const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
		//    return s_name;
		//}
		
		std::string PBToJsonString(const google::protobuf::Message& message);
		
		template<class Iter>
		std::string Join(Iter begin, Iter end, const std::string& tag) 
        {
		    std::stringstream ss;
		    for (Iter it = begin; it != end; ++it) 
            {
		        if (it != begin) 
                {
		            ss << tag;
		        }
		        ss << *it;
		    }
		    return ss.str();
		}
		
        //[begin, end)
        //if rt > 0, 存在,返回对应index
        //   rt < 0, 不存在,返回对于应该存在的-(index + 1)
        template<class T>
        int BinarySearch(const T* arr, int length, const T& v) 
        {
            int m = 0;
            int begin = 0;
            int end = length - 1;
            while (begin <= end) 
            {
                m = (begin + end) / 2;
                if (v < arr[m]) 
                {
                    end = m - 1;
                } 
                else if (arr[m] < v) 
                {
                    begin = m + 1;
                } 
                else 
                {
                    return m;
                }
            }
            return -begin - 1;
        }
        
        inline bool ReadFixFromStream(std::istream& is, char* data, const uint64_t& size) 
        {
            uint64_t pos = 0;
            while (is && (pos < size)) 
            {
                is.read(data + pos, size - pos);
                pos += is.gcount();
            }
            return pos == size;
        }
        
        template<class T>
        bool ReadFromStream(std::istream& is, T& v) 
        {
            return ReadFixFromStream(is, (char*)&v, sizeof(v));
        }
        
        template<class T>
        bool ReadFromStream(std::istream& is, std::vector<T>& v) 
        {
            return ReadFixFromStream(is, (char*)&v[0], sizeof(T) * v.size());
        }
        
        template<class T>
        bool WriteToStream(std::ostream& os, const T& v) 
        {
            if (!os) 
            {
                return false;
            }
            os.write((const char*)&v, sizeof(T));
            return (bool)os;
        }
        
        template<class T>
        bool WriteToStream(std::ostream& os, const std::vector<T>& v) 
        {
            if (!os) 
            {
                return false;
            }
            os.write((const char*)&v[0], sizeof(T) * v.size());
            return (bool)os;
        }
        
        class SpeedLimit {
        public:
            using ptr = std::shared_ptr<SpeedLimit>;
            SpeedLimit(uint32_t speed);
            void Add(uint32_t v);
        private:
            uint32_t speed_;
            float count_per_ms_;

            uint32_t cur_count_;
            uint32_t cur_sec_;
        };
        
        bool ReadFixFromStreamWithSpeed(std::istream& is, char* data,
        		const uint64_t& size, const uint64_t& speed = -1);
        
        bool WriteFixToStreamWithSpeed(std::ostream& os, const char* data,
        		const uint64_t& size, const uint64_t& speed = -1);
        
        template<class T>
        bool WriteToStreamWithSpeed(std::ostream& os, const T& v,
                                    const uint64_t& speed = -1) 
        {
            if (os) 
            {
                return WriteFixToStreamWithSpeed(os, (const char*)&v, sizeof(T), speed);
            }
            return false;
        }
        
        template<class T>
        bool WriteToStreamWithSpeed(std::ostream& os, const std::vector<T>& v,
                                    const uint64_t& speed = -1,
                                    const uint64_t& min_duration_ms = 10) {
            if (os) 
            {
                return WriteFixToStreamWithSpeed(os, (const char*)&v[0], sizeof(T) * v.size(), speed);
            }
            return false;
        }
        
        template<class T>
        bool ReadFromStreamWithSpeed(std::istream& is, const std::vector<T>& v,
                                    const uint64_t& speed = -1) 
        {
            if (is) 
            {
                return ReadFixFromStreamWithSpeed(is, (char*)&v[0], sizeof(T) * v.size(), speed);
            }
            return false;
        }
        
        template<class T>
        bool ReadFromStreamWithSpeed(std::istream& is, const T& v,
                                    const uint64_t& speed = -1) 
        {
            if (is) 
            {
                return ReadFixFromStreamWithSpeed(is, (char*)&v, sizeof(T), speed);
            }
            return false;
        }
        
        std::string Format(const char* fmt, ...);
        std::string Formatv(const char* fmt, va_list ap);
        
        template<class T>
        void Slice(std::vector<std::vector<T> >& dst, const std::vector<T>& src, size_t size) 
        {
            size_t left = src.size();
            size_t pos = 0;
            while (left > size) 
            {
                std::vector<T> tmp;
                tmp.reserve(size);
                for (size_t i = 0; i < size; ++i) 
                {
                    tmp.push_back(src[pos + i]);
                }
                pos += size;
                left -= size;
                dst.push_back(tmp);
            }
        
            if (left > 0) 
            {
                std::vector<T> tmp;
                tmp.reserve(left);
                for (size_t i = 0; i < left; ++i) 
                {
                    tmp.push_back(src[pos + i]);
                }
                dst.push_back(tmp);
            }
        }

    } // namespace util 

    //-----------------------------------------------

} // namespace ygw

#endif //__YGW_UTIL_H__
