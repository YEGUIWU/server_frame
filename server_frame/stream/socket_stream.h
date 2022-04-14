/**
 * @file socket_stream.h
 * @brief Socket流 接口封装
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#ifndef __YGW_SOCKET_STREAM_H__
#define __YGW_SOCKET_STREAM_H__

#include "server_frame/base/mutex.h"
#include "server_frame/iomanager.h"
#include "server_frame/socket.h"
#include "stream.h"

namespace ygw {

    //-----------------------------------------------------------------------------

    namespace stream {

        /**
         * @brief Socket流
         */
        class SocketStream : public Stream {
        public:
            using ptr = std::shared_ptr<SocketStream>;
            /**
             * @brief 构造函数
             * @param[in] sock Socket类
             * @param[in] owner 是否完全控制
             */
            SocketStream(socket::Socket::ptr sock, bool owner = true);

            /**
             * @brief 析构函数
             * @details 如果owner_=true,则close
             */
            ~SocketStream();

            /**
             * @brief 读取数据
             * @param[out] buffer 待接收数据的内存
             * @param[in] length 待接收数据的内存长度
             * @return
             *      @retval >0 返回实际接收到的数据长度
             *      @retval =0 socket被远端关闭
             *      @retval <0 socket错误
             */
            virtual int Read(void* buffer, size_t length) override;

            /**
             * @brief 读取数据
             * @param[out] ba 接收数据的container::ByteArray
             * @param[in] length 待接收数据的内存长度
             * @return
             *      @retval >0 返回实际接收到的数据长度
             *      @retval =0 socket被远端关闭
             *      @retval <0 socket错误
             */
            virtual int Read(container::ByteArray::ptr ba, size_t length) override;

            /**
             * @brief 写入数据
             * @param[in] buffer 待发送数据的内存
             * @param[in] length 待发送数据的内存长度
             * @return
             *      @retval >0 返回实际接收到的数据长度
             *      @retval =0 socket被远端关闭
             *      @retval <0 socket错误
             */
            virtual int Write(const void* buffer, size_t length) override;

            /**
             * @brief 写入数据
             * @param[in] ba 待发送数据的container::ByteArray
             * @param[in] length 待发送数据的内存长度
             * @return
             *      @retval >0 返回实际接收到的数据长度
             *      @retval =0 socket被远端关闭
             *      @retval <0 socket错误
             */
            virtual int Write(container::ByteArray::ptr ba, size_t length) override;

            /**
             * @brief 关闭socket
             */
            virtual void Close() override;

            /**
             * @brief 返回Socket类
             */
            socket::Socket::ptr GetSocket() const { return socket_;}

            /**
             * @brief 返回是否连接
             */
            bool IsConnected() const;

            socket::Address::ptr GetRemoteAddress();
            socket::Address::ptr GetLocalAddress();
            std::string GetRemoteAddressString();
            std::string GetLocalAddressString();
        protected:
            /// Socket类
            socket::Socket::ptr socket_;
            /// 是否主控
            bool owner_;
        };






    } // namespace stream

    //-----------------------------------------------------------------------------

} // namespace ygw

#endif // __YGW_SOCKET_STREAM_H__
