/**
 * @file server_frame/sys/daemon.h
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-10-28
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#ifndef __YGW_DAEMON_H__
#define __YGW_DAEMON_H__

#include <unistd.h>
#include <functional>
#include <server_frame/singleton.h>

namespace ygw {

    namespace sys {

        /**
         * @brief 进程信息
         */
        struct ProcessInfo {
            /// 父进程id
            pid_t parent_id = 0;
            /// 主进程id
            pid_t main_id = 0;
            /// 父进程启动时间
            uint64_t parent_start_time = 0;
            /// 主进程启动时间
            uint64_t main_start_time = 0;
            /// 主进程重启的次数
            uint32_t restart_count = 0;

            std::string ToString() const;
        };

        using ProcessInfoManager = ygw::mode::Singleton<ProcessInfo>;

        /**
         * @brief 启动程序可以选择用守护进程的方式
         * @param[in] argc 参数个数
         * @param[in] argv 参数值数组
         * @param[in] main_cb 启动函数
         * @param[in] is_daemon 是否守护进程的方式
         * @return 返回程序的执行结果
         */
        int StartDaemon(int argc, char** argv
                , std::function<int(int argc, char** argv)> main_cb
                , bool is_daemon);

    }  // namespace sys

} // namespace ygw


#endif // __YGW_DAEMON_H__
