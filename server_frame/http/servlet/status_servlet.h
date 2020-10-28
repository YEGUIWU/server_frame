/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: status_servlet.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-20
 *  Description: 
 * ====================================================
 */

#ifndef __YGW_STATUS_SERVLET_H__
#define __YGW_STATUS_SERVLET_H__

#include "server_frame/http/servlet.h"

namespace ygw {

    //--------------------------------------------------------------

    namespace http {

        class StatusServlet : public Servlet {
        public:
            StatusServlet();
            virtual int32_t Handle(ygw::http::HttpRequest::ptr request
                    , ygw::http::HttpResponse::ptr response
                    , ygw::http::HttpSession::ptr session) override;
        };


    } // namespace http

    //--------------------------------------------------------------

} // namespace ygw


#endif // __YGW_STATUS_SERVLET_H__
