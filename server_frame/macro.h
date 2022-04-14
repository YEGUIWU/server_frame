/**
 * @file macro.h
 * @brief 常用宏的封装
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
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
