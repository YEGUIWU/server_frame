/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/macro.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-05
 *  Description: 常用宏的封装
 * ====================================================
 */
#ifndef __YGW_MACRO_H__
#define __YGW_MACRO_H__

#include <string.h>
#include <assert.h>
#include "util.h"

#if defined __GNUC__ || defined __llvm__
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率成立
#   define YGW_LIKELY(x)       __builtin_expect(!!(x), 1)
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率不成立
#   define YGW_UNLIKELY(x)     __builtin_expect(!!(x), 0)
#else
#   define YGW_LIKELY(x)      (x)
#   define YGW_UNLIKELY(x)      (x)
#endif

#define YGW_ASSERT(x) \
    if (YGW_UNLIKELY(!(x))) { \
        YGW_LOG_ERROR(YGW_LOG_ROOT()) << "ASSERTION: " #x \
        << "\nbacktrace:\n" \
        << ygw::util::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define YGW_MSG_ASSERT(x, w) \
    if (YGW_UNLIKELY(!(x))) { \
        YGW_LOG_ERROR(YGW_LOG_ROOT()) << "ASSERTION: " #x \
        << "\n" << w \
        << "\nbacktrace:\n" \
        << ygw::util::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }
#endif // __YGW_MACRO_H__
