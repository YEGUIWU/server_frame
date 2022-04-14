/**
 * @file crypto_util.h
 * @brief 加解密工具函数
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */

#ifndef __YGW_CRYPTO_UTIL_H__
#define __YGW_CRYPTO_UTIL_H__

#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <stdint.h>
#include <memory>
#include <string>

namespace ygw {

    //------------------------------------------------------------

    namespace util {

        class CryptoUtil {
        public:
            //key 32字节
            static int32_t AES256Ecb(const void* key
                                    ,const void* in
                                    ,int32_t in_len
                                    ,void* out
                                    ,bool encode);
        
            //key 16字节
            static int32_t AES128Ecb(const void* key
                                    ,const void* in
                                    ,int32_t in_len
                                    ,void* out
                                    ,bool encode);
        
            //key 32字节
            //iv 16字节
            static int32_t AES256Cbc(const void* key, const void* iv
                                    ,const void* in, int32_t in_len
                                    ,void* out, bool encode);
        
            //key 16字节
            //iv 16字节
            static int32_t AES128Cbc(const void* key, const void* iv
                                    ,const void* in, int32_t in_len
                                    ,void* out, bool encode);
        
            static int32_t Crypto(const EVP_CIPHER* cipher, bool enc
                                  ,const void* key, const void* iv
                                  ,const void* in, int32_t in_len
                                  ,void* out, int32_t* out_len);
        };


        class RSACipher {
        public:
            using ptr = std::shared_ptr<RSACipher>;
        
            static int32_t GenerateKey(const std::string& pubkey_file
                                       ,const std::string& prikey_file
                                       ,uint32_t length = 1024);
        
            static RSACipher::ptr Create(const std::string& pubkey_file
                                        ,const std::string& prikey_file);
        
            RSACipher();
            ~RSACipher();
        
            int32_t PrivateEncrypt(const void* from, int flen,
                                   void* to, int padding = RSA_NO_PADDING);
            int32_t PublicEncrypt(const void* from, int flen,
                                   void* to, int padding = RSA_NO_PADDING);
            int32_t PrivateDecrypt(const void* from, int flen,
                                   void* to, int padding = RSA_NO_PADDING);
            int32_t PublicDecrypt(const void* from, int flen,
                                   void* to, int padding = RSA_NO_PADDING);
            int32_t PrivateEncrypt(const void* from, int flen,
                                   std::string& to, int padding = RSA_NO_PADDING);
            int32_t PublicEncrypt(const void* from, int flen,
                                   std::string& to, int padding = RSA_NO_PADDING);
            int32_t PrivateDecrypt(const void* from, int flen,
                                   std::string& to, int padding = RSA_NO_PADDING);
            int32_t PublicDecrypt(const void* from, int flen,
                                   std::string& to, int padding = RSA_NO_PADDING);
        
        
            const std::string& GetPubkeyStr() const { return pubkey_str_;}
            const std::string& GetPrikeyStr() const { return prikey_str_;}
        
            int32_t GetPubRSASize();
            int32_t GetPriRSASize();
        private:
            RSA* pubkey_;
            RSA* prikey_;
            std::string pubkey_str_;
            std::string prikey_str_;
        };

    }

    //------------------------------------------------------------

} // namespace ygw

#endif // __YGW_CRYPTO_UTIL_H__
