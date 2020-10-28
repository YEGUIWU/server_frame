/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/tcp_server.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-19
 *  Description: 
 * ====================================================
 */
#include "tcp_server.h"

namespace ygw {

    namespace tcp {

        static ygw::config::ConfigVar<uint64_t>::ptr g_tcp_server_read_timeout =
            ygw::config::Config::Lookup("tcp_server.read_timeout", (uint64_t)(60 * 1000 * 2),
                                    "tcp server read timeout");

        static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

        TcpServer::TcpServer(ygw::scheduler::IOManager* worker,
                ygw::scheduler::IOManager* io_worker,
                ygw::scheduler::IOManager* accept_worker)
            :worker_(worker)
            ,io_worker_(io_worker)
            ,accept_worker_(accept_worker)
            ,recv_timeout_(g_tcp_server_read_timeout->GetValue())
            ,name_("ygw/1.0.0")
            ,is_stop_(true) 
        {
        }

        TcpServer::~TcpServer() // 关闭全部socket
        {
            for (auto& i : socks_)
            {
                i->Close();
            }
            socks_.clear();
        }

        void TcpServer::SetConf(const TcpServerConf& v) 
        {
            conf_.reset(new TcpServerConf(v));
        }

        bool TcpServer::Bind(ygw::socket::Address::ptr addr, bool ssl) 
        {
            std::vector<ygw::socket::Address::ptr> addrs;
            std::vector<ygw::socket::Address::ptr> fails;
            addrs.push_back(addr);
            return Bind(addrs, fails, ssl);
        }

        bool TcpServer::Bind(const std::vector<ygw::socket::Address::ptr>& addrs
                ,std::vector<ygw::socket::Address::ptr>& fails
                ,bool ssl) 
        {
            ssl_ = ssl;
            for (auto& addr : addrs) 
            {
                ygw::socket::Socket::ptr sock = ssl ? ygw::socket::SSLSocket::CreateTCP(addr) : ygw::socket::Socket::CreateTCP(addr);
                if (!sock->Bind(addr)) 
                {
                    YGW_LOG_ERROR(g_logger) << "bind fail errno="
                        << errno << " errstr=" << strerror(errno)
                        << " addr=[" << addr->ToString() << "]";
                    fails.push_back(addr);
                    continue;
                }
                if (!sock->Listen()) 
                {
                    YGW_LOG_ERROR(g_logger) << "listen fail errno="
                        << errno << " errstr=" << strerror(errno)
                        << " addr=[" << addr->ToString() << "]";
                    fails.push_back(addr);
                    continue;
                }
                socks_.push_back(sock);
            }

            if (!fails.empty()) 
            {
                socks_.clear();
                return false;
            }

            for (auto& i : socks_) {
                YGW_LOG_INFO(g_logger) << "type=" << type_
                    << " name=" << name_
                    << " ssl=" << ssl_
                    << " server bind success: " << *i;
            }
            return true;
        }

        void TcpServer::StartAccept(ygw::socket::Socket::ptr sock) 
        {
            while (!is_stop_) 
            {
                ygw::socket::Socket::ptr client = sock->Accept();
                if (client)
                {
                    client->SetRecvTimeout(recv_timeout_);
                    io_worker_->Schedule(std::bind(&TcpServer::HandleClient,
                                shared_from_this(), client));
                } 
                else 
                {
                    YGW_LOG_ERROR(g_logger) << "accept errno=" << errno
                        << " errstr=" << strerror(errno);
                }
            }
        }

        bool TcpServer::Start() 
        {
            if (!is_stop_) 
            {
                return true;
            }
            is_stop_ = false;
            for (auto& sock : socks_) 
            {
                accept_worker_->Schedule(std::bind(&TcpServer::StartAccept,
                            shared_from_this(), sock));
            }
            return true;
        }

        void TcpServer::Stop() 
        {
            is_stop_ = true;
            auto self = shared_from_this();
            accept_worker_->Schedule([this, self]() {
                for (auto& sock : socks_) 
                {
                    sock->CancelAll();
                    sock->Close();
                }
                socks_.clear();
            });
        }

        void TcpServer::HandleClient(ygw::socket::Socket::ptr client) 
        {
            YGW_LOG_INFO(g_logger) << "HandleClient: " << *client;
        }

        bool TcpServer::LoadCertificates(const std::string& cert_file, const std::string& key_file) 
        {
            for (auto& i : socks_) 
            {
                auto ssl_socket = std::dynamic_pointer_cast<ygw::socket::SSLSocket>(i);
                if (ssl_socket) 
                {
                    if (!ssl_socket->LoadCertificates(cert_file, key_file)) 
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        std::string TcpServer::ToString(const std::string& prefix) 
        {
            std::stringstream ss;
            ss << prefix << "[type=" << type_
                << " name=" << name_ << " ssl=" << ssl_
                << " worker=" << (worker_ ? worker_->GetName() : "")
                << " accept=" << (accept_worker_ ? accept_worker_->GetName() : "")
                << " recv_timeout=" << recv_timeout_ << "]" << std::endl;
            std::string pfx = prefix.empty() ? "    " : prefix;
            for (auto& i : socks_) 
            {
                ss << pfx << pfx << *i << std::endl;
            }
            return ss.str();
        }



    } // namespace tcp

} // namespace ygw
