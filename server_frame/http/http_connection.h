/**
 * @file http_connection.h
 * @brief http连接
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */

#ifndef __YGW_HTTP_CONNECTION_H__
#define __YGW_HTTP_CONNECTION_H__

#include <list>

#include "http.h"
#include "uri.h"
#include "server_frame/base/thread.h"
#include "server_frame/stream/socket_stream.h"

namespace ygw {

    //-----------------------------------------------------------------------

    namespace http {

        /**
         * @brief HTTP响应结果
         */
        struct HttpResult {
            /// 智能指针类型定义
            using ptr = std::shared_ptr<HttpResult>;
            /**
             * @brief 错误码定义
             */
            enum class Error {
                /// 正常
                kOK                = 0,
                /// 非法URL
                kInvalidUrl        = 1,
                /// 无法解析HOST
                kInvalidHost       = 2,
                /// 连接失败
                kConnectFail       = 3,
                /// 连接被对端关闭
                kSendCloseByPeer   = 4,
                /// 发送请求产生Socket错误
                kSendSocketError   = 5,
                /// 超时
                kTimeout           = 6,
                /// 创建Socket失败
                kCreateSocketError = 7,
                /// 从连接池中取连接失败
                kPoolGetConnection = 8,
                /// 无效的连接
                kPoolInvalidConnection= 9,
            };

            /**
             * @brief 构造函数
             * @param[in] _result 错误码
             * @param[in] _response HTTP响应结构体
             * @param[in] _error 错误描述
             */
            HttpResult(int _result
                    ,HttpResponse::ptr _response
                    ,const std::string& _error)
                :result(_result)
                ,response(_response)
                ,error(_error) 
            {
            }

            /// 错误码
            int result;
            /// HTTP响应结构体
            HttpResponse::ptr response;
            /// 错误描述
            std::string error;

            std::string ToString() const;
        };

        class HttpConnectionPool;
        /**
         * @brief HTTP客户端类
         */
        class HttpConnection : public stream::SocketStream {
            friend class HttpConnectionPool;
            public:
            /// HTTP客户端类智能指针
            using ptr = std::shared_ptr<HttpConnection>;

            /**
             * @brief 发送HTTP的GET请求
             * @param[in] url 请求的url
             * @param[in] timeout_ms 超时时间(毫秒)
             * @param[in] headers HTTP请求头部参数
             * @param[in] body 请求消息体
             * @return 返回HTTP结果结构体
             */
            static HttpResult::ptr DoGet(const std::string& url
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

            /**
             * @brief 发送HTTP的GET请求
             * @param[in] uri URI结构体
             * @param[in] timeout_ms 超时时间(毫秒)
             * @param[in] headers HTTP请求头部参数
             * @param[in] body 请求消息体
             * @return 返回HTTP结果结构体
             */
            static HttpResult::ptr DoGet(Uri::ptr uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

            /**
             * @brief 发送HTTP的POST请求
             * @param[in] url 请求的url
             * @param[in] timeout_ms 超时时间(毫秒)
             * @param[in] headers HTTP请求头部参数
             * @param[in] body 请求消息体
             * @return 返回HTTP结果结构体
             */
            static HttpResult::ptr DoPost(const std::string& url
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

            /**
             * @brief 发送HTTP的POST请求
             * @param[in] uri URI结构体
             * @param[in] timeout_ms 超时时间(毫秒)
             * @param[in] headers HTTP请求头部参数
             * @param[in] body 请求消息体
             * @return 返回HTTP结果结构体
             */
            static HttpResult::ptr DoPost(Uri::ptr uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

            /**
             * @brief 发送HTTP请求
             * @param[in] method 请求类型
             * @param[in] uri 请求的url
             * @param[in] timeout_ms 超时时间(毫秒)
             * @param[in] headers HTTP请求头部参数
             * @param[in] body 请求消息体
             * @return 返回HTTP结果结构体
             */
            static HttpResult::ptr DoRequest(HttpMethod method
                    , const std::string& url
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

            /**
             * @brief 发送HTTP请求
             * @param[in] method 请求类型
             * @param[in] uri URI结构体
             * @param[in] timeout_ms 超时时间(毫秒)
             * @param[in] headers HTTP请求头部参数
             * @param[in] body 请求消息体
             * @return 返回HTTP结果结构体
             */
            static HttpResult::ptr DoRequest(HttpMethod method
                    , Uri::ptr uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

            /**
             * @brief 发送HTTP请求
             * @param[in] req 请求结构体
             * @param[in] uri URI结构体
             * @param[in] timeout_ms 超时时间(毫秒)
             * @return 返回HTTP结果结构体
             */
            static HttpResult::ptr DoRequest(HttpRequest::ptr req
                    , Uri::ptr uri
                    , uint64_t timeout_ms);

            /**
             * @brief 构造函数
             * @param[in] sock Socket类
             * @param[in] owner 是否掌握所有权
             */
            HttpConnection(socket::Socket::ptr sock, bool owner = true);

            /**
             * @brief 析构函数
             */
            ~HttpConnection();

            /**
             * @brief 接收HTTP响应
             */
            HttpResponse::ptr RecvResponse();

            /**
             * @brief 发送HTTP请求
             * @param[in] req HTTP请求结构
             */
            int SendRequest(HttpRequest::ptr req);

            private:
            uint64_t create_time_ = 0;
            uint64_t request_ = 0;
        };

        class HttpConnectionPool {
        public:
            using ptr = std::shared_ptr<HttpConnectionPool>;
            using MutexType = ygw::thread::Mutex;

            static HttpConnectionPool::ptr Create(const std::string& uri
                    ,const std::string& vhost
                    ,uint32_t max_size
                    ,uint32_t max_alive_time
                    ,uint32_t max_request);

            HttpConnectionPool(const std::string& host
                    ,const std::string& vhost
                    ,uint32_t port
                    ,bool is_https
                    ,uint32_t max_size
                    ,uint32_t max_alive_time
                    ,uint32_t max_request);

            HttpConnection::ptr GetConnection();


            /**
             * @brief 发送HTTP的GET请求
             * @param[in] url 请求的url
             * @param[in] timeout_ms 超时时间(毫秒)
             * @param[in] headers HTTP请求头部参数
             * @param[in] body 请求消息体
             * @return 返回HTTP结果结构体
             */
            HttpResult::ptr DoGet(const std::string& url
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

            /**
             * @brief 发送HTTP的GET请求
             * @param[in] uri URI结构体
             * @param[in] timeout_ms 超时时间(毫秒)
             * @param[in] headers HTTP请求头部参数
             * @param[in] body 请求消息体
             * @return 返回HTTP结果结构体
             */
            HttpResult::ptr DoGet(Uri::ptr uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

            /**
             * @brief 发送HTTP的POST请求
             * @param[in] url 请求的url
             * @param[in] timeout_ms 超时时间(毫秒)
             * @param[in] headers HTTP请求头部参数
             * @param[in] body 请求消息体
             * @return 返回HTTP结果结构体
             */
            HttpResult::ptr DoPost(const std::string& url
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

            /**
             * @brief 发送HTTP的POST请求
             * @param[in] uri URI结构体
             * @param[in] timeout_ms 超时时间(毫秒)
             * @param[in] headers HTTP请求头部参数
             * @param[in] body 请求消息体
             * @return 返回HTTP结果结构体
             */
            HttpResult::ptr DoPost(Uri::ptr uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

            /**
             * @brief 发送HTTP请求
             * @param[in] method 请求类型
             * @param[in] uri 请求的url
             * @param[in] timeout_ms 超时时间(毫秒)
             * @param[in] headers HTTP请求头部参数
             * @param[in] body 请求消息体
             * @return 返回HTTP结果结构体
             */
            HttpResult::ptr DoRequest(HttpMethod method
                    , const std::string& url
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

            /**
             * @brief 发送HTTP请求
             * @param[in] method 请求类型
             * @param[in] uri URI结构体
             * @param[in] timeout_ms 超时时间(毫秒)
             * @param[in] headers HTTP请求头部参数
             * @param[in] body 请求消息体
             * @return 返回HTTP结果结构体
             */
            HttpResult::ptr DoRequest(HttpMethod method
                    , Uri::ptr uri
                    , uint64_t timeout_ms
                    , const std::map<std::string, std::string>& headers = {}
                    , const std::string& body = "");

            /**
             * @brief 发送HTTP请求
             * @param[in] req 请求结构体
             * @param[in] timeout_ms 超时时间(毫秒)
             * @return 返回HTTP结果结构体
             */
            HttpResult::ptr DoRequest(HttpRequest::ptr req
                    , uint64_t timeout_ms);
        private:
            static void ReleasePtr(HttpConnection* ptr, HttpConnectionPool* pool);
        private:
            std::string host_;
            std::string vhost_;
            uint32_t port_;
            uint32_t max_size_;
            uint32_t max_alive_time_;
            uint32_t max_request_;
            bool is_https_;

            MutexType mutex_;
            std::list<HttpConnection*> conns_;
            std::atomic<int32_t> total_ = {0};
        }; 

        //-----------------------------------------------------------------------

    } // namespace http

    //-----------------------------------------------------------------------

} // namespace ygw


#endif // __YGW_HTTP_CONNECTION_H__
