/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: ../tests/test_address.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-14
 *  Description: 
 * ====================================================
 */

#include <server_frame/address.h>
#include <server_frame/log.h>

ygw::log::Logger::ptr g_logger = YGW_LOG_ROOT();

// 测试地址
void test_address()
{
    std::vector<ygw::socket::Address::ptr> addrs;
    bool v = ygw::socket::Address::Lookup(addrs, "www.baidu.com:80", AF_INET, SOCK_SEQPACKET);
    if (!v)
    {
        YGW_LOG_ERROR(g_logger) << "lookup fail";
    }

    for (size_t i = 0; i < addrs.size(); ++i)
    {
        YGW_LOG_INFO(g_logger) << i << " - " << addrs[i]->ToString();
    }
}


// 根据网卡 获取本地地址
void test_iface()
{
    std::multimap<std::string, std::pair<ygw::socket::Address::ptr, uint32_t>> results;
    bool v = ygw::socket::Address::GetInterfaceAddresses(results, AF_UNSPEC);
    if (!v)
    {
        YGW_LOG_ERROR(g_logger) << "GetInterfaceAddress fail";
        return;
    }
    for (auto& i : results)
    {
        YGW_LOG_INFO(g_logger) << i.first << " - " << i.second.first->ToString() << " - "
            << i.second.second;
    }

}

// 测试IPv4
void test_ipv4()
{
    auto addr = ygw::socket::IPv4Address::Create("www.baidu.com");
    //auto addr = ygw::socket::IPv4Address::Create("127.0.0.8");
    if (addr)
    {
        YGW_LOG_INFO(g_logger) << addr->ToString();
    }
}

int main(int argc, char** argv)
{
    test_address();
    //test_iface();
    //test_ipv4();
    return 0;
}
