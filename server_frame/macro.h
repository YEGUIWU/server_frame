/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/macro.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-05
 *  Description: 
 * ====================================================
 */
#ifndef __YGW_MACRO_H__
#define __YGW_MACRO_H__

#include <string.h>
#include <assert.h>
#include "util.h"

#define YGW_ASSERT(x) \
    if (!(x)) { \
        YGW_LOG_ERROR(YGW_LOG_ROOT()) << "ASSERTION: " #x \
        << "\nbacktrace:\n" \
        << ygw::util::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define YGW_MSG_ASSERT(x, w) \
    if (!(x)) { \
        YGW_LOG_ERROR(YGW_LOG_ROOT()) << "ASSERTION: " #x \
        << "\n" << w \
        << "\nbacktrace:\n" \
        << ygw::util::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }
#endif // __YGW_MACRO_H__
