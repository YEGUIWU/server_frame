/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: http/servlet.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-20
 *  Description: 
 * ====================================================
 */
#include "servlet.h"
#include <fnmatch.h>

namespace ygw {

    //-----------------------------------------------------

    namespace http {

        FunctionServlet::FunctionServlet(callback cb)
            :Servlet("FunctionServlet")
            ,cb_(cb) 
        {
        }

        int32_t FunctionServlet::Handle(ygw::http::HttpRequest::ptr request
                , ygw::http::HttpResponse::ptr response
                , ygw::http::HttpSession::ptr session) 
        {
            return cb_(request, response, session);
        }



        ServletDispatch::ServletDispatch()
            :Servlet("ServletDispatch") 
        {
            default_.reset(new NotFoundServlet("ygw/1.0"));
        }

        int32_t ServletDispatch::Handle(ygw::http::HttpRequest::ptr request
                , ygw::http::HttpResponse::ptr response
                , ygw::http::HttpSession::ptr session) 
        {
            auto slt = GetMatchedServlet(request->GetPath());
            if (slt) 
            {
                slt->Handle(request, response, session);
            }
            return 0;
        }

        void ServletDispatch::AddServlet(const std::string& uri, Servlet::ptr slt) 
        {
            RWMutexType::WriteLock lock(mutex_);
            datas_[uri] = std::make_shared<HoldServletCreator>(slt);
        }

        void ServletDispatch::AddServletCreator(const std::string& uri, IServletCreator::ptr creator) 
        {
            RWMutexType::WriteLock lock(mutex_);
            datas_[uri] = creator;
        }

        void ServletDispatch::AddGlobServletCreator(const std::string& uri, IServletCreator::ptr creator) 
        {
            RWMutexType::WriteLock lock(mutex_);
            for (auto it = globs_.begin();
                    it != globs_.end(); ++it) 
            {
                if (it->first == uri) 
                {
                    globs_.erase(it);
                    break;
                }
            }
            globs_.push_back(std::make_pair(uri, creator));
        }

        void ServletDispatch::AddServlet(const std::string& uri
                ,FunctionServlet::callback cb) 
        {
            RWMutexType::WriteLock lock(mutex_);
            datas_[uri] = std::make_shared<HoldServletCreator>(
                    std::make_shared<FunctionServlet>(cb));
        }

        void ServletDispatch::AddGlobServlet(const std::string& uri
                ,Servlet::ptr slt) 
        {
            RWMutexType::WriteLock lock(mutex_);
            for (auto it = globs_.begin();
                    it != globs_.end(); ++it) 
            {
                if (it->first == uri)
                {
                    globs_.erase(it);
                    break;
                }
            }
            globs_.push_back(std::make_pair(uri
                        , std::make_shared<HoldServletCreator>(slt)));
        }

        void ServletDispatch::AddGlobServlet(const std::string& uri
                ,FunctionServlet::callback cb) 
        {
            return AddGlobServlet(uri, std::make_shared<FunctionServlet>(cb));
        }

        void ServletDispatch::DelServlet(const std::string& uri) 
        {
            RWMutexType::WriteLock lock(mutex_);
            datas_.erase(uri);
        }

        void ServletDispatch::DelGlobServlet(const std::string& uri) 
        {
            RWMutexType::WriteLock lock(mutex_);
            for (auto it = globs_.begin();
                    it != globs_.end(); ++it)
            {
                if (it->first == uri) 
                {
                    globs_.erase(it);
                    break;
                }
            }
        }

        Servlet::ptr ServletDispatch::GetServlet(const std::string& uri) 
        {
            RWMutexType::ReadLock lock(mutex_);
            auto it = datas_.find(uri);
            return it == datas_.end() ? nullptr : it->second->Get();
        }

        Servlet::ptr ServletDispatch::GetGlobServlet(const std::string& uri) 
        {
            RWMutexType::ReadLock lock(mutex_);
            for (auto it = globs_.begin();
                    it != globs_.end(); ++it) 
            {
                if (it->first == uri) 
                {
                    return it->second->Get();
                }
            }
            return nullptr;
        }

        Servlet::ptr ServletDispatch::GetMatchedServlet(const std::string& uri) 
        {
            RWMutexType::ReadLock lock(mutex_);
            auto mit = datas_.find(uri);
            if (mit != datas_.end()) 
            {
                return mit->second->Get();
            }
            for (auto it = globs_.begin();
                    it != globs_.end(); ++it) 
            {
                if (!fnmatch(it->first.c_str(), uri.c_str(), 0)) 
                {
                    return it->second->Get();
                }
            }
            return default_;
        }

        void ServletDispatch::ListAllServletCreator(std::map<std::string, IServletCreator::ptr>& infos) 
        {
            RWMutexType::ReadLock lock(mutex_);
            for (auto& i : datas_) 
            {
                infos[i.first] = i.second;
            }
        }

        void ServletDispatch::ListAllGlobServletCreator(std::map<std::string, IServletCreator::ptr>& infos) 
        {
            RWMutexType::ReadLock lock(mutex_);
            for (auto& i : globs_) 
            {
                infos[i.first] = i.second;
            }
        }

        NotFoundServlet::NotFoundServlet(const std::string& name)
            :Servlet("NotFoundServlet")
            ,name_(name)
        {
            content_ = "<html><head><title>404 Not Found"
                "</title></head><body><center><h1>404 Not Found</h1></center>"
                "<hr><center>" + name + "</center></body></html>";

        }

        int32_t NotFoundServlet::Handle(ygw::http::HttpRequest::ptr request
                , ygw::http::HttpResponse::ptr response
                , ygw::http::HttpSession::ptr session) 
        {
            response->SetStatus(ygw::http::HttpStatus::NOT_FOUND);
            response->SetHeader("Server", "ygw/1.0.0");
            response->SetHeader("Content-Type", "text/html");
            response->SetBody(content_);
            return 0;
        }

    } // namespace http

    //-----------------------------------------------------

} // namespace ygw
