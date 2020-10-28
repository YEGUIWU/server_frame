/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/http/servlet/status_servlet.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-20
 *  Description: 
 * ====================================================
 */

#include "status_servlet.h"

namespace ygw {

    //-------------------------------------------------------------

    namespace http {

        StatusServlet::StatusServlet()
            :Servlet("StatusServlet") {
            }

        std::string FormatUsedTime(int64_t ts) 
        {
            std::stringstream ss;
            bool v = false;
            if (ts >= 3600 * 24) 
            {
                ss << (ts / 3600 / 24) << "d ";
                ts = ts % (3600 * 24);
                v = true;
            }
            if (ts >= 3600) 
            {
                ss << (ts / 3600) << "h ";
                ts = ts % 3600;
                v = true;
            } 
            else if (v) 
            {
                ss << "0h ";
            }

            if (ts >= 60) 
            {
                ss << (ts / 60) << "m ";
                ts = ts % 60;
            } 
            else if (v) 
            {
                ss << "0m ";
            }
            ss << ts << "s";
            return ss.str();
        }

        int32_t StatusServlet::Handle(ygw::http::HttpRequest::ptr request
                ,ygw::http::HttpResponse::ptr response
                ,ygw::http::HttpSession::ptr session) 
        {
            response->SetHeader("Content-Type", "text/text; charset=utf-8");
#define XX(key) \
            ss << std::setw(30) << std::right << key ": "
            std::stringstream ss;
            ss << "===================================================" << std::endl;
            XX("server_version") << "ygw/1.0.0" << std::endl;

            std::vector<Module::ptr> ms;
            ModuleMgr::GetInstance()->ListAll(ms);

            XX("modules");
            for (size_t i = 0; i < ms.size(); ++i) {
                if (i) {
                    ss << ";";
                }
                ss << ms[i]->GetId();
            }
            ss << std::endl;
            XX("host") << GetHostName() << std::endl;
            XX("ipv4") << GetIPv4() << std::endl;
            XX("daemon_id") << ProcessInfoMgr::GetInstance()->parent_id << std::endl;
            XX("main_id") << ProcessInfoMgr::GetInstance()->main_id << std::endl;
            XX("daemon_start") << ygw::util::TimeUtil::Time2Str(ProcessInfoMgr::GetInstance()->parent_start_time) << std::endl;
            XX("main_start") << ygw::util::TimeUtil::Time2Str(ProcessInfoMgr::GetInstance()->main_start_time) << std::endl;
            XX("restart_count") << ProcessInfoMgr::GetInstance()->restart_count << std::endl;
            XX("daemon_running_time") << FormatUsedTime(time(0) - ProcessInfoMgr::GetInstance()->parent_start_time) << std::endl;
            XX("main_running_time") << FormatUsedTime(time(0) - ProcessInfoMgr::GetInstance()->main_start_time) << std::endl;
            ss << "===================================================" << std::endl;
            XX("fibers") << ygw::Fiber::TotalFibers() << std::endl;
            ss << "===================================================" << std::endl;
            ss << "<Logger>" << std::endl;
            ss << ygw::LoggerMgr::GetInstance()->toYamlString() << std::endl;
            ss << "===================================================" << std::endl;
            ss << "<Woker>" << std::endl;
            ygw::WorkerMgr::GetInstance()->dump(ss) << std::endl;

            std::map<std::string, std::vector<TcpServer::ptr> > servers;
            ygw::Application::GetInstance()->ListAllServer(servers);
            ss << "===================================================" << std::endl;

            for (auto it = servers.begin();
                    it != servers.end(); ++it) 
            {
                if (it != servers.begin()) 
                {
                    ss << "***************************************************" << std::endl;
                }
                ss << "<Server." << it->first << ">" << std::endl;
                ygw::http::HttpServer::ptr hs;
                for (auto iit = it->second.begin();
                        iit != it->second.end(); ++iit) 
                {
                    if (iit != it->second.begin()) 
                    {
                        ss << "---------------------------------------------------" << std::endl;
                    }
                    if (!hs) 
                    {
                        hs = std::dynamic_pointer_cast<ygw::http::HttpServer>(*iit);
                    }
                    ss << (*iit)->ToString() << std::endl;
                }
                if (hs) 
                {
                    auto sd = hs->GetServletDispatch();
                    if (sd) 
                    {
                        std::map<std::string, IServletCreator::ptr> infos;
                        sd->ListAllServletCreator(infos);
                        if (!infos.empty()) 
                        {
                            ss << "[Servlets]" << std::endl;
#define XX2(key) \
                            ss << std::setw(30) << std::right << key << ": "
                            for (auto& i : infos) 
                            {
                                XX2(i.first) << i.second->GetName() << std::endl;
                            }
                            infos.clear();
                        } // end of if 3
                        sd->ListAllGlobServletCreator(infos);
                        if (!infos.empty()) 
                        {
                            ss << "[Servlets.Globs]" << std::endl;
                            for (auto& i : infos) 
                            {
                                XX2(i.first) << i.second->GetName() << std::endl;
                            }
                            infos.clear();
                        } // end of if 3 
                    } // end of if 2 
                } // end of if 1 
            } // end of for
            ss << "===================================================" << std::endl;
            for (size_t i = 0; i < ms.size(); ++i) {
                if (i) {
                    ss << "***************************************************" << std::endl;
                }
                ss << ms[i]->StatusString() << std::endl;
            }

            response->SetBody(ss.str());
            return 0;
        }


    } // namespace http

    //-------------------------------------------------------------

} // namespace ygw
