/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: tests/test_config.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-02
 *  Description: 
 * ====================================================
 */
#include <server_frame/log.h>
#include <server_frame/config.h>
#include <server_frame/sys/env.h>
#include <iostream>
ygw::config::ConfigVar<int>::ptr g_int_value_config =
ygw::config::Config::Lookup("system.port", 8080, "system port");

ygw::config::ConfigVar<float>::ptr g_float_value_config =
ygw::config::Config::Lookup("system.value", 3.14f, "system value");

ygw::config::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config =
ygw::config::Config::Lookup("system.int_vec", std::vector<int>{1,2,3}, "system int vec");

ygw::config::ConfigVar<std::list<int>>::ptr g_int_lst_value_config =
ygw::config::Config::Lookup("system.int_lst", std::list<int>{11,22,33}, "system int lst");

ygw::config::ConfigVar<std::set<int>>::ptr g_int_set_value_config =
ygw::config::Config::Lookup("system.int_set", std::set<int>{110,119}, "system int set");

ygw::config::ConfigVar<std::unordered_set<int>>::ptr g_int_uset_value_config =
ygw::config::Config::Lookup("system.int_uset", std::unordered_set<int>{110,119}, "system int uset");

ygw::config::ConfigVar<std::map<std::string, int>>::ptr g_int_map_value_config =
ygw::config::Config::Lookup("system.int_map", std::map<std::string, int>{{"a", 1}, {"b", 2}}, "system int map");

ygw::config::ConfigVar<std::unordered_map<std::string, int>>::ptr g_int_umap_value_config =
ygw::config::Config::Lookup("system.int_umap", std::unordered_map<std::string, int>{{"a", 1}, {"b", 2}}, "system int umap");

void print_yaml(const YAML::Node& node, int level)
{
    if (node.IsScalar())
    {
        YGW_LOG_INFO(YGW_LOG_ROOT()) << std::string(level * 4, ' ')
            << node.Scalar() << " - " << node.Type() << " - " << level;
    }
    else if (node.IsNull())
    {
        YGW_LOG_INFO(YGW_LOG_ROOT()) << std::string(level * 4, ' ')
            << "NULL - " << node.Type() << " - " << level;
    }
    else if (node.IsMap())
    {
        for (auto ib = node.begin(), ie = node.end();
                ib != ie; ++ib)
        {
            YGW_LOG_INFO(YGW_LOG_ROOT()) << std::string(level * 4, ' ')
                << ib->first <<  " - " << node.Type() << " - " << level;
            print_yaml(ib->second, level + 1);
        }
    }
    else if (node.IsSequence())
    {
        for (std::size_t i = 0; i < node.size(); ++i)
        {
            YGW_LOG_INFO(YGW_LOG_ROOT()) << std::string(level * 4, ' ')
                << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

void test_yaml()
{
    YAML::Node root = YAML::LoadFile("./bin/conf/test.yml");
    print_yaml(root, 0);
    //YGW_LOG_INFO(YGW_LOG_ROOT()) << root;
    YGW_LOG_INFO(YGW_LOG_ROOT()) << root.Scalar();
}

void test_config() 
{
    YGW_LOG_INFO(YGW_LOG_ROOT()) << "before: " << g_int_value_config->GetValue();
    YGW_LOG_INFO(YGW_LOG_ROOT()) << "before: " << g_float_value_config->GetValue();

#define XX(g_var, name, prefix) \
    { \
        auto& v = g_var->GetValue(); \
        for (auto& i : v) { \
            YGW_LOG_INFO(YGW_LOG_ROOT()) << #prefix" "#name  << i; \
        } \
        YGW_LOG_INFO(YGW_LOG_ROOT()) << #prefix" "#name" yaml: " << g_var->ToString(); \
    }

#define YY(g_var, name, prefix) \
    { \
        auto& v = g_var->GetValue(); \
        for (auto& i : v) { \
            YGW_LOG_INFO(YGW_LOG_ROOT()) << #prefix" "#name  << i.first << " : " << i.second; \
        } \
        YGW_LOG_INFO(YGW_LOG_ROOT()) << #prefix" "#name" yaml: " << g_var->ToString(); \
    }
    XX(g_int_vec_value_config, ivec, before)
    XX(g_int_lst_value_config, ilst, before)
    XX(g_int_set_value_config, iset, before)
    XX(g_int_uset_value_config, iuset, before)
    YY(g_int_map_value_config, imap, before)
    YY(g_int_umap_value_config, iumap, before)
    

    YAML::Node root = YAML::LoadFile("./bin/conf/test.yml");
    ygw::config::Config::LoadFromYaml(root);

    YGW_LOG_INFO(YGW_LOG_ROOT()) << "after: " << g_int_value_config->GetValue();
    YGW_LOG_INFO(YGW_LOG_ROOT()) << "after: " << g_float_value_config->GetValue();

    XX(g_int_vec_value_config, ivec, after)
    XX(g_int_lst_value_config, ilst, after)
    XX(g_int_set_value_config, iset, after)
    XX(g_int_uset_value_config, iuset, after)
    YY(g_int_map_value_config, imap, after)
    YY(g_int_umap_value_config, iumap, after)
#undef YY
#undef XX
}


class Person {
public:
    Person() 
    {

    };


    std::string ToString() const 
    {
        std::stringstream ss;
        ss << "[Person name=" << name_
            << " age=" << age_
            << " sex=" << sex_
            << "]";
        return ss.str();
    }

    bool operator==(const Person& oth) const 
    {
        return name_ == oth.name_
            && age_  == oth.age_
            && sex_  == oth.sex_;
    }
//private:
    std::string name_;
    int age_ = 0;
    bool sex_ = 0;
};

namespace ygw{
    namespace config {
    template<>
    class LexicalCast<std::string, Person> {
    public:
        Person operator()(const std::string& v) {
            YAML::Node node = YAML::Load(v);
            Person p;
            p.name_ = node["name"].as<std::string>();
            p.age_ = node["age"].as<int>();
            p.sex_ = node["sex"].as<bool>();
            return p;
        }
    };

    template<>
    class LexicalCast<Person, std::string> {
    public:
        std::string operator()(const Person& p) {
            YAML::Node node;
            node["name"] = p.name_;
            node["age"] = p.age_;
            node["sex"] = p.sex_;
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };
    }
}

ygw::config::ConfigVar<Person>::ptr g_person =
ygw::config::Config::Lookup("class.person", Person(), "system person");

ygw::config::ConfigVar<std::map<std::string, Person> >::ptr g_person_map =
ygw::config::Config::Lookup("class.map", std::map<std::string, Person>(), "system person");

ygw::config::ConfigVar<std::map<std::string, std::vector<Person> > >::ptr g_person_vec_map =
ygw::config::Config::Lookup("class.vec_map", std::map<std::string, std::vector<Person> >(), "system person");

void test_class()
{
        YGW_LOG_INFO(YGW_LOG_ROOT()) << "before: " << g_person->GetValue().ToString() << " - " << g_person->ToString();

#define XX_PM(g_var, prefix) \
        { \
            auto m = g_person_map->GetValue(); \
            for(auto& i : m) { \
                YGW_LOG_INFO(YGW_LOG_ROOT()) <<  prefix << ": " << i.first << " - " << i.second.ToString(); \
            } \
            YGW_LOG_INFO(YGW_LOG_ROOT()) <<  prefix << ": size=" << m.size(); \
        }

        g_person->AddListener([](const Person& old_value, const Person& new_value){
                YGW_LOG_INFO(YGW_LOG_ROOT()) << "old_value=" << old_value.ToString()
                << " new_value=" << new_value.ToString();
                });

        XX_PM(g_person_map, "class.map before");
        YGW_LOG_INFO(YGW_LOG_ROOT()) << "before: " << g_person_vec_map->ToString();

        YAML::Node root = YAML::LoadFile("./bin/conf/test.yml");
        ygw::config::Config::LoadFromYaml(root);

        YGW_LOG_INFO(YGW_LOG_ROOT()) << "after: " << g_person->GetValue().ToString() << " - " << g_person->ToString();
        XX_PM(g_person_map, "class.map after");
        YGW_LOG_INFO(YGW_LOG_ROOT()) << "after: " << g_person_vec_map->ToString();
#undef XX_PM
}

void test_log()
{
    static ygw::log::Logger::ptr system_log = YGW_LOG_NAME("system");
    YGW_LOG_INFO(system_log) << "hello system" << std::endl;
    std::cout << ygw::log::LogManager::GetInstance()->ToYamlString() << std::endl;
    YAML::Node root = YAML::LoadFile("./bin/conf/log.yml");
    ygw::config::Config::LoadFromYaml(root);
    std::cout << "------------------" << std::endl;
    std::cout << ygw::log::LogManager::GetInstance()->ToYamlString() << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << root << std::endl;
    YGW_LOG_INFO(system_log) << "hello system" << std::endl;
}

void test_visit()
{
    ygw::config::Config::Visit([](ygw::config::ConfigVarBase::ptr var) {
        YGW_LOG_INFO(YGW_LOG_ROOT()) << "name = " << var->GetName()
        << " description = " << var->GetDescription()
        << " typename = " << var->GetTypeName()
        << " value = " << var->ToString();
    });
}

void test_my()
{
    YAML::Node conf = YAML::LoadFile("./bin/conf/http_server.yml");
    ygw::config::Config::LoadFromYaml(conf);
    //ygw::config::ConfigVar<std::string>::ptr root_path = ygw::config::Config::Lookup("conf.root_path", "./", "http server root path");
    ygw::config::ConfigVar<int>::ptr num = ygw::config::Config::Lookup("conf.num", 123, "http server root path");
    //ygw::config::ConfigVar<std::string>::ptr root_path = nullptr;
    //YGW_LOG_INFO(g_logger) << root_path;
}

void test_loadconf()
{
    ygw::config::Config::LoadFromConfDir("conf");
}


int main(int argc, char* argv[])
{
    //YGW_LOG_INFO(YGW_LOG_ROOT()) << g_int_value_config->GetValue();
    //YGW_LOG_INFO(YGW_LOG_ROOT()) << g_float_value_config->ToString();
    //test_yaml();
    //test_config();
    //test_class();
    //test_log();

    ygw::sys::EnvManager::GetInstance()->Init(argc, argv);
    test_visit();
    std::cout << "------------------------------------------------------" << std::endl;
    test_loadconf();
    test_visit();


    return 0;
}
