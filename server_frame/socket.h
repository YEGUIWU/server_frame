/**
 * @file server_frame/socket.h
 * @brief Socket api封装
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#ifndef __YGW_SOCKET_H__
#define __YGW_SOCKET_H__

#include <memory>

#include <netinet/tcp.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "address.h"
#include "noncopyable.h"

namespace ygw {

    //------------------------------------------------------------------------------

    namespace socket {

        //--------------------------------------------------------------------------------

        class Socket : public std::enable_shared_from_this<Socket>, able::Noncopyable {
        public:
            using ptr = std::shared_ptr<Socket>;
            using weak_ptr = std::weak_ptr<Socket>;

               /**
				* @brief Socket类型
				*/
			enum Type {
				/// TCP类型
				kTCP = SOCK_STREAM,
				/// UDP类型
				kUDP = SOCK_DGRAM
			};

			/**
			 * @brief Socket协议簇
			 */
			enum Family {
				/// IPv4 socket
				kIPv4 = AF_INET,
				/// IPv6 socket
				kIPv6 = AF_INET6,
				/// Unix socket
				kUNIX = AF_UNIX,
			};

			/**
			 * @brief 创建TCP Socket(满足地址类型)
			 * @param[in] address 地址
			 */
			static Socket::ptr CreateTCP(ygw::socket::Address::ptr address);

			/**
			 * @brief 创建UDP Socket(满足地址类型)
			 * @param[in] address 地址
			 */
			static Socket::ptr CreateUDP(ygw::socket::Address::ptr address);

			/**
			 * @brief 创建IPv4的TCP Socket
			 */
			static Socket::ptr CreateTCPSocket();

			/**
			 * @brief 创建IPv4的UDP Socket
			 */
			static Socket::ptr CreateUDPSocket();

			/**
			 * @brief 创建IPv6的TCP Socket
			 */
			static Socket::ptr CreateTCPSocket6();

			/**
			 * @brief 创建IPv6的UDP Socket
			 */
			static Socket::ptr CreateUDPSocket6();

			/**
			 * @brief 创建Unix的TCP Socket
			 */
			static Socket::ptr CreateUnixTCPSocket();

			/**
			 * @brief 创建Unix的UDP Socket
			 */
			static Socket::ptr CreateUnixUDPSocket();

            //------------------------------------------------------------------
			/**
			 * @brief Socket构造函数
			 * @param[in] family 协议簇
			 * @param[in] type 类型
			 * @param[in] protocol 协议
			 */
			Socket(int family, int type, int protocol = 0);

			/**
			 * @brief 析构函数
			 */
			virtual ~Socket();

			/**
			 * @brief 获取发送超时时间(毫秒)
			 */
			int64_t GetSendTimeout();

			/**
			 * @brief 设置发送超时时间(毫秒)
			 */
			void SetSendTimeout(int64_t v);

			/**
			 * @brief 获取接受超时时间(毫秒)
			 */
			int64_t GetRecvTimeout();

			/**
			 * @brief 设置接受超时时间(毫秒)
			 */
			void SetRecvTimeout(int64_t v);

			/**
			 * @brief 获取sockopt @see getsockopt
			 */
			bool GetOption(int level, int option, void* result, socklen_t* len);

			/**
			 * @brief 获取sockopt模板 @see getsockopt
			 */
			template<class T>
			bool GetOption(int level, int option, T& result) 
            {
				socklen_t length = sizeof(T);
				return GetOption(level, option, &result, &length);
			}

			/**
			 * @brief 设置sockopt @see setsockopt
			 */
			bool SetOption(int level, int option, const void* result, socklen_t len);

			/**
			 * @brief 设置sockopt模板 @see setsockopt
			 */
			template<class T>
			bool SetOption(int level, int option, const T& value) 
            {
				return SetOption(level, option, &value, sizeof(T));
			}

			/**
			 * @brief 接收connect链接
			 * @return 成功返回新连接的socket,失败返回nullptr
			 * @pre Socket必须 bind , listen  成功
			 */
			virtual Socket::ptr Accept();

			/**
			 * @brief 绑定地址
			 * @param[in] addr 地址
			 * @return 是否绑定成功
			 */
			virtual bool Bind(const Address::ptr addr);

			/**
			 * @brief 连接地址
			 * @param[in] addr 目标地址
			 * @param[in] timeout_ms 超时时间(毫秒)
			 */
			virtual bool Connect(const Address::ptr addr, uint64_t timeout_ms = -1);

			virtual bool Reconnect(uint64_t timeout_ms = -1);

			/**
			 * @brief 监听socket
			 * @param[in] backlog 未完成连接队列的最大长度
			 * @result 返回监听是否成功
			 * @pre 必须先 bind 成功
			 */
			virtual bool Listen(int backlog = SOMAXCONN);

			/**
			 * @brief 关闭socket
			 */
			virtual bool Close();

			/**
			 * @brief 发送数据
			 * @param[in] buffer 待发送数据的内存
			 * @param[in] length 待发送数据的长度
			 * @param[in] flags 标志字
			 * @return
			 *      @retval >0 发送成功对应大小的数据
			 *      @retval =0 socket被关闭
			 *      @retval <0 socket出错
			 */
			virtual int Send(const void* buffer, size_t length, int flags = 0);

			/**
			 * @brief 发送数据
			 * @param[in] buffers 待发送数据的内存(iovec数组)
			 * @param[in] length 待发送数据的长度(iovec长度)
			 * @param[in] flags 标志字
			 * @return
			 *      @retval >0 发送成功对应大小的数据
			 *      @retval =0 socket被关闭
			 *      @retval <0 socket出错
			 */
			virtual int Send(const iovec* buffers, size_t length, int flags = 0);

			/**
			 * @brief 发送数据
			 * @param[in] buffer 待发送数据的内存
			 * @param[in] length 待发送数据的长度
			 * @param[in] to 发送的目标地址
			 * @param[in] flags 标志字
			 * @return
			 *      @retval >0 发送成功对应大小的数据
			 *      @retval =0 socket被关闭
			 *      @retval <0 socket出错
			 */
			virtual int SendTo(const void* buffer, size_t length, const Address::ptr to, int flags = 0);

			/**
			 * @brief 发送数据
			 * @param[in] buffers 待发送数据的内存(iovec数组)
			 * @param[in] length 待发送数据的长度(iovec长度)
			 * @param[in] to 发送的目标地址
			 * @param[in] flags 标志字
			 * @return
			 *      @retval >0 发送成功对应大小的数据
			 *      @retval =0 socket被关闭
			 *      @retval <0 socket出错
			 */
			virtual int SendTo(const iovec* buffers, size_t length, const Address::ptr to, int flags = 0);

			/**
			 * @brief 接受数据
			 * @param[out] buffer 接收数据的内存
			 * @param[in] length 接收数据的内存大小
			 * @param[in] flags 标志字
			 * @return
			 *      @retval >0 接收到对应大小的数据
			 *      @retval =0 socket被关闭
			 *      @retval <0 socket出错
			 */
			virtual int Recv(void* buffer, size_t length, int flags = 0);

			/**
			 * @brief 接受数据
			 * @param[out] buffers 接收数据的内存(iovec数组)
			 * @param[in] length 接收数据的内存大小(iovec数组长度)
			 * @param[in] flags 标志字
			 * @return
			 *      @retval >0 接收到对应大小的数据
			 *      @retval =0 socket被关闭
			 *      @retval <0 socket出错
			 */
			virtual int Recv(iovec* buffers, size_t length, int flags = 0);

			/**
			 * @brief 接受数据
			 * @param[out] buffer 接收数据的内存
			 * @param[in] length 接收数据的内存大小
			 * @param[out] from 发送端地址
			 * @param[in] flags 标志字
			 * @return
			 *      @retval >0 接收到对应大小的数据
			 *      @retval =0 socket被关闭
			 *      @retval <0 socket出错
			 */
			virtual int RecvFrom(void* buffer, size_t length, Address::ptr from, int flags = 0);

			/**
			 * @brief 接受数据
			 * @param[out] buffers 接收数据的内存(iovec数组)
			 * @param[in] length 接收数据的内存大小(iovec数组长度)
			 * @param[out] from 发送端地址
			 * @param[in] flags 标志字
			 * @return
			 *      @retval >0 接收到对应大小的数据
			 *      @retval =0 socket被关闭
			 *      @retval <0 socket出错
			 */
			virtual int RecvFrom(iovec* buffers, size_t length, Address::ptr from, int flags = 0);

			/**
			 * @brief 获取远端地址
			 */
			Address::ptr GetRemoteAddress();

			/**
			 * @brief 获取本地地址
			 */
			Address::ptr GetLocalAddress();

			/**
			 * @brief 获取协议簇
			 */
			int GetFamily() const { return family_;}

			/**
			 * @brief 获取类型
			 */
			int GetType() const { return type_;}

			/**
			 * @brief 获取协议
			 */
			int GetProtocol() const { return protocol_;}

			/**
			 * @brief 返回是否连接
			 */
			bool IsConnected() const { return is_connected_;}

			/**
			 * @brief 是否有效(sockfd_ != -1)
			 */
			bool IsValid() const;

			/**
			 * @brief 返回Socket错误
			 */
			int GetError();

			/**
			 * @brief 输出信息到流中
			 */
			virtual std::ostream& Dump(std::ostream& os) const;

			virtual std::string ToString() const;

			/**
			 * @brief 返回socket句柄
			 */
			int GetSocket() const { return sockfd_;}

			/**
			 * @brief 取消读
			 */
			bool CancelRead();

			/**
			 * @brief 取消写
			 */
			bool CancelWrite();

			/**
			 * @brief 取消accept
			 */
			bool CancelAccept();

			/**
			 * @brief 取消所有事件
			 */
			bool CancelAll();
		protected:
			/**
			 * @brief 初始化socket
			 */
			void InitSock();

			/**
			 * @brief 创建socket
			 */
			void NewSock();

			/**
			 * @brief 初始化sock
			 */
			virtual bool Init(int sockfd);
		protected:
			/// socket句柄
			int sockfd_;
			/// 协议簇
			int family_;
			/// 类型
			int type_;
			/// 协议
			int protocol_;
			/// 是否连接
			bool is_connected_;
			/// 本地地址
			Address::ptr local_address_;
			/// 远端地址
			Address::ptr remote_address_;

        };


		class SSLSocket : public Socket {
		public:
	        using ptr = std::shared_ptr<SSLSocket>;

			static SSLSocket::ptr CreateTCP(ygw::socket::Address::ptr address);
			static SSLSocket::ptr CreateTCPSocket();
			static SSLSocket::ptr CreateTCPSocket6();

			SSLSocket(int family, int type, int protocol = 0);
			virtual Socket::ptr Accept() override;
			virtual bool Bind(const Address::ptr addr) override;
			virtual bool Connect(const Address::ptr addr, uint64_t timeout_ms = -1) override;
			virtual bool Listen(int backlog = SOMAXCONN) override;
			virtual bool Close() override;
			virtual int Send(const void* buffer, size_t length, int flags = 0) override;
			virtual int Send(const iovec* buffers, size_t length, int flags = 0) override;
			virtual int SendTo(const void* buffer, size_t length, const Address::ptr to, int flags = 0) override;
			virtual int SendTo(const iovec* buffers, size_t length, const Address::ptr to, int flags = 0) override;
			virtual int Recv(void* buffer, size_t length, int flags = 0) override;
			virtual int Recv(iovec* buffers, size_t length, int flags = 0) override;
			virtual int RecvFrom(void* buffer, size_t length, Address::ptr from, int flags = 0) override;
			virtual int RecvFrom(iovec* buffers, size_t length, Address::ptr from, int flags = 0) override;

			bool LoadCertificates(const std::string& cert_file, const std::string& key_file);
			virtual std::ostream& Dump(std::ostream& os) const override;
		protected:
			virtual bool Init(int sock) override;
		private:
			std::shared_ptr<SSL_CTX> ctx_;
			std::shared_ptr<SSL> ssl_;
		};

		/**
		 * @brief 流式输出socket
		 * @param[in, out] os 输出流
		 * @param[in] sock Socket类
		 */
		std::ostream& operator<<(std::ostream& os, const Socket& sock);


        
        //------------------------------------------------------------------------------

    }

    //------------------------------------------------------------------------------

} // namespace ygw 


#endif // __YGW_SOCKET_H__
