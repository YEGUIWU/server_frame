/**
 * @file http_session.h
 * @brief http会话
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#ifndef __YGW_HTTP_SESSION_H__
#define __YGW_HTTP_SESSION_H__

#include "http.h"
#include "server_frame/stream/socket_stream.h"

namespace ygw {

    //-------------------------------------------------------------------------

    namespace http {

        class HttpSession : public stream::SocketStream {
        public:
            /// 智能指针类型定义
            using ptr = std::shared_ptr<HttpSession>;

            /**
             * @brief 构造函数
             * @param[in] sock Socket类型
             * @param[in] owner 是否托管
             */
            HttpSession(socket::Socket::ptr sock, bool owner = true);

            /**
             * @brief 接收HTTP请求
             */
            HttpRequest::ptr RecvRequest();

            /**
             * @brief 发送HTTP响应
             * @param[in] rsp HTTP响应
             * @return >0 发送成功
             *         =0 对方关闭
             *         <0 Socket异常
             */
            int SendResponse(HttpResponse::ptr rsp);

        }; // class HttpSession

    } // namespace http

    //-------------------------------------------------------------------------

} // namespace ygw

#endif // __YGW_HTTP_SESSION_H__
