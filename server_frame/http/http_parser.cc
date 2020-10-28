/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: http/http_parser.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-18
 *  Description: 
 * ====================================================
 */

#include <cstring>
#include <cstdlib>

#include "http_parser.h"
#include "server_frame/log.h"
#include "server_frame/config.h"

namespace ygw {

    //--------------------------------------------------------------

    namespace http {

        static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system"); // 
        // 请求缓存大小 4kb
        static ygw::config::ConfigVar<uint64_t>::ptr g_http_request_buffer_size = 
            ygw::config::Config::Lookup("http.request.buffer_size", 
                    (uint64_t)(4 * 1024), "http request buffer size");
        // 请求消息体 64mb
        static ygw::config::ConfigVar<uint64_t>::ptr g_http_request_max_body_size = 
            ygw::config::Config::Lookup("http.request.max_body_size",
                    (uint64_t)(64 * 1024 * 1024), "http request max body size");
        // 响应缓存大小 4kb
        static ygw::config::ConfigVar<uint64_t>::ptr g_http_response_buffer_size = 
            ygw::config::Config::Lookup("http.response.buffer_size",
                    (uint64_t)(4 * 1024), "http response max body size");
        // 响应消息体 64mb
        static ygw::config::ConfigVar<uint64_t>::ptr g_http_response_max_body_size = 
            ygw::config::Config::Lookup("http.response.buffer.size",
                    (uint64_t)(64 * 1024 * 1024), "http response max body size");


        static uint64_t s_http_request_buffer_size = 0;
        static uint64_t s_http_request_max_body_size = 0;
        static uint64_t s_http_response_buffer_size = 0;
        static uint64_t s_http_response_max_body_size = 0;

        uint64_t HttpRequestParser::GetHttpRequestBufferSize() 
        {
                return s_http_request_buffer_size;
        }

        uint64_t HttpRequestParser::GetHttpRequestMaxBodySize() 
        {
                return s_http_request_max_body_size;
        }

        uint64_t HttpResponseParser::GetHttpResponseBufferSize() 
        {
                return s_http_response_buffer_size;
        }

        uint64_t HttpResponseParser::GetHttpResponseMaxBodySize() 
        {
                return s_http_response_max_body_size;
        }

        namespace 
        {
            struct _RequestSizeIniter 
            {
                _RequestSizeIniter()
                {
                    // 
                    s_http_request_buffer_size = g_http_request_buffer_size->GetValue();
                    s_http_request_max_body_size = g_http_request_max_body_size->GetValue();
                    s_http_response_buffer_size = g_http_response_buffer_size->GetValue();
                    s_http_response_max_body_size = g_http_response_max_body_size->GetValue();

                    // 添加监听器
                    g_http_request_buffer_size->AddListener(
                            [](const uint64_t& ov, const uint64_t& nv){
                            s_http_request_buffer_size = nv;
                    });

                    g_http_request_max_body_size->AddListener(
                            [](const uint64_t& ov, const uint64_t& nv){
                            s_http_request_max_body_size = nv;
                    });

                    g_http_response_buffer_size->AddListener(
                            [](const uint64_t& ov, const uint64_t& nv){
                            s_http_response_buffer_size = nv;
                    });

                    g_http_response_max_body_size->AddListener(
                            [](const uint64_t& ov, const uint64_t& nv){
                            s_http_response_max_body_size = nv;
                    });
                }
            };
            static _RequestSizeIniter _init;
        }

        

        //---------------------------------------------------------------------------------------
        //                  HttpRequestParser
        //---------------------------------------------------------------------------------------
        void OnRequestMethod(void *data, const char *at, size_t length) 
        {
            HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
            HttpMethod m = CharsToHttpMethod(at);

            if (m == HttpMethod::INVALID_METHOD) 
            {
                YGW_LOG_WARN(g_logger) << "invalid http request method: "
                    << std::string(at, length);
                parser->SetError(1000);
                return;
            }
            parser->GetData()->SetMethod(m);
        }

        void OnRequestUri(void *data, const char *at, size_t length) 
        {
        }

        void OnRequestFragment(void *data, const char *at, size_t length) 
        {
            //YGW_LOG_INFO(g_logger) << "on_request_fragment:" << std::string(at, length);
            HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
            parser->GetData()->SetFragment(std::string(at, length));
        }

        void OnRequestPath(void *data, const char *at, size_t length) 
        {
            HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
            parser->GetData()->SetPath(std::string(at, length));
        }

        void OnRequestQuery(void *data, const char *at, size_t length)
        {
            HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
            parser->GetData()->SetQuery(std::string(at, length));
        }

        void OnRequestVersion(void *data, const char *at, size_t length) 
        {
            HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
            uint8_t v = 0;
            if (strncmp(at, "HTTP/1.1", length) == 0) 
            {
                v = 0x11;
            } 
            else if (strncmp(at, "HTTP/1.0", length) == 0) 
            {
                v = 0x10;
            } 
            else 
            {
                YGW_LOG_WARN(g_logger) << "invalid http request version: "
                    << std::string(at, length);
                parser->SetError(1001);
                return;
            }
            parser->GetData()->SetVersion(v);
        }

        void OnRequestHeaderDone(void *data, const char *at, size_t length) 
        {
            //HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
        }

        void OnRequestHttpField(void *data, const char *field, size_t flen
                ,const char *value, size_t vlen) 
        {
            HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
            if (flen == 0) 
            {
                YGW_LOG_WARN(g_logger) << "invalid http request field length == 0";
                // parser->SetError(1002);
                // 在此处并不设置错误，我们并不会希望在解析head时遇到某个字段解析错误后，直接返回不再解析
                // 在此处 当作错误比不当作错误，产生的问题会严重一点。
                return;
            }
            parser->GetData()->SetHeader(std::string(field, flen)
                    ,std::string(value, vlen));
        }

        HttpRequestParser::HttpRequestParser()
            : error_(0)
        {
            data_.reset(new ygw::http::HttpRequest);
            http_parser_init(&parser_);
            parser_.request_method = OnRequestMethod;
            parser_.request_uri = OnRequestUri;
            parser_.fragment = OnRequestFragment;
            parser_.request_path = OnRequestPath;
            parser_.query_string = OnRequestQuery;
            parser_.http_version = OnRequestVersion;
            parser_.header_done = OnRequestHeaderDone;
            parser_.http_field = OnRequestHttpField;
            parser_.data = this;
        }

        uint64_t HttpRequestParser::GetContentLength() 
        {
            return data_->GetHeaderAs<uint64_t>("content-length", 0);
        }

        //1: 成功
        //-1: 有错误
        //>0: 已处理的字节数，且data有效数据为len - v;
        size_t HttpRequestParser::Execute(char* data, size_t len) 
        {
            size_t offset = http_parser_execute(&parser_, data, len, 0);
            memmove(data, data + offset, (len - offset));
            return offset;
        }

        int HttpRequestParser::IsFinished()
        {
            return http_parser_finish(&parser_);
        }

        int HttpRequestParser::HasError() 
        {
            return error_ || http_parser_has_error(&parser_);
        }


        //---------------------------------------------------------------------------------------
        //                  HttpResponseParser
        //---------------------------------------------------------------------------------------


        void OnResponseReason(void *data, const char *at, size_t length) 
        {
            HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
            parser->GetData()->SetReason(std::string(at, length));
        }

        void OnResponseStatus(void *data, const char *at, size_t length) 
        {
            HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
            HttpStatus status = (HttpStatus)(atoi(at));
            parser->GetData()->SetStatus(status);
        }

        void OnResponseChunk(void *data, const char *at, size_t length) {
        }

        void OnResponseVersion(void *data, const char *at, size_t length) 
        {
            HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
            uint8_t v = 0;
            if (strncmp(at, "HTTP/1.1", length) == 0) 
            {
                v = 0x11;
            } 
            else if (strncmp(at, "HTTP/1.0", length) == 0) 
            {
                v = 0x10;
            } 
            else 
            {
                YGW_LOG_WARN(g_logger) << "invalid http response version: "
                    << std::string(at, length);
                parser->SetError(1001);
                return;
            }

            parser->GetData()->SetVersion(v);
        }

        void OnResponseHeaderDone(void *data, const char *at, size_t length) 
        {
        }

        void OnResponseLastChunk(void *data, const char *at, size_t length) 
        {
        }

        void OnResponseHttpField(void *data, const char *field, size_t flen
                                ,const char *value, size_t vlen) 
        {
            HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
            if (flen == 0)
            {
                YGW_LOG_WARN(g_logger) << "invalid http response field length == 0";
                //parser->SetError(1002);
                return;
            }
            parser->GetData()->SetHeader(std::string(field, flen)
                    ,std::string(value, vlen));
        }

        HttpResponseParser::HttpResponseParser()
            :error_(0) 
        {
            data_.reset(new ygw::http::HttpResponse);
            httpclient_parser_init(&parser_);
            parser_.reason_phrase = OnResponseReason;
            parser_.status_code = OnResponseStatus;
            parser_.chunk_size = OnResponseChunk;
            parser_.http_version = OnResponseVersion;
            parser_.header_done = OnResponseHeaderDone;
            parser_.last_chunk = OnResponseLastChunk;
            parser_.http_field = OnResponseHttpField;
            parser_.data = this;
        }

        size_t HttpResponseParser::Execute(char* data, size_t len, bool chunck) 
        {
            if (chunck) 
            {
                httpclient_parser_init(&parser_);
            }
            size_t offset = httpclient_parser_execute(&parser_, data, len, 0);
            memmove(data, data + offset, (len - offset));
            return offset;
        }

        int HttpResponseParser::IsFinished() 
        {
            return httpclient_parser_finish(&parser_);
        }

        int HttpResponseParser::HasError() 
        {
            return error_ || httpclient_parser_has_error(&parser_);
        }

        uint64_t HttpResponseParser::GetContentLength() 
        {
            return data_->GetHeaderAs<uint64_t>("content-length", 0);
        }

    } // namespace http

    //--------------------------------------------------------------

} // namespace ygw 
