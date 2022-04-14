/**
 * @file http/servlet/config_servlet.h
 * @brief 配置servlet
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#ifndef __YGW_CONFIG_SERVLET_H__
#define __YGW_CONFIG_SERVLET_H__

#include "server_frame/http/servlet.h"

namespace ygw {

    //---------------------------------------------------------

    namespace http {

        class ConfigServlet : public Servlet 
        {
        public:
            ConfigServlet();
            virtual int32_t Handle(ygw::http::HttpRequest::ptr request
                    , ygw::http::HttpResponse::ptr response
                    , ygw::http::HttpSession::ptr session) override;
        };


    } // namespace http

    //---------------------------------------------------------

} // namespace ygw


#endif // __YGW_CONFIG_SERVLET_H__
