/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: http/http.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-18
 *  Description: 
 * ====================================================
 */
#include "http.h"
#include "server_frame/util.h"

namespace ygw {

    namespace http {

        // 字符串转HttpMethod枚举
        HttpMethod StringToHttpMethod(const std::string& m) 
        {
#define XX(num, name, string) \
            if (strcmp(#string, m.c_str()) == 0) { \
                return HttpMethod::name; \
            }
            HTTP_METHOD_MAP(XX);
#undef XX
            return HttpMethod::INVALID_METHOD;
        }

        // C字符串转HttpMethod枚举
        HttpMethod CharsToHttpMethod(const char* m) 
        {
#define XX(num, name, string) \
            if (strncmp(#string, m, strlen(#string)) == 0) { \
                return HttpMethod::name; \
            }
            HTTP_METHOD_MAP(XX);
#undef XX
            return HttpMethod::INVALID_METHOD;
        }

        static const char* s_method_string[] = {
#define XX(num, name, string) #string,
            HTTP_METHOD_MAP(XX)
#undef XX
        };

        // HttpMethod枚举转C字符串 
        const char* HttpMethodToString(const HttpMethod& m) 
        {
            uint32_t idx = (uint32_t)m;
            if (idx >= (sizeof(s_method_string) / sizeof(s_method_string[0]))) 
            {
                return "<unknown>";
            }
            return s_method_string[idx];
        }

        // HttpStatus枚举转C字符串 
        const char* HttpStatusToString(const HttpStatus& s) 
        {
            switch(s) 
            {
#define XX(code, name, msg) \
            case HttpStatus::name: \
                return #msg;
                HTTP_STATUS_MAP(XX);
#undef XX
            default:
                return "<unknown>";
            }
        }

        bool CaseInsensitiveLess::operator()(const std::string& lhs
                ,const std::string& rhs) const 
        {
            return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
        }

        //--------------------------------------------------------------------
        //              HttpRequest Method
        //--------------------------------------------------------------------
        HttpRequest::HttpRequest(uint8_t version, bool close)
            :method_(HttpMethod::GET)
            ,version_(version)
            ,close_(close)
            ,websocket_(false)
            ,parser_param_flag_(0)
            ,path_("/") 
        {
        }

        std::string HttpRequest::GetHeader(const std::string& key
                                          ,const std::string& def) const 
        {
            auto it = headers_.find(key);
            return it == headers_.end() ? def : it->second;
        }

        std::shared_ptr<HttpResponse> HttpRequest::CreateResponse() 
        {
            HttpResponse::ptr rsp(new HttpResponse(GetVersion()
                        ,IsClose()));
            return rsp;
        }

        std::string HttpRequest::GetParam(const std::string& key
                ,const std::string& def) 
        {
            InitQueryParam();
            InitBodyParam();
            auto it = params_.find(key);
            return it == params_.end() ? def : it->second;
        }

        std::string HttpRequest::GetCookie(const std::string& key
                                          ,const std::string& def) 
        {
            InitCookies();
            auto it = cookies_.find(key);
            return it == cookies_.end() ? def : it->second;
        }

        void HttpRequest::SetHeader(const std::string& key, const std::string& val) 
        {
            headers_[key] = val;
        }

        void HttpRequest::SetParam(const std::string& key, const std::string& val) 
        {
            params_[key] = val;
        }

        void HttpRequest::SetCookie(const std::string& key, const std::string& val) 
        {
            cookies_[key] = val;
        }

        void HttpRequest::DelHeader(const std::string& key) 
        {
            headers_.erase(key);
        }

        void HttpRequest::DelParam(const std::string& key) 
        {
            params_.erase(key);
        }

        void HttpRequest::DelCookie(const std::string& key) 
        {
            cookies_.erase(key);
        }

        bool HttpRequest::HasHeader(const std::string& key, std::string* val)
        {
            auto it = headers_.find(key);
            if (it == headers_.end()) 
            {
                return false;
            }
            if (val) 
            {
                *val = it->second;
            }
            return true;
        }

        bool HttpRequest::HasParam(const std::string& key, std::string* val) 
        {
            InitQueryParam();
            InitBodyParam();
            auto it = params_.find(key);
            if (it == params_.end()) 
            {
                return false;
            }
            if (val) 
            {
                *val = it->second;
            }
            return true;
        }

        bool HttpRequest::HasCookie(const std::string& key, std::string* val) {
            InitCookies();
            auto it = cookies_.find(key);
            if (it == cookies_.end()) 
            {
                return false;
            }
            if (val) 
            {
                *val = it->second;
            }
            return true;
        }

        std::string HttpRequest::ToString() const
        {
            std::stringstream ss;
            Dump(ss);
            return ss.str();
        }

        std::ostream& HttpRequest::Dump(std::ostream& os) const 
        {
            //GET /uri HTTP/1.1
            //Host: wwww.baidu.com
            os << HttpMethodToString(method_) << " "
                << path_
                << (query_.empty() ? "" : "?")
                << query_
                << (fragment_.empty() ? "" : "#")
                << fragment_
                << " HTTP/"
                << ((uint32_t)(version_ >> 4))
                << "."
                << ((uint32_t)(version_ & 0x0F))
                << "\r\n";
            if (!websocket_) 
            {
                os << "connection: " << (close_ ? "close" : "keep-alive") << "\r\n";
            }
            //请求头
            for (auto& i : headers_) 
            {
                if (!websocket_ && strcasecmp(i.first.c_str(), "connection") == 0) 
                {
                    continue;
                }
                os << i.first << ": " << i.second << "\r\n";
            }

            //请求体
            if (!body_.empty()) 
            {
                os << "content-length: " << body_.size() << "\r\n\r\n"
                    << body_;
            } 
            else 
            {
                os << "\r\n";
            }
            return os;
        }

        void HttpRequest::Init() 
        {
            std::string conn = GetHeader("connection");
            if (!conn.empty()) 
            {
                if (strcasecmp(conn.c_str(), "keep-alive") == 0) 
                {
                    close_ = false;
                } 
                else 
                {
                    close_ = true;
                }
            }
        }

        void HttpRequest::InitParam() 
        {
            InitQueryParam();
            InitBodyParam();
            InitCookies();
        }

        void HttpRequest::InitQueryParam() 
        {
            if (parser_param_flag_ & 0x1) 
            {
                return;
            }

#define PARSE_PARAM(str, m, flag, trim) \
            size_t pos = 0; \
            do { \
                size_t last = pos; \
                pos = str.find('=', pos); \
                if (pos == std::string::npos) { \
                    break; \
                } \
                size_t key = pos; \
                pos = str.find(flag, pos); \
                m.insert(std::make_pair(trim(str.substr(last, key - last)), \
                            ygw::util::StringUtil::UrlDecode(str.substr(key + 1, pos - key - 1)))); \
                if (pos == std::string::npos) { \
                    break; \
                } \
                ++pos; \
            } while (true);

            PARSE_PARAM(query_, params_, '&',);
            parser_param_flag_ |= 0x1;
        }

        void HttpRequest::InitBodyParam() 
        {
            if (parser_param_flag_ & 0x2) 
            {
                return;
            }
            std::string content_type = GetHeader("content-type");
            if (strcasestr(content_type.c_str(), "application/x-www-form-urlencoded") == nullptr) 
            {
                parser_param_flag_ |= 0x2;
                return;
            }
            PARSE_PARAM(body_, params_, '&',);
            parser_param_flag_ |= 0x2;
        }

        void HttpRequest::InitCookies() 
        {
            if (parser_param_flag_ & 0x4) {
                return;
            }
            std::string cookie = GetHeader("cookie");
            if (cookie.empty()) {
                parser_param_flag_ |= 0x4;
                return;
            }
            PARSE_PARAM(cookie, cookies_, ';', ygw::util::StringUtil::Trim);
            parser_param_flag_ |= 0x4;
        }



        //---------------------------------------------------------------------------
        //              class HttpResponse Method
        //---------------------------------------------------------------------------
        HttpResponse::HttpResponse(uint8_t version, bool close)
            :status_(HttpStatus::OK)
             ,version_(version)
             ,close_(close)
             ,websocket_(false) 
        {
        }

        std::string HttpResponse::GetHeader(const std::string& key, const std::string& def) const 
        {
            auto it = headers_.find(key);
            return it == headers_.end() ? def : it->second;
        }

        void HttpResponse::SetHeader(const std::string& key, const std::string& val) 
        {
            headers_[key] = val;
        }

        void HttpResponse::DelHeader(const std::string& key) 
        {
            headers_.erase(key);
        }

        void HttpResponse::SetRedirect(const std::string& uri) 
        {
            status_ = HttpStatus::FOUND;
            SetHeader("Location", uri);
        }

        void HttpResponse::SetCookie(const std::string& key, const std::string& val,
                time_t expired, const std::string& path,
                const std::string& domain, bool secure) 
        {
            std::stringstream ss;
            ss << key << "=" << val;
            if (expired > 0) 
            {
                ss << ";expires=" << ygw::util::TimeUtil::Time2Str(expired, "%a, %d %b %Y %H:%M:%S") << " GMT";
            }
            if (!domain.empty()) 
            {
                ss << ";domain=" << domain;
            }
            if (!path.empty()) 
            {
                ss << ";path=" << path;
            }
            if (secure) 
            {
                ss << ";secure";
            }
            cookies_.push_back(ss.str());
        }


        std::string HttpResponse::ToString() const 
        {
            std::stringstream ss;
            Dump(ss);
            return ss.str();
        }

        std::ostream& HttpResponse::Dump(std::ostream& os) const 
        {
            os << "HTTP/"
                << ((uint32_t)(version_ >> 4))
                << "."
                << ((uint32_t)(version_ & 0x0F))
                << " "
                << (uint32_t)status_
                << " "
                << (reason_.empty() ? HttpStatusToString(status_) : reason_)
                << "\r\n";

            // 响应头
            for (auto& i : headers_) 
            {
                if (!websocket_ && strcasecmp(i.first.c_str(), "connection") == 0) {
                    continue;
                }
                os << i.first << ": " << i.second << "\r\n";
            }
            // 响应体 
            for (auto& i : cookies_)  // cookie
            {
                os << "Set-Cookie: " << i << "\r\n";
            }
            if (!websocket_) 
            {
                os << "connection: " << (close_ ? "close" : "keep-alive") << "\r\n";
            }
            if (!body_.empty())  // body
            {
                os << "content-length: " << body_.size() << "\r\n\r\n"
                    << body_;
            } 
            else 
            {
                os << "\r\n";
            }
            return os;
        }

        std::ostream& operator<<(std::ostream& os, const HttpRequest& req) 
        {
            return req.Dump(os);
        }

        std::ostream& operator<<(std::ostream& os, const HttpResponse& rsp) 
        {
            return rsp.Dump(os);
        }

    } // namespace http

} // namespace ygw
