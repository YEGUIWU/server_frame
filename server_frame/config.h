/*
 * ====================================================
 * Copyright (c) 2020-2100
 *     FileName: config.h
 *       Author: Ye Gui Wu
 *        Email: yeguiwu@qq.com
 *      Version: 1.0
 *     Compiler: gcc
 *  Create Date: 2020-05-02
 *  Description: 
 * ====================================================
 */
#ifndef __YGW_CONFIG_H__
#define __YGW_CONFIG_H__


#include <cstdint>

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <set>
#include <list>
#include <unordered_map>
#include <unordered_set>

#include <yaml-cpp/yaml.h>
#include <boost/lexical_cast.hpp>

#include "log.h"
#include "util.h"
#include "mutex.h"


namespace ygw {

    //-------------------------------------------------

    namespace config {

        //-------------------------------------------
        class ConfigVarBase {
        public:
            using ptr = std::shared_ptr<ConfigVarBase>;

            /**
             ** @brief 构造函数
             ** @param[in] name 配置参数名称[0-9a-z_.]
             ** @param[in] description 配置参数描述
             **/
            ConfigVarBase(const std::string& name, 
                    const std::string& description = "")
                : name_(name)
                , description_(description)
            {
                std::transform(name_.begin(), name_.end(), name_.begin(), ::tolower);
            }

            /**
             ** @brief 析构函数
             **/
            virtual ~ConfigVarBase() {}

            /**
             ** @brief 返回配置参数名称
             **/
            const std::string& GetName() const { return name_; }

            /**
             ** @brief 返回配置参数的描述
             **/
            const std::string& GetDescription() const { return description_; }

            /**
             ** @brief 转成字符串
             **/
            virtual std::string ToString() = 0;

            /**
             ** @brief 从字符串初始化值
             **/
            virtual bool FromString(const std::string& val) = 0;

            /**
             ** @brief 返回配置参数值的类型名称
             **/
            virtual std::string GetTypeName() const = 0;
        protected:
            /// 配置参数的名称
            std::string name_;
            /// 配置参数的描述
            std::string description_;
        };

        //----------------------------------------------------------------
        /**
         ** @brief 类型转换模板类(F 源类型, T 目标类型)
         **/
        //F from_type, T to_type
        template<class F, class T>
        class LexicalCast {
        public:
            /**
             ** @brief 类型转换
             ** @param[in] v 源类型值
             ** @return 返回v转换后的目标类型
             ** @exception 当类型不可转换时抛出异常
             **/
            T operator()(const F& v) 
            {
                return boost::lexical_cast<T>(v);
            }
        };
        //----------------------------------------------------------------
        /**
         ** @brief 类型转换模板类片特化(YAML String 转换成 std::vector<T>)
         **/
        template<class T>
        class LexicalCast<std::string, std::vector<T> > {
        public:
            std::vector<T> operator()(const std::string& v) 
            {
                YAML::Node node = YAML::Load(v);
                typename std::vector<T> vec;
                std::stringstream ss;
                for (std::size_t i = 0; i < node.size(); ++i) 
                {
                    ss.str("");
                    ss << node[i];
                    vec.push_back(LexicalCast<std::string, T>()(ss.str()));
                }
                return vec;
            }
        };
		
		//----------------------------------------------------------------
		/**
		 * @brief 类型转换模板类片特化(std::vector<T> 转换成 YAML String)
		 */
		template<class T>
		class LexicalCast<std::vector<T>, std::string> {
		public:
			std::string operator()(const std::vector<T>& v) {
				YAML::Node node(YAML::NodeType::Sequence);
				for (auto& i : v) {
					node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
				}
				std::stringstream ss;
				ss << node;
				return ss.str();
			}
		};
        //----------------------------------------------------------------
		/**
		 * @brief 类型转换模板类片特化(YAML String 转换成 std::list<T>)
		 */
		template<class T>
		class LexicalCast<std::string, std::list<T> > {
		public:
			std::list<T> operator()(const std::string& v) {
				YAML::Node node = YAML::Load(v);
				typename std::list<T> vec;
				std::stringstream ss;
				for (size_t i = 0; i < node.size(); ++i) {
					ss.str("");
					ss << node[i];
					vec.push_back(LexicalCast<std::string, T>()(ss.str()));
				}
				return vec;
			}
		};
        //----------------------------------------------------------------
		/**
		 * @brief 类型转换模板类片特化(std::list<T> 转换成 YAML String)
		 */
		template<class T>
		class LexicalCast<std::list<T>, std::string> {
		public:
			std::string operator()(const std::list<T>& v) {
				YAML::Node node(YAML::NodeType::Sequence);
				for (auto& i : v) {
					node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
				}
				std::stringstream ss;
				ss << node;
				return ss.str();
			}
		};
        //----------------------------------------------------------------
		/**
		 * @brief 类型转换模板类片特化(YAML String 转换成 std::set<T>)
		 */
		template<class T>
		class LexicalCast<std::string, std::set<T> > {
		public:
			std::set<T> operator()(const std::string& v) {
				YAML::Node node = YAML::Load(v);
				typename std::set<T> vec;
				std::stringstream ss;
				for (size_t i = 0; i < node.size(); ++i) {
					ss.str("");
					ss << node[i];
					vec.insert(LexicalCast<std::string, T>()(ss.str()));
				}
				return vec;
			}
		};
        //----------------------------------------------------------------
		/**
		 * @brief 类型转换模板类片特化(std::set<T> 转换成 YAML String)
		 */
		template<class T>
		class LexicalCast<std::set<T>, std::string> {
		public:
			std::string operator()(const std::set<T>& v) {
				YAML::Node node(YAML::NodeType::Sequence);
				for (auto& i : v) {
					node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
				}
				std::stringstream ss;
				ss << node;
				return ss.str();
			}
		};
        //----------------------------------------------------------------
		/**
		 * @brief 类型转换模板类片特化(YAML String 转换成 std::unordered_set<T>)
		 */
		template<class T>
		class LexicalCast<std::string, std::unordered_set<T> > {
		public:
			std::unordered_set<T> operator()(const std::string& v) {
				YAML::Node node = YAML::Load(v);
				typename std::unordered_set<T> vec;
				std::stringstream ss;
				for (size_t i = 0; i < node.size(); ++i) {
					ss.str("");
					ss << node[i];
					vec.insert(LexicalCast<std::string, T>()(ss.str()));
				}
				return vec;
			}
		};
        //----------------------------------------------------------------
		/**
		 * @brief 类型转换模板类片特化(std::unordered_set<T> 转换成 YAML String)
		 */
		template<class T>
		class LexicalCast<std::unordered_set<T>, std::string> {
		public:
			std::string operator()(const std::unordered_set<T>& v) {
				YAML::Node node(YAML::NodeType::Sequence);
				for (auto& i : v) {
					node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
				}
				std::stringstream ss;
				ss << node;
				return ss.str();
			}
		};
        //----------------------------------------------------------------
		/**
		 * @brief 类型转换模板类片特化(YAML String 转换成 std::map<std::string, T>)
		 */
		template<class T>
		class LexicalCast<std::string, std::map<std::string, T> > {
		public:
			std::map<std::string, T> operator()(const std::string& v) {
				YAML::Node node = YAML::Load(v);
				typename std::map<std::string, T> vec;
				std::stringstream ss;
				for (auto it = node.begin();
						it != node.end(); ++it) {
					ss.str("");
					ss << it->second;
					vec.insert(std::make_pair(it->first.Scalar(),
								LexicalCast<std::string, T>()(ss.str())));
				}
				return vec;
			}
		};
        //----------------------------------------------------------------
		/**
		 * @brief 类型转换模板类片特化(std::map<std::string, T> 转换成 YAML String)
		 */
		template<class T>
		class LexicalCast<std::map<std::string, T>, std::string> {
		public:
			std::string operator()(const std::map<std::string, T>& v) 
            {
				YAML::Node node(YAML::NodeType::Map);
				for (auto& i : v) {
					node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
				}
				std::stringstream ss;
				ss << node;
				return ss.str();
			}
		};
        //----------------------------------------------------------------
		/**
		 * @brief 类型转换模板类片特化(YAML String 转换成 std::unordered_map<std::string, T>)
		 */
		template<class T>
		class LexicalCast<std::string, std::unordered_map<std::string, T> > {
		public:
			std::unordered_map<std::string, T> operator()(const std::string& v) 
            {
				YAML::Node node = YAML::Load(v);
				typename std::unordered_map<std::string, T> vec;
				std::stringstream ss;
				for (auto it = node.begin();
						it != node.end(); ++it) {
					ss.str("");
					ss << it->second;
					vec.insert(std::make_pair(it->first.Scalar(),
								LexicalCast<std::string, T>()(ss.str())));
				}
				return vec;
			}
		};
        //----------------------------------------------------------------
		/**
		 * @brief 类型转换模板类片特化(std::unordered_map<std::string, T> 转换成 YAML String)
		 */
		template<class T>
		class LexicalCast<std::unordered_map<std::string, T>, std::string> {
		public:
			std::string operator()(const std::unordered_map<std::string, T>& v) 
            {
				YAML::Node node(YAML::NodeType::Map);
				for (auto& i : v) {
					node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
				}
				std::stringstream ss;
				ss << node;
				return ss.str();
			}
		};




        //-------------------------------------------------------------------------------------------
        /**
         ** @brief 配置参数模板子类,保存对应类型的参数值
         ** @details T 参数的具体类型
         **          FromStr 从std::string转换成T类型的仿函数
         **          ToStr 从T转换成std::string的仿函数
         **          std::string 为YAML格式的字符串
         **/
        template<class T, class FromStr = LexicalCast<std::string, T>, 
            class ToStr = LexicalCast<T, std::string> >
        class ConfigVar : public ConfigVarBase 
        {
        public:
            using RWMutexType = thread::RWMutex;
            using ptr = std::shared_ptr<ConfigVar>;
            //配置变更回调
            using on_change_cb = std::function<void (const T& old_value, const T& new_value)>; 

            /**
             ** @brief 通过参数名,参数值,描述构造ConfigVar
             ** @param[in] name 参数名称有效字符为[0-9a-z_.]
             ** @param[in] default_value 参数的默认值
             ** @param[in] description 参数的描述
             **/
            ConfigVar(const std::string& name
                    ,const T& default_value
                    ,const std::string& description = "")
                :ConfigVarBase(name, description)
                ,val_(default_value) 
            {
            }

            /**
             ** @brief 将参数值转换成YAML String
             ** @exception 当转换失败抛出异常
             **/
            std::string ToString() override 
            {
                try 
                {
                    RWMutexType::ReadLock lock(mutex_);
                    return ToStr()(val_);
                }
                catch (std::exception& e) 
                {
                    YGW_LOG_ERROR(YGW_LOG_ROOT()) << "ConfigVar::ToString exception "
                        << e.what() << " convert: " << util::TypeToName<T>() << " to string"
                        << " name=" << name_;
                }
                return "";
            }

            /**
             ** @brief 从YAML String 转成参数的值
             ** @exception 当转换失败抛出异常
             **/
            bool FromString(const std::string& val) override 
            {
                try 
                {
                    SetValue(FromStr()(val));
                } 
                catch (std::exception& e) 
                {
                    YGW_LOG_ERROR(YGW_LOG_ROOT()) << "ConfigVar::fromString exception "
                        << e.what() << " convert: string to " << util::TypeToName<T>()
                        << " name=" << name_
                        << " - " << val;
                }
                return false;
            }

            /**
             ** @brief 获取当前参数的值
             **/
            const T GetValue() 
            {
                RWMutexType::ReadLock lock(mutex_);
                return val_;
            }

            /**
             ** @brief 设置当前参数的值
             ** @details 如果参数的值有发生变化,则通知对应的注册回调函数
             **/
            void SetValue(const T& v) 
            {
                {
                    RWMutexType::ReadLock lock(mutex_);
                    if (v == val_) 
                    {
                        return;
                    }
                    for (auto& i : cbs_) 
                    {
                        i.second(val_, v);
                    }
                }
                RWMutexType::ReadLock lock(mutex_);
                val_ = v;
            }

            /**
             ** @brief 返回参数值的类型名称(typeinfo)
             **/
            std::string GetTypeName() const override { return util::TypeToName<T>(); }


            //----------------------------------------------------
            //          on change call back
            //----------------------------------------------------
            /**
             ** @brief 添加变化回调函数
             ** @return 返回该回调函数对应的唯一id,用于删除回调
             **/
            uint64_t AddListener(on_change_cb cb) 
            {
                static uint64_t s_fun_id = 0;
                RWMutexType::ReadLock lock(mutex_);
                ++s_fun_id;
                cbs_[s_fun_id] = cb;
                return s_fun_id;
            }

            /**
             ** @brief 删除回调函数
             ** @param[in] key 回调函数的唯一id
             **/
            void DelListener(uint64_t key) 
            {
                RWMutexType::ReadLock lock(mutex_);
                cbs_.erase(key);
            }

            /**
             ** @brief 获取回调函数
             ** @param[in] key 回调函数的唯一id
             ** @return 如果存在返回对应的回调函数,否则返回nullptr
             **/
            on_change_cb GetListener(uint64_t key) 
            {
                RWMutexType::ReadLock lock(mutex_);
                auto it = cbs_.find(key);
                return it == cbs_.end() ? nullptr : it->second;
            }

            /**
             ** @brief 清理所有的回调函数
             **/
            void ClearListener() 
            {
                RWMutexType::ReadLock lock(mutex_);
                cbs_.clear();
            }
        private:
            /// mutex
            RWMutexType mutex_;
            /// value
            T val_;
            //变更回调函数组, uint64_t key,要求唯一，一般可以用hash
            //用map的原因：function对象无法比较，难以删除
            std::map<uint64_t, on_change_cb> cbs_;
        };
       
        //----------------------------------------------------------------------------------
        /**
         ** @brief ConfigVar的管理类
         ** @details 提供便捷的方法创建/访问ConfigVar
         **/
        class Config {
        public:
            using ConfigVarMap = std::unordered_map<std::string, ConfigVarBase::ptr>;
            //
            using RWMutexType = thread::RWMutex;

            /**
             ** @brief 获取/创建对应参数名的配置参数
             ** @param[in] name 配置参数名称
             ** @param[in] default_value 参数默认值
             ** @param[in] description 参数描述
             ** @details 获取参数名为name的配置参数,如果存在直接返回
             **          如果不存在,创建参数配置并用default_value赋值
             ** @return 返回对应的配置参数,如果参数名存在但是类型不匹配则返回nullptr
             ** @exception 如果参数名包含非法字符[^0-9a-z_.] 抛出异常 std::invalid_argument
             **/
            template<class T>
            static typename ConfigVar<T>::ptr Lookup(const std::string& name,
                   const T& default_value, const std::string& description = "") 
            {
                RWMutexType::WriteLock lock(GetMutex());
                auto it = GetDatas().find(name);
                if (it != GetDatas().end()) 
                {
                    auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
                    if (tmp) //能转换成功则正常返回
                    {
                        YGW_LOG_INFO(YGW_LOG_ROOT()) << "Lookup name=" << name << " exists";
                        return tmp;
                    } 
                    else  //转换失败，返回nullptr
                    {
                        YGW_LOG_ERROR(YGW_LOG_ROOT()) << "Lookup name=" << name << " exists but type not "
                            << util::TypeToName<T>() << " real_type=" << it->second->GetTypeName()
                            << " " << it->second->ToString();
                        return nullptr;
                    }
                }

                if (name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678")
                        != std::string::npos) 
                {
                    YGW_LOG_ERROR(YGW_LOG_ROOT()) << "Lookup name invalid " << name;
                    throw std::invalid_argument(name);
                }

                typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
                GetDatas()[name] = v;
                return v;
            }

            /**
             ** @brief 查找配置参数
             ** @param[in] name 配置参数名称
             ** @return 返回配置参数名为name的配置参数
             **/
            template<class T>
            static typename ConfigVar<T>::ptr Lookup(const std::string& name) 
            {
                RWMutexType::ReadLock lock(GetMutex());
                auto it = GetDatas().find(name);
                if (it == GetDatas().end()) 
                {
                    return nullptr;
                }
                return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
            }

            /**
             ** @brief 使用YAML::Node初始化配置模块
             **/
            static void LoadFromYaml(const YAML::Node& root);

            /**
             ** @brief 加载filepath配置文件
             **/
            static void LoadFromYamlFile(const std::string& file_path);

            /**
             ** @brief 加载path文件夹里面的配置文件
             **/
            static void LoadFromConfDir(const std::string& path, bool force = false);

            /**
             ** @brief 查找配置参数,返回配置参数的基类
             ** @param[in] name 配置参数名称
             **/
            static ConfigVarBase::ptr LookupBase(const std::string& name);

            /**
             ** @brief 遍历配置模块里面所有配置项
             ** @param[in] cb 配置项回调函数
             **/
            static void Visit(std::function<void(ConfigVarBase::ptr)> cb);
        private:

            /**
             ** @brief 返回所有的配置项
             **/
            static ConfigVarMap& GetDatas() 
            {
                static ConfigVarMap s_datas;
                return s_datas;
            }

            /**
             ** @brief 配置项的RWMutex
             **/
            static RWMutexType& GetMutex() 
            {
                static RWMutexType s_mutex;
                return s_mutex;
            }
        };
    } // namespace config

    //-------------------------------------------------

} // namespace ygw 

#endif // __YGW_CONFIG_H__