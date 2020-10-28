/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: http/servlet.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-20
 *  Description: 
 * ====================================================
 */
#ifndef __YGW_HTTP_SERVER_H__
#define __YGW_HTTP_SERVER_H__

#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include "http.h"
#include "http_session.h"
#include "server_frame/base/thread.h"
#include "server_frame/util.h"

namespace ygw {

    //-------------------------------------------------------------------------

    namespace http {

        //-------------------------------------------------------------------------
        /**
         * @brief Servlet封装
         */
        class Servlet {
        public:
            /// 智能指针类型定义
            using ptr = std::shared_ptr<Servlet>;

            /**
             * @brief 构造函数
             * @param[in] name 名称
             */
            Servlet(const std::string& name)
                :name_(name) {}

            /**
             * @brief 析构函数
             */
            virtual ~Servlet() {}

            /**
             * @brief 处理请求
             * @param[in] request HTTP请求
             * @param[in] response HTTP响应
             * @param[in] session HTTP连接
             * @return 是否处理成功
             */
            virtual int32_t Handle(
                      ygw::http::HttpRequest::ptr request
                    , ygw::http::HttpResponse::ptr response
                    , ygw::http::HttpSession::ptr session) = 0;

            /**
             * @brief 返回Servlet名称
             */
            const std::string& GetName() const { return name_; }
        protected:
            /// 名称
            std::string name_;
        }; // class Servlet



        //-------------------------------------------------------------------------
        /**
         * @brief 函数式Servlet
         */
        class FunctionServlet : public Servlet {
        public:
            /// 智能指针类型定义
            using ptr = std::shared_ptr<FunctionServlet>;
            /// 函数回调类型定义
            using callback = std::function<
                int32_t (ygw::http::HttpRequest::ptr request
                    , ygw::http::HttpResponse::ptr response
                    , ygw::http::HttpSession::ptr session)> ;


            /**
             * @brief 构造函数
             * @param[in] cb 回调函数
             */
            FunctionServlet(callback cb);
            virtual int32_t Handle(
                      ygw::http::HttpRequest::ptr request
                    , ygw::http::HttpResponse::ptr response
                    , ygw::http::HttpSession::ptr session) override;
        private:
            /// 回调函数
            callback cb_;
        };

        //-------------------------------------------------------------------------
        class IServletCreator {
        public:
            typedef std::shared_ptr<IServletCreator> ptr;
            virtual ~IServletCreator() {}
            virtual Servlet::ptr Get() const = 0;
            virtual std::string GetName() const = 0;
        };


        //-------------------------------------------------------------------------
        class HoldServletCreator : public IServletCreator {
        public:
            using ptr = std::shared_ptr<HoldServletCreator>;
            HoldServletCreator(Servlet::ptr slt)
                :servlet_(slt) 
            {
            }

            Servlet::ptr Get() const override 
            {
                return servlet_;
            }

            std::string GetName() const override 
            {
                return servlet_->GetName();
            }
        private:
            Servlet::ptr servlet_;
        };

        template<class T>
        class ServletCreator : public IServletCreator {
        public:
            using ptr = std::shared_ptr<ServletCreator>;

            ServletCreator() 
            {
            }

            Servlet::ptr Get() const override 
            {
                return Servlet::ptr(new T);
            }

            std::string GetName() const override 
            {
                return util::TypeToName<T>();
            }
        };

        //-------------------------------------------------------------------------
        /**
         * @brief Servlet分发器
         */
        class ServletDispatch : public Servlet 
        {
        public:
            /// 智能指针类型定义
            using ptr = std::shared_ptr<ServletDispatch>;
            /// 读写锁类型定义
            using RWMutexType = thread::RWMutex;

            /**
             * @brief 构造函数
             */
            ServletDispatch();
            virtual int32_t Handle(ygw::http::HttpRequest::ptr request
                           , ygw::http::HttpResponse::ptr response
                           , ygw::http::HttpSession::ptr session) override;

            /**
             * @brief 添加servlet
             * @param[in] uri uri
             * @param[in] slt serlvet
             */
            void AddServlet(const std::string& uri, Servlet::ptr slt);

            /**
             * @brief 添加servlet
             * @param[in] uri uri
             * @param[in] cb FunctionServlet回调函数
             */
            void AddServlet(const std::string& uri, FunctionServlet::callback cb);

            /**
             * @brief 添加模糊匹配servlet
             * @param[in] uri uri 模糊匹配 /ygw_*
             * @param[in] slt servlet
             */
            void AddGlobServlet(const std::string& uri, Servlet::ptr slt);

            /**
             * @brief 添加模糊匹配servlet
             * @param[in] uri uri 模糊匹配 /ygw_*
             * @param[in] cb FunctionServlet回调函数
             */
            void AddGlobServlet(const std::string& uri, FunctionServlet::callback cb);

            void AddServletCreator(const std::string& uri, IServletCreator::ptr creator);

            void AddGlobServletCreator(const std::string& uri, IServletCreator::ptr creator);

            template<class T>
            void AddServletCreator(const std::string& uri) 
            {
                AddServletCreator(uri, std::make_shared<ServletCreator<T> >());
            }

            template<class T>
            void AddGlobServletCreator(const std::string& uri) 
            {
                AddGlobServletCreator(uri, std::make_shared<ServletCreator<T> >());
            }

            /**
             * @brief 删除servlet
             * @param[in] uri uri
             */
            void DelServlet(const std::string& uri);

            /**
             * @brief 删除模糊匹配servlet
             * @param[in] uri uri
             */
            void DelGlobServlet(const std::string& uri);

            /**
             * @brief 返回默认servlet
             */
            Servlet::ptr GetDefault() const { return default_; }

            /**
             * @brief 设置默认servlet
             * @param[in] v servlet
             */
            void SetDefault(Servlet::ptr v) { default_ = v;}


            /**
             * @brief 通过uri获取servlet
             * @param[in] uri uri
             * @return 返回对应的servlet
             */
            Servlet::ptr GetServlet(const std::string& uri);

            /**
             * @brief 通过uri获取模糊匹配servlet
             * @param[in] uri uri
             * @return 返回对应的servlet
             */
            Servlet::ptr GetGlobServlet(const std::string& uri);

            /**
             * @brief 通过uri获取servlet
             * @param[in] uri uri
             * @return 优先精准匹配,其次模糊匹配,最后返回默认
             */
            Servlet::ptr GetMatchedServlet(const std::string& uri);

            void ListAllServletCreator(std::map<std::string, IServletCreator::ptr>& infos);

            void ListAllGlobServletCreator(std::map<std::string, IServletCreator::ptr>& infos);
        private:
            /// 读写互斥量
            RWMutexType mutex_;
            /// 精准匹配servlet MAP
            /// uri(/ygw/xxx) -> servlet
            std::unordered_map<std::string, IServletCreator::ptr> datas_;
            /// 模糊匹配servlet 数组
            /// uri(/ygw/*) -> servlet
            std::vector<std::pair<std::string, IServletCreator::ptr> > globs_;
            /// 默认servlet，所有路径都没匹配到时使用
            Servlet::ptr default_;
        };

        //-------------------------------------------------------------------------
        /**
         * @brief NotFoundServlet(默认返回404)
         */
        class NotFoundServlet : public Servlet {
        public:
            /// 智能指针类型定义
            typedef std::shared_ptr<NotFoundServlet> ptr;
            /**
             * @brief 构造函数
             */
            NotFoundServlet(const std::string& name);
            virtual int32_t Handle(
                             ygw::http::HttpRequest::ptr request
                           , ygw::http::HttpResponse::ptr response
                           , ygw::http::HttpSession::ptr session) override;

        private:
            std::string name_;
            std::string content_;
        };

        //-------------------------------------------------------------------------

    } // namespace http

    //-------------------------------------------------------------------------

} // namespace ygw

#endif // __YGW_HTTP_SERVER_H__
