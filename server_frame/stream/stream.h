/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: stream.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-19
 *  Description: 流接口封装
 * ====================================================
 */

#ifndef __YGW_STREAM_H__
#define __YGW_STREAM_H__

#include <memory>

#include "server_frame/bytearray.h"

namespace ygw {

    //--------------------------------------------------------------------

    namespace stream {

        //-----------------------------------------------------------------

        /**
         * @brief 流结构
         */
        class Stream {
        public:
            using ptr = std::shared_ptr<Stream>;

            /**
             * @brief 析构函数
             */
            virtual ~Stream() {}

            /**
             * @brief 读数据
             * @param[out] buffer 接收数据的内存
             * @param[in] length 接收数据的内存大小
             * @return
             *      @retval >0 返回接收到的数据的实际大小
             *      @retval =0 被关闭
             *      @retval <0 出现流错误
             */
            virtual int Read(void* buffer, size_t length) = 0;

            /**
             * @brief 读数据
             * @param[out] ba 接收数据的container::ByteArray
             * @param[in] length 接收数据的内存大小
             * @return
             *      @retval >0 返回接收到的数据的实际大小
             *      @retval =0 被关闭
             *      @retval <0 出现流错误
             */
            virtual int Read(container::ByteArray::ptr ba, size_t length) = 0;

            /**
             * @brief 读固定长度的数据
             * @param[out] buffer 接收数据的内存
             * @param[in] length 接收数据的内存大小
             * @return
             *      @retval >0 返回接收到的数据的实际大小
             *      @retval =0 被关闭
             *      @retval <0 出现流错误
             */
            virtual int ReadFixSize(void* buffer, size_t length);

            /**
             * @brief 读固定长度的数据
             * @param[out] ba 接收数据的container::ByteArray
             * @param[in] length 接收数据的内存大小
             * @return
             *      @retval >0 返回接收到的数据的实际大小
             *      @retval =0 被关闭
             *      @retval <0 出现流错误
             */
            virtual int ReadFixSize(container::ByteArray::ptr ba, size_t length);

            /**
             * @brief 写数据
             * @param[in] buffer 写数据的内存
             * @param[in] length 写入数据的内存大小
             * @return
             *      @retval >0 返回写入到的数据的实际大小
             *      @retval =0 被关闭
             *      @retval <0 出现流错误
             */
            virtual int Write(const void* buffer, size_t length) = 0;

            /**
             * @brief 写数据
             * @param[in] ba 写数据的container::ByteArray
             * @param[in] length 写入数据的内存大小
             * @return
             *      @retval >0 返回写入到的数据的实际大小
             *      @retval =0 被关闭
             *      @retval <0 出现流错误
             */
            virtual int Write(container::ByteArray::ptr ba, size_t length) = 0;

            /**
             * @brief 写固定长度的数据
             * @param[in] buffer 写数据的内存
             * @param[in] length 写入数据的内存大小
             * @return
             *      @retval >0 返回写入到的数据的实际大小
             *      @retval =0 被关闭
             *      @retval <0 出现流错误
             */
            virtual int WriteFixSize(const void* buffer, size_t length);

            /**
             * @brief 写固定长度的数据
             * @param[in] ba 写数据的container::ByteArray
             * @param[in] length 写入数据的内存大小
             * @return
             *      @retval >0 返回写入到的数据的实际大小
             *      @retval =0 被关闭
             *      @retval <0 出现流错误
             */
            virtual int WriteFixSize(container::ByteArray::ptr ba, size_t length);

            /**
             * @brief 关闭流
             */
            virtual void Close() = 0;
        };



        //-----------------------------------------------------------------

    } // namespace stream

    //--------------------------------------------------------------------

} // namespace ygw

#endif // __YGW_STREAM_H__
