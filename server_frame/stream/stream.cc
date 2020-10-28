/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/stream.cc
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-19
 *  Description: 
 * ====================================================
 */
#include "stream.h"

namespace ygw {

    //--------------------------------------------------------------------
    namespace stream {

        //--------------------------------------------------------------------

        int Stream::ReadFixSize(void* buffer, size_t length) 
        {
            size_t offset = 0;
            int64_t left = length;
            int64_t len;
            while (left > 0) 
            {
                len = Read((char*)buffer + offset, left);
                if (len <= 0) 
                {
                    return len;
                }
                offset += len;
                left -= len;
            }
            return length;
        }

        int Stream::ReadFixSize(container::ByteArray::ptr ba, size_t length) {
            int64_t left = length;
            int64_t len;
            while (left > 0) 
            {
                len = Read(ba, left);
                if (len <= 0) 
                {
                    return len;
                }
                left -= len;
            }
            return length;
        }

        int Stream::WriteFixSize(const void* buffer, size_t length) 
        {
            size_t offset = 0;
            int64_t left = length;
            int64_t len;
            while (left > 0) 
            {
                len = Write((const char*)buffer + offset, left);
                if (len <= 0) 
                {
                    return len;
                }
                offset += len;
                left -= len;
            }
            return length;

        }

        int Stream::WriteFixSize(container::ByteArray::ptr ba, size_t length) 
        {
            int64_t left = length;
            int len;
            while (left > 0) 
            {
                len = Write(ba, left);
                if (len <= 0) 
                {
                    return len;
                }
                left -= len;
            }
            return length;
        }

        //--------------------------------------------------------------------

    } // namespace stream

    //--------------------------------------------------------------------

} // namespace ygw
