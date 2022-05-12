/**
 * @file http_server.h
 * @brief http服务器
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#ifndef __YGE_HTTP_SERVER_H__
#define __YGE_HTTP_SERVER_H__

#include "server_frame/tcp_server.h"
#include "http_session.h"
#include "servlet.h"

namespace ygw {

    //---------------------------------------------------------------------

    namespace http {

        /**
         * @brief http server config
         */

        class HttpServerConfig {
        public:
            bool operator==(const HttpServerConfig& cfg) const
            {
                return root_ == cfg.root_
                    && name_ == cfg.name_;
            }
            bool operator<(const HttpServerConfig& cfg) const
            {
                return name_ < cfg.name_;
            }
            bool IsValid() const
            {
                return !root_.empty();
            }
            void SetRoot(const std::string& root) { root_ = root; }
            void SetName(const std::string& name) { name_ = name; }
            std::string GetRoot() const { return root_; }
            std::string GetName() const { return name_; }
        private:
            std::string root_; // 根路径
            std::string name_; // 名字
        };

        /**
         * @brief HTTP服务器类
         */
        class HttpServer : public tcp::TcpServer 
        {
        public:
            /// 智能指针类型
            using ptr = std::shared_ptr<HttpServer>;
            /**
             * @brief 构造函数
             * @param[in] keepalive 是否长连接
             * @param[in] worker 工作调度器
             * @param[in] io_worker io工作调度器
             * @param[in] accept_worker 接收连接调度器
             */
            HttpServer(bool keepalive = false
                    ,ygw::scheduler::IOManager* io_worker = ygw::scheduler::IOManager::GetThis()
                    ,ygw::scheduler::IOManager* accept_worker = ygw::scheduler::IOManager::GetThis());

            /**
             * @brief 获取ServletDispatch
             */
            ServletDispatch::ptr GetServletDispatch() const { return dispatch_; }

            /**
             * @brief 设置ServletDispatch
             */
            void SetServletDispatch(ServletDispatch::ptr v) { dispatch_ = v; }

            /**
             * @brief 设置root_path_
             */
            void SetRoot(const std::string& path) { root_path_ = path; }

            /**
             * @brief 设置服务器名
             */
            virtual void SetName(const std::string& v) override;
        protected:
            virtual void HandleClient(ygw::socket::Socket::ptr client) override;
            bool SendDocument(HttpRequest::ptr request, HttpResponse::ptr response, HttpSession::ptr session);
        private:
            /// 是否支持长连接
            bool is_keepalive_;
            /// Servlet分发器
            ServletDispatch::ptr dispatch_;
            /// 根路径
            std::string root_path_;
        };



    } // namespace http

    //---------------------------------------------------------------------

} // namespace ygw

#endif // __YGE_HTTP_SERVER_H__
