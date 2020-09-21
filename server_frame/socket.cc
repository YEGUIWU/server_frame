/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/socket.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-14
 *  Description: 
 * ====================================================
 */

#include <limits.h>

#include "base/fd_manager.h"
#include "iomanager.h"
#include "log.h"
#include "macro.h"
#include "server_frame/hook.h"
#include "socket.h"

namespace ygw {

    //--------------------------------------------------------------------------

    namespace socket {

        static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

        //--------------------------------------------------------------------------

        // static function
        Socket::ptr Socket::CreateTCP(ygw::socket::Address::ptr address) {
            Socket::ptr sock(new Socket(address->GetFamily(), kTCP, 0));
            return sock;
        }

        Socket::ptr Socket::CreateUDP(ygw::socket::Address::ptr address) {
            Socket::ptr sock(new Socket(address->GetFamily(), kUDP, 0));
            sock->NewSock();
            sock->is_connected_ = true;
            return sock;
        }

        Socket::ptr Socket::CreateTCPSocket() {
            Socket::ptr sock(new Socket(kIPv4, kTCP, 0));
            return sock;
        }

        Socket::ptr Socket::CreateUDPSocket() {
            Socket::ptr sock(new Socket(kIPv4, kUDP, 0));
            sock->NewSock();
            sock->is_connected_= true;
            return sock;
        }

        Socket::ptr Socket::CreateTCPSocket6() {
            Socket::ptr sock(new Socket(kIPv6, kTCP, 0));
            return sock;
        }

        Socket::ptr Socket::CreateUDPSocket6() {
            Socket::ptr sock(new Socket(kIPv6, kUDP, 0));
            sock->NewSock();
            sock->is_connected_= true;
            return sock;
        }

        Socket::ptr Socket::CreateUnixTCPSocket() {
            Socket::ptr sock(new Socket(kUNIX, kTCP, 0));
            return sock;
        }

        Socket::ptr Socket::CreateUnixUDPSocket() {
            Socket::ptr sock(new Socket(kUNIX, kUDP, 0));
            return sock;
        }


        // 构造
        Socket::Socket(int family, int type, int protocol)
            : sockfd_(-1)
             ,family_(family)
             ,type_(type)
             ,protocol_(protocol)
             ,is_connected_(false) 
        {
        }

        // 析构
        Socket::~Socket() 
        {
            Close();
        }

        //  set 发送超时时间
        void Socket::SetSendTimeout(int64_t v)
        {
            struct timeval tv { int(v / 1000), int(v % 1000 * 1000) };
            SetOption(SOL_SOCKET, SO_SNDTIMEO, tv);
        }

        // get 发送超时时间
        int64_t Socket::GetSendTimeout()
        {
            handle::FdContext::ptr ctx = handle::FdManager::GetInstance()->Get(sockfd_);
            if (ctx)
            {
                return ctx->GetTimeout(SO_SNDTIMEO);
            }
            return -1;
        }


        // 接受超时时间
        void Socket::SetRecvTimeout(int64_t v)
        {
            struct timeval tv { int(v / 1000), int(v % 1000 * 1000) };
            SetOption(SOL_SOCKET, SO_RCVTIMEO, tv);
        }

        //
        int64_t Socket::GetRecvTimeout()
        {
            handle::FdContext::ptr ctx = handle::FdManager::GetInstance()->Get(sockfd_);
            if (ctx)
            {
                return ctx->GetTimeout(SO_RCVTIMEO);
            }
            return -1;
        }


        // Option
        bool Socket::GetOption(int level, int option, void* result, socklen_t* len)
        {
            int rt = getsockopt(sockfd_, level, option, result, (socklen_t*)len);
            if (rt)
            {
                YGW_LOG_DEBUG(g_logger) << "GetOption sockfd=" << sockfd_
                    << " level=" << level << " option=" << option
                    << " errno=" << errno << " errstr=" << strerror(errno);
                return false;
            }
            return true;
        }

        //
        bool Socket::SetOption(int level, int option, const void* result, socklen_t len)
        {
            if (setsockopt(sockfd_, level, option, result, (socklen_t)len))
            {
                YGW_LOG_DEBUG(g_logger) << "SetOption sockfd=" << sockfd_
                    << " level=" << level << " option=" << option
                    << " errno=" << errno << " errstr=" << strerror(errno);
                return false;
            }
            return true;
        }

        // Init
        bool Socket::Init(int sockfd)
        {
            handle::FdContext::ptr ctx = handle::FdManager::GetInstance()->Get(sockfd);
            if (ctx && ctx->IsSocket() && !ctx->IsClose())
            {
                sockfd_ = sockfd;
                is_connected_ = true;
                InitSock();
                GetLocalAddress();
                GetRemoteAddress();
                return true;
            }
            return false;
        }


        // Accept
        Socket::ptr Socket::Accept()
        {
            Socket::ptr sock(new Socket(family_, type_, protocol_));
            int newsockfd = ::accept(sockfd_, nullptr, nullptr);
            if (newsockfd == -1)
            {
                YGW_LOG_ERROR(g_logger) << "Accpet(" << sockfd_ << ") errno="
                    << errno << " errstr=" << strerror(errno);
                return nullptr;
            }
            if (sock->Init(newsockfd))
            {
                return sock;
            }
            return nullptr;
        }

        // Bind
        bool Socket::Bind(const Address::ptr addr)
        {
            // 检测有效
            if (!IsValid())
            {
                NewSock();
                if (YGW_UNLIKELY(!IsValid()))
                {
                    return false;
                }
            }

            // 检测协议族
            if (YGW_UNLIKELY(addr->GetFamily() != family_))
            {
                YGW_LOG_ERROR(g_logger) << "bind sock.family("
                    << family_ << ") addr.family(" << addr->GetFamily()
                    << ") not equal, addr=" << addr->ToString();
            }

            UnixAddress::ptr uaddr = std::dynamic_pointer_cast<UnixAddress>(addr);
            if (uaddr)
            {
                Socket::ptr sock = Socket::CreateUnixTCPSocket();
                if (sock->Connect(uaddr))
                {
                    return false;
                }
                else
                {
                    ygw::util::FSUtil::Unlink(uaddr->GetPath(), true);
                }
            }

            if (::bind(sockfd_, addr->GetAddr(), addr->GetAddrLen()))
            {
                YGW_LOG_ERROR(g_logger) << "bind error errno=" << errno
                    << " errstr=" << strerror(errno);
                return false;
            }
            GetLocalAddress();
            return true;
        }

        // Reconnect
        bool Socket::Reconnect(uint64_t timeout_ms)
        {
            if (remote_address_)
            {
                YGW_LOG_ERROR(g_logger) << "Reconnect remote_address_ is null";
                return false;
            }
            local_address_.reset();
            return Connect(remote_address_, timeout_ms);
        }

        // Connect
        bool Socket::Connect(const Address::ptr addr, uint64_t timeout_ms)
        {
            remote_address_ = addr;
            if (!IsValid())
            {
                NewSock();
                if (YGW_UNLIKELY(!IsValid()))
                {
                    return false;
                }
            }
            
            if (YGW_UNLIKELY(addr->GetFamily() != family_))
            {
                YGW_LOG_ERROR(g_logger) << "connect sock.family("
                    << family_ << ") addr.family(" << addr->GetFamily()
                    << ") not equal, addr=" << addr->ToString();
                return false;
            }

            if (timeout_ms == (uint64_t) -1)
            {
                if (::connect(sockfd_, addr->GetAddr(), addr->GetAddrLen()))
                {
                    YGW_LOG_ERROR(g_logger) << "sockfd_=" << sockfd_ << " connect(" << addr->ToString()
                        << ") error errno=" << errno << " errstr=" << strerror(errno);
                    Close(); // connect失败，释放句柄
                    return false;
                }
            }
            else
            {
                if (::connect_with_timeout(sockfd_, addr->GetAddr(), addr->GetAddrLen(), timeout_ms))
                {
                    YGW_LOG_ERROR(g_logger) << "sockfd_=" << sockfd_ << " connect(" << addr->ToString()
                        << ") timeout=" << timeout_ms << " error errno="
                        << errno << " errstr=" << strerror(errno);
                    Close(); // connect失败，释放句柄
                    return false;
                }
            }
            is_connected_ = true;
            GetRemoteAddress();
            GetLocalAddress();
            return true;
        }

        //Listen
        bool Socket::Listen(int backlog)
        {
            if (!IsValid())
            {
                YGW_LOG_ERROR(g_logger) << "Listen error sockfd=-1";
                return false;
            }
            if (::listen(sockfd_, backlog))
            {
                YGW_LOG_ERROR(g_logger) << "Listen error errno=" << errno
                    << " errstr=" << strerror(errno);
                return false;
            }
            return true;
        }

        // Close
        bool Socket::Close()
        {
            if (!is_connected_ && sockfd_ == -1)
            {
                return true;
            }
            is_connected_ = false;
            if (sockfd_ != -1)
            {
                ::close(sockfd_);
                sockfd_ = -1;
            }
            return false;
        }

        // Send
        int Socket::Send(const void* buffer, size_t length, int flags)
        {
            if (IsConnected())
            {
                return ::send(sockfd_, buffer, length, flags);
            }
            return -1;
        }

        // Send
        int Socket::Send(const iovec* buffers, size_t length, int flags)
        {
            if (IsConnected())
            {
                msghdr msg;
                memset(&msg, 0, sizeof(msg));
                msg.msg_iov = (iovec*)buffers;
                msg.msg_iovlen = length;
                return ::sendmsg(sockfd_, &msg, flags);
            }
            return -1;
        }

        // SendTo
        int Socket::SendTo(const void* buffer, size_t length, const Address::ptr to, int flags)
        {
            if (IsConnected())
            {
                return ::sendto(sockfd_, buffer, length, flags, to->GetAddr(), to->GetAddrLen());
            }
            return -1;
        }

        int Socket::SendTo(const iovec* buffers, size_t length, const Address::ptr to, int flags)
        {
            if (IsConnected())
            {
                msghdr msg;
                memset(&msg, 0, sizeof(msg));
                msg.msg_iov = (iovec*)buffers;
                msg.msg_iovlen = length;
                msg.msg_name = to->GetAddr();
                msg.msg_namelen = to->GetAddrLen();
                return ::sendmsg(sockfd_, &msg, flags);
            }
            return -1;
        }
        
    
        // Recv
        int Socket::Recv(void* buffer, size_t length, int flags)
        {
            if (IsConnected())
            {
                return ::recv(sockfd_, buffer, length, flags);
            }
            return -1;
        }

        int Socket::Recv(iovec* buffers, size_t length, int flags)
        {
            if (IsConnected())
            {
                msghdr msg;
                memset(&msg, 0, sizeof(msg));
                msg.msg_iov = (iovec*)buffers;
                msg.msg_iovlen = length;
                return ::recvmsg(sockfd_, &msg, flags);
            }
            return -1;
        }


        // RecvFrom
        int Socket::RecvFrom(void* buffer, size_t length, Address::ptr from, int flags)
        {
            if (IsConnected())
            {
                socklen_t len = from->GetAddrLen();
                return ::recvfrom(sockfd_, buffer, length, flags, from->GetAddr(), &len);
            }
            return -1;
        }

        //
        int Socket::RecvFrom(iovec* buffers, size_t length, Address::ptr from, int flags)
        {
            if (IsConnected())
            {
                msghdr msg;
                memset(&msg, 0, sizeof(msg));
                msg.msg_iov = (iovec*)buffers;
                msg.msg_iovlen = length;
                msg.msg_name = from->GetAddr();
                msg.msg_namelen = from->GetAddrLen();
                return ::recvmsg(sockfd_, &msg, flags);
            }
            return -1;
        }

        // GetRemoteAddress
        Address::ptr Socket::GetRemoteAddress()
        {
            if (remote_address_)
            {
                return remote_address_;
            }

            Address::ptr result;
            switch(family_)
            {
            case AF_INET:
                result.reset(new IPv4Address());
                break;
            case AF_INET6:
                result.reset(new IPv6Address());
                break;
            case AF_UNIX:
                result.reset(new UnixAddress());
                break;
            default:
                result.reset(new UnknownAddress(family_));
                break;
            }
            socklen_t addrlen = result->GetAddrLen();
            if (getpeername(sockfd_, result->GetAddr(), &addrlen))
            {
                YGW_LOG_ERROR(g_logger) << "getpeername error sockfd=" << sockfd_
                    << " errno=" << errno << " errstr=" << strerror(errno);
                return Address::ptr(new UnknownAddress(family_));
            }
            if (family_ == AF_UNIX)
            {
                UnixAddress::ptr addr = std::dynamic_pointer_cast<UnixAddress>(result);
                addr->SetAddrLen(addrlen);
            }
            remote_address_ = result;
            return remote_address_;
        }

        Address::ptr Socket::GetLocalAddress()
        {
            if (local_address_)
            {
                return local_address_;
            }

            Address::ptr result;
            switch(family_)
            {
            case AF_INET:
                result.reset(new IPv4Address());
                break;
            case AF_INET6:
                result.reset(new IPv6Address());
                break;
            case AF_UNIX:
                result.reset(new UnixAddress());
                break;
            default:
                result.reset(new UnknownAddress(family_));
                break;
            }

            socklen_t addrlen = result->GetAddrLen();
            if (getsockname(sockfd_, result->GetAddr(), &addrlen))
            {
                YGW_LOG_ERROR(g_logger) << "getsockname error sockfd=" << sockfd_
                    << " errno=" << errno << " errstr=" << strerror(errno);
                return Address::ptr(new UnknownAddress(family_));
            }
            if (family_ == AF_UNIX)
            {
                UnixAddress::ptr addr = std::dynamic_pointer_cast<UnixAddress>(result);
                addr->SetAddrLen(addrlen);
            }
            local_address_ = result;
            return local_address_; 
        }

        bool Socket::IsValid() const 
        {
            return sockfd_ != -1;
        }

        int Socket::GetError()
        {
            int error = 0;
            socklen_t len = sizeof(error);
            if (!GetOption(SOL_SOCKET, SO_ERROR, &error, &len))
            {
                error = errno;
            }
            return error;
        }

        std::ostream& Socket::Dump(std::ostream& os) const
        {
            os << "[Socket sockfd_=" << sockfd_
                << " is_connected_=" << is_connected_
                << " family_=" << family_
                << " type=" << type_
                << " protocol_=" << protocol_;
            if (local_address_)
            {
                os << " local_address_=" << local_address_->ToString();
            }
            if (remote_address_)
            {
                os << " remote_address_=" << remote_address_->ToString();
            }
            os << "]";
            return os;
        }

        std::string Socket::ToString() const 
        {
            std::stringstream ss;
            Dump(ss);
            return ss.str();
        }

        //Cancel
        bool Socket::CancelRead()
        {
            return scheduler::IOManager::GetThis()->CancelEvent(sockfd_, ygw::scheduler::IOManager::kRead);
        }

        bool Socket::CancelWrite()
        {
            return scheduler::IOManager::GetThis()->CancelEvent(sockfd_, ygw::scheduler::IOManager::kWrite);
        }

        bool Socket::CancelAccept()
        {
            return scheduler::IOManager::GetThis()->CancelEvent(sockfd_, ygw::scheduler::IOManager::kRead);
        }

        bool Socket::CancelAll()
        {
            return scheduler::IOManager::GetThis()->CancelAll(sockfd_);
        }

        void Socket::InitSock()
        {
            int val = 1;
            SetOption(SOL_SOCKET, SO_REUSEADDR, val);
            if (type_ == SOCK_STREAM)
            {
                SetOption(IPPROTO_TCP, TCP_NODELAY, val);
            }
        }

        void Socket::NewSock()
        {
            sockfd_ = ::socket (family_, type_, protocol_);
            if (YGW_LIKELY(sockfd_ != -1))
            {
                InitSock();
            }
            else 
            {
                YGW_LOG_ERROR(g_logger) << "socket(" << family_
                    << ", " << type_ << ", " << protocol_ << ") errno="
                    << errno << " errstr=" << strerror(errno);
            }
        }


        //----------------------------------------------
        namespace {
            struct _SSLInit {
                _SSLInit() {
                    SSL_library_init();
                    SSL_load_error_strings();
                    OpenSSL_add_all_algorithms();
                }
            };

            static _SSLInit s_init;
        } // namespace {} 
        //----------------------------------------------
        SSLSocket::SSLSocket(int family, int type, int protocol)
            :Socket(family, type, protocol) 
        {
        }

        Socket::ptr SSLSocket::Accept() 
        {
            SSLSocket::ptr sock(new SSLSocket(family_, type_, protocol_));
            int newsock = ::accept(sockfd_, nullptr, nullptr);
            if (newsock == -1)
            {
                YGW_LOG_ERROR(g_logger) << "accept(" << sockfd_ << ") errno="
                    << errno << " errstr=" << strerror(errno);
                return nullptr;
            }
            sock->ctx_ = ctx_;
            if (sock->Init(newsock)) 
            {
                return sock;
            }
            return nullptr;
        }

        bool SSLSocket::Bind(const Address::ptr addr) 
        {
            return Socket::Bind(addr);
        }

        bool SSLSocket::Connect(const Address::ptr addr, uint64_t timeout_ms) 
        {
            bool v = Socket::Connect(addr, timeout_ms);
            if (v) 
            {
                ctx_.reset(SSL_CTX_new(SSLv23_client_method()), SSL_CTX_free);
                ssl_.reset(SSL_new(ctx_.get()),  SSL_free);
                SSL_set_fd(ssl_.get(), sockfd_);
                v = (SSL_connect(ssl_.get()) == 1);
            }
            return v;
        }

        bool SSLSocket::Listen(int backlog) 
        {
            return Socket::Listen(backlog);
        }

        bool SSLSocket::Close()
        {
            return Socket::Close();
        }

        int SSLSocket::Send(const void* buffer, size_t length, int flags) {
            if (ssl_) 
            {
                return SSL_write(ssl_.get(), buffer, length);
            }
            return -1;
        }

        int SSLSocket::Send(const iovec* buffers, size_t length, int flags) {
            if (!ssl_) 
            {
                return -1;
            }
            int total = 0;
            for (size_t i = 0; i < length; ++i) 
            {
                int tmp = SSL_write(ssl_.get(), buffers[i].iov_base, buffers[i].iov_len);
                if (tmp <= 0) 
                {
                    return tmp;
                }
                total += tmp;
                if (tmp != (int)buffers[i].iov_len)
                {
                    break;
                }
            }
            return total;
        }

        int SSLSocket::SendTo(const void* buffer, size_t length, const Address::ptr to, int flags) {
            YGW_ASSERT(false);
            return -1;
        }

        int SSLSocket::SendTo(const iovec* buffers, size_t length, const Address::ptr to, int flags) 
        {
            YGW_ASSERT(false);
            return -1;
        }

        int SSLSocket::Recv(void* buffer, size_t length, int flags)
        {
            if (ssl_) 
            {
                return SSL_read(ssl_.get(), buffer, length);
            }
            return -1;
        }

        int SSLSocket::Recv(iovec* buffers, size_t length, int flags) 
        {
            if (!ssl_) 
            {
                return -1;
            }
            int total = 0;
            for (size_t i = 0; i < length; ++i)
            {
                int tmp = SSL_read(ssl_.get(), buffers[i].iov_base, buffers[i].iov_len);
                if (tmp <= 0) 
                {
                    return tmp;
                }
                total += tmp;
                if (tmp != (int)buffers[i].iov_len)
                {
                    break;
                }
            }
            return total;
        }

        int SSLSocket::RecvFrom(void* buffer, size_t length, Address::ptr from, int flags) 
        {
            YGW_ASSERT(false);
            return -1;
        }

        int SSLSocket::RecvFrom(iovec* buffers, size_t length, Address::ptr from, int flags)
        {
            YGW_ASSERT(false);
            return -1;
        }

        bool SSLSocket::Init(int sock) 
        {
            bool v = Socket::Init(sock);
            if (v) 
            {
                ssl_.reset(SSL_new(ctx_.get()),  SSL_free);
                SSL_set_fd(ssl_.get(), sockfd_);
                v = (SSL_accept(ssl_.get()) == 1);
            }
            return v;
        }

        bool SSLSocket::LoadCertificates(const std::string& cert_file, const std::string& key_file) 
        {
            ctx_.reset(SSL_CTX_new(SSLv23_server_method()), SSL_CTX_free);
            if (SSL_CTX_use_certificate_chain_file(ctx_.get(), cert_file.c_str()) != 1) 
            {
                YGW_LOG_ERROR(g_logger) << "SSL_CTX_use_certificate_chain_file("
                    << cert_file << ") error";
                return false;
            }
            if (SSL_CTX_use_PrivateKey_file(ctx_.get(), key_file.c_str(), SSL_FILETYPE_PEM) != 1) 
            {
                YGW_LOG_ERROR(g_logger) << "SSL_CTX_use_PrivateKey_file("
                    << key_file << ") error";
                return false;
            }
            if (SSL_CTX_check_private_key(ctx_.get()) != 1) 
            {
                YGW_LOG_ERROR(g_logger) << "SSL_CTX_check_private_key cert_file="
                    << cert_file << " key_file=" << key_file;
                return false;
            }
            return true;
        }

        SSLSocket::ptr SSLSocket::CreateTCP(ygw::socket::Address::ptr address) 
        {
            SSLSocket::ptr sock(new SSLSocket(address->GetFamily(), kTCP, 0));
            return sock;
        }

        SSLSocket::ptr SSLSocket::CreateTCPSocket()
        {
            SSLSocket::ptr sock(new SSLSocket(kIPv4, kTCP, 0));
            return sock;
        }

        SSLSocket::ptr SSLSocket::CreateTCPSocket6() 
        {
            SSLSocket::ptr sock(new SSLSocket(kIPv6, kTCP, 0));
            return sock;
        }

        std::ostream& SSLSocket::Dump(std::ostream& os) const
        {
            os << "[SSLSocket sock=" << sockfd_
                << " is_connected=" << is_connected_
                << " family=" << family_
                << " type=" << type_
                << " protocol=" << protocol_;
            if (local_address_) 
            {
                os << " local_address=" << local_address_->ToString();
            }
            if (remote_address_) 
            {
                os << " remote_address=" << remote_address_->ToString();
            }
            os << "]";
            return os;
        }

        std::ostream& operator<<(std::ostream& os, const Socket& sock) 
        {
            return sock.Dump(os);
        }


        //--------------------------------------------------------------------------
    } //namespace socket 

    //--------------------------------------------------------------------------
} // namespace ygw
