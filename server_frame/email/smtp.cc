/**
 * @file server_frame/email/smtp.cc
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-09-28
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.top
 */

#include "smtp.h"
#include <server_frame/log.h>

namespace ygw {

    namespace smtp {

        static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

        
        SmtpClient::SmtpClient(Socket::ptr sock)
            :ygw::stream::SocketStream(sock) 
        {
        }

        SmtpClient::ptr SmtpClient::Create(const std::string& host, uint32_t port, bool ssl) 
        {
            ygw::socket::IPAddress::ptr addr = ygw::socket::Address::LookupAnyIPAddress(host);
            if (!addr) 
            {
                YGW_LOG_ERROR(g_logger) << "invalid smtp server: " << host << ":" << port
                    << " ssl=" << ssl;
                return nullptr;
            }
            addr->SetPort(port);

            socket::Socket::ptr sock = ssl ? ygw::socket::SSLSocket::CreateTCP(addr) :
                ygw::socket::Socket::CreateTCP(addr);

            if (!sock->Connect(addr)) // connect failed
            {
                YGW_LOG_ERROR(g_logger) << "Connect smtp server: " << host << ":" << port
                    << " ssl=" << ssl << " fail";
                return nullptr;
            }
            std::string buf;
            buf.resize(1024);

            SmtpClient::ptr rt(new SmtpClient(sock));
            int len = rt->Read(&buf[0], buf.size());
            if (len <= 0) 
            {
                return nullptr;
            }
            buf.resize(len);
            if (ygw::util::TypeUtil::Atoi(buf) != 220) 
            {
                return nullptr;
            }
            rt->host_ = host;
            return rt;
        }

        SmtpResult::ptr SmtpClient::DoCmd(const std::string& cmd, bool debug) 
        {
            if (WriteFixSize(cmd.c_str(), cmd.size()) <= 0) 
            {
                return std::make_shared<SmtpResult>(SmtpResult::kIoError, "write io error");
            }
            std::string buf;
            buf.resize(4096);
            auto len = read(&buf[0], buf.size());
            if (len <= 0) 
            {
                return std::make_shared<SmtpResult>(SmtpResult::kIoError, "read io error");
            }
            buf.resize(len);
            if (debug) 
            {
                ss_ << "C: " << cmd;
                ss_ << "S: " << buf;
            }

            int code = ygw::util::TypeUtil::Atoi(buf);
            if (code >= 400) 
            {
                return std::make_shared<SmtpResult>(code,
                        ygw::util::replace(
                            buf.substr(buf.find(' ') + 1)
                            , "\r\n", ""));
            }
            return nullptr;
        }

        SmtpResult::ptr SmtpClient::Send(EMail::ptr email, int64_t timeout_ms, bool debug) 
        {
#define DO_CMD() \
            result = doCmd(cmd, debug); \
            if (result) {\
                return result; \
            }

            Socket::ptr sock = GetSocket();
            if (sock) 
            {
                sock->setRecvTimeout(timeout_ms);
                sock->setSendTimeout(timeout_ms);
            }

            SmtpResult::ptr result;
            std::string cmd = "HELO " + host_ + "\r\n";
            DO_CMD();
            if (!authed_ && !email->GetFromEMailAddress().empty()) 
            {
                cmd = "AUTH LOGIN\r\n";
                DO_CMD();
                auto pos = email->GetFromEMailAddress().find('@');
                cmd = sylar::base64encode(email->GetFromEMailAddress().substr(0, pos)) + "\r\n";
                DO_CMD();
                cmd = sylar::base64encode(email->GetFromEMailPasswd()) + "\r\n";
                DO_CMD();

                authed_ = true;
            }

            cmd = "MAIL FROM: <" + email->GetFromEMailAddress() + ">\r\n";
            DO_CMD();
            std::set<std::string> targets;
#define XX(fun) \
            for (auto& i : email->fun()) { \
                targets.insert(i); \
            } 
            XX(GetToEMailAddress);
            XX(GetCcEMailAddress);
            XX(GetBccEMailAddress);
#undef XX
            for (auto& i : targets) 
            {
                cmd = "RCPT TO: <" + i + ">\r\n";
                DO_CMD();
            }

            cmd = "DATA\r\n";
            DO_CMD();

            auto& entitys = email->GetEntitys();

            std::stringstream ss;
            ss << "From: <" << email->GetFromEMailAddress() << ">\r\n"
                << "To: ";
#define XX(fun) \
            do {\
                auto& v = email->fun(); \
                for (size_t i = 0; i < v.size(); ++i) {\
                    if (i) {\
                        ss << ","; \
                    } \
                    ss << "<" << v[i] << ">"; \
                } \
                if (!v.empty()) { \
                    ss << "\r\n"; \
                } \
            } while(0);
            XX(GetToEMailAddress);
            if (!email->GetCcEMailAddress().empty()) 
            {
                ss << "Cc: ";
                XX(GetCcEMailAddress);
            }
            ss << "Subject: " << email->GetTitle() << "\r\n";
            std::string boundary;
            if (!entitys.empty()) 
            {
                boundary = ygw::util::random_string(16);
                ss << "Content-Type: multipart/mixed;boundary="
                    << boundary << "\r\n";
            }
            ss << "MIME-Version: 1.0\r\n";
            if (!boundary.empty()) 
            {
                ss << "\r\n--" << boundary << "\r\n";
            }
            ss << "Content-Type: text/html;charset=\"utf-8\"\r\n"
                << "\r\n"
                << email->GetBody() << "\r\n";
            for (auto& i : entitys) 
            {
                ss << "\r\n--" << boundary << "\r\n";
                ss << i->toString();
            }
            if (!boundary.empty()) 
            {
                ss << "\r\n--" << boundary << "--\r\n";
            }
            ss << "\r\n.\r\n";
            cmd = ss.str();
            DO_CMD();
#undef XX
#undef DO_CMD
            return std::make_shared<SmtpResult>(0, "ok");
        }

        std::string SmtpClient::GetDebugInfo() 
        {
            return ss_.str();
        }




    } // namespace smtp

} // namespace ygw
