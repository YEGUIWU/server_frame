/**
 * @file server_frame/http/http_server.cc
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-09-26
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.top
 */

#include <server_frame/log.h>
#include <server_frame/config.h>
#include <server_frame/sys/env.h>

#include <sys/stat.h>
#include <dirent.h>

#include "http_server.h"
//#include "server_frame/http/servlet/config_servlet.h"
//#include "server_frame/http/servlet/status_servlet.h"

namespace ygw {

    //-----------------------------------------------------------------------------

    namespace http {

        //-----------------------------------------------------------------------------
        static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

        HttpServer::HttpServer(bool keepalive
                ,ygw::scheduler::IOManager* io_worker
                ,ygw::scheduler::IOManager* accept_worker)
            :TcpServer(io_worker, accept_worker)
            ,is_keepalive_(keepalive) 
            ,root_path_(ygw::sys::EnvManager::GetInstance()->GetAbsolutePath(""))
        {
                 dispatch_.reset(new ServletDispatch);
                 type_ = "http";
                 //dispatch_->AddServlet("/_/status", Servlet::ptr(new StatusServlet));
                 //dispatch_->AddServlet("/_/config", Servlet::ptr(new ConfigServlet));
        }

        void HttpServer::SetName(const std::string& v) 
        {
            TcpServer::SetName(v);
            dispatch_->SetDefault(std::make_shared<NotFoundServlet>(v));
        }

        void HttpServer::HandleClient(socket::Socket::ptr client) 
        {
            YGW_LOG_DEBUG(g_logger) << "HandleClient " << *client;
            HttpSession::ptr session(new HttpSession(client));
            do {
                auto req = session->RecvRequest(); // 接受请求
                if (!req) 
                {
                    YGW_LOG_DEBUG(g_logger) << "recv http request fail, errno="
                        << errno << " errstr=" << strerror(errno)
                        << " cliet:" << *client << " keep_alive=" << is_keepalive_;
                    break;
                }
                //YGW_LOG_DEBUG(g_logger) << "recv http request successful, "
                //    << " cliet:" << *client 
                //    << " is_close=" << req->IsClose()
                //    << " keep_alive=" << is_keepalive_;

                HttpResponse::ptr rsp(new HttpResponse(req->GetVersion()
                            ,req->IsClose() || !is_keepalive_));

                rsp->SetHeader("Server", GetName());
                if (!SendDocument(req, rsp, session))
                {
                    dispatch_->Handle(req, rsp, session);
                }
                
                session->SendResponse(rsp);

                if (!is_keepalive_ || req->IsClose()) 
                {
                    break;
                }
            } while(true);
            session->Close();
        }


        bool HttpServer::SendDocument(HttpRequest::ptr request, HttpResponse::ptr response, HttpSession::ptr session)
        {
            //std::string cur_path = root_path_;// FSUtil::GetCurDir();
            std::string cur_path = root_path_;
            std::string path = request->GetPath();
            std::string obj_path = cur_path + path;
            if (strstr(obj_path.data(), ".."))
                return false;

            struct stat st;


            if (stat(obj_path.data(), &st) < 0) {
                return false;
            }
            bool is_file_path = (S_IFREG == (S_IFREG & st.st_mode));
            bool is_dir_path = (S_IFDIR == (S_IFDIR & st.st_mode));

            // index.html
            //std::cout << cur_path << std::endl;
            //std::cout << obj_path << std::endl;
            if (is_dir_path)
            {
                std::string index_path = obj_path + "/index.html";
                //if (!access(index_path.data(), F_OK))
                int ret = stat(index_path.data(), &st); 
                if (!ret)
                {
                    if (S_IFREG == (S_IFREG & st.st_mode))
                    {
                        is_file_path = true;
                        is_dir_path = false;
                        obj_path = index_path;
                    }
                }
                else
                {
                    std::cout << strerror(errno) << std::endl;
                }
            }

            if (is_dir_path)
            {
                std::string body;
                DIR* d;
                struct dirent* ent;

                const char* trailing_slash = "";

                if (!strlen(path.data()) || path[strlen(path.data()) - 1] != '/')
                    trailing_slash = "/";

                if (!(d = opendir(obj_path.data())))
                    return false;

                body = ygw::util::StringUtil::Format("<!DOCTYPE html>\n"
                        "<html>\n <head>\n"
                        "  <meta charset='utf-8'>\n"
                        "  <title>%s</title>\n"
                        "  <base href='%s%s'>\n"
                        " </head>\n"
                        " <body>\n"
                        "  <h1>%s</h1>\n"
                        "  <ul>\n",
                        path.data(), /* XXX html-escape this. */
                        path.data(), /* XXX html-escape this? */
                        trailing_slash,
                        path.data() /* XXX html-escape this */);

                while ((ent = readdir(d))) {
                    const char* name = ent->d_name;

                    body += ygw::util::StringUtil::Format("    <li><a href=\"%s\">%s</a>\n",
                            name, name);

                }
                body += "</ul></body></html>\n";
                closedir(d);
                response->SetHeader("Content-Type", "text/html");
                response->SetBody(body);
                return true;
            }
            else if (is_file_path)
            {
                const char* type = ygw::util::StringUtil::GuessContentType(obj_path);
                //if (st.st_size > 256 * 1024 * 1024 && !strcmp(type, "application/misc")) // >8m and unknow type
                if (st.st_size > 256 * 1024 * 1024) // >8m and unknow type
                { 
                    // too big
                    // TODO transfer big file
                    YGW_LOG_DEBUG(g_logger) << "SendDocument: transfer too large file";
                    return false;
                }
                else
                {
                    auto fp = ygw::util::FSUtil::OpenFile(obj_path, "rb");
                    int src_size = ygw::util::FSUtil::GetFileSize(fp.get());
                    int size = src_size;
                    std::string body;
                    body.resize(size);
                    constexpr int per_size = 4096;
                    int offset;
                    for (offset = 0; size > per_size; offset += per_size, size -= per_size)
                    {
                        if (fread(&body[0] + offset, 1, per_size, fp.get()) != per_size)
                        {
                            YGW_LOG_ERROR(g_logger) << "fread error";
                            return false;
                        }
                    }
                    if (fread(&body[0] + offset, 1, size, fp.get()) != (size_t)size)
                    {
                        YGW_LOG_ERROR(g_logger) << "fread error";
                        return false;
                    }
                    response->SetBody(body);
                    response->SetHeader("Content-Type", type);
                    return true;
                }

            }
            else
            {
                return false;
            }
        }

        //-----------------------------------------------------------------------------

    } // namespace http

    //-----------------------------------------------------------------------------
    namespace config {
        
        template<>
        class LexicalCast<std::string, ygw::http::HttpServerConfig>{
        public:
            ygw::http::HttpServerConfig operator()(const std::string& yaml_str)
            {
                YAML::Node node = YAML::Load(yaml_str);
                ygw::http::HttpServerConfig cfg;
                cfg.SetName(node["name"].as<std::string>());
                cfg.SetRoot(node["root"].as<std::string>());
                return cfg;
            }
        };
        template<>
        class LexicalCast<ygw::http::HttpServerConfig, std::string> {
        public:
            std::string operator()(const ygw::http::HttpServerConfig& cfg)
            {
                YAML::Node node;
                node["name"] = cfg.GetName();
                node["root"] = cfg.GetRoot();
                std::stringstream ss;
                ss << node;
                return ss.str();
            }
        };

        ConfigVar<ygw::http::HttpServerConfig>::ptr g_http_server_config = 
            Config::Lookup("http_server.default", ygw::http::HttpServerConfig(), "http server default config");
    }


    //-----------------------------------------------------------------------------

} // namespace ygw
