/**
 * @file server_frame/http/uri.h
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-10-28
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#ifndef __YGW_URI_H__
#define __YGW_URI_H__
#include <memory>
#include <string>
#include <stdint.h>
#include "server_frame/address.h"

namespace ygw {

    namespace http {


/*
     foo://user@sylar.com:8042/over/there?name=ferret#nose
       \_/   \______________/\_________/ \_________/ \__/
        |           |            |            |        |
     scheme     authority       path        query   fragment
*/

        /**
         * @brief URI类
         */
        class Uri {
        public:
            /// 智能指针类型定义
            using ptr = std::shared_ptr<Uri>;

            /**
             * @brief 创建Uri对象
             * @param uri uri字符串
             * @return 解析成功返回Uri对象否则返回nullptr
             */
            static Uri::ptr Create(const std::string& uri);

            /**
             * @brief 构造函数
             */
            Uri();

            /**
             * @brief 返回scheme
             */
            const std::string& GetScheme() const { return scheme_;}

            /**
             * @brief 返回用户信息
             */
            const std::string& GetUserinfo() const { return userinfo_;}

            /**
             * @brief 返回host
             */
            const std::string& GetHost() const { return host_;}

            /**
             * @brief 返回路径
             */
            const std::string& GetPath() const;

            /**
             * @brief 返回查询条件
             */
            const std::string& GetQuery() const { return query_;}

            /**
             * @brief 返回fragment
             */
            const std::string& GetFragment() const { return fragment_;}

            /**
             * @brief 返回端口
             */
            int32_t GetPort() const;

            /**
             * @brief 设置scheme
             * @param v scheme
             */
            void SetScheme(const std::string& v) { scheme_ = v;}

            /**
             * @brief 设置用户信息
             * @param v 用户信息
             */
            void SetUserinfo(const std::string& v) { userinfo_ = v;}

            /**
             * @brief 设置host信息
             * @param v host
             */
            void SetHost(const std::string& v) { host_ = v;}

            /**
             * @brief 设置路径
             * @param v 路径
             */
            void SetPath(const std::string& v) { path_ = v;}

            /**
             * @brief 设置查询条件
             * @param v
             */
            void SetQuery(const std::string& v) { query_ = v;}

            /**
             * @brief 设置fragment
             * @param v fragment
             */
            void SetFragment(const std::string& v) { fragment_ = v;}

            /**
             * @brief 设置端口号
             * @param v 端口
             */
            void SetPort(int32_t v) { port_ = v;}

            /**
             * @brief 序列化到输出流
             * @param os 输出流
             * @return 输出流
             */
            std::ostream& Dump(std::ostream& os) const;

            /**
             * @brief 转成字符串
             */
            std::string ToString() const;

            /**
             * @brief 获取Address
             */
            socket::Address::ptr CreateAddress() const;
        private:

            /**
             * @brief 是否默认端口
             */
            bool IsDefaultPort() const;
        private:
            /// schema
            std::string scheme_;
            /// 用户信息
            std::string userinfo_;
            /// host
            std::string host_;
            /// 路径
            std::string path_;
            /// 查询参数
            std::string query_;
            /// fragment
            std::string fragment_;
            /// 端口
            int32_t port_;
        };

    }


} // namespace ygw

#endif // __YGW_URI_H__
