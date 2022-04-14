/**
 * @file http_parser.h
 * @brief http协议解析器
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */

#ifndef __YGW_HTTP_PARSER_H__
#define __YGW_HTTP_PARSER_H__

#include "http.h"
#include "parser/http11_parser.h"
#include "parser/httpclient_parser.h"

namespace ygw { 


    //--------------------------------------------------------------------------
    namespace http {


        /**-------------------------------------------------------
         * @brief HTTP请求解析类
         */
        class HttpRequestParser {
        public:
            /// HTTP解析类的智能指针
            using ptr = std::shared_ptr<HttpRequestParser>;

            /**
             * @brief 构造函数
             */
            HttpRequestParser();

            /**
             * @brief 解析协议
             * @param[in, out] data 协议文本内存
             * @param[in] len 协议文本内存长度
             * @return 返回实际解析的长度,并且将已解析的数据移除
             */
            size_t Execute(char* data, size_t len);

            /**
             * @brief 是否解析完成
             * @return 是否解析完成
             */
            int IsFinished();

            /**
             * @brief 是否有错误
             * @return 是否有错误
             */
            int HasError(); 

            /**
             * @brief 设置错误
             * @param[in] v 错误值
             */
            void SetError(int v) { error_ = v;}

            /**
             * @brief 获取消息体长度
             */
            uint64_t GetContentLength();

            /**
             * @brief 返回HttpRequest结构体
             */
            HttpRequest::ptr GetData() const { return data_;}

            /**
             * @brief 获取HttpParser结构体
             */
            const HttpParser& GetParser() const { return parser_;}


        public:
            /**
             * @brief 返回HttpRequest协议解析的缓存大小
             */
            static uint64_t GetHttpRequestBufferSize();

            /**
             * @brief 返回HttpRequest协议的最大消息体大小
             */
            static uint64_t GetHttpRequestMaxBodySize();
        private:
            /// HttpParser 
            HttpParser parser_;
            /// HttpRequest结构
            HttpRequest::ptr data_;
            /// 错误码
            /// 1000: invalid method
            /// 1001: invalid version
            /// 1002: invalid field
            int error_;
        };

        /**------------------------------------------------------------------
         * @brief Http响应解析结构体
         */
        class HttpResponseParser {
        public:
            /// 智能指针类型
            using ptr = std::shared_ptr<HttpResponseParser>;

            /**
             * @brief 构造函数
             */
            HttpResponseParser();

            /**
             * @brief 解析HTTP响应协议
             * @param[in, out] data 协议数据内存
             * @param[in] len 协议数据内存大小
             * @param[in] chunck 是否在解析chunck
             * @return 返回实际解析的长度,并且移除已解析的数据
             */
            size_t Execute(char* data, size_t len, bool chunck);

            /**
             * @brief 是否解析完成
             */
            int IsFinished();

            /**
             * @brief 是否有错误
             */
            int HasError(); 

            /**
             * @brief 返回HttpResponse
             */
            HttpResponse::ptr GetData() const { return data_;}

            /**
             * @brief 设置错误码
             * @param[in] v 错误码
             */
            void SetError(int v) { error_ = v; }

            /**
             * @brief 获取消息体长度
             */
            uint64_t GetContentLength();

            /**
             * @brief 返回HttpClientParser
             */
            const HttpClientParser& GetParser() const { return parser_;}
        public:
            /**
             * @brief 返回HTTP响应解析缓存大小
             */
            static uint64_t GetHttpResponseBufferSize();

            /**
             * @brief 返回HTTP响应最大消息体大小
             */
            static uint64_t GetHttpResponseMaxBodySize();
        private:
            /// HttpClientParser 
            HttpClientParser parser_;
            /// HttpResponse
            HttpResponse::ptr data_;
            /// 错误码
            /// 1001: invalid version
            /// 1002: invalid field
            int error_;
        }; // class HttpResponseParser

        //--------------------------------------------------------------------------

    } // namespace http 

    //--------------------------------------------------------------------------

} // namespace ygw 


#endif // __YGW_HTTP_PARSER_H__
