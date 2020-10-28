/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: json_util.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-09-21
 *  Description: 
 * ====================================================
 */
#ifndef __YGW_UTIL_JSON_UTIL_H__
#define __YGW_UTIL_JSON_UTIL_H__

#include <string>
#include <iostream>
#include <json/json.h>

namespace ygw {

    //---------------------------------------------------------------

    namespace util {

        class JsonUtil {
        public:
            /**
             * @brief 是否需要处理转义
             * @param[in] v 需要判断的字符串
             * @return 是否需要处理
             */
			static bool NeedEscape(const std::string& v);

            /**
             * @brief 处理转移字符
             */
			static std::string Escape(const std::string& v);

            /**
             * @brief  
             */
			static std::string GetString(const Json::Value& json
					                    ,const std::string& name
					                    ,const std::string& default_value = "");
			static double GetDouble(const Json::Value& json
					               ,const std::string& name
					               ,double default_value = 0);
			static int32_t GetInt32(const Json::Value& json
					               ,const std::string& name
					               ,int32_t default_value = 0);
			static uint32_t GetUint32(const Json::Value& json
					                 ,const std::string& name
					                 ,uint32_t default_value = 0);
			static int64_t GetInt64(const Json::Value& json
					               ,const std::string& name
					               ,int64_t default_value = 0);
			static uint64_t GetUint64(const Json::Value& json
					                 ,const std::string& name
					                 ,uint64_t default_value = 0);
			static bool FromString(Json::Value& json, const std::string& v);

			static std::string ToString(const Json::Value& json);

        }; // class JsonUtil

    } // namespace util

    //---------------------------------------------------------------

} // namespace ygw 

#endif // __YGW_UTIL_JSON_UTIL_H__
