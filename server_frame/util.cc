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
#include <Winsock2.h>
#include <ws2tcpip.h>
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
#include <cstring>
#include <google/protobuf/unknown_field_set.h>

#include "log.h"
#include "base/fiber.h"
#include "util.h"

#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32")
#endif // _MSC_VER

namespace ygw {
     
    //--------------------------------------------------
     
    namespace util {

        static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

        static std::string Demangle(const char* str) 
		{
			size_t size = 0;
			int status = 0;
			std::string rt;
			rt.resize(256);
			if (1 == sscanf(str, "%*[^(]%*[^_]%255[^)+]", &rt[0])) 
            {
				char* v = abi::__cxa_demangle(&rt[0], nullptr, &size, &status);
				if (v) 
                {
					std::string result(v);
					free(v);
					return result;
				}
			}
			if (1 == sscanf(str, "%255s", &rt[0])) 
            {
				return rt;
			}
			return str;
		}

        // 获取线程ID
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


        //
        std::string GetHostName() 
        {
            std::shared_ptr<char[]> host(new char[512]);
            memset(host.get(), 0, 512);
            gethostname(host.get(), 511);
            return host.get();
        }

        in_addr_t GetIPv4Inet() 
        {
            struct ifaddrs* ifas = nullptr;
            struct ifaddrs* ifa = nullptr;

            in_addr_t localhost = inet_addr("127.0.0.1");
            if (getifaddrs(&ifas)) 
            {
                YGW_LOG_ERROR(g_logger) << "getifaddrs errno=" << errno
                    << " errstr=" << strerror(errno);
                return localhost;
            }

            in_addr_t ipv4 = localhost;

            for (ifa = ifas; ifa && ifa->ifa_addr; ifa = ifa->ifa_next) 
            {
                if (ifa->ifa_addr->sa_family != AF_INET) 
                {
                    continue;
                }
                if (!strncasecmp(ifa->ifa_name, "lo", 2)) 
                {
                    continue;
                }
                ipv4 = ((struct sockaddr_in*)ifa->ifa_addr)->sin_addr.s_addr;
                if (ipv4 == localhost) 
                {
                    continue;
                }
            }
            if (ifas != nullptr) 
            {
                freeifaddrs(ifas);
            }
            return ipv4;
        }

        std::string _GetIPv4() 
        {
            std::shared_ptr<char[]> ipv4(new char[INET_ADDRSTRLEN]);
            memset(ipv4.get(), 0, INET_ADDRSTRLEN);
            auto ia = GetIPv4Inet();
            inet_ntop(AF_INET, &ia, ipv4.get(), INET_ADDRSTRLEN);
            return ipv4.get();
        }

        std::string GetIPv4() 
        {
            static const std::string ip = _GetIPv4();
            return ip;
        }
        //-------------------------------------------------------------------------------------
        //                         TimeUtil
        //-------------------------------------------------------------------------------------
        // 获取当前时间
        uint64_t TimeUtil::GetCurrentMS() 
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return tv.tv_sec * 1000ul  + tv.tv_usec / 1000;
        }

        uint64_t TimeUtil::GetCurrentUS() 
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return tv.tv_sec * 1000 * 1000ul  + tv.tv_usec;
        }

        // 时间 字符串 互转
        std::string TimeUtil::Time2Str(time_t ts, const std::string& format) 
        {
            struct tm tm;
            localtime_r(&ts, &tm);
            char buf[64];
            strftime(buf, sizeof(buf), format.c_str(), &tm);
            return buf;
        }

        time_t TimeUtil::Str2Time(const char* str, const char* format) 
        {
            struct tm t;
            memset(&t, 0, sizeof(t));
            if (!strptime(str, format, &t)) 
            {
                return 0;
            }
            return mktime(&t);
        }
        
        
        //-----------------------------------------------------------------------------------
        //                  FSUtil Method
        //-----------------------------------------------------------------------------------

        void FSUtil::ListAllFile(std::vector<std::string>& files
                            ,const std::string& path
                            ,const std::string& subfix) 
        {
            if (access(path.c_str(), 0) != 0) 
            {
                return;
            }
            DIR* dir = opendir(path.c_str());
            if (dir == nullptr) 
            {
                return;
            }
            struct dirent* dp = nullptr;
            while ((dp = readdir(dir)) != nullptr) 
            {
                if (dp->d_type == DT_DIR) 
                {
                    if (!strcmp(dp->d_name, ".")
                            || !strcmp(dp->d_name, "..")) 
                    {
                        continue;
                    }
                    ListAllFile(files, path + "/" + dp->d_name, subfix);
                } 
                else if (dp->d_type == DT_REG)
                {
                    std::string filename(dp->d_name);
                    if (subfix.empty()) 
                    {
                        files.push_back(path + "/" + filename);
                    } 
                    else 
                    {
                        if (filename.size() < subfix.size()) 
                        {
                            continue;
                        }
                        if (filename.substr(filename.length() - subfix.size()) == subfix) 
                        {
                            files.push_back(path + "/" + filename);
                        }
                    }
                }
            }
            closedir(dir);
        }

        static int __lstat(const char* file, struct stat* st = nullptr) 
        {
            struct stat lst;
            int ret = lstat(file, &lst);
            if (st) 
            {
                *st = lst;
            }
            return ret;
        }

        bool FSUtil::Unlink(const std::string& filename, bool exist) 
        {
            if (!exist && __lstat(filename.c_str())) 
            {
                return true;
            }
            return ::unlink(filename.c_str()) == 0;
        }

		static int __mkdir(const char* dirname) 
        {
            if (access(dirname, F_OK) == 0) 
            {
				return 0;
			}
			return mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		}

		bool FSUtil::Mkdir(const std::string& dirname) 
        {
            if (__lstat(dirname.c_str()) == 0) 
            {
                return true;
			}
			char* path = strdup(dirname.c_str());
			char* ptr = strchr(path + 1, '/');
			do {
				for (; ptr; *ptr = '/', ptr = strchr(ptr + 1, '/')) 
                {
					*ptr = '\0';
					if (__mkdir(path) != 0) 
                    {
						break;
					}
				}
				if (ptr != nullptr) 
                {
					break;
				} 
                else if (__mkdir(path) != 0) 
                {
					break;
				}
				free(path);
				return true;
			} while (0);
			free(path);
			return false;
		}

        bool FSUtil::IsRunningPidfile(const std::string& pidfile) 
        {
            if (__lstat(pidfile.c_str()) != 0) 
            {
                return false;
            }
            std::ifstream ifs(pidfile);
            std::string line;
            if (!ifs || !std::getline(ifs, line)) 
            {
                return false;
            }
            if (line.empty()) 
            {
                return false;
            }
            pid_t pid = atoi(line.c_str());
            if (pid <= 1)
            {
                return false;
            }
            if (kill(pid, 0) != 0) 
            {
                return false;
            }
            return true;
        }

        bool FSUtil::Rm(const std::string& path) 
        {
            struct stat st;
            if (lstat(path.c_str(), &st))
            {
                return true;
            }
            if (!(st.st_mode & S_IFDIR)) 
            {
                return Unlink(path);
            }

            DIR* dir = opendir(path.c_str());
            if (!dir) 
            {
                return false;
            }

            bool ret = true;
            struct dirent* dp = nullptr;
            while ((dp = readdir(dir))) 
            {
                if (!strcmp(dp->d_name, ".")
                        || !strcmp(dp->d_name, ".."))
                {
                    continue;
                }
                std::string dirname = path + "/" + dp->d_name;
                ret = Rm(dirname);
            }
            closedir(dir);
            if (::rmdir(path.c_str())) 
            {
                ret = false;
            }
            return ret;
        }

        bool FSUtil::Mv(const std::string& from, const std::string& to) 
        {
            if (!Rm(to)) 
            {
                return false;
            }
            return rename(from.c_str(), to.c_str()) == 0;
        }

        bool FSUtil::Realpath(const std::string& path, std::string& rpath) 
        {
            if (__lstat(path.c_str())) {
                return false;
            }
            char* ptr = ::realpath(path.c_str(), nullptr);
            if (nullptr == ptr) {
                return false;
            }
            std::string(ptr).swap(rpath);
            free(ptr);
            return true;
        }

        bool FSUtil::Symlink(const std::string& from, const std::string& to) 
        {
            if (!Rm(to)) 
            {
                return false;
            }
            return ::symlink(from.c_str(), to.c_str()) == 0;
        }

        std::string FSUtil::Dirname(const std::string& filename) 
        {
            if (filename.empty()) 
            {
                return ".";
            }
            auto pos = filename.rfind('/');
            if (pos == 0) 
            {
                return "/";
            } 
            else if (pos == std::string::npos) 
            {
                return ".";
            } 
            else 
            {
                return filename.substr(0, pos);
            }
        }

        std::string FSUtil::Basename(const std::string& filename) 
        {
            if (filename.empty()) 
            {
                return filename;
            }
            auto pos = filename.rfind('/');
            if (pos == std::string::npos) 
            {
                return filename;
            } 
            else 
            {
                return filename.substr(pos + 1);
            }
        }

        bool FSUtil::OpenForRead(std::ifstream& ifs, const std::string& filename
                ,std::ios_base::openmode mode) 
        {
            ifs.open(filename.c_str(), mode);
            return ifs.is_open();
        }

        bool FSUtil::OpenForWrite(std::ofstream& ofs, const std::string& filename
                ,std::ios_base::openmode mode) 
        {
            ofs.open(filename.c_str(), mode);   
            if (!ofs.is_open()) 
            {
                std::string dir = Dirname(filename);
                Mkdir(dir);
                ofs.open(filename.c_str(), mode);
            }
            return ofs.is_open();
        }

        std::string FSUtil::GetCurDir()
        {
            std::string path;
            path.resize(256);
            if (getcwd(&path[0], path.size()))
            {
                return path;
            }
            else
            {
                YGW_LOG_ERROR(g_logger) << "getcwd error";
                return "./";
            }
        }

        int FSUtil::GetFileSize(FILE* pf)
        {
            if (pf)
            {
                int cur = ftell(pf);
                fseek(pf, 0, SEEK_END);
                int ret = ftell(pf);
                fseek(pf, cur, SEEK_SET);
                return ret;
            }
            else
            {
                return -1;
            }
        }
        std::shared_ptr<FILE> FSUtil::OpenFile(const std::string& filename, const char* mode)
        {
            FILE* pf = fopen(filename.data(), mode);
            if (pf) return std::shared_ptr<FILE>(pf, fclose);
            else return nullptr;
        }


        //-----------------------------------------------------------------------------------
        //                  StringUtil Method
        //-----------------------------------------------------------------------------------

        int8_t  TypeUtil::ToChar(const std::string& str) 
        {
            if (str.empty()) 
            {
                return 0;
            }
            return *str.begin();
        }

        int64_t TypeUtil::Atoi(const std::string& str) 
        {
            if (str.empty()) 
            {
                return 0;
            }
            return strtoull(str.c_str(), nullptr, 10);
        }

        double  TypeUtil::Atof(const std::string& str) 
        {
            if (str.empty()) 
            {
                return 0;
            }
            return atof(str.c_str());
        }

        int8_t  TypeUtil::ToChar(const char* str) 
        {
            if (str == nullptr) 
            {
                return 0;
            }
            return str[0];
        }

        int64_t TypeUtil::Atoi(const char* str) 
        {
            if (str == nullptr) 
            {
                return 0;
            }
            return strtoull(str, nullptr, 10);
        }

        double  TypeUtil::Atof(const char* str) 
        {
            if (str == nullptr) 
            {
                return 0;
            }
            return atof(str);
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
            if (begin == std::string::npos) 
            {
                return "";
            }
            return str.substr(begin);
        }

        std::string StringUtil::TrimRight(const std::string& str, const std::string& delimit) 
        {
            auto end = str.find_last_not_of(delimit);
            if (end == std::string::npos) 
            {
                return "";
            }
            return str.substr(0, end);
        }

        std::string StringUtil::Format(const char* fmt, ...) {
            va_list ap;
            va_start(ap, fmt);
            auto v = Formatv(fmt, ap);
            va_end(ap);
            return v;
        }

        std::string StringUtil::Formatv(const char* fmt, va_list ap) {
            char* buf = nullptr;
            auto len = vasprintf(&buf, fmt, ap);
            if(len == -1) {
                return "";
            }
            std::string ret(buf, len);
            free(buf);
            return ret;
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
        std::string StringUtil::UrlEncode(const std::string& str, bool space_as_plus) 
        {
            static const char *hexdigits = "0123456789ABCDEF";
            std::string* ss = nullptr;
            const char* end = str.c_str() + str.length();
            for (const char* c = str.c_str() ; c < end; ++c) 
            {
                if (!CHAR_IS_UNRESERVED(*c)) 
                {
                    if (!ss) 
                    {
                        ss = new std::string;
                        ss->reserve(str.size() * 1.2);
                        ss->append(str.c_str(), c - str.c_str());
                    }
                    if (*c == ' ' && space_as_plus) 
                    {
                        ss->append(1, '+');
                    } 
                    else
                    {
                        ss->append(1, '%');
                        ss->append(1, hexdigits[(uint8_t)*c >> 4]);
                        ss->append(1, hexdigits[*c & 0xf]);
                    }
                } 
                else if (ss) 
                {
                    ss->append(1, *c);
                }
            }
            if (!ss) 
            {
                return str;
            } 
            else 
            {
                std::string rt = *ss;
                delete ss;
                return rt;
            }
        }

        std::string StringUtil::UrlDecode(const std::string& str, bool space_as_plus)
        {
            std::string* ss = nullptr;
            const char* end = str.c_str() + str.length();
            for (const char* c = str.c_str(); c < end; ++c)
            {
                if (*c == '+' && space_as_plus) 
                {
                    if (!ss) 
                    {
                        ss = new std::string;
                        ss->append(str.c_str(), c - str.c_str());
                    }
                    ss->append(1, ' ');
                } 
                else if (*c == '%' && (c + 2) < end
                        && isxdigit(*(c + 1)) && isxdigit(*(c + 2)))
                {
                    if (!ss) 
                    {
                        ss = new std::string;
                        ss->append(str.c_str(), c - str.c_str());
                    }
                    ss->append(1, (char)(xdigit_chars[(int)*(c + 1)] << 4 | xdigit_chars[(int)*(c + 2)]));
                    c += 2;
                } 
                else if (ss) 
                {
                    ss->append(1, *c);
                }
            }
            if (!ss)
            {
                return str;
            } 
            else 
            {
                std::string rt = *ss;
                delete ss;
                return rt;
            }
        }

        static const struct table_entry {
            const char* extension;
            const char* content_type;
        } content_type_table[] = {
            { "txt", "text/plain" },
            { "c", "text/plain" },
            { "h", "text/plain" },
            { "html", "text/html" },
            { "htm", "text/htm" },
            { "css", "text/css" },
            { "gif", "image/gif" },
            { "jpg", "image/jpeg" },
            { "jpeg", "image/jpeg" },
            { "png", "image/png" },
            { "pdf", "application/pdf" },
            { "ps", "application/postscript" },
            { NULL, NULL },
        };

        const char* StringUtil::GuessContentType(const std::string& path)
        {
            const char* last_period, * extension;
            const struct table_entry* ent;
            last_period = strrchr(path.data(), '.');
            if (!last_period || strchr(last_period, '/'))
                goto not_found; /* no exension */
            extension = last_period + 1;
            for (ent = &content_type_table[0]; ent->extension; ++ent) {
                if (!strcasecmp(ent->extension, extension))
                    return ent->content_type;
            }

not_found:
            return "application/misc";
        }

        std::string StringUtil::WStringToString(const std::wstring& ws) 
        {
            std::string str_locale = setlocale(LC_ALL, "");
            const wchar_t* wch_src = ws.c_str();
            size_t n_dest_size = wcstombs(NULL, wch_src, 0) + 1;
            char *ch_dest = new char[n_dest_size];
            memset(ch_dest,0,n_dest_size);
            wcstombs(ch_dest,wch_src,n_dest_size);
            std::string str_result = ch_dest;
            delete []ch_dest;
            setlocale(LC_ALL, str_locale.c_str());
            return str_result;
        }

        std::wstring StringUtil::StringToWString(const std::string& s) 
        {
            std::string str_locale = setlocale(LC_ALL, "");
            const char* chSrc = s.c_str();
            size_t n_dest_size = mbstowcs(NULL, chSrc, 0) + 1;
            wchar_t* wch_dest = new wchar_t[n_dest_size];
            wmemset(wch_dest, 0, n_dest_size);
            mbstowcs(wch_dest,chSrc,n_dest_size);
            std::wstring wstr_result = wch_dest;
            delete []wch_dest;
            setlocale(LC_ALL, str_locale.c_str());
            return wstr_result;
        }

        // Yaml Json 互转
        bool YamlToJson(const YAML::Node& ynode, Json::Value& jnode) 
        {
            try 
            {
                if (ynode.IsScalar()) 
                {
                    Json::Value v(ynode.Scalar());
                    jnode.swapPayload(v);
                    return true;
                }
                if (ynode.IsSequence()) 
                {
                    for (size_t i = 0; i < ynode.size(); ++i) 
                    {
                        Json::Value v;
                        if (YamlToJson(ynode[i], v)) 
                        {
                            jnode.append(v);
                        } 
                        else 
                        {
                            return false;
                        }
                    }
                } 
                else if (ynode.IsMap()) 
                {
                    for (auto it = ynode.begin();
                            it != ynode.end(); ++it) 
                    {
                        Json::Value v;
                        if (YamlToJson(it->second, v)) 
                        {
                            jnode[it->first.Scalar()] = v;
                        } 
                        else 
                        {
                            return false;
                        }
                    }
                }
            } 
            catch(...) 
            {
                return false;
            }
            return true;
        }

        bool JsonToYaml(const Json::Value& jnode, YAML::Node& ynode) 
        {
            try 
            {
                if (jnode.isArray())
                {
                    for (int i = 0; i < (int)jnode.size(); ++i) 
                    {
                        YAML::Node n;
                        if (JsonToYaml(jnode[i], n)) 
                        {
                            ynode.push_back(n);
                        } 
                        else 
                        {
                            return false;
                        }
                    }
                } 
                else if (jnode.isObject()) 
                {
                    for (auto it = jnode.begin();
                            it != jnode.end();
                            ++it) 
                    {
                        YAML::Node n;
                        if (JsonToYaml(*it, n)) 
                        {
                            ynode[it.name()] = n;
                        } 
                        else 
                        {
                            return false;
                        }
                    }
                } 
                else 
                {
                    ynode = jnode.asString();
                }
            } 
            catch (...) 
            {
                return false;
            }
            return true;
        }

        //
        static void serialize_unknowfieldset(const google::protobuf::UnknownFieldSet& ufs, Json::Value& jnode) 
        {
            std::map<int, std::vector<Json::Value> > kvs;
            for (int i = 0; i < ufs.field_count(); ++i) 
            {
                const auto& uf = ufs.field(i);
                switch((int)uf.type()) 
                {
                    case google::protobuf::UnknownField::TYPE_VARINT:
                        kvs[uf.number()].push_back((Json::Int64)uf.varint());
                        break;
                    case google::protobuf::UnknownField::TYPE_FIXED32:
                        kvs[uf.number()].push_back((Json::UInt)uf.fixed32());
                        break;
                    case google::protobuf::UnknownField::TYPE_FIXED64:
                        kvs[uf.number()].push_back((Json::UInt64)uf.fixed64());
                        break;
                    case google::protobuf::UnknownField::TYPE_LENGTH_DELIMITED:
                        google::protobuf::UnknownFieldSet tmp;
                        auto& v = uf.length_delimited();
                        if (!v.empty() && tmp.ParseFromString(v)) 
                        {
                            Json::Value vv;
                            serialize_unknowfieldset(tmp, vv);
                            kvs[uf.number()].push_back(vv);
                        } 
                        else 
                        {
                            kvs[uf.number()].push_back(v);
                        }
                        break;
                }
            }

            for (auto& i : kvs) 
            {
                if (i.second.size() > 1) 
                {
                    for (auto& n : i.second) 
                    {
                        jnode[std::to_string(i.first)].append(n);
                    }
                } 
                else 
                {
                    jnode[std::to_string(i.first)] = i.second[0];
                }
            }
        }

        static void serialize_message(const google::protobuf::Message& message, Json::Value& jnode) {
                            const google::protobuf::Descriptor* descriptor = message.GetDescriptor();
                            const google::protobuf::Reflection* reflection = message.GetReflection();

            for (int i = 0; i < descriptor->field_count(); ++i) {
                const google::protobuf::FieldDescriptor* field = descriptor->field(i);

                if (field->is_repeated()) {
                    if (!reflection->FieldSize(message, field)) {
                        continue;
                    }
                } else {
                    if (!reflection->HasField(message, field)) {
                        continue;
                    }
                }

                if (field->is_repeated()) {
                    switch(field->cpp_type()) {
#define XX(cpptype, method, valuetype, jsontype) \
                        case google::protobuf::FieldDescriptor::CPPTYPE_##cpptype: { \
                            int size = reflection->FieldSize(message, field); \
                            for (int n = 0; n < size; ++n) { \
                                jnode[field->name()].append((jsontype)reflection->GetRepeated##method(message, field, n)); \
                            } \
                            break; \
                        }
                    XX(INT32, Int32, int32_t, Json::Int);
                    XX(UINT32, UInt32, uint32_t, Json::UInt);
                    XX(FLOAT, Float, float, double);
                    XX(DOUBLE, Double, double, double);
                    XX(BOOL, Bool, bool, bool);
                    XX(INT64, Int64, int64_t, Json::Int64);
                    XX(UINT64, UInt64, uint64_t, Json::UInt64);
#undef XX
                        case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: 
                        {
                            int size = reflection->FieldSize(message, field);
                            for (int n = 0; n < size; ++n) 
                            {
                                jnode[field->name()].append(reflection->GetRepeatedEnum(message, field, n)->number());
                            }
                            break;
                        }
                        case google::protobuf::FieldDescriptor::CPPTYPE_STRING: 
                        {
                            int size = reflection->FieldSize(message, field);
                            for (int n = 0; n < size; ++n)
                            {
                                jnode[field->name()].append(reflection->GetRepeatedString(message, field, n));
                            }
                            break;
                        }
                        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: 
                        {
                            int size = reflection->FieldSize(message, field);
                            for (int n = 0; n < size; ++n) 
                            {
                                Json::Value vv;
                                serialize_message(reflection->GetRepeatedMessage(message, field, n), vv);
                                jnode[field->name()].append(vv);
                            }
                            break;
                        }
                    }
                    continue;
                }

                switch(field->cpp_type()) {
#define XX(cpptype, method, valuetype, jsontype) \
                    case google::protobuf::FieldDescriptor::CPPTYPE_##cpptype: { \
                        jnode[field->name()] = (jsontype)reflection->Get##method(message, field); \
                        break; \
                    }
                    XX(INT32, Int32, int32_t, Json::Int);
                    XX(UINT32, UInt32, uint32_t, Json::UInt);
                    XX(FLOAT, Float, float, double);
                    XX(DOUBLE, Double, double, double);
                    XX(BOOL, Bool, bool, bool);
                    XX(INT64, Int64, int64_t, Json::Int64);
                    XX(UINT64, UInt64, uint64_t, Json::UInt64);
#undef XX
                    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
                        jnode[field->name()] = reflection->GetEnum(message, field)->number();
                        break;
                    }
                    case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
                        jnode[field->name()] = reflection->GetString(message, field);
                        break;
                    }
                    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
                        serialize_message(reflection->GetMessage(message, field), jnode[field->name()]);
                        break;
                    }
                }
            }
        
            const auto& ufs = reflection->GetUnknownFields(message);
            serialize_unknowfieldset(ufs, jnode);
        }
        
        std::string PBToJsonString(const google::protobuf::Message& message) 
        {
            Json::Value jnode;
            serialize_message(message, jnode);
            return ygw::util::JsonUtil::ToString(jnode);
        }

        SpeedLimit::SpeedLimit(uint32_t speed)
            :speed_(speed)
            ,count_per_ms_(0)
            ,cur_count_(0)
            ,cur_sec_(0) 
        {
            if (speed == 0) 
            {
                speed_ = (uint32_t)-1;
            }
            count_per_ms_ = speed_ / 1000.0;
        }

        void SpeedLimit::Add(uint32_t v) 
        {
            uint64_t curms = ygw::util::TimeUtil::GetCurrentMS();
            if (curms / 1000 != cur_sec_) 
            {
                cur_sec_ = curms / 1000;
                cur_count_ = v;
                return;
            }

            cur_count_ += v;

            int usedms = curms % 1000;
            int limitms = cur_count_ / count_per_ms_;

            if (usedms < limitms) 
            {
                usleep(1000 * (limitms - usedms));
            }
        }


        bool ReadFixFromStreamWithSpeed(std::istream& is, char* data,
                const uint64_t& size, const uint64_t& speed) {
            SpeedLimit::ptr limit;
            if (dynamic_cast<std::ifstream*>(&is)) 
            {
                limit.reset(new SpeedLimit(speed));
            }

            uint64_t offset = 0;
            uint64_t per = std::max((uint64_t)ceil(speed / 100.0), (uint64_t)1024 * 64);
            while (is && (offset < size)) 
            {
                uint64_t s = size - offset > per ? per : size - offset;
                is.read(data + offset, s);
                offset += is.gcount();

                if (limit) 
                {
                    limit->Add(is.gcount());
                }
            }
            return offset == size;
        }

        bool WriteFixToStreamWithSpeed(std::ostream& os, const char* data,
                const uint64_t& size, const uint64_t& speed) 
        {
            SpeedLimit::ptr limit;
            if (dynamic_cast<std::ofstream*>(&os)) 
            {
                limit.reset(new SpeedLimit(speed));
            }

            uint64_t offset = 0;
            uint64_t per = std::max((uint64_t)ceil(speed / 100.0), (uint64_t)1024 * 64);
            while (os && (offset < size)) 
            {
                uint64_t s = size - offset > per ? per : size - offset;
                os.write(data + offset, s);
                offset += s;

                if (limit) 
                {
                    limit->Add(s);
                }
            }

            return offset == size;
        }



    } // namespace util

    //----------------------------------------------------

} // namespace ygw 
