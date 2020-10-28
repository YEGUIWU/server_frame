/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/http/http_session.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-20
 *  Description: 
 * ====================================================
 */
#include "http_parser.h"
#include "http_session.h"

namespace ygw {

    namespace http {

		HttpSession::HttpSession(socket::Socket::ptr sock, bool owner)
			:SocketStream(sock, owner) 
        {
        }

		HttpRequest::ptr HttpSession::RecvRequest() 
        {
			HttpRequestParser::ptr parser(new HttpRequestParser); //使用HttpParser解析
			uint64_t buff_size = HttpRequestParser::GetHttpRequestBufferSize(); // 获取缓冲区大小
            std::shared_ptr<char[]> buffer(new char[buff_size]); // 创建缓存
            // 缓冲区已经塞满，还没有解析完：恶意，有问题的请求 
			char* data = buffer.get();
			int offset = 0;
			do {
				int len = Read(data + offset, buff_size - offset);
				if (len <= 0)  //发生错误 读不到
                {
					Close();
					return nullptr;
				}
				len += offset;
				size_t nparse = parser->Execute(data, len);
				if (parser->HasError()) // 有错误
                {
					Close();
					return nullptr;
				}
				offset = len - nparse;
				if (offset == (int)buff_size)  //out of range
                {
					Close();
					return nullptr;
				}
				if (parser->IsFinished())  //结束
                {
					break;
				}
			} while(true);

			int64_t length = parser->GetContentLength();
			if (length > 0) // 有内容 
            {
				std::string body;
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
				parser->GetData()->SetBody(body);
			}

			parser->GetData()->Init();
            
			return parser->GetData();
		}

		int HttpSession::SendResponse(HttpResponse::ptr rsp) 
        {
			std::stringstream ss;
			ss << *rsp;
			std::string data = ss.str();
			return WriteFixSize(data.c_str(), data.size());
		}
    } // namespace http

} // namespace ygw
