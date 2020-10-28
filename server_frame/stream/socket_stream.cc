/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/stream/socket_stream.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-19
 *  Description: 
 * ====================================================
 */
#include "server_frame/util.h"
#include "socket_stream.h"

namespace ygw {

    //----------------------------------------------------------------------

    namespace stream {

        //----------------------------------------------------------------------
        SocketStream::SocketStream(socket::Socket::ptr sock, bool owner)
            :socket_(sock)
            ,owner_(owner) 
        {
        }

        SocketStream::~SocketStream() 
        {
            if (owner_ && socket_) 
            {
                socket_->Close();
            }
        }

        bool SocketStream::IsConnected() const
        {
            return socket_ && socket_->IsConnected();
        }

        int SocketStream::Read(void* buffer, size_t length) 
        {
            if (!IsConnected()) 
            {
                return -1;
            }
            return socket_->Recv(buffer, length);
        }

        int SocketStream::Read(container::ByteArray::ptr ba, size_t length) 
        {
            if (!IsConnected()) 
            {
                return -1;
            }
            std::vector<iovec> iovs;
            ba->GetWriteBuffers(iovs, length);
            int rt = socket_->Recv(&iovs[0], iovs.size());
            if (rt > 0) 
            {
                ba->SetPosition(ba->GetPosition() + rt);
            }
            return rt;
        }

        int SocketStream::Write(const void* buffer, size_t length) 
        {
            if (!IsConnected()) 
            {
                return -1;
            }
            return socket_->Send(buffer, length);
        }

        int SocketStream::Write(container::ByteArray::ptr ba, size_t length) 
        {
            if (!IsConnected()) 
            {
                return -1;
            }
            std::vector<iovec> iovs;
            ba->GetReadBuffers(iovs, length);
            int rt = socket_->Send(&iovs[0], iovs.size());
            if (rt > 0) 
            {
                ba->SetPosition(ba->GetPosition() + rt);
            }
            return rt;
        }

        void SocketStream::Close() 
        {
            if (socket_)
            {
                socket_->Close();
            }
        }

        socket::Address::ptr SocketStream::GetRemoteAddress()
        {
            if (socket_)
            {
                return socket_->GetRemoteAddress();
            }
            return nullptr;
        }

        socket::Address::ptr SocketStream::GetLocalAddress() 
        {
            if (socket_)
            {
                return socket_->GetLocalAddress();
            }
            return nullptr;
        }

        std::string SocketStream::GetRemoteAddressString() 
        {
            auto addr = GetRemoteAddress();
            if (addr) 
            {
                return addr->ToString();
            }
            return "";
        }

        std::string SocketStream::GetLocalAddressString() 
        {
            auto addr = GetLocalAddress();
            if (addr) 
            {
                return addr->ToString();
            }
            return "";
        }

        //----------------------------------------------------------------------

    } // namespace stream

    //----------------------------------------------------------------------

} // namespace ygw
