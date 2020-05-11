/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: log.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-02
 *  Description: 
 * ====================================================
 */



#include <cstdio>
#include <iostream>
#include <functional>
#include "log.h"
#include "config.h"
namespace ygw {

    //------------------------------------------------

    namespace log {
        //-------------------------------------------
        //LogLevel
        const char* LogLevel::ToString(LogLevel::Level level) 
        {
            switch(level) 
            {
            case LogLevel::Level::kDebug:
                return "DEBUG";
            case LogLevel::Level::kInfo:
                return "INFO";
            case LogLevel::Level::kWarn:
                return "WARN";
            case LogLevel::Level::kError:
                return "ERROR";
            case LogLevel::Level::kFatal:
                return "FATAL";
            case LogLevel::Level::kUnknown:
                return "UNKNOWN";
            }
            return "UNKNOWN";
        }


        LogLevel::Level LogLevel::FromString(const std::string& str) {
#define XX(level, v) \
            if (str == #v) { \
                return LogLevel::Level::level; \
            }
            XX(kDebug, debug);
            XX(kInfo, info);
            XX(kWarn, warn);
            XX(kError, error);
            XX(kFatal, fatal);

            XX(kDebug, DEBUG);
            XX(kInfo, INFO);
            XX(kWarn, WARN);
            XX(kError, ERROR);
            XX(kFatal, FATAL);
            return LogLevel::Level::kUnknown;
#undef XX
        }

        //-----------------------------------------------------------
        // LogEventWrap
        LogEventWrap::LogEventWrap(LogEvent::ptr e)
            :event_(e) 
        {
        }

        LogEventWrap::~LogEventWrap() 
        {
            event_->GetLogger()->Log(event_->GetLevel(), event_);
        }
        
        std::stringstream& LogEventWrap::GetStringStream() 
        {
                return event_->GetStringStream();
        }


        //-----------------------------------------------------------
        //LogEvent
        void LogEvent::Format(const char* fmt, ...) 
        {
            va_list al;
            va_start(al, fmt);
            Format(fmt, al);
            va_end(al);
        }

        void LogEvent::Format(const char* fmt, va_list al) 
        {
#ifdef __GNUC__
            char* buf = nullptr;
            int len = vasprintf(&buf, fmt, al);
            if (len != -1) 
            {
                string_stream_ << std::string(buf, len);
                free(buf);
            }
#elif _MSC_VER
            char buf[512];
            int len = vsprintf_s(buf, sizeof(buf), fmt, al);
            if (len != -1)
            {
                string_stream_ << std::string(buf, len);
            }
#endif 
        }

        LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
                ,const char* file, int32_t line, uint32_t elapse
                ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
                ,const std::string& thread_name)
            : filename_(file)
            , line_(line)
            , elapse_(elapse)
            , thread_id_(thread_id)
            , fiber_id_(fiber_id)
            , time_(time)
            , thread_name_(thread_name)
            , logger_(logger)
            , level_(level) 
        {
        }
        //-----------------------------------------------------------
        //LogAppender
        void LogAppender::SetFormatter(LogFormatter::ptr val) 
        {
            MutexType::Lock lock(mutex_);
            if (val) 
            {
                formatter_ = val;
                has_formatter_ = true;
            } 
            else 
            {
                has_formatter_ = false;
            }
        }

        LogFormatter::ptr LogAppender::GetFormatter() 
        {
            MutexType::Lock lock(mutex_);
            return formatter_;
        }

        //-----------------------------------------------------------
        //FormatItem
        class MessageFormatItem : public LogFormatter::FormatItem 
        {
        public:
            MessageFormatItem(const std::string& str = "") {}
            void Format(std::ostream& os, Logger::ptr logger, 
                    LogLevel::Level level, LogEvent::ptr event) override 
            {
                os << event->GetContent();
            }
        };

        //--------------------------------------------------------
        //
        class LevelFormatItem : public LogFormatter::FormatItem 
        {
        public:
            LevelFormatItem(const std::string& str = "") {}
            void Format(std::ostream& os, Logger::ptr logger, 
                    LogLevel::Level level, LogEvent::ptr event) override 
            {
                os << LogLevel::ToString(level);
            }
        };

        //--------------------------------------------------------
        //
        class ElapseFormatItem : public LogFormatter::FormatItem 
        {
        public:
            ElapseFormatItem(const std::string& str = "") {}
            void Format(std::ostream& os, Logger::ptr logger, 
                    LogLevel::Level level, LogEvent::ptr event) override 
            {
                os << event->GetElapse();
            }
        };

        //--------------------------------------------------------
        //
        class NameFormatItem : public LogFormatter::FormatItem {
        public:
            NameFormatItem(const std::string& str = "") {}
            void Format(std::ostream& os, Logger::ptr logger, 
                    LogLevel::Level level, LogEvent::ptr event) override 
            {
                os << event->GetLogger()->GetName();
            }
        };

        //--------------------------------------------------------
        //
        class ThreadIdFormatItem : public LogFormatter::FormatItem {
        public:
            ThreadIdFormatItem(const std::string& str = "") {}
            void Format(std::ostream& os, Logger::ptr logger, 
                    LogLevel::Level level, LogEvent::ptr event) override 
            {
                os << event->GetThreadId();
            }
        };

        //--------------------------------------------------------
        //
        class FiberIdFormatItem : public LogFormatter::FormatItem {
        public:
            FiberIdFormatItem(const std::string& str = "") {}
            void Format(std::ostream& os, Logger::ptr logger, 
                    LogLevel::Level level, LogEvent::ptr event) override 
            {
                os << event->GetFiberId();
            }
        };

        //--------------------------------------------------------
        //
        class ThreadNameFormatItem : public LogFormatter::FormatItem 
        {
        public:
            ThreadNameFormatItem(const std::string& str = "") {}
            void Format(std::ostream& os, Logger::ptr logger, 
                    LogLevel::Level level, LogEvent::ptr event) override 
            {
                os << event->GetThreadName();
            }
        };
        //--------------------------------------------------------
        //
        class DateTimeFormatItem : public LogFormatter::FormatItem 
        {
        public:
            DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
                :format_(format) 
            {
                if (format_.empty()) 
                {
                    format_ = "%Y-%m-%d %H:%M:%S";
                }
            }

            void Format(std::ostream& os, Logger::ptr logger, 
                    LogLevel::Level level, LogEvent::ptr event) override 
            {
                struct tm tm;
                time_t time = event->GetTime();
#ifdef _MSC_VER
                localtime_s(&tm, &time);
#elif __GNUC__
                localtime_r(&time, &tm);
#endif //
                char buf[64];
                strftime(buf, sizeof(buf), format_.c_str(), &tm);
                os << buf;
            }
        private:
            std::string format_;
        };

        //--------------------------------------------------------
        //
        class FilenameFormatItem : public LogFormatter::FormatItem {
            public:
                FilenameFormatItem(const std::string& str = "") {}
                void Format(std::ostream& os, Logger::ptr logger, 
                        LogLevel::Level level, LogEvent::ptr event) override 
                {
                    os << event->GetFile();
                }
        };

        //--------------------------------------------------------
        //
        class LineFormatItem : public LogFormatter::FormatItem {
            public:
                LineFormatItem(const std::string& str = "") {}
                void Format(std::ostream& os, Logger::ptr logger, 
                        LogLevel::Level level, LogEvent::ptr event) override 
                {
                    os << event->GetLine();
                }
        };

        //--------------------------------------------------------
        //
        class NewLineFormatItem : public LogFormatter::FormatItem {
            public:
                NewLineFormatItem(const std::string& str = "") {}
                void Format(std::ostream& os, Logger::ptr logger, 
                        LogLevel::Level level, LogEvent::ptr event) override 
                {
                    os << std::endl;
                }
        };


        //--------------------------------------------------------
        //
        class StringFormatItem : public LogFormatter::FormatItem {
        public:
            StringFormatItem(const std::string& str)
                :m_string(str) {}
            void Format(std::ostream& os, Logger::ptr logger, 
                    LogLevel::Level level, LogEvent::ptr event) override 
            {
                os << m_string;
            }
        private:
            std::string m_string;
        };

        //--------------------------------------------------------
        //
        class TabFormatItem : public LogFormatter::FormatItem {
            public:
                TabFormatItem(const std::string& str = "") {}
                void Format(std::ostream& os, Logger::ptr logger, 
                        LogLevel::Level level, LogEvent::ptr event) override 
                {
                    os << "\t";
                }
            private:
                std::string m_string;
        };
        
        //-----------------------------------------------------------
        //Logger
        Logger::Logger(const std::string& name)
            :name_(name)
            ,level_(LogLevel::Level::kDebug) 
        {
            formatter_.reset(
            new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
        }

        //-----------------------------------------------------------
        // 
        void Logger::Log(LogLevel::Level level, LogEvent::ptr event)
        {
            if (level >= level_)
            {
                auto self = shared_from_this();
                MutexType::Lock lock(mutex_);
                if (!appenders_.empty()) 
                {
                    for (auto& i : appenders_) 
                    {
                        i->Log(self, level, event);
                    }
                } 
                else if (root_) 
                {
                    root_->Log(level, event);
                }
            }
        }
        void Logger::Debug(LogEvent::ptr event)
        {
            Log(LogLevel::Level::kDebug, event);
        }
        void Logger::Info(LogEvent::ptr event)
        {
            Log(LogLevel::Level::kInfo, event);
        }
        void Logger::Warn(LogEvent::ptr event)
        {
            Log(LogLevel::Level::kWarn, event);
        }
        void Logger::Error(LogEvent::ptr event)
        {
            Log(LogLevel::Level::kError, event);
        }
        void Logger::Fatal(LogEvent::ptr event)
        {
            Log(LogLevel::Level::kFatal, event);
        }
        //-----------------------------------------------------------
        //
        void Logger::SetFormatter(LogFormatter::ptr val) 
        {
            MutexType::Lock lock(mutex_);
            formatter_ = val;

            for (auto& i : appenders_) 
            {
                MutexType::Lock ll(i->mutex_);
                if (!i->has_formatter_) 
                {
                    i->formatter_ = formatter_;
                }
            }
        }

        void Logger::SetFormatter(const std::string& val) {
            std::cout << "---" << val << std::endl;
            ygw::log::LogFormatter::ptr new_val(new ygw::log::LogFormatter(val));
            if (new_val->IsError()) 
            {
                std::cout << "Logger setFormatter name=" << name_
                    << " value=" << val << " invalid formatter"
                    << std::endl;
                return;
            }
            //m_formatter = new_val;
            SetFormatter(new_val);
        }

        std::string Logger::ToYamlString() 
        {
            MutexType::Lock lock(mutex_);
            YAML::Node node;
            node["name"] = name_;
            if (level_ != LogLevel::Level::kUnknown) 
            {
                node["level"] = LogLevel::ToString(level_);
            }
            if (formatter_) 
            {
                node["formatter"] = formatter_->GetPattern();
            }

            for (auto& i : appenders_) 
            {
                node["appenders"].push_back(YAML::Load(i->ToYamlString()));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }


        LogFormatter::ptr Logger::GetFormatter() 
        {
            MutexType::Lock lock(mutex_);
            return formatter_;
        }

        void Logger::AddAppender(LogAppender::ptr appender)
        {
            MutexType::Lock lock(mutex_);
            if (!appender->GetFormatter()) 
            {
                MutexType::Lock ll(appender->mutex_);
                appender->formatter_ = formatter_;
            }
            appenders_.push_back(appender);
        }

        void Logger::DelAppender(LogAppender::ptr appender)
        {
            MutexType::Lock lock(mutex_);
            for (auto ib = appenders_.begin(), ie = appenders_.end();
                    ib != ie; ++ib)
            {
                if (*ib == appender)
                {
                    appenders_.erase(ib); 
                    break;
                }
            }
        }

        void Logger::ClearAppenders() 
        {
            MutexType::Lock lock(mutex_);
            appenders_.clear();
        }


        //----------------------------------------------------------------------
        //StdoutLogAppender
        void StdoutLogAppender::Log(Logger::ptr logger,
                LogLevel::Level level, LogEvent::ptr event)
        {
            if (level >= level_)
            {
                MutexType::Lock lock(mutex_);
                formatter_->Format(std::cout, logger, level, event);
            }
        }

        std::string StdoutLogAppender::ToYamlString() 
        {
            MutexType::Lock lock(mutex_);
            YAML::Node node;
            node["type"] = "StdoutLogAppender";
            if (level_ != LogLevel::Level::kUnknown) 
            {
                node["level"] = LogLevel::ToString(level_);
            }
            if (has_formatter_ && formatter_) 
            {
                node["formatter"] = formatter_->GetPattern();
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }

        //---------------------------------------------------------------------
        //FileLogAppender
        bool FileLogAppender::ReOpen()
        {
            MutexType::Lock lock(mutex_);
            if (fileout_)
            {
                fileout_.close();
            }
            fileout_.open(filename_);
            return !fileout_;
        }
        FileLogAppender::FileLogAppender(const std::string& filename)
            : filename_(filename)
              , fileout_(filename_)
        {

        }
        void FileLogAppender::Log(Logger::ptr logger, 
                LogLevel::Level level, LogEvent::ptr event) 
        {
            if (level >= level_) 
            {
                uint64_t now = event->GetTime();
                if (now >= (last_time_ + 3)) //防止强制删除
                {//每三秒重新打开一次
                    ReOpen();//
                    last_time_ = now;
                }
                MutexType::Lock lock(mutex_);
                //if (!(m_filestream << m_formatter->format(logger, level, event))) {
                if (!formatter_->Format(fileout_, logger, level, event)) 
                {
                    std::cout << "error" << std::endl;
                }
            }
        }

        std::string FileLogAppender::ToYamlString() 
        {
            YAML::Node node;
            MutexType::Lock lock(mutex_);
            node["type"] = "FileLogAppender";
            node["file"] = filename_;
            if (level_ != LogLevel::Level::kUnknown) 
            {
                node["level"] = LogLevel::ToString(level_);
            }
            if (has_formatter_ && formatter_) 
            {
                node["formatter"] = formatter_->GetPattern();
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }


        //---------------------------------------------------------------------
        // LogFormatter
        LogFormatter::LogFormatter(const std::string& pattern)
            :pattern_(pattern)
        {
            Init();
        }

        std::string LogFormatter::Format(std::shared_ptr<Logger> logger,
                LogLevel::Level level, LogEvent::ptr event) 
        {
            std::stringstream ss;
            for (auto& i : items_) 
            {
                i->Format(ss, logger, level, event);
            }
            return ss.str();
        }

        std::ostream& LogFormatter::Format(std::ostream& ofs, 
                std::shared_ptr<Logger> logger, 
                LogLevel::Level level, LogEvent::ptr event)
        {
            for (auto& i : items_) 
            {
                i->Format(ofs, logger, level, event);
            }
            return ofs;
        }

        //%xxx %xxx{xxx} %%
        void LogFormatter::Init()
        {
            //string, format, type
            std::vector<std::tuple<std::string, std::string, int>> vec;
            std::string str, nstr, fmt;
            std::size_t n, fmt_begin;
            int fmt_status = 0;
            for (std::size_t i = 0; i < pattern_.size(); ++i)
            {
                //查找%
                if (pattern_[i] != '%')
                {
                    nstr.append(1, pattern_[i]);
                    continue;
                }
                //%% -> %
                if ((i + 1) < pattern_.size())
                {
                    if (pattern_[i + 1] == '%')
                    {
                        nstr.append(1, '%');
                        continue;
                    }
                }
                //%d %s ...
                n = i + 1;//d s ...
                fmt_status = 0;
                fmt_begin = 0;

                //
                str.clear();
                fmt.clear();
                while (n < pattern_.size())
                {
                    if (!fmt_status && 
                            (!isalpha(pattern_[n]) && pattern_[n] != '{' && pattern_[n] != '}')
                       )
                    {
                        str =  pattern_.substr(i + 1, n - i - 1);
                        break;
                    }
                    if (fmt_status == 0)
                    {
                        if (pattern_[n] == '{')
                        {
                            str = pattern_.substr(i + 1, n - i - 1);
                            //
                            fmt_status = 1;//解析格式
                            fmt_begin = n;
                            ++n;
                            continue;
                        }
                    }
                    else if (fmt_status == 1)
                    {
                        if (pattern_[n] == '}')
                        {
                            fmt = pattern_.substr(fmt_begin + 1, n -  fmt_begin - 1);
                            fmt_status = 0;
                            ++n;
                            break;
                        }
                    }
                    ++n;
                    if (n == pattern_.size())
                    {
                        if (str.empty())
                        {
                            str = pattern_.substr(i + 1);
                        }
                    }
                }

                if (fmt_status == 0)
                {
                    if (!nstr.empty())
                    {
                        vec.push_back(std::make_tuple(nstr, std::string(), 0));
                        nstr.clear();
                    }
                    vec.push_back(std::make_tuple(str, fmt, 1));
                    i = n - 1;
                }
                else if (fmt_status == 1)
                {
                    std::cout << "pattern parse error: " << pattern_ << " - "
                        << pattern_.substr(i) << std::endl;
                    is_error_ = true;
                    vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
                }

           }
            if (!nstr.empty())
            {
                vec.push_back(std::make_tuple(nstr, "", 0));
            }

            static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str, C) \
                {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

                XX(m, MessageFormatItem),           //m:消息
                XX(p, LevelFormatItem),             //p:日志级别
                XX(r, ElapseFormatItem),            //r:累计毫秒数
                XX(c, NameFormatItem),              //c:日志名称
                XX(t, ThreadIdFormatItem),          //t:线程id
                XX(n, NewLineFormatItem),           //n:换行
                XX(d, DateTimeFormatItem),          //d:时间
                XX(f, FilenameFormatItem),          //f:文件名
                XX(l, LineFormatItem),              //l:行号
                XX(T, TabFormatItem),               //T:Tab
                XX(F, FiberIdFormatItem),           //F:协程id
                XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
            };

            for (auto& i : vec)
            {
                if (std::get<2>(i) == 0)
                {
                    items_.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
                }
                else 
                {
                    auto it = s_format_items.find(std::get<0>(i));
                    if (it == s_format_items.end()) 
                    {
                        items_.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                        is_error_ = true;
                    } 
                    else 
                    {
                        items_.push_back(it->second(std::get<1>(i)));
                    }
                }
            }

        }
        //----------------------------------------------------------------
        //
        class LogAppenderDefine {
        public:

            bool operator==(const LogAppenderDefine& oth) const 
            {
                return type_ == oth.type_
                    && level_ == oth.level_
                    && formatter_ == oth.formatter_
                    && file_ == oth.file_;
            }

        public:
            int type_ = 0; //1 File, 2 Stdout
            LogLevel::Level level_ = LogLevel::Level::kUnknown;
            std::string formatter_;
            std::string file_;
        };

        class LogDefine {
        public:
            bool operator==(const LogDefine& oth) const 
            {
                return name_ == oth.name_
                    && level_ == oth.level_
                    && formatter_ == oth.formatter_
                    && appenders_ == appenders_;
            }

            bool operator<(const LogDefine& oth) const 
            {
                return name_ < oth.name_;
            }

            bool IsValid() const 
            {
                return !name_.empty();
            }
        public:
            std::string name_;
            LogLevel::Level level_ = LogLevel::Level::kUnknown;
            std::string formatter_;
            std::vector<LogAppenderDefine> appenders_;
        };


        ygw::config::ConfigVar<std::set<LogDefine> >::ptr g_log_defines =
            ygw::config::Config::Lookup("logs", std::set<LogDefine>(), "logs config");


        class LogIniter 
        {
        public:
            LogIniter() 
            {
                g_log_defines->AddListener([](
                            const std::set<LogDefine>& old_value,
                            const std::set<LogDefine>& new_value) {
                        YGW_LOG_INFO(YGW_LOG_ROOT()) << "on_logger_conf_changed";
                    for (auto& i : new_value) 
                    {
                        auto it = old_value.find(i);
                        ygw::log::Logger::ptr logger;
                        if (it == old_value.end()) 
                        {//新增logger
                            logger = YGW_LOG_NAME(i.name_);
                        } 
                        else 
                        {
                            if (!(i == *it)) 
                            {//修改的logger
                                logger = YGW_LOG_NAME(i.name_);
                            } 
                            else 
                            {
                                continue;
                            }
                        }
                        logger->SetLevel(i.level_);
                        //std::cout << "** " << i.name << " level=" << i.level
                        //<< "  " << logger << std::endl;
                        if (!i.formatter_.empty())
                        {
                            logger->SetFormatter(i.formatter_);
                        }

                        logger->ClearAppenders();
                        for (auto& a : i.appenders_)
                        {
                            ygw::log::LogAppender::ptr ap;
                            if (a.type_ == 1)
                            {
                                ap.reset(new FileLogAppender(a.file_));
                            } 
                            else if (a.type_ == 2)
                            {
                                //if (!ygw::log::EnvMgr::GetInstance()->has("d")) 
                                if (true)
                                {
                                    ap.reset(new StdoutLogAppender);
                                } 
                                else 
                                {
                                    continue;
                                }
                            }
                            ap->SetLevel(a.level_);
                            if (!a.formatter_.empty())
                            {
                                LogFormatter::ptr fmt(new LogFormatter(a.formatter_));
                                if (!fmt->IsError()) 
                                {
                                    ap->SetFormatter(fmt);
                                } 
                                else 
                                {
                                    std::cout << "log.name=" << i.name_ << " appender type="
                                        << a.type_ << " formatter=" <<
                                        a.formatter_ << " is invalid" << std::endl;
                                }
                            }
                            logger->AddAppender(ap);
                        }
                    }

                    for (auto& i : old_value) 
                    {
                        auto it = new_value.find(i);
                        if (it == new_value.end()) 
                        {
                            //删除logger
                            auto logger = YGW_LOG_NAME(i.name_);
                            logger->SetLevel((LogLevel::Level)0);
                            logger->ClearAppenders();
                        }
                    }
                });
            }
        };


        static LogIniter __log_init;

        //----------------------------------------------------------------
        //
        LoggerManager::LoggerManager() 
        {
            root_.reset(new Logger);
            root_->AddAppender(LogAppender::ptr(new StdoutLogAppender));

            loggers_[root_->name_] = root_;

            Init();
        }

        Logger::ptr LoggerManager::GetLogger(const std::string& name) 
        {
            MutexType::Lock lock(mutex_);
            auto it = loggers_.find(name);
            if (it != loggers_.end()) 
            {
                return it->second;
            }

            Logger::ptr logger(new Logger(name));
            logger->root_ = root_;
            loggers_[name] = logger;
            return logger;
        }
        std::string LoggerManager::ToYamlString() 
        {
            MutexType::Lock lock(mutex_);
            YAML::Node node;
            for (auto& i : loggers_) 
            {
                node.push_back(YAML::Load(i.second->ToYamlString()));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
        void LoggerManager::Init() 
        {

        }


    } // namespace log 


    namespace config {
        template<>
        class LexicalCast<std::string, log::LogDefine> {
        public:
            log::LogDefine operator()(const std::string& v)
            {
                YAML::Node n = YAML::Load(v);
                log::LogDefine ld;
                if (!n["name"].IsDefined())
                {
                    std::cout << "log config error: name is null, " << n
                        << std::endl;
                    throw std::logic_error("log config name is null");
                }
                ld.name_ = n["name"].as<std::string>();
                ld.level_ = log::LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
                if (n["formatter"].IsDefined())
                {
                    ld.formatter_ = n["formatter"].as<std::string>();
                }

                if (n["appenders"].IsDefined())
                {
                    //std::cout << "==" << ld.name << " = " << n["appenders"].size() << std::endl;
                    for (std::size_t x = 0; x < n["appenders"].size(); ++x)
                    {
                        auto a = n["appenders"][x];
                        if (!a["type"].IsDefined())
                        {
                            std::cout << "log config error: appender type is null, " << a
                                << std::endl;
                            continue;
                        }
                        std::string type = a["type"].as<std::string>();
                        log::LogAppenderDefine lad;
                        if (type == "FileLogAppender")
                        {
                            lad.type_ = 1;
                            if (!a["file"].IsDefined())
                            {
                                std::cout << "log config error: fileappender file is null, " << a
                                    << std::endl;
                                continue;
                            }
                            lad.file_ = a["file"].as<std::string>();
                            if (a["formatter"].IsDefined())
                            {
                                lad.formatter_ = a["formatter"].as<std::string>();
                            }
                        }
                        else if (type == "StdoutLogAppender")
                        {
                            lad.type_ = 2;
                            if (a["formatter"].IsDefined())
                            {
                                lad.formatter_ = a["formatter"].as<std::string>();
                            }
                        }
                        else
                        {
                            std::cout << "log config error: appender type is invalid, " << a
                                << std::endl;
                            continue;
                        }

                        ld.appenders_.push_back(lad);
                    }
                }
                return ld;
            }
        };

        template<>
        class LexicalCast<log::LogDefine, std::string> {
        public:
            std::string operator()(const log::LogDefine& i)
            {
                YAML::Node n;
                n["name"] = i.name_;
                if (i.level_ != log::LogLevel::Level::kUnknown)
                {
                    n["level"] = log::LogLevel::ToString(i.level_);
                }
                if (!i.formatter_.empty())
                {
                    n["formatter"] = i.formatter_;
                }

                for (auto& a : i.appenders_)
                {
                    YAML::Node na;
                    if (a.type_ == 1)
                    {
                        na["type"] = "FileLogAppender";
                        na["file"] = a.file_;
                    }
                    else if (a.type_ == 2)
                    {
                        na["type"] = "StdoutLogAppender";
                    }
                    if (a.level_ != log::LogLevel::Level::kUnknown)
                    {
                        na["level"] = log::LogLevel::ToString(a.level_);
                    }
                    if (!a.formatter_.empty())
                    {
                        na["formatter"] = a.formatter_;
                    }

                    n["appenders"].push_back(na);
                }
                std::stringstream ss;
                ss << n;
                return ss.str();
            }
        };
    }
    //------------------------------------------------

} // namespace ygw 
