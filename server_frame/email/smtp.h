/**
 * @file smtp.h
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-09-28
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.top
 */

#include <sstream>

#include "email.h"
#include <server_frame/streams/socket_stream.h>

namespace ygw {

    namespace smtp {

        struct SmtpResult {
            using ptr = std::shared_ptr<SmtpResult>;
            enum Result {
                kOK = 0,
                kIoError= -1
            }; // enum Result

            SmtpResult(int r, const std::string& m)
                :result(r)
                 ,msg(m) 
            {
            }

            int result;
            std::string msg;
        };

        /**
         * @breif stmp client
         */
        class SmtpClient : public ygw::stream::SocketStream {
        public:
            using ptr = std::shared_ptr<SmtpClient> ptr;

            static SmtpClient::ptr Create(const std::string& host, uint32_t port, bool ssl= false);

            SmtpResult::ptr Send(EMail::ptr email, int64_t timeout_ms = 1000, bool debug = false);

            std::string GetDebugInfo();
        private:
            SmtpResult::ptr DoCmd(const std::string& cmd, bool debug);
        private:
            SmtpClient(Socket::ptr sock);
        private:
            std::string host_;
            std::stringstream ss_;
            bool authed_ = false;
        };


    } // namespace stmp

} // namespace ygw
