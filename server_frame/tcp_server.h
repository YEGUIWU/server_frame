/**
 * @file server_frame/tcp_server.h
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-10-05
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.top
 */

#ifndef __YGW_TCP_SERVER_H__
#define __YGW_TCP_SERVER_H__

#include <functional>
#include <memory>

#include "address.h"
#include "config.h"
#include "iomanager.h"
#include "noncopyable.h"
#include "socket.h"

namespace ygw {
    
    //----------------------------------------------------------------------------
    namespace tcp { 

        //------------------------------------------------------------------------
        /**
         * @brief Tcp Server config
         */
        struct TcpServerConf {
            using ptr = std::shared_ptr<TcpServerConf>;
            std::vector<std::string> address;
            int keepalive = 0;
            int timeout = 1000 * 2 * 60;
            int ssl = 0;
            std::string id;
            /// 服务器类型，http, ws, rock
            std::string type = "tcp";
            std::string name;
            std::string cert_file;
            std::string key_file;
            std::string accept_worker;
            std::string io_worker;
            std::string process_worker;
            std::map<std::string, std::string> args;

            bool IsValid() const 
            {
                return !address.empty();
            }

            bool operator==(const TcpServerConf& oth) const 
            {
                return address == oth.address
                    && keepalive == oth.keepalive
                    && timeout == oth.timeout
                    && name == oth.name
                    && ssl == oth.ssl
                    && cert_file == oth.cert_file
                    && key_file == oth.key_file
                    && accept_worker == oth.accept_worker
                    && io_worker == oth.io_worker
                    && process_worker == oth.process_worker
                    && args == oth.args
                    && id == oth.id
                    && type == oth.type;
            }
        };


        /**
         * @brief TCP服务器封装
         */
        class TcpServer : public std::enable_shared_from_this<TcpServer>
                          , able::Noncopyable 
        {
        public:
            using ptr = std::shared_ptr<TcpServer>;
            /**
             * @brief 构造函数
             * @param[in] worker socket客户端工作的协程调度器
             * @param[in] accept_worker 服务器socket执行接收socket连接的协程调度器
             */
            //TcpServer(ygw::scheduler::IOManager* worker = ygw::scheduler::IOManager::GetThis()
            //        ,ygw::scheduler::IOManager* io_woker = ygw::scheduler::IOManager::GetThis()
            //        ,ygw::scheduler::IOManager* accept_worker = ygw::scheduler::IOManager::GetThis());
            TcpServer(ygw::scheduler::IOManager* io_woker = ygw::scheduler::IOManager::GetThis()
                    ,ygw::scheduler::IOManager* accept_worker = ygw::scheduler::IOManager::GetThis());

            /**
             * @brief 析构函数
             */
            virtual ~TcpServer();

            /**
             * @brief 绑定地址
             * @return 返回是否绑定成功
             */
            virtual bool Bind(ygw::socket::Address::ptr addr, bool ssl = false);

            /**
             * @brief 绑定地址数组
             * @param[in] addrs 需要绑定的地址数组
             * @param[out] fails 绑定失败的地址
             * @return 是否绑定成功
             */
            virtual bool Bind(const std::vector<ygw::socket::Address::ptr>& addrs
                    ,std::vector<ygw::socket::Address::ptr>& fails
                    ,bool ssl = false);


            bool LoadCertificates(const std::string& cert_file, const std::string& key_file);

            /**
             * @brief 启动服务
             * @pre 需要bind成功后执行
             */
            virtual bool Start();

            /**
             * @brief 停止服务
             */
            virtual void Stop();

            /**
             * @brief 返回读取超时时间(毫秒)
             */
            uint64_t GetRecvTimeout() const { return recv_timeout_;}

            /**
             * @brief 返回服务器名称
             */
            std::string GetName() const { return name_;}

            /**
             * @brief 设置读取超时时间(毫秒)
             */
            void SetRecvTimeout(uint64_t v) { recv_timeout_ = v;}

            /**
             * @brief 设置服务器名称
             */
            virtual void SetName(const std::string& v) { name_ = v;}

            /**
             * @brief 是否停止
             */
            bool IsStop() const { return is_stop_;}

            /**
             * @brief 获取配置
             */
            TcpServerConf::ptr GetConf() const { return conf_;}


            /**
             * @brief 设置配置
             */
            void SetConf(TcpServerConf::ptr v) { conf_ = v;}
            void SetConf(const TcpServerConf& v);

            /**
             * @brief 转字符串
             */
            virtual std::string ToString(const std::string& prefix = "");

            /**
             * @brief 获取socks_
             */
            std::vector<ygw::socket::Socket::ptr> GetSocks() const { return socks_;}
        protected:
            /**
             * @brief 处理新连接的Socket类
             */
            virtual void HandleClient(ygw::socket::Socket::ptr client);

            /**
             * @brief 开始接受连接
             */
            virtual void StartAccept(ygw::socket::Socket::ptr sock);
        protected:
            /// 监听Socket数组
            std::vector<ygw::socket::Socket::ptr> socks_;
            /// 新连接的Socket工作的调度器
            //scheduler::IOManager* worker_;
            scheduler::IOManager* io_worker_;
            /// 服务器Socket接收连接的调度器
            scheduler::IOManager* accept_worker_;
            /// 接收超时时间(毫秒)
            uint64_t recv_timeout_;
            /// 服务器名称
            std::string name_;
            /// 服务器类型
            std::string type_ = "tcp";
            /// 服务是否停止
            bool is_stop_;

            bool ssl_ = false;

            TcpServerConf::ptr conf_;

        }; // class TcpServer




    } // namespace tcp

    //----------------------------------------------------------------------------

    namespace config {

        template<>
		class LexicalCast<std::string, ygw::tcp::TcpServerConf> {
		public:
            ygw::tcp::TcpServerConf operator()(const std::string& v) 
            {
                YAML::Node node = YAML::Load(v);
                ygw::tcp::TcpServerConf conf;
                conf.id = node["id"].as<std::string>(conf.id);
                conf.type = node["type"].as<std::string>(conf.type);
                conf.keepalive = node["keepalive"].as<int>(conf.keepalive);
                conf.timeout = node["timeout"].as<int>(conf.timeout);
                conf.name = node["name"].as<std::string>(conf.name);
                conf.ssl = node["ssl"].as<int>(conf.ssl);
                conf.cert_file = node["cert_file"].as<std::string>(conf.cert_file);
                conf.key_file = node["key_file"].as<std::string>(conf.key_file);
                conf.accept_worker = node["accept_worker"].as<std::string>();
                conf.io_worker = node["io_worker"].as<std::string>();
                conf.process_worker = node["process_worker"].as<std::string>();
                conf.args = LexicalCast<std::string
                    ,std::map<std::string, std::string> >()(node["args"].as<std::string>(""));
                if (node["address"].IsDefined()) 
                {
                    for (size_t i = 0; i < node["address"].size(); ++i) 
                    {
                        conf.address.push_back(node["address"][i].as<std::string>());
                    }
                }
                return conf;
            }
		};


        template<>
        class LexicalCast<ygw::tcp::TcpServerConf, std::string> {
        public:
            std::string operator()(const ygw::tcp::TcpServerConf& conf) 
            {
                YAML::Node node;
                node["id"] = conf.id;
                node["type"] = conf.type;
                node["name"] = conf.name;
                node["keepalive"] = conf.keepalive;
                node["timeout"] = conf.timeout;
                node["ssl"] = conf.ssl;
                node["cert_file"] = conf.cert_file;
                node["key_file"] = conf.key_file;
                node["accept_worker"] = conf.accept_worker;
                node["io_worker"] = conf.io_worker;
                node["process_worker"] = conf.process_worker;
                node["args"] = YAML::Load(LexicalCast<std::map<std::string, std::string>
                        , std::string>()(conf.args));
                for (auto& i : conf.address) 
                {
                    node["address"].push_back(i);
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
            }
        };


    } // namespace config

} // namespace ygw




#endif // __YGW_TCP_SERVER_H__
