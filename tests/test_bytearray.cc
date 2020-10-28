/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: tests/test_bytearray.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-17
 *  Description: 测试ByteArray
 * ====================================================
 */

//#include <vector>

#include <server_frame/bytearray.h>
#include <server_frame/log.h>
#include <server_frame/macro.h>

ygw::log::Logger::ptr g_logger = YGW_LOG_NAME("system");

void test()
{
#define XX(type, len, write_fun, read_fun, base_len) { \
    std::vector<type> vec; \
    for (int i = 0; i < len; ++i) { \
        vec.push_back(rand()); \
    } \
    ygw::container::ByteArray::ptr ba(new ygw::container::ByteArray(base_len));  \
    for (auto& i : vec) { \
        ba->write_fun(i); \
    } \
    ba->SetPosition(0); \
    for (size_t i = 0; i < vec.size(); ++i) {  \
        type v = ba->read_fun();  \
        YGW_ASSERT(v == vec[i]);  \
    } \
    YGW_ASSERT(ba->GetReadSize() == 0); \
    YGW_LOG_INFO(g_logger) << #write_fun "/" #read_fun \
    " (" #type " ) len=" << len \
    << " base_len=" << base_len \
    << " size=" << ba->GetSize(); \
}

    XX(int8_t, 100, WriteFint8, ReadFint8, 1); 
    XX(uint8_t, 100, WriteFuint8, ReadFuint8, 1); 
    XX(int16_t, 100, WriteFint16, ReadFint16, 1); 
    XX(uint16_t, 100, WriteFuint16, ReadFuint16, 1); 
    XX(int32_t, 100, WriteFint32, ReadFint32, 1); 
    XX(uint32_t, 100, WriteFuint32, ReadFuint32, 1); 
    XX(int64_t, 100, WriteFint64, ReadFint64, 1); 
    XX(uint64_t, 100, WriteFuint64, ReadFuint64, 1); 

    XX(int32_t, 100, WriteInt32, ReadInt32, 1);
    XX(uint32_t, 100, WriteUint32, ReadUint32, 1);
    XX(int64_t, 100, WriteInt64, ReadInt64, 1);
    XX(uint64_t, 100, WriteUint64, ReadUint64, 1);
#undef XX 

#define XX(type, len, write_fun, read_fun, base_len) { \
    std::vector<type> vec; \
    for (int i = 0; i < len; ++i) { \
        vec.push_back(rand()); \
    } \
    ygw::container::ByteArray::ptr ba(new ygw::container::ByteArray(base_len));  \
    for (auto& i : vec) { \
        ba->write_fun(i); \
    } \
    ba->SetPosition(0); \
    for (size_t i = 0; i < vec.size(); ++i) {  \
        type v = ba->read_fun();  \
        YGW_ASSERT(v == vec[i]);  \
    } \
    YGW_ASSERT(ba->GetReadSize() == 0); \
    YGW_LOG_INFO(g_logger) << #write_fun "/" #read_fun \
    " (" #type " ) len=" << len \
    << " base_len=" << base_len \
    << " size=" << ba->GetSize(); \
    ba->SetPosition(0); \
    YGW_ASSERT(ba->WriteToFile("./tmp/" #type "_" #len "-" #read_fun ".dat"));  \
    ygw::container::ByteArray::ptr ba2(new ygw::container::ByteArray(base_len * 2)); \
    YGW_ASSERT(ba2->ReadFromFile("./tmp/" #type "_" #len "-" #read_fun ".dat")); \
    ba2->SetPosition(0); \
    YGW_ASSERT(ba->ToString() == ba2->ToString()); \
    YGW_ASSERT(ba->GetPosition() == 0); \
    YGW_ASSERT(ba->GetPosition() == 0); \
}
    
    XX(int8_t, 100, WriteFint8, ReadFint8, 1); 
    XX(uint8_t, 100, WriteFuint8, ReadFuint8, 1); 
    XX(int16_t, 100, WriteFint16, ReadFint16, 1); 
    XX(uint16_t, 100, WriteFuint16, ReadFuint16, 1); 
    XX(int32_t, 100, WriteFint32, ReadFint32, 1); 
    XX(uint32_t, 100, WriteFuint32, ReadFuint32, 1); 
    XX(int64_t, 100, WriteFint64, ReadFint64, 1); 
    XX(uint64_t, 100, WriteFuint64, ReadFuint64, 1); 

    XX(int32_t, 100, WriteInt32, ReadInt32, 1);
    XX(uint32_t, 100, WriteUint32, ReadUint32, 1);
    XX(int64_t, 100, WriteInt64, ReadInt64, 1);
    XX(uint64_t, 100, WriteUint64, ReadUint64, 1);
#undef XX

}

int main(int argc, char** argv)
{
    test();


    return 0;
}
