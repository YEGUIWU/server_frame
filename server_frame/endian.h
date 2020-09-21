/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: server_frame/endian.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-13
 *  Description: 字节序操作函数(大端/小端)
 * ====================================================
 */
#ifndef __YGW_ENDIAN_H__
#define __YGW_ENDIAN_H__

#define YGW_LITTLE_ENDIAN 1
#define YGW_BIG_ENDIAN 2

#include <byteswap.h>
#include <cstdint>

namespace ygw {

    namespace util {

        //---------------------------------------------------------------------

        /**
         ** @brief 8字节类型的字节序转化
         **/
        template<class T>
        typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
        Byteswap(T value) 
        {
            return (T)bswap_64((uint64_t)value);
        }

        /**
         ** @brief 4字节类型的字节序转化
         **/
        template<class T>
        typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
        Byteswap(T value) 
        {
            return (T)bswap_32((uint32_t)value);
        }

        /**
         ** @brief 2字节类型的字节序转化
         **/
        template<class T>
        typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
        Byteswap(T value) 
        {
            return (T)bswap_16((uint16_t)value);
        }

#if BYTE_ORDER == BIG_ENDIAN
#define YGW_BYTE_ORDER YGW_BIG_ENDIAN
#else
#define YGW_BYTE_ORDER YGW_LITTLE_ENDIAN
#endif //BYTE_ORDER == BIG_ENDIAN



#if YGW_BYTE_ORDER == YGW_BIG_ENDIAN 
//-----------------------------------------------------------------------------
// 大端字节序
        /**
         ** @brief 只在小端机器上执行byteswap, 在大端机器上什么都不做
         **/
        template<class T>
        T ByteswapOnLittleEndian(T t) 
        {
            return t;
        }

        /**
         ** @brief 只在大端机器上执行byteswap, 在小端机器上什么都不做
         **/
        template<class T>
        T ByteswapOnBigEndian(T t) 
        {
            return Byteswap(t);
        }
#else                              
//-----------------------------------------------------------------------------
// 小端字节序
        /**
         ** @brief 只在小端机器上执行byteswap, 在大端机器上什么都不做
         **/
        template<class T>
        T ByteswapOnLittleEndian(T t) 
        {
            return Byteswap(t);
        }

        /**
         ** @brief 只在大端机器上执行byteswap, 在小端机器上什么都不做
         **/
        template<class T>
        T ByteswapOnBigEndian(T t) 
        {
            return t;
        }
#endif // YGW_BYTE_ORDER == YGW_BIG_ENDIAN



    } // namespace util

} // namespace ygw

#endif // __YGW_ENDIAN_H__
