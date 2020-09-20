/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/fd_manager.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-11
 *  Description: 
 * ====================================================
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "fd_manager.h"
#include "hook.h"

namespace ygw {

    //----------------------------------------------------

    namespace handle {

        FdContext::FdContext(int fd)
            :is_init_(false)
             ,is_socket_(false)
             ,is_sys_nonblock_(false)
             ,is_user_nonblock_(false)
             ,is_closed_(false)
             ,fd_(fd)
             ,recv_timeout_(-1)
             ,send_timeout_(-1) 
        {
            Init();
        }

        FdContext::~FdContext() 
        {
        }

        bool FdContext::Init() 
        {
            if (is_init_) 
            {
                return true;
            }
            recv_timeout_ = -1;
            send_timeout_ = -1;

            struct stat fd_stat;
            if (-1 == fstat(fd_, &fd_stat)) 
            {
                is_init_ = false;
                is_socket_ = false;
            } 
            else 
            {
                is_init_ = true;
                is_socket_ = S_ISSOCK(fd_stat.st_mode);
            }

            if (is_socket_) 
            {
                int flags = fcntl_f(fd_, F_GETFL, 0);
                if (!(flags & O_NONBLOCK)) 
                {
                    fcntl_f(fd_, F_SETFL, flags | O_NONBLOCK);
                }
                is_sys_nonblock_ = true;
            }
            else 
            {
                is_sys_nonblock_ = false;
            }

            is_user_nonblock_ = false;
            is_closed_ = false;
            return is_init_;
        }

        void FdContext::SetTimeout(int type, uint64_t v) 
        {
            if (type == SO_RCVTIMEO) 
            {
                recv_timeout_ = v;
            } 
            else 
            {
                send_timeout_ = v;
            }
        }

        uint64_t FdContext::GetTimeout(int type)
        {
            if (type == SO_RCVTIMEO) 
            {
                return recv_timeout_;
            } 
            else 
            {
                return send_timeout_;
            }
        }

        FdContextManager::FdContextManager() 
        {
            datas_.resize(64);
        }

        FdContext::ptr FdContextManager::Get(int fd, bool auto_create) 
        {
            if (fd < 0) 
            {
                return nullptr;
            }
            {
                //够就直接给，不够就扩充再给
                RWMutexType::ReadLock lock(mutex_);
                if (static_cast<int>(datas_.size()) <= fd) 
                {
                    if (auto_create == false) 
                    {
                        return nullptr;
                    }
                } 
                else 
                {
                    if (datas_[fd] || !auto_create) 
                    {
                        return datas_[fd];
                    }
                }
            }

            RWMutexType::WriteLock lock(mutex_);
            FdContext::ptr ctx(new FdContext(fd));
            if (fd >= static_cast<int>(datas_.size()))
            {
                datas_.resize(fd * 1.5);
            }
            datas_[fd] = ctx;
            return ctx;
        }

        void FdContextManager::Del(int fd) 
        {
            RWMutexType::WriteLock lock(mutex_);
            if ((int)datas_.size() <= fd)
            {
                return;
            }
            datas_[fd].reset();
        }

    } // namespace handle

    //----------------------------------------------------

} // namespace ygw 
