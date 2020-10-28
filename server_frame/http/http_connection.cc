/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/http/http_connection.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-21
 *  Description: 
 * ====================================================
 */
#include "http_connection.h"
#include "http_parser.h"
#include "server_frame/log.h"
#include "server_frame/stream/zlib_stream.h"

namespace ygw {

    namespace http {

        static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

        std::string HttpResult::ToString() const 
        {
            std::stringstream ss;
            ss << "[HttpResult result=" << result
               << " error=" << error
               << " response=" << (response ? response->ToString() : "nullptr")
               << "]";
            return ss.str();
        }

        HttpConnection::HttpConnection(socket::Socket::ptr sock, bool owner)
            :SocketStream(sock, owner) 
        {
        }

        HttpConnection::~HttpConnection()
        {
            YGW_LOG_DEBUG(g_logger) << "HttpConnection::~HttpConnection";
        }

        HttpResponse::ptr HttpConnection::RecvResponse() 
        {
            HttpResponseParser::ptr parser(new HttpResponseParser);
            uint64_t buff_size = HttpRequestParser::GetHttpRequestBufferSize();
            std::shared_ptr<char[]> buffer(new char[buff_size+1]);
            char* data = buffer.get();
            int offset = 0;
            int len;
            size_t nparse;
            do {
                len = Read(data + offset, buff_size - offset);
                if (len <= 0) 
                {
                    Close();
                    return nullptr;
                }
                len += offset;
                data[len] = '\0';
                nparse = parser->Execute(data, len, false);
                if (parser->HasError()) 
                {
                    Close();
                    return nullptr;
                }
                offset = len - nparse;
                if (offset == (int)buff_size) 
                {
                    Close();
                    return nullptr;
                }
                if (parser->IsFinished()) 
                {
                    break;
                }
            } while(true);

            auto& client_parser = parser->GetParser();
            std::string body;

            if (client_parser.chunked) 
            { // 解析chunck
                int len = offset;
                do {
                    bool begin = true;
                    do {
                        if (!begin || len == 0) 
                        {
                            int rt = Read(data + len, buff_size - len);
                            if (rt <= 0) 
                            {
                                Close();
                                return nullptr;
                            }
                            len += rt;
                        }
                        data[len] = '\0';
                        size_t nparse = parser->Execute(data, len, true);
                        if (parser->HasError()) 
                        {
                            Close();
                            return nullptr;
                        }
                        len -= nparse;
                        if (len == (int)buff_size) 
                        {
                            Close();
                            return nullptr;
                        }
                        begin = false;
                    } while(!parser->IsFinished());
                    //len -= 2;

                    YGW_LOG_DEBUG(g_logger) << "content_len=" << client_parser.content_len;
                    if (client_parser.content_len + 2 <= len) 
                    {
                        body.append(data, client_parser.content_len);
                        memmove(data, data + client_parser.content_len + 2
                                , len - client_parser.content_len - 2);
                        len -= client_parser.content_len + 2;
                    } 
                    else 
                    {
                        body.append(data, len);
                        int left = client_parser.content_len - len + 2;
                        while(left > 0) 
                        {
                            int rt = Read(data, left > (int)buff_size ? (int)buff_size : left);
                            if (rt <= 0) 
                            {
                                Close();
                                return nullptr;
                            }
                            body.append(data, rt);
                            left -= rt;
                        }
                        body.resize(body.size() - 2);
                        len = 0;
                    }

                } while(!client_parser.chunks_done);

            } 
            else 
            {
                int64_t length = parser->GetContentLength();
                if (length > 0) 
                {
                    body.resize(length);

                    int len = 0;
                    if (length >= offset) 
                    {
                        memcpy(&body[0], data, offset);
                        len = offset;
                    } 
                    else 
                    {
                        memcpy(&body[0], data, length);
                        len = length;
                    }
                    length -= offset;
                    if (length > 0) 
                    {
                        if (ReadFixSize(&body[len], length) <= 0) 
                        {
                            Close();
                            return nullptr;
                        }
                    }
                }
            }
            if (!body.empty()) 
            {
                auto content_encoding = parser->GetData()->GetHeader("content-encoding");
                YGW_LOG_DEBUG(g_logger) << "content_encoding: " << content_encoding
                    << " size=" << body.size();
                if (strcasecmp(content_encoding.c_str(), "gzip") == 0) 
                {
                    auto zs = stream::ZlibStream::CreateGzip(false);
                    zs->Write(body.c_str(), body.size());
                    zs->Flush();
                    zs->GetResult().swap(body);
                } 
                else if (strcasecmp(content_encoding.c_str(), "deflate") == 0) 
                {
                    auto zs = stream::ZlibStream::CreateDeflate(false);
                    zs->Write(body.c_str(), body.size());
                    zs->Flush();
                    zs->GetResult().swap(body);
                }
                parser->GetData()->SetBody(body);
            }
            return parser->GetData();
        }

        int HttpConnection::SendRequest(HttpRequest::ptr rsp) 
        {
            std::stringstream ss;
            ss << *rsp;
            std::string data = ss.str();
            //std::cout << ss.str() << std::endl;
            return WriteFixSize(data.c_str(), data.size());
        }

        HttpResult::ptr HttpConnection::DoGet(const std::string& url
                , uint64_t timeout_ms
                , const std::map<std::string, std::string>& headers
                , const std::string& body) 
        {
            Uri::ptr uri = Uri::Create(url);
            if (!uri) 
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::kInvalidUrl
                        , nullptr, "invalid url: " + url);
            }
            return DoGet(uri, timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnection::DoGet(Uri::ptr uri
                , uint64_t timeout_ms
                , const std::map<std::string, std::string>& headers
                , const std::string& body) 
        {
            return DoRequest(HttpMethod::GET, uri, timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnection::DoPost(const std::string& url
                , uint64_t timeout_ms
                , const std::map<std::string, std::string>& headers
                , const std::string& body) 
        {
            Uri::ptr uri = Uri::Create(url);
            if (!uri) 
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::kInvalidUrl
                        , nullptr, "invalid url: " + url);
            }
            return DoPost(uri, timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnection::DoPost(Uri::ptr uri
                , uint64_t timeout_ms
                , const std::map<std::string, std::string>& headers
                , const std::string& body) 
        {
            return DoRequest(HttpMethod::POST, uri, timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnection::DoRequest(HttpMethod method
                , const std::string& url
                , uint64_t timeout_ms
                , const std::map<std::string, std::string>& headers
                , const std::string& body) 
        {
            Uri::ptr uri = Uri::Create(url);
            if (!uri) 
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::kInvalidUrl
                        , nullptr, "invalid url: " + url);
            }
            return DoRequest(method, uri, timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnection::DoRequest(HttpMethod method
                , Uri::ptr uri
                , uint64_t timeout_ms
                , const std::map<std::string, std::string>& headers
                , const std::string& body) 
        {
            HttpRequest::ptr req = std::make_shared<HttpRequest>();
            req->SetPath(uri->GetPath());
            req->SetQuery(uri->GetQuery());
            req->SetFragment(uri->GetFragment());
            req->SetMethod(method);
            bool has_host = false;
            for(auto& i : headers) 
            {
                if (strcasecmp(i.first.c_str(), "connection") == 0) 
                {
                    if (strcasecmp(i.second.c_str(), "keep-alive") == 0) 
                    {
                        req->SetClose(false);
                    }
                    continue;
                }

                if (!has_host && strcasecmp(i.first.c_str(), "host") == 0) 
                {
                    has_host = !i.second.empty();
                }

                req->SetHeader(i.first, i.second);
            }
            if (!has_host) 
            {
                req->SetHeader("Host", uri->GetHost());
            }
            req->SetBody(body);
            return DoRequest(req, uri, timeout_ms);
        }

        HttpResult::ptr HttpConnection::DoRequest(HttpRequest::ptr req
                , Uri::ptr uri
                , uint64_t timeout_ms) 
        {
            bool is_ssl = uri->GetScheme() == "https";
            socket::Address::ptr addr = uri->CreateAddress();
            if (!addr) 
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::kInvalidHost
                        , nullptr, "invalid host: " + uri->GetHost());
            }
            socket::Socket::ptr sock = is_ssl ? socket::SSLSocket::CreateTCP(addr) : socket::Socket::CreateTCP(addr);
            if (!sock) 
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::kCreateSocketError
                        , nullptr, "create socket fail: " + addr->ToString()
                        + " errno=" + std::to_string(errno)
                        + " errstr=" + std::string(strerror(errno)));
            }
            if (!sock->Connect(addr)) 
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::kConnectFail
                        , nullptr, "connect fail: " + addr->ToString());
            }
            sock->SetRecvTimeout(timeout_ms);
            HttpConnection::ptr conn = std::make_shared<HttpConnection>(sock);
            int rt = conn->SendRequest(req);
            if (rt == 0) 
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::kSendCloseByPeer
                        , nullptr, "send request closed by peer: " + addr->ToString());
            }
            if (rt < 0) 
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::kSendSocketError
                        , nullptr, "send request socket error errno=" + std::to_string(errno)
                        + " errstr=" + std::string(strerror(errno)));
            }
            auto rsp = conn->RecvResponse();
            if (!rsp) 
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::kTimeout
                        , nullptr, "recv response timeout: " + addr->ToString()
                        + " timeout_ms:" + std::to_string(timeout_ms));
            }
            return std::make_shared<HttpResult>((int)HttpResult::Error::kOK, rsp, "ok");
        }

        HttpConnectionPool::ptr HttpConnectionPool::Create(const std::string& uri
                ,const std::string& vhost
                ,uint32_t max_size
                ,uint32_t max_alive_time
                ,uint32_t max_request) {
            Uri::ptr turi = Uri::Create(uri);
            if (!turi) {
                YGW_LOG_ERROR(g_logger) << "invalid uri=" << uri;
            }
            return std::make_shared<HttpConnectionPool>(turi->GetHost()
                    , vhost, turi->GetPort(), turi->GetScheme() == "https"
                    , max_size, max_alive_time, max_request);
        }

        HttpConnectionPool::HttpConnectionPool(const std::string& host
                ,const std::string& vhost
                ,uint32_t port
                ,bool is_https
                ,uint32_t max_size
                ,uint32_t max_alive_time
                ,uint32_t max_request)
            :host_(host)
             ,vhost_(vhost)
             ,port_(port ? port : (is_https ? 443 : 80))
             ,max_size_(max_size)
             ,max_alive_time_(max_alive_time)
             ,max_request_(max_request)
             ,is_https_(is_https) 
        {
        }

        HttpConnection::ptr HttpConnectionPool::GetConnection() 
        {
            uint64_t now_ms = ygw::util::TimeUtil::GetCurrentMS();
            std::vector<HttpConnection*> invalid_conns;
            HttpConnection* ptr = nullptr;
            MutexType::Lock lock(mutex_);
            while(!conns_.empty()) 
            {
                auto conn = *conns_.begin();
                conns_.pop_front();
                if (!conn->IsConnected()) 
                {
                    invalid_conns.push_back(conn);
                    continue;
                }
                if ((conn->create_time_ + max_alive_time_) > now_ms) 
                {
                    invalid_conns.push_back(conn);
                    continue;
                }
                ptr = conn;
                break;
            }
            lock.unlock();
            for(auto i : invalid_conns) 
            {
                delete i;
            }
            total_ -= invalid_conns.size();

            if (!ptr) 
            {
                socket::IPAddress::ptr addr = socket::Address::LookupAnyIPAddress(host_);
                if (!addr) 
                {
                    YGW_LOG_ERROR(g_logger) << "get addr fail: " << host_;
                    return nullptr;
                }
                addr->SetPort(port_);
                socket::Socket::ptr sock = is_https_ ? socket::SSLSocket::CreateTCP(addr) : socket::Socket::CreateTCP(addr);
                if (!sock) 
                {
                    YGW_LOG_ERROR(g_logger) << "create sock fail: " << *addr;
                    return nullptr;
                }
                if (!sock->Connect(addr)) 
                {
                    YGW_LOG_ERROR(g_logger) << "sock connect fail: " << *addr;
                    return nullptr;
                }

                ptr = new HttpConnection(sock);
                ++total_;
            }
            return HttpConnection::ptr(ptr, std::bind(&HttpConnectionPool::ReleasePtr
                        , std::placeholders::_1, this));
        }

        void HttpConnectionPool::ReleasePtr(HttpConnection* ptr, HttpConnectionPool* pool) 
        {
            ++ptr->request_;
            if (!ptr->IsConnected()
                    || ((ptr->create_time_ + pool->max_alive_time_) >= ygw::util::TimeUtil::GetCurrentMS())
                    || (ptr->request_ >= pool->max_request_)) 
            {
                delete ptr;
                --pool->total_;
                return;
            }
            MutexType::Lock lock(pool->mutex_);
            pool->conns_.push_back(ptr);
        }

        HttpResult::ptr HttpConnectionPool::DoGet(const std::string& url
                , uint64_t timeout_ms
                , const std::map<std::string, std::string>& headers
                , const std::string& body) 
        {
            return DoRequest(HttpMethod::GET, url, timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnectionPool::DoGet(Uri::ptr uri
                , uint64_t timeout_ms
                , const std::map<std::string, std::string>& headers
                , const std::string& body) 
        {
            std::stringstream ss;
            ss << uri->GetPath()
                << (uri->GetQuery().empty() ? "" : "?")
                << uri->GetQuery()
                << (uri->GetFragment().empty() ? "" : "#")
                << uri->GetFragment();
            return DoGet(ss.str(), timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnectionPool::DoPost(const std::string& url
                , uint64_t timeout_ms
                , const std::map<std::string, std::string>& headers
                , const std::string& body) 
        {
            return DoRequest(HttpMethod::POST, url, timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnectionPool::DoPost(Uri::ptr uri
                , uint64_t timeout_ms
                , const std::map<std::string, std::string>& headers
                , const std::string& body) 
        {
            std::stringstream ss;
            ss << uri->GetPath()
                << (uri->GetQuery().empty() ? "" : "?")
                << uri->GetQuery()
                << (uri->GetFragment().empty() ? "" : "#")
                << uri->GetFragment();
            return DoPost(ss.str(), timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnectionPool::DoRequest(HttpMethod method
                , const std::string& url
                , uint64_t timeout_ms
                , const std::map<std::string, std::string>& headers
                , const std::string& body) 
        {
            HttpRequest::ptr req = std::make_shared<HttpRequest>();
            req->SetPath(url);
            req->SetMethod(method);
            req->SetClose(false);
            bool has_host = false;
            for(auto& i : headers) 
            {
                if (strcasecmp(i.first.c_str(), "connection") == 0) 
                {
                    if (strcasecmp(i.second.c_str(), "keep-alive") == 0)
                    {
                        req->SetClose(false);
                    }
                    continue;
                }

                if (!has_host && strcasecmp(i.first.c_str(), "host") == 0) {
                    has_host = !i.second.empty();
                }

                req->SetHeader(i.first, i.second);
            }
            if (!has_host) 
            {
                if (vhost_.empty()) 
                {
                    req->SetHeader("Host", host_);
                } 
                else 
                {
                    req->SetHeader("Host", vhost_);
                }
            }
            req->SetBody(body);
            return DoRequest(req, timeout_ms);
        }

        HttpResult::ptr HttpConnectionPool::DoRequest(HttpMethod method
                , Uri::ptr uri
                , uint64_t timeout_ms
                , const std::map<std::string, std::string>& headers
                , const std::string& body) 
        {
            std::stringstream ss;
            ss << uri->GetPath()
                << (uri->GetQuery().empty() ? "" : "?")
                << uri->GetQuery()
                << (uri->GetFragment().empty() ? "" : "#")
                << uri->GetFragment();
            return DoRequest(method, ss.str(), timeout_ms, headers, body);
        }

        HttpResult::ptr HttpConnectionPool::DoRequest(HttpRequest::ptr req
                , uint64_t timeout_ms) 
        {
            auto conn = GetConnection();
            if (!conn) 
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::kPoolGetConnection
                        , nullptr, "pool host:" + host_ + " port:" + std::to_string(port_));
            }
            auto sock = conn->GetSocket();
            if (!sock) 
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::kPoolInvalidConnection
                        , nullptr, "pool host:" + host_ + " port:" + std::to_string(port_));
            }
            sock->SetRecvTimeout(timeout_ms);
            int rt = conn->SendRequest(req);
            if (rt == 0) 
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::kSendCloseByPeer
                        , nullptr, "send request closed by peer: " + sock->GetRemoteAddress()->ToString());
            }
            if (rt < 0) 
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::kSendSocketError
                        , nullptr, "send request socket error errno=" + std::to_string(errno)
                        + " errstr=" + std::string(strerror(errno)));
            }
            auto rsp = conn->RecvResponse();
            if (!rsp) 
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::kTimeout
                        , nullptr, "recv response timeout: " + sock->GetRemoteAddress()->ToString()
                        + " timeout_ms:" + std::to_string(timeout_ms));
            }
            return std::make_shared<HttpResult>((int)HttpResult::Error::kOK, rsp, "ok");
        }


    }

} // namespace ygw
