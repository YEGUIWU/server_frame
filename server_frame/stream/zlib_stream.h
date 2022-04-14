/**
 * @file zlib_stream.h
 * @brief 通过zlib压缩解压的流
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */
#ifndef __YGW_ZLIB_STREAM_H__
#define __YGW_ZLIB_STREAM_H__

#include <memory>
#include <stdint.h>
#include <string>
#include <sys/uio.h>
#include <vector>
#include <zlib.h>

#include "server_frame/stream/stream.h"
#include "server_frame/macro.h"
#include "server_frame/log.h"

namespace ygw {

    namespace stream {

        class ZlibStream : public Stream {
        public:
            using ptr = std::shared_ptr<ZlibStream>;
        
            enum Type {
                kZLib,
                kDeflate,
                kGZip
            };
        
            enum Strategy {
                kDefault  = Z_DEFAULT_STRATEGY,
                kFiltered = Z_FILTERED,
                kHuffman  = Z_HUFFMAN_ONLY,
                kFixed    = Z_FIXED,
                kRle      = Z_RLE
            };
        
            enum CompressLevel {
                kNoCompression      = Z_NO_COMPRESSION,
                kBestSpeed          = Z_BEST_SPEED,
                kBestCompression    = Z_BEST_COMPRESSION,
                kDefaultCompression = Z_DEFAULT_COMPRESSION
            };
        
            static ZlibStream::ptr CreateGzip(bool encode, uint32_t buff_size = 4096);
            static ZlibStream::ptr CreateZlib(bool encode, uint32_t buff_size = 4096);
            static ZlibStream::ptr CreateDeflate(bool encode, uint32_t buff_size = 4096);
            static ZlibStream::ptr Create(bool encode, uint32_t buff_size = 4096,
                    Type type = kDeflate, int level = kDefaultCompression, int window_bits = 15
                    ,int memlevel = 8, Strategy strategy = kDefault);
        
            ZlibStream(bool encode, uint32_t buff_size = 4096);
            ~ZlibStream();
        
            virtual int Read(void* buffer, size_t length) override;
            virtual int Read(ygw::container::ByteArray::ptr ba, size_t length) override;
            virtual int Write(const void* buffer, size_t length) override;
            virtual int Write(ygw::container::ByteArray::ptr ba, size_t length) override;
            virtual void Close() override;
        
            int Flush();
        
            bool IsFree() const { return free_;}
            void SetFree(bool v) { free_ = v;}
        
            bool IsEncode() const { return encode_;}
            void SetEndcode(bool v) { encode_ = v;}
        
            std::vector<iovec>& GetBuffers() { return buffs_;}
            std::string GetResult() const;
            ygw::container::ByteArray::ptr GetByteArray();
        private:
            int Init(Type type = kDeflate, int level = kDefaultCompression 
                     ,int window_bits = 15, int memlevel = 8, Strategy strategy = kDefault);
        
            int Encode(const iovec* v, const uint64_t& size, bool finish);
            int Decode(const iovec* v, const uint64_t& size, bool finish);
        private:
            z_stream zstream_;
            uint32_t buff_size_;
            bool encode_;
            bool free_;
            std::vector<iovec> buffs_;
        }; // class ZlibStream
            
    } // namespace stream

} // namespace ygw


#endif // __YGW_ZLIB_STREAM_H__
