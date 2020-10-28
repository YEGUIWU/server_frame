/**
 * @file server_frame/email/email.cc
 * @brief 
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2020-09-28
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.top
 */
#include <fstream>
#include <sstream>

#include <server_frame/util.h>
#include "email.h"

namespace ygw {

    namespace smtp {

        EMailEntity::ptr EMailEntity::CreateAttach(const std::string& filename) 
        {
            std::ifstream ifs(filename, std::ios::binary);
            std::string buf;
            buf.resize(1024);
            EMailEntity::ptr entity(new EMailEntity);
            while (!ifs.eof()) 
            {
                ifs.read(&buf[0], buf.size());
                entity->content_.append(buf.c_str(), ifs.gcount());
            }
            entity->content_ = ygw::util::base64encode(entity->content_);
            entity->AddHeader("Content-Transfer-Encoding", "base64");
            entity->AddHeader("Content-Disposition", "attachment");
            entity->AddHeader("Content-Type", "application/octet-stream;name=" + ygw::util::FSUtil::Basename(filename));
            return entity;
        }

        void EMailEntity::AddHeader(const std::string& key, const std::string& val) 
        {
            headers_[key] = val;
        }

        std::string EMailEntity::GetHeader(const std::string& key, const std::string& def) 
        {
            auto it = headers_.find(key);
            return it == headers_.end() ? def : it->second;
        }

        std::string EMailEntity::ToString() const 
        {
            std::stringstream ss;
            for (auto& i : headers_) 
            {
                ss << i.first << ": " << i.second << "\r\n";
            }
            ss << content_ << "\r\n";
            return ss.str();
        }

        EMail::ptr EMail::Create(const std::string& from_address, const std::string& from_passwd
                ,const std::string& title, const std::string& body
                ,const std::vector<std::string>& to_address
                ,const std::vector<std::string>& cc_address
                ,const std::vector<std::string>& bcc_address) 
        {
            EMail::ptr email(new EMail);
            email->SetFromEMailAddress(from_address);
            email->SetFromEMailPasswd(from_passwd);
            email->SetTitle(title);
            email->SetBody(body);
            email->SetToEMailAddress(to_address);
            email->SetCcEMailAddress(cc_address);
            email->SetBccEMailAddress(bcc_address);
            return email;
        }

        void EMail::AddEntity(EMailEntity::ptr entity) 
        {
            entitys_.push_back(entity);
        }


    } // namespace smtp

} // namespace ygw
