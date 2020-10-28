/**
 * @file email.h
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-09-28
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.top
 */

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace ygw {

    namespace smtp {

        class EMailEntity {
        public:
            typedef std::shared_ptr<EMailEntity> ptr;
            static EMailEntity::ptr CreateAttach(const std::string& filename);

            void AddHeader(const std::string& key, const std::string& val);
            std::string GetHeader(const std::string& key, const std::string& def = "");

            const std::string& GetContent() const { return content_;}
            void setContent(const std::string& v) { content_ = v;}

            std::string ToString() const;
        private:
            std::map<std::string, std::string> headers_;
            std::string content_;
        };


        class EMail {
        public:
            using ptr = std::shared_ptr<EMail>;
            static EMail::ptr Create(const std::string& from_address, const std::string& from_passwd
                    ,const std::string& title, const std::string& body
                    ,const std::vector<std::string>& to_address
                    ,const std::vector<std::string>& cc_address = {}
                    ,const std::vector<std::string>& bcc_address = {});

            const std::string& GetFromEMailAddress() const { return from_email_address_;}
            const std::string& GetFromEMailPasswd() const { return from_email_passwd_;}
            const std::string& GetTitle() const { return title_;}
            const std::string& GetBody() const { return body_;}

            void SetFromEMailAddress(const std::string& v) { from_email_address_ = v;}
            void SetFromEMailPasswd(const std::string& v) { from_email_passwd_ = v;}
            void SetTitle(const std::string& v) { title_ = v;}
            void SetBody(const std::string& v) { body_ = v;}

            const std::vector<std::string>& GetToEMailAddress() const { return to_email_address_;}
            const std::vector<std::string>& GetCcEMailAddress() const { return cc_email_address_;}
            const std::vector<std::string>& GetBccEMailAddress() const { return bcc_email_address_;}

            void SetToEMailAddress(const std::vector<std::string>& v) { to_email_address_ = v;}
            void SetCcEMailAddress(const std::vector<std::string>& v) { cc_email_address_ = v;}
            void SetBccEMailAddress(const std::vector<std::string>& v) { bcc_email_address_ = v;}

            void AddEntity(EMailEntity::ptr entity);
            const std::vector<EMailEntity::ptr>& GetEntitys() const { return entitys_;}
        private:
            std::string from_email_address_;
            std::string from_email_passwd_;
            std::string title_;
            std::string body_;
            std::vector<std::string> to_email_address_;
            std::vector<std::string> cc_email_address_;
            std::vector<std::string> bcc_email_address_;
            std::vector<EMailEntity::ptr> entitys_;
        };


    } // namespace smtp

} // namespace ygw
