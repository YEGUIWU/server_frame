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
#include <config.h>
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
    YAML::Node root = YAML::LoadFile("./bin/conf/log.yml");
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
    

    YAML::Node root = YAML::LoadFile("./bin/conf/log.yml");
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


int main()
{
    //YGW_LOG_INFO(YGW_LOG_ROOT()) << g_int_value_config->GetValue();
    //YGW_LOG_INFO(YGW_LOG_ROOT()) << g_float_value_config->ToString();
    //test_yaml();
    test_config();
    return 0;
}
