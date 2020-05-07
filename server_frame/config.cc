/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: config.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-02
 *  Description: 
 * ====================================================
 */
#include <sys/stat.h>
#include <sys/types.h>

#ifdef __GNU__
#include <unistd.h>
#endif //__GNU__

#ifdef _MSC_VER
#include <io.h>
#include <fcntl.h>
#endif// _MSC_VER

#include <utility>

#include "config.h"
namespace ygw {

    namespace config {


        static ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

        ConfigVarBase::ptr Config::LookupBase(const std::string& name) 
        {
            RWMutexType::ReadLock lock(GetMutex());
            auto it = GetDatas().find(name);
            return it == GetDatas().end() ? nullptr : it->second;
        }

        static void ListAllMember(const std::string& prefix,
                const YAML::Node& node,
                std::list<std::pair<std::string, const YAML::Node> >* output) 
        {
            if (prefix.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678")
                    != std::string::npos) //非法
            {
                YGW_LOG_ERROR(g_logger) << "Config invalid name: " << prefix << " : " << node;
                return;
            }

            output->push_back(std::make_pair(prefix, node));
            if (node.IsMap()) 
            {
                for (auto ib = node.begin(), ie = node.end();
                        ib != ie; ++ib) 
                {//递归添加
                    ListAllMember(prefix.empty() ? ib->first.Scalar()
                            : prefix + "." + ib->first.Scalar(), ib->second, output);
                }
            }
        }

        void Config::LoadFromYaml(const YAML::Node& root) 
        {
            std::list<std::pair<std::string, const YAML::Node> > all_nodes;
            ListAllMember("", root, &all_nodes);//将所有信息存入列表

            //遍历列表
            std::string key;
            for (auto& i : all_nodes) 
            {
                key = i.first;
                if (key.empty()) 
                {
                    continue;
                }
                //全部转小写
                std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                ConfigVarBase::ptr var = LookupBase(key);

                if (var) 
                {
                    if (i.second.IsScalar()) 
                    {
                        var->FromString(i.second.Scalar());
                    } 
                    else
                    {
                        std::stringstream ss;
                        ss << i.second;
                        var->FromString(ss.str());
                    }
                }
            }

        }


        void Config::LoadFromYamlFile(const std::string& file_path)
        {
             LoadFromYaml(YAML::LoadFile(file_path));
        }

        static std::map<std::string, uint64_t> s_file2modifytime;
        static ygw::thread::Mutex s_mutex;

        void Config::LoadFromConfDir(const std::string& path, bool force) 
        {
            //std::string absoulte_path = sylar::EnvMgr::GetInstance()->getAbsolutePath(path);
            std::vector<std::string> files;
            //FSUtil::ListAllFile(files, absoulte_path, ".yml");

            for (auto& i : files) 
            {
                {
#ifdef __GNUC__
                    struct stat st;
                    lstat(i.c_str(), &st);
#elif _MSC_VER
                    struct _stat st;
                    int fd, result;

                    _sopen_s(&fd,
                            i.c_str(),
                            _O_CREAT | _O_WRONLY | _O_TRUNC,
                            _SH_DENYNO,
                            _S_IREAD | _S_IWRITE);
                    if (fd != -1)
                    {
                        result = _fstat(fd, &st);
                    }
                    else
                    {

                    }
#endif 
                    
                    ygw::thread::Mutex::Lock lock(s_mutex);
                    if (!force && s_file2modifytime[i] == (uint64_t)st.st_mtime) 
                    {
                        continue;
                    }
                    s_file2modifytime[i] = st.st_mtime;
                }
                try 
                {
                    YAML::Node root = YAML::LoadFile(i);
                    LoadFromYaml(root);
                    YGW_LOG_INFO(g_logger) << "LoadConfFile file="
                        << i << " ok";
                } 
                catch (...) 
                {
                    YGW_LOG_ERROR(g_logger) << "LoadConfFile file="
                        << i << " failed";
                }
            }

        }

        void Config::Visit(std::function<void(ConfigVarBase::ptr)> cb) 
        {
            RWMutexType::ReadLock lock(GetMutex()); 
            ConfigVarMap& m = GetDatas();
            for (auto ib = m.begin(), ie = m.end();
                    ib != ie; ++ib) 
            {
                cb(ib->second);
            }
        }
    } // namespace config


    //-----------------------------------------------------------------------


} // namespace ygw  

