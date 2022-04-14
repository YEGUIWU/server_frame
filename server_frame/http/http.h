/**
 * @file http.h
 * @brief http模块
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#ifndef __YGW_HTTP_HTTP_H__
#define __YGW_HTTP_HTTP_H__

#include <memory>
#include <string>
#include <map>
#include <iostream>
#include <sstream>

#include <boost/lexical_cast.hpp>

namespace ygw {

    namespace http {

    //-------------------------------------------------------------

    /* Request Methods */
#define HTTP_METHOD_MAP(XX)         \
  XX(0,  DELETE,      DELETE)       \
  XX(1,  GET,         GET)          \
  XX(2,  HEAD,        HEAD)         \
  XX(3,  POST,        POST)         \
  XX(4,  PUT,         PUT)          \
  /* pathological */                \
  XX(5,  CONNECT,     CONNECT)      \
  XX(6,  OPTIONS,     OPTIONS)      \
  XX(7,  TRACE,       TRACE)        \
  /* WebDAV */                      \
  XX(8,  COPY,        COPY)         \
  XX(9,  LOCK,        LOCK)         \
  XX(10, MKCOL,       MKCOL)        \
  XX(11, MOVE,        MOVE)         \
  XX(12, PROPFIND,    PROPFIND)     \
  XX(13, PROPPATCH,   PROPPATCH)    \
  XX(14, SEARCH,      SEARCH)       \
  XX(15, UNLOCK,      UNLOCK)       \
  XX(16, BIND,        BIND)         \
  XX(17, REBIND,      REBIND)       \
  XX(18, UNBIND,      UNBIND)       \
  XX(19, ACL,         ACL)          \
  /* subversion */                  \
  XX(20, REPORT,      REPORT)       \
  XX(21, MKACTIVITY,  MKACTIVITY)   \
  XX(22, CHECKOUT,    CHECKOUT)     \
  XX(23, MERGE,       MERGE)        \
  /* upnp */                        \
  XX(24, MSEARCH,     M-SEARCH)     \
  XX(25, NOTIFY,      NOTIFY)       \
  XX(26, SUBSCRIBE,   SUBSCRIBE)    \
  XX(27, UNSUBSCRIBE, UNSUBSCRIBE)  \
  /* RFC-5789 */                    \
  XX(28, PATCH,       PATCH)        \
  XX(29, PURGE,       PURGE)        \
  /* CalDAV */                      \
  XX(30, MKCALENDAR,  MKCALENDAR)   \
  /* RFC-2068, section 19.6.1.2 */  \
  XX(31, LINK,        LINK)         \
  XX(32, UNLINK,      UNLINK)       \
  /* icecast */                     \
  XX(33, SOURCE,      SOURCE)       \



/* Status Codes */
#define HTTP_STATUS_MAP(XX)                                                 \
  XX(100, CONTINUE,                        Continue)                        \
  XX(101, SWITCHING_PROTOCOLS,             Switching Protocols)             \
  XX(102, PROCESSING,                      Processing)                      \
  XX(200, OK,                              OK)                              \
  XX(201, CREATED,                         Created)                         \
  XX(202, ACCEPTED,                        Accepted)                        \
  XX(203, NON_AUTHORITATIVE_INFORMATION,   Non-Authoritative Information)   \
  XX(204, NO_CONTENT,                      No Content)                      \
  XX(205, RESET_CONTENT,                   Reset Content)                   \
  XX(206, PARTIAL_CONTENT,                 Partial Content)                 \
  XX(207, MULTI_STATUS,                    Multi-Status)                    \
  XX(208, ALREADY_REPORTED,                Already Reported)                \
  XX(226, IM_USED,                         IM Used)                         \
  XX(300, MULTIPLE_CHOICES,                Multiple Choices)                \
  XX(301, MOVED_PERMANENTLY,               Moved Permanently)               \
  XX(302, FOUND,                           Found)                           \
  XX(303, SEE_OTHER,                       See Other)                       \
  XX(304, NOT_MODIFIED,                    Not Modified)                    \
  XX(305, USE_PROXY,                       Use Proxy)                       \
  XX(307, TEMPORARY_REDIRECT,              Temporary Redirect)              \
  XX(308, PERMANENT_REDIRECT,              Permanent Redirect)              \
  XX(400, BAD_REQUEST,                     Bad Request)                     \
  XX(401, UNAUTHORIZED,                    Unauthorized)                    \
  XX(402, PAYMENT_REQUIRED,                Payment Required)                \
  XX(403, FORBIDDEN,                       Forbidden)                       \
  XX(404, NOT_FOUND,                       Not Found)                       \
  XX(405, METHOD_NOT_ALLOWED,              Method Not Allowed)              \
  XX(406, NOT_ACCEPTABLE,                  Not Acceptable)                  \
  XX(407, PROXY_AUTHENTICATION_REQUIRED,   Proxy Authentication Required)   \
  XX(408, REQUEST_TIMEOUT,                 Request Timeout)                 \
  XX(409, CONFLICT,                        Conflict)                        \
  XX(410, GONE,                            Gone)                            \
  XX(411, LENGTH_REQUIRED,                 Length Required)                 \
  XX(412, PRECONDITION_FAILED,             Precondition Failed)             \
  XX(413, PAYLOAD_TOO_LARGE,               Payload Too Large)               \
  XX(414, URI_TOO_LONG,                    URI Too Long)                    \
  XX(415, UNSUPPORTED_MEDIA_TYPE,          Unsupported Media Type)          \
  XX(416, RANGE_NOT_SATISFIABLE,           Range Not Satisfiable)           \
  XX(417, EXPECTATION_FAILED,              Expectation Failed)              \
  XX(421, MISDIRECTED_REQUEST,             Misdirected Request)             \
  XX(422, UNPROCESSABLE_ENTITY,            Unprocessable Entity)            \
  XX(423, LOCKED,                          Locked)                          \
  XX(424, FAILED_DEPENDENCY,               Failed Dependency)               \
  XX(426, UPGRADE_REQUIRED,                Upgrade Required)                \
  XX(428, PRECONDITION_REQUIRED,           Precondition Required)           \
  XX(429, TOO_MANY_REQUESTS,               Too Many Requests)               \
  XX(431, REQUEST_HEADER_FIELDS_TOO_LARGE, Request Header Fields Too Large) \
  XX(451, UNAVAILABLE_FOR_LEGAL_REASONS,   Unavailable For Legal Reasons)   \
  XX(500, INTERNAL_SERVER_ERROR,           Internal Server Error)           \
  XX(501, NOT_IMPLEMENTED,                 Not Implemented)                 \
  XX(502, BAD_GATEWAY,                     Bad Gateway)                     \
  XX(503, SERVICE_UNAVAILABLE,             Service Unavailable)             \
  XX(504, GATEWAY_TIMEOUT,                 Gateway Timeout)                 \
  XX(505, HTTP_VERSION_NOT_SUPPORTED,      HTTP Version Not Supported)      \
  XX(506, VARIANT_ALSO_NEGOTIATES,         Variant Also Negotiates)         \
  XX(507, INSUFFICIENT_STORAGE,            Insufficient Storage)            \
  XX(508, LOOP_DETECTED,                   Loop Detected)                   \
  XX(510, NOT_EXTENDED,                    Not Extended)                    \
  XX(511, NETWORK_AUTHENTICATION_REQUIRED, Network Authentication Required) \

        /**
         * @brief HTTP方法枚举
         */
        enum class HttpMethod
        {
#define XX(num, name, string) name = num,
            HTTP_METHOD_MAP(XX)
#undef XX
                INVALID_METHOD
        };

        /**
         * @brief HTTP状态枚举
         */
        enum class HttpStatus
        {
#define XX(num, name, string) name = num,
            HTTP_STATUS_MAP(XX)
#undef XX
        };

        /**
         * @brief 将字符串方法名转成HTTP方法枚举
         * @param[in] m HTTP方法
         * @return HTTP方法枚举
         */
        HttpMethod StringToHttpMethod(const std::string& m);

        /**
         * @brief 将字符串指针转换成HTTP方法枚举
         * @param[in] m 字符串方法枚举
         * @return HTTP方法枚举
         */
        HttpMethod CharsToHttpMethod(const char* m);

        /**
         * @brief 将HTTP方法枚举转换成字符串
         * @param[in] m HTTP方法枚举
         * @return 字符串
         */
        const char* HttpMethodToString(const HttpMethod& m);

        /**
         * @brief 将HTTP状态枚举转换成字符串
         * @param[in] m HTTP状态枚举
         * @return 字符串
         */
        const char* HttpStatusToString(const HttpStatus& s);

        /**
         * @brief 忽略大小写比较仿函数
         */
        struct CaseInsensitiveLess {
            /**
             * @brief 忽略大小写比较字符串
             */
            bool operator()(const std::string& lhs, const std::string& rhs) const;
        };


        /**
         * @brief 获取Map中的key值,并转成对应类型,返回是否成功
         * @param[in] m Map数据结构
         * @param[in] key 关键字
         * @param[out] val 保存转换后的值
         * @param[in] def 默认值
         * @return
         *      @retval true 转换成功, val 为对应的值
         *      @retval false 不存在或者转换失败 val = def
         */
        template<class MapType, class T>
        bool CheckGetAs(const MapType& m, const std::string& key, T& val, const T& def = T()) 
        {
            auto it = m.find(key);
            if (it == m.end()) 
            {
                val = def;
                return false;
            }
            try 
            {
                val = boost::lexical_cast<T>(it->second);
                return true;
            } 
            catch (...) 
            {
                val = def;
            }
            return false;
        }

        /**
         * @brief 获取Map中的key值,并转成对应类型
         * @param[in] m Map数据结构
         * @param[in] key 关键字
         * @param[in] def 默认值
         * @return 如果存在且转换成功返回对应的值,否则返回默认值
         */
        template<class MapType, class T>
        T GetAs(const MapType& m, const std::string& key, const T& def = T()) 
        {
            auto it = m.find(key);
            if (it == m.end()) 
            {
                return def;
            }
            try 
            {
                return boost::lexical_cast<T>(it->second);
            } 
            catch (...) 
            {
            }
            return def;
        }


        //-------------------------------------------------------------------------------
        /**
         * @brief HTTP请求结构
         */
        class HttpResponse; // 

        class HttpRequest {
        public:
            /// HTTP请求的智能指针
            using ptr = std::shared_ptr<HttpRequest>;
            /// MAP结构
            using MapType = std::map<std::string, std::string, CaseInsensitiveLess>;

            /**
             * @brief 构造函数
             * @param[in] version 版本
             * @param[in] close 是否keepalive
             */
            HttpRequest(uint8_t version = 0x11, bool close = true);

            std::shared_ptr<HttpResponse> CreateResponse();
            /**
             * @brief 返回HTTP方法
             */
            HttpMethod GetMethod() const { return method_; }

            /**
             * @brief 返回HTTP版本
             */
            uint8_t GetVersion() const { return version_; }

            /**
             * @brief 返回HTTP请求的路径
             */
            const std::string& GetPath() const { return path_; }

            /**
             * @brief 返回HTTP请求的查询参数
             */
            const std::string& GetQuery() const { return query_; }

            /**
             * @brief 返回HTTP请求的消息体
             */
            const std::string& GetBody() const { return body_; }

            /**
             * @brief 返回fragment
             */
            const std::string& GetFragment() const { return fragment_; }

            /**
             * @brief 返回HTTP请求的消息头MAP
             */
            const MapType& GetHeaders() const { return headers_; }

            /**
             * @brief 返回HTTP请求的参数MAP
             */
            const MapType& GetParams() const { return params_; }

            /**
             * @brief 返回HTTP请求的cookie MAP
             */
            const MapType& GetCookies() const { return cookies_; }

            /**
             * @brief 设置HTTP请求的方法名
             * @param[in] v HTTP请求
             */
            void SetMethod(HttpMethod v) { method_ = v;}

            /**
             * @brief 设置HTTP请求的协议版本
             * @param[in] v 协议版本0x11, 0x10
             */
            void SetVersion(uint8_t v) { version_ = v; }

            /**
             * @brief 设置HTTP请求的路径
             * @param[in] v 请求路径
             */
            void SetPath(const std::string& v) { path_ = v; }

            /**
             * @brief 设置HTTP请求的查询参数
             * @param[in] v 查询参数
             */
            void SetQuery(const std::string& v) { query_ = v; }

            /** 
             * @brief 设置HTTP请求的Fragment参数
             * @param[in] v fragment 
             */
            void SetFragment(const std::string& v) { fragment_ = v; }

            /**
             * @brief 设置HTTP请求的消息体
             * @param[in] v 消息体
             */
            void SetBody(const std::string& v) { body_ = v; }

            /**
             * @brief 是否自动关闭
             */
            bool IsClose() const { return close_; }

            /**
             * @brief 设置是否自动关闭
             */
            void SetClose(bool v) { close_ = v; }

            /**
             * @brief 是否websocket
             */
            bool IsWebsocket() const { return websocket_; }

            /**
             * @brief 设置是否websocket
             */
            void SetWebsocket(bool v) { websocket_ = v; }

            /**
             * @brief 设置HTTP请求头的头部MAP
             * @param[in] v map
             */
            void SetHeaders(const MapType& v) { headers_ = v; }

            /**
             * @brief 设置HTTP请求的参数MAP
             * @param[in] v map
             */
            void SetParams(const MapType& v) { params_ = v; }

            /**
             * @brief 设置HTTP请求的Cookie MAP
             * @param[in] v map
             */
            void SetCookies(const MapType& v) { cookies_ = v; }

            /**
             * @brief 获取HTTP请求的头部x参数
             * @param[in] key 关键字
             * @param[in] def 默认值
             * @return 如果存在则返回对应值，否则返回默认值
             */
            std::string GetHeader(const std::string& key, const std::string& def="") const;

            /**
             * @breif 获取HTTP请求的请求参数
             * @param[in] key 关键字
             * @param[in[ def 默认值
             * @return 如果存在则返回对应值，否则返回默认值
             */
            std::string GetParam(const std::string& key, const std::string& def="");

            /**
             * @brief 获取HTTP请求的Cookie参数
             * @param[in] key 关键字
             * @param[in] def 默认值
             * @return 如果存在则返回对应值，否则返回默认值
             */
            std::string GetCookie(const std::string& key, const std::string& def="");

            /**
             * @brief 设置HTTP请求的头部参数
             * @param[in] key 关键字
             * @param[in] val 值
             */
            void SetHeader(const std::string& key, const std::string& val);

            /**
             * @brief 设置HTTP请求的请求参数
             * @param[in] key 关键字
             * @param[in] val 值
             */
            void SetParam(const std::string& key, const std::string& val);

            /** 
             * @brief 设置HTTP请求的Cookie参数
             * @param[in] key 关键字
             * @param[in] val 值
             */
            void SetCookie(const std::string& key, const std::string& val);

            /**
             * @brief 删除HTTP请求的头部参数
             * @param[in] key 关键字
             */
            void DelHeader(const std::string& key);

            /**
             * @brief 删除HTTP请求的头部参数
             * @param[in] key 关键字
             */
            void DelParam(const std::string& key);

            /**
             * @brief 删除HTTP请求的Cookie参数
             * @param[in] key 关键字
             */
            void DelCookie(const std::string& key);

            /**
             * @brief 判断HTTP请求的头部参数是否存在
             * @param[in] key 关键字
             * @param[out] val 如果存在,val非空则赋值
             * @return 是否存在
             */
            bool HasHeader(const std::string& key, std::string* val = nullptr);

            /**
             * @brief 判断HTTP请求的请求参数是否存在
             * @param[in] key 关键字
             * @param[out] val 如果存在,val非空则赋值
             * @return 是否存在
             */
            bool HasParam(const std::string& key, std::string* val = nullptr);

            /**
             * @brief 判断HTTP请求的Cookie参数是否存在
             * @param[in] key 关键字
             * @param[out] val 如果存在,val非空则赋值
             * @return 是否存在
             */
            bool HasCookie(const std::string& key, std::string* val = nullptr);

            /**
             * @brief 检查并获取HTTP请求的头部参数
             * @tparam T 转换类型
             * @param[in] key 关键字
             * @param[out] val 返回值
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回true,否则失败val=def
             */
            template<class T>
            bool CheckGetHeaderAs(const std::string& key, T& val, const T& def = T()) 
            {
                return CheckGetAs(headers_, key, val, def);
            }

            /**
             * @brief 获取HTTP请求的头部参数
             * @tparam T 转换类型
             * @param[in] key 关键字
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回对应的值,否则返回def
             */
            template<class T>
            T GetHeaderAs(const std::string& key, const T& def = T()) 
            {
                return GetAs(headers_, key, def);
            }

            /**
             * @brief 检查并获取HTTP请求的请求参数
             * @tparam T 转换类型
             * @param[in] key 关键字
             * @param[out] val 返回值
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回true,否则失败val=def
             */
            template<class T>
            bool CheckGetParamAs(const std::string& key, T& val, const T& def = T()) 
            {
                InitQueryParam();
                InitBodyParam();
                return CheckGetAs(params_, key, val, def);
            }

            /**
             * @brief 获取HTTP请求的请求参数
             * @tparam T 转换类型
             * @param[in] key 关键字
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回对应的值,否则返回def
             */
            template<class T>
            T GetParamAs(const std::string& key, const T& def = T()) 
            {
                InitQueryParam();
                InitBodyParam();
                return GetAs(params_, key, def);
            }

            /**
             * @brief 检查并获取HTTP请求的Cookie参数
             * @tparam T 转换类型
             * @param[in] key 关键字
             * @param[out] val 返回值
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回true,否则失败val=def
             */
            template<class T>
            bool CheckGetCookieAs(const std::string& key, T& val, const T& def = T()) 
            {
                InitCookies();
                return CheckGetAs(cookies_, key, val, def);
            }

            /**
              _* @brief 获取HTTP请求的Cookie参数
             * @tparam T 转换类型
             * @param[in] key 关键字
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回对应的值,否则返回def
             */
            template<class T>
            T GetCookieAs(const std::string& key, const T& def = T()) 
            {
                InitCookies();
                return GetAs(cookies_, key, def);
            }

            /**
             * @brief 序列化输出到流中
             * @param[in, out] os 输出流
             * @return 输出流
             */
            std::ostream& Dump(std::ostream& os) const;

            /**
             * @brief 转成字符串类型
             * @return 字符串
             */
            std::string ToString() const;

            void Init();
            void InitParam();
            void InitQueryParam();
            void InitBodyParam();
            void InitCookies();
        private:
            /// HTTP方法
            HttpMethod method_;
            /// HTTP版本
            uint8_t version_;
            /// 是否自动关闭
            bool close_;
            /// 是否为websocket
            bool websocket_;

            uint8_t parser_param_flag_;
            /// 请求路径
            std::string path_;
            /// 请求参数
            std::string query_;
            /// 请求fragment
            std::string fragment_;
            /// 请求消息体
            std::string body_;
            /// 请求头部MAP
            MapType headers_;
            /// 请求参数MAP
            MapType params_;
            /// 请求Cookie MAP
            MapType cookies_;
        };  // class HttpRequest

        //-------------------------------------------------------------------------------
        /**
         * @brief HTTP响应结构体
         */
        class HttpResponse 
        {
        public:
            /// HTTP响应结构智能指针
            using ptr = std::shared_ptr<HttpResponse>;
            /// MapType
            using MapType = std::map<std::string, std::string, CaseInsensitiveLess>;
            /**
             * @brief 构造函数
             * @param[in] version 版本
             * @param[in] close 是否自动关闭
             */
            HttpResponse(uint8_t version = 0x11, bool close = true);

            /**
             * @brief 返回响应状态
             * @return 请求状态
             */
            HttpStatus GetStatus() const { return status_;}

            /**
             * @brief 返回响应版本
             * @return 版本
             */
            uint8_t GetVersion() const { return version_;}

            /**
             * @brief 返回响应消息体
             * @return 消息体
             */
            const std::string& GetBody() const { return body_;}

            /**
             * @brief 返回响应原因
             */
            const std::string& GetReason() const { return reason_;}

            /**
             * @brief 返回响应头部MAP
             * @return MAP
             */
            const MapType& GetHeaders() const { return headers_;}

            /**
             * @brief 设置响应状态
             * @param[in] v 响应状态
             */
            void SetStatus(HttpStatus v) { status_ = v;}

            /**
             * @brief 设置响应版本
             * @param[in] v 版本
             */
            void SetVersion(uint8_t v) { version_ = v;}

            /**
             * @brief 设置响应消息体
             * @param[in] v 消息体
             */
            void SetBody(const std::string& v) { body_ = v;}

            /**
             * @brief 设置响应原因
             * @param[in] v 原因
             */
            void SetReason(const std::string& v) { reason_ = v;}

            /**
             * @brief 设置响应头部MAP
             * @param[in] v MAP
             */
            void SetHeaders(const MapType& v) { headers_ = v;}

            /**
             * @brief 是否自动关闭
             */
            bool IsClose() const { return close_;}

            /**
             * @brief 设置是否自动关闭
             */
            void SetClose(bool v) { close_ = v;}

            /**
             * @brief 是否websocket
             */
            bool IsWebsocket() const { return websocket_;}

            /**
             * @brief 设置是否websocket
             */
            void SetWebsocket(bool v) { websocket_ = v;}

            /**
             * @brief 获取响应头部参数
             * @param[in] key 关键字
             * @param[in] def 默认值
             * @return 如果存在返回对应值,否则返回def
             */
            std::string GetHeader(const std::string& key, const std::string& def = "") const;

            /**
             * @brief 设置响应头部参数
             * @param[in] key 关键字
             * @param[in] val 值
             */
            void SetHeader(const std::string& key, const std::string& val);

            /**
             * @brief 删除响应头部参数
             * @param[in] key 关键字
             */
            void DelHeader(const std::string& key);

            /**
             * @brief 检查并获取响应头部参数
             * @tparam T 值类型
             * @param[in] key 关键字
             * @param[out] val 值
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回true,否则失败val=def
             */
            template<class T>
            bool CheckGetHeaderAs(const std::string& key, T& val, const T& def = T()) 
            {
                return CheckGetAs(headers_, key, val, def);
            }

            /**
             * @brief 获取响应的头部参数
             * @tparam T 转换类型
             * @param[in] key 关键字
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回对应的值,否则返回def
             */
            template<class T>
            T GetHeaderAs(const std::string& key, const T& def = T()) 
            {
                return GetAs(headers_, key, def);
            }

            /**
             * @brief 序列化输出到流
             * @param[in, out] os 输出流
             * @return 输出流
             */
            std::ostream& Dump(std::ostream& os) const;

            /**
             * @brief 转成字符串
             */
            std::string ToString() const;

            /**
             * @brief 设置重定向
             * @param[in] uri 重定向到的uri
             */
            void SetRedirect(const std::string& uri);

            /**
             * @brief 设置cookie
             * @param[in] key
             * @param[in] val
             * @param[in] expired
             * @param[in] path
             * @param[in] domain
             * @param[in] secure
             */
            void SetCookie(const std::string& key, const std::string& val,
                    time_t expired = 0, const std::string& path = "",
                    const std::string& domain = "", bool secure = false);
         private:
            /// 响应状态
            HttpStatus status_;
            /// 版本
            uint8_t version_;
            /// 是否自动关闭
            bool close_;
            /// 是否为websocket
            bool websocket_;
            /// 响应消息体
            std::string body_;
            /// 响应原因
            std::string reason_;
            /// 响应头部MAP
            MapType headers_;
            /// cookies
            std::vector<std::string> cookies_;
        };

        /**
         * @brief 流式输出HttpRequest
         * @param[in, out] os 输出流
         * @param[in] req HTTP请求
         * @return 输出流
         */
        std::ostream& operator<<(std::ostream& os, const HttpRequest& req);

        /**
         * @brief 流式输出HttpResponse
         * @param[in, out] os 输出流
         * @param[in] rsp HTTP响应
         * @return 输出流
         */
        std::ostream& operator<<(std::ostream& os, const HttpResponse& rsp);

         //-------------------------------------------------------------

    } // namespace http

    //-------------------------------------------------------------

} // namespace ygw 
#endif // __YGW_HTTP_HTTP_H__
