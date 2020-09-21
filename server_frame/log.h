/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: log.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-01
 *  Description: 
 * ====================================================
 */
#ifndef __YGW_LOG_H__
#define __YGW_LOG_H__
#include <cstdarg>
#include <cstdint>
#include <ctime>

#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "util.h"
#include "singleton.h"
#include "base/thread.h"

/**
 ** @brief 使用流式方式将日志级别level的日志写入到logger
 **/
#define YGW_LOG_LEVEL(logger, level) \
    if (logger->GetLevel() <= level) \
    ygw::log::LogEventWrap(ygw::log::LogEvent::ptr(new ygw::log::LogEvent(logger, level, \
                    __FILE__, __LINE__, 0, ygw::util::GetThreadId(),\
                    ygw::util::GetFiberId(), time(0), ygw::thread::Thread::GetThisName()))).GetStringStream()

/**
 ** @brief 使用流式方式将日志级别debug的日志写入到logger
 **/
#define YGW_LOG_DEBUG(logger) YGW_LOG_LEVEL(logger, ygw::log::LogLevel::kDebug)

/**
 ** @brief 使用流式方式将日志级别info的日志写入到logger
 **/
#define YGW_LOG_INFO(logger) YGW_LOG_LEVEL(logger, ygw::log::LogLevel::kInfo)

/**
 ** @brief 使用流式方式将日志级别warn的日志写入到logger
 **/
#define YGW_LOG_WARN(logger) YGW_LOG_LEVEL(logger, ygw::log::LogLevel::kWarn)

/**
 ** @brief 使用流式方式将日志级别error的日志写入到logger
 **/
#define YGW_LOG_ERROR(logger) YGW_LOG_LEVEL(logger, ygw::log::LogLevel::kError)

/**
 ** @brief 使用流式方式将日志级别fatal的日志写入到logger
 **/
#define YGW_LOG_FATAL(logger) YGW_LOG_LEVEL(logger, ygw::log::LogLevel::kFatal)

/**
 ** @brief 使用格式化方式将日志级别level的日志写入到logger
 **/
#define YGW_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->GetLevel() <= level) \
    ygw::log::LogEventWrap(ygw::log::LogEvent::ptr(new ygw::log::LogEvent(logger, level, \
                    __FILE__, __LINE__, 0, ygw::util::GetThreadId(),\
                    ygw::util::GetFiberId(), time(0), "hello"))).GetEvent()->Format(fmt, __VA_ARGS__)

/**
 ** @brief 使用格式化方式将日志级别debug的日志写入到logger
 **/
#define YGW_LOG_FMT_DEBUG(logger, fmt, ...) YGW_LOG_FMT_LEVEL(logger, ygw::log::LogLevel::kDebug, fmt, __VA_ARGS__)

/**
 ** @brief 使用格式化方式将日志级别info的日志写入到logger
 **/
#define YGW_LOG_FMT_INFO(logger, fmt, ...)  YGW_LOG_FMT_LEVEL(logger, ygw::log::LogLevel::kInfo, fmt, __VA_ARGS__)

/**
 ** @brief 使用格式化方式将日志级别warn的日志写入到logger
 **/
#define YGW_LOG_FMT_WARN(logger, fmt, ...)  YGW_LOG_FMT_LEVEL(logger, ygw::log::LogLevel::kWarn, fmt, __VA_ARGS__)

/**
 ** @brief 使用格式化方式将日志级别error的日志写入到logger
 **/
#define YGW_LOG_FMT_ERROR(logger, fmt, ...) YGW_LOG_FMT_LEVEL(logger, ygw::log::LogLevel::kError, fmt, __VA_ARGS__)

/**
 ** @brief 使用格式化方式将日志级别fatal的日志写入到logger
 **/
#define YGW_LOG_FMT_FATAL(logger, fmt, ...) YGW_LOG_FMT_LEVEL(logger, ygw::log::LogLevel::kFatal, fmt, __VA_ARGS__)

/**
 ** @brief 获取主日志器
 **/
#define YGW_LOG_ROOT() ygw::log::LogManager::GetInstance()->GetRoot()

/**
 ** @brief 获取name的日志器
 **/
#define YGW_LOG_NAME(name) ygw::log::LogManager::GetInstance()->GetLogger(name)

namespace ygw {

    
    namespace log {
        //
        class Logger;
        class LoggerManager;
        //------------------------------------------------
        //------------------------------------------------
        // 日志级别
        class LogLevel {
        public:
            /**
             ** @brief 日志级别枚举
             **/
            enum Level {
                /// 未知级别
                kUnknown = 0,
                /// DEBUG 级别
                kDebug = 1,
                /// INFO 级别
                kInfo = 2,
                /// WARN 级别
                kWarn = 3,
                /// ERROR 级别
                kError = 4,
                /// FATAL 级别
                kFatal = 5
            };

            /**
             ** @brief 将日志级别转成文本输出
             ** @param[in] level 日志级别
             **/
            static const char* ToString(LogLevel::Level level);

            /**
             ** @brief 将文本转换成日志级别
             ** @param[in] str 日志级别文本
             **/
            static LogLevel::Level FromString(const std::string& str);
        }; // class LogLevel 


        //------------------------------------------------
        // 日志事件
        class LogEvent {
        public:
            using ptr = std::shared_ptr<LogEvent>;
            /**
             ** @brief 构造函数
             ** @param[in] logger 日志器
             ** @param[in] level 日志级别
             ** @param[in] file 文件名
             ** @param[in] line 文件行号
             ** @param[in] elapse 程序启动依赖的耗时(毫秒)
             ** @param[in] thread_id 线程id
             ** @param[in] fiber_id 协程id
             ** @param[in] time 日志事件(秒)
             ** @param[in] thread_name 线程名称
             **/
            LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
                    ,const char* file, int32_t line, uint32_t elapse
                    ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
                    ,const std::string& thread_name);


            /**
             ** @brief 返回文件名
             **/
            const char* GetFile() const { return filename_;}

            /**
             ** @brief 返回行号
             **/
            int32_t GetLine() const { return line_;}

            /**
             ** @brief 返回耗时
             **/
            uint32_t GetElapse() const { return elapse_;}

            /**
             ** @brief 返回线程ID
             **/
            uint32_t GetThreadId() const { return thread_id_;}

            /**
             ** @brief 返回协程ID
             **/
            uint32_t GetFiberId() const { return fiber_id_;}

            /**
             ** @brief 返回时间
             **/
            uint64_t GetTime() const { return time_;}

            /**
             ** @brief 返回线程名称
             **/
            const std::string& GetThreadName() const { return thread_name_;}

            /**
             ** @brief 返回日志内容
             **/
            std::string GetContent() const { return string_stream_.str();}

            /**
             ** @brief 返回日志器
             **/
            std::shared_ptr<Logger> GetLogger() const { return logger_;}

            /**
             ** @brief 返回日志级别
             **/
            LogLevel::Level GetLevel() const { return level_;}

            /**
             ** @brief 返回日志内容字符串流
             **/
            std::stringstream& GetStringStream() { return string_stream_;}

            /**
             ** @brief 格式化写入日志内容
             **/
            void Format(const char* fmt, ...);

            /**
             ** @brief 格式化写入日志内容
             **/
            void Format(const char* fmt, va_list al);
        private:
            /// 文件名
            const char* filename_ = nullptr;   
            /// 行号
            int32_t line_ = 0;              
            /// 程序启动到现在的毫秒数
            uint32_t elapse_ = 0;          
            /// 线程id
            uint32_t thread_id_ = 0;
            /// 协程id
            uint32_t fiber_id_ = 0;         
            /// 时间戳
            uint64_t time_ = 0;             
            /// 线程名称
            std::string thread_name_;
            /// 日志内容流
            std::stringstream string_stream_;
            /// 日志器
            std::shared_ptr<Logger> logger_;
            /// 日志等级
            LogLevel::Level level_;
        }; // class LogEvent

        //--------------------------------------------------
        //日志事件包装器
        class LogEventWrap {
        public:
            /**
             ** @brief 构造函数
             ** @param[in] e 日志事件
             **/
            LogEventWrap(LogEvent::ptr e);

            /**
             ** @brief 析构函数
             **/
            ~LogEventWrap();

            /**
             ** @brief 获取日志事件
             **/
            LogEvent::ptr GetEvent() const { return event_;}

            /**
             ** @brief 获取日志内容流
             **/
            std::stringstream& GetStringStream();
        private:
            /**
             ** @brief 日志事件
             **/
            LogEvent::ptr event_;
        };

        //------------------------------------------------
        // 日志格式器
        class LogFormatter {
        public:
            using ptr = std::shared_ptr<LogFormatter>;
            /**
             ** @brief 构造函数
             ** @param[in] pattern 格式模板
             ** @details 
             **  %m 消息
             **  %p 日志级别
             **  %r 累计毫秒数
             **  %c 日志名称
             **  %t 线程id
             **  %n 换行
             **  %d 时间
             **  %f 文件名
             **  %l 行号
             **  %T 制表符
             **  %F 协程id
             **  %N 线程名称
             **
             **  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
             **/
            LogFormatter(const std::string& pattern);

            /**
             ** @brief 返回格式化日志文本
             ** @param[in] logger 日志器
             ** @param[in] level 日志级别
             ** @param[in] event 日志事件
             **/
            std::string Format(std::shared_ptr<Logger> logger,
                    LogLevel::Level level, LogEvent::ptr event);
            std::ostream& Format(std::ostream& ofs, std::shared_ptr<Logger> logger,
                    LogLevel::Level level, LogEvent::ptr event);
        //private:
            //-------------------------------------------
            //日志内容项格式化
            class FormatItem {
            public:
                using ptr = std::shared_ptr<FormatItem> ;
                /**
                 ** @brief 析构函数
                 **/
                virtual ~FormatItem() {}
                /**
                 ** @brief 格式化日志到流
                 ** @param[in, out] os 日志输出流
                 ** @param[in] logger 日志器
                 ** @param[in] level 日志等级
                 ** @param[in] event 日志事件
                 **/
                virtual void Format(std::ostream& os, std::shared_ptr<Logger> logger, 
                        LogLevel::Level level, LogEvent::ptr event) = 0;
            };// class FormatItem
            //--------------------------------------------
            /**
             ** @brief 初始化,解析日志模板
             **/
            void Init(); 

            /**
             ** @brief 是否有错误
             **/
            bool IsError() const { return is_error_;}

            /**
             ** @brief 返回日志模板
             **/
            const std::string GetPattern() const { return pattern_; }
        private:
            /// 日志格式模板
            std::string pattern_;
            /// 日志格式解析后格式
            std::vector<FormatItem::ptr> items_;
            /// 是否有错误
            bool is_error_= false;
        }; // class LogFormatter


        //------------------------------------------------
        // 日志输出目标
        class LogAppender {
            friend class Logger;
        public:
            using ptr = std::shared_ptr<LogAppender> ;
            using MutexType = thread::Spinlock;
            //析构
            virtual ~LogAppender() {}
            /**
             ** @brief 写入日志
             ** @param[in] logger 日志器
             ** @param[in] level 日志级别
             ** @param[in] event 日志事件
             **/
            virtual void Log(std::shared_ptr<Logger> logger, 
                    LogLevel::Level level, LogEvent::ptr event) = 0;

            /**
             ** @brief 将日志输出目标的配置转成YAML String
             **/
            virtual std::string ToYamlString() = 0;

            /**
             ** @brief 更改日志格式器
             **/
            void SetFormatter(LogFormatter::ptr val);

            /**
             ** @brief 获取日志格式器
             **/
            LogFormatter::ptr GetFormatter();

            /**
             ** @brief 获取日志级别
             **/
            LogLevel::Level GetLevel() const { return level_;}

            /**
             ** @brief 设置日志级别
             **/
            void SetLevel(LogLevel::Level val) { level_ = val;}
        protected:
            /// 日志级别
            LogLevel::Level level_ = LogLevel::Level::kDebug;
            /// 是否有自己的日志格式器
            bool has_formatter_ = false;
            /// Mutex
            MutexType mutex_;
            /// 日志格式器
            LogFormatter::ptr formatter_;
        }; // class LogAppender


        //-------------------------------------------------
        // 日志器
        class Logger : public std::enable_shared_from_this<Logger> 
        {
            friend class LoggerManager;
        public:
            using ptr = std::shared_ptr<Logger> ;
            using MutexType = thread::Spinlock;
            /**
             ** @brief 构造函数
             ** @param[in] name 日志器名称
             **/
            Logger(const std::string& name = "root");

            /**
             ** @brief 写日志
             ** @param[in] level 日志级别
             ** @param[in] event 日志事件
             **/
            void Log(LogLevel::Level level, LogEvent::ptr event);

            /**
             ** @brief 写debug级别日志
             ** @param[in] event 日志事件
             **/
            void Debug(LogEvent::ptr event);

            /**
             ** @brief 写info级别日志
             ** @param[in] event 日志事件
             **/
            void Info(LogEvent::ptr event);

            /**
             ** @brief 写warn级别日志
             ** @param[in] event 日志事件
             **/
            void Warn(LogEvent::ptr event);

            /**
             ** @brief 写error级别日志
             ** @param[in] event 日志事件
             **/
            void Error(LogEvent::ptr event);

            /**
             ** @brief 写fatal级别日志
             ** @param[in] event 日志事件
             **/
            void Fatal(LogEvent::ptr event);
    
            /**
             ** @brief 添加日志目标
             ** @param[in] appender 日志目标
             **/
            void AddAppender(LogAppender::ptr appender);

            /**
             ** @brief 删除日志目标
             ** @param[in] appender 日志目标
             **/
            void DelAppender(LogAppender::ptr appender);


            /**
             ** @brief 清空日志目标
             **/
            void ClearAppenders();

            /**
             ** @brief 返回日志级别
             **/
            LogLevel::Level GetLevel() const { return level_;}

            /**
             ** @brief 设置日志级别
             **/
            void SetLevel(LogLevel::Level val) { level_ = val;}

            /**
             ** @brief 返回日志名称
             **/
            const std::string& GetName() const { return name_;}

            /**
             ** @brief 设置日志格式器
             **/
            void SetFormatter(LogFormatter::ptr val);

            /**
             ** @brief 设置日志格式模板
             **/
            void SetFormatter(const std::string& val);

            /**
             ** @brief 获取日志格式器
             **/
            LogFormatter::ptr GetFormatter();

            /**
             ** @brief 将日志器的配置转成YAML String
             **/
            std::string ToYamlString();

        private:
            /// 日志名称
            std::string name_;                  
            /// 日志级别
            LogLevel::Level level_;                 
            /// Mutex
            MutexType mutex_;
            /// 日志目标集合
            std::list<LogAppender::ptr> appenders_;
            /// 日志格式器
            LogFormatter::ptr formatter_;
            /// 主日志器
            Logger::ptr root_;
        }; // class Logger


        //------------------------------------------------
        //输出到控制台的LogAppender
        class StdoutLogAppender : public LogAppender {
        public:
            using ptr = std::shared_ptr<StdoutLogAppender> ; 
            void Log(Logger::ptr logger, LogLevel::Level level, 
                    LogEvent::ptr event) override;
            std::string ToYamlString() override;
        };

        //-------------------------------------------------
        //输出到文件的Appender
        class FileLogAppender : public LogAppender {
        public:
            using ptr = std::shared_ptr<FileLogAppender> ;
            FileLogAppender(const std::string& filename);

            void Log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

            std::string ToYamlString() override;

            //重新打开文件，成功：true，失败：false
            bool ReOpen();
        private:
            /// 文件路径
            std::string filename_;
            /// 文件流
            std::ofstream fileout_; 
            /// 上次重新打开时间
            uint64_t last_time_ = 0;
        };//

        /**
         ** @brief 日志器管理类
         **/
        class LoggerManager {
        public:
            using MutexType = thread::Spinlock;
            /**
             ** @brief 构造函数
             **/
            LoggerManager();

            /**
             ** @brief 获取日志器
             ** @param[in] name 日志器名称
             **/
            Logger::ptr GetLogger(const std::string& name);

            /**
             ** @brief 初始化
             **/
            void Init();

            /**
             ** @brief 返回主日志器
             **/
            Logger::ptr GetRoot() const { return root_;}

            /**
             ** @brief 将所有的日志器配置转成YAML String
             **/
            std::string ToYamlString();
        private:
            /// Mutex
            MutexType mutex_;
            /// 日志器容器
            std::map<std::string, Logger::ptr> loggers_;
            /// 主日志器
            Logger::ptr root_;
        };


        //----------------------------------------------------
        // 日志器管理类单例模式
        using LogManager = ygw::mode::Singleton<LoggerManager>;

        //-----------------------------------------

    } // namespace log

    //---------------------------------------

} // namespace ygw

#endif // __YGW_LOG_H__
