/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/http/servlet/config_servlet.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-20
 *  Description: 
 * ====================================================
 */
#include "config_servlet.h"
#include "server_frame/config.h"

namespace  ygw {

    //----------------------------------------------------

    namespace http {

        ConfigServlet::ConfigServlet()
            :Servlet("ConfigServlet") 
        {
        }

        int32_t ConfigServlet::Handle(
                ygw::http::HttpRequest::ptr request
                ,ygw::http::HttpResponse::ptr response
                ,ygw::http::HttpSession::ptr session) 
        {
            std::string type = request->GetParam("type");
            if (type == "json") 
            {
                response->SetHeader("Content-Type", "text/json charset=utf-8");
            } 
            else 
            {
                response->SetHeader("Content-Type", "text/yaml charset=utf-8");
            }
            YAML::Node node;
            ygw::config::Config::Visit([&node](ygw::config::ConfigVarBase::ptr base) {
                YAML::Node n;
                try 
                {
                    n = YAML::Load(base->ToString());
                } 
                catch(...) 
                {
                    return;
                }
                node[base->GetName()] = n;
                node[base->GetName() + "$description"] = base->GetDescription();
            });
            if (type == "json") 
            {
                Json::Value jvalue;
                if (YamlToJson(node, jvalue)) 
                {
                    response->SetBody(JsonUtil::ToString(jvalue));
                    return 0;
                }
            }
            std::stringstream ss;
            ss << node;
            response->SetBody(ss.str());
            return 0;
        }

    } // namespace http

    //----------------------------------------------------

} // namespace ygw
