#include "iomodule.h"
#include "utils.h"
#include "../include/settings/config.h"
#include <iostream>
#include "modbusclient.h"
#include <fstream>
#include <pugixml.hpp>
#include <thread>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include "log.h"

IOModule::IOModule(const std::string& name, int com, int id)
    : Device(name, com, id) {
    this->name_ = name;
    this->id_ = id;
    this->com_ = com;
    // 初始化JSON数据结构
    Device::init_json_structure(name);
    // 加载配置，使用IOModule专用的配置文件路径
    init_config(Config::IOMODULE_COMMUNICATION_FILEPATH);
    
    // 初始化寄存器段配置（根据协议XML，主要处理线圈和离散输入）
    this->segments01_ = {
        {0, 8},   // 第1段：线圈（功能码01）- 8个DO
    };

    this->segments02_ = {
        {0, 8},   // 第2段：离散输入（功能码02）- 8个DI
    };

    init_useful_indexes_from_map(this->fc01_nameToAddr_map, this->segments01_,
                                 parsed_registers_fc01_);
    init_useful_indexes_from_map(this->fc02_nameToAddr_map, this->segments02_,
                                 parsed_registers_fc02_);
    build_parsed_registers();
    
    // 预分配数据缓冲区

    this->data_buffer_vec01_ = std::vector<std::vector<uint8_t>>(this->segments01_.size());
    this->data_buffer_vec02_ = std::vector<std::vector<uint8_t>>(this->segments02_.size());
}

void IOModule::init_config(const std::string& config_file) {
    LOG_INFO_LOC("Loading IOModule config from: " + config_file);
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(config_file.c_str());

    if (!result) {
        LOG_ERROR_LOC("Failed to load config file: " + config_file +
                     ", Error: " + std::string(result.description()));
        return;
    }

    pugi::xml_node root = doc.document_element();
    if (!root) {
        LOG_ERROR_LOC("Invalid XML format");
        return;
    }

    // 解析功能码01（线圈）- DO
    pugi::xml_node fc01_node = root.child("function_code01");
    if (fc01_node) {
        for (pugi::xml_node coil : fc01_node.children("coil")) {
            std::string name = coil.attribute("name").as_string();
            if (!name.empty()) {
                RegisterData reg_data;
                reg_data.address = static_cast<uint16_t>(std::stoi(coil.attribute("address").as_string()));
                reg_data.datatype = "BOOL";

                reg_data.value = 0.0;

                // 存储到基类Device的映射和字典中（与DCDC保持一致）
                this->fc01_nameToAddr_map[name] = reg_data.address;
                this->data_dict_[name] = reg_data;
                this->dev_data_keys_.push_back(name);
                
            }
        }
    }

    // 解析功能码02（离散输入）- DI
    pugi::xml_node fc02_node = root.child("function_code02");
    if (fc02_node) {
        for (pugi::xml_node di : fc02_node.children("di")) {
            std::string name = di.attribute("name").as_string();
            if (!name.empty()) {
                RegisterData reg_data;
                reg_data.address = static_cast<uint16_t>(std::stoi(di.attribute("address").as_string()));

                reg_data.datatype = "BOOL";

                reg_data.value = 0.0;

                // 存储到基类Device的映射和字典中
                this->fc02_nameToAddr_map[name] = reg_data.address;
                this->data_dict_[name] = reg_data;
                this->dev_data_keys_.push_back(name);
                
                // 初始化DI状态为false
                this->data_to_qt[name] = false;
            }
        }
    }

    // 解析告警信息
    parse_alarm_config(root);
    
    // 初始化数据数组大小，与数据字典数量一致
    this->data_to_qt["data"] = json::array();
    for (size_t i = 0; i < this->data_dict_.size(); ++i) {
        this->data_to_qt["data"].push_back(0.0);
    }
    
    LOG_INFO_LOC("IOModule Config loaded successfully: " +
                 std::to_string(this->data_dict_.size()) + " registers, " +
                 std::to_string(this->alarm_map.size()) + " alarms.");
}

void IOModule::parse_rawdata(const std::vector<uint16_t>& data_list)
{
    this->online_status = true;
    json data_array = json::array();
    json temp_updates;

    // 使用 parsed_registers_（已通过 build_parsed_registers() 构建全局偏移）
    for (const auto& pr : parsed_registers_) {
        if (pr.buffer_index >= data_list.size()) break;
        bool status = (data_list[pr.buffer_index] != 0);
        double actual_value = status ? 1 : 0;
        this->updateRegisterValue(pr.key, actual_value);
        data_array.push_back(actual_value);
        temp_updates[pr.key] = status;
    }

    {
        std::unique_lock<std::shared_mutex> lock(this->data_to_qt_rwlock_);
        this->data_to_qt["online_status"] = true;
        this->data_to_qt["data"] = data_array;
        for (auto it = temp_updates.begin(); it != temp_updates.end(); ++it) {
            this->data_to_qt[it.key()] = it.value();
        }
    }
    
    update_di_status();
}

void IOModule::update_di_status()
{
    // 检查DI状态并更新告警（与DCDC保持一致）
    // 例如：系统急停DI触发时，设置相应告警
    for (size_t i = 0; i < this->alarm_map.size(); ++i) {
        const std::string& alarm_name = this->alarm_map[i].first;
        const uint8_t& level = this->alarm_map[i].second;
        
        // 检查该告警名称是否在数据字典中
        auto data_it = this->data_dict_.find(alarm_name);
        if (data_it != this->data_dict_.end()) {
            bool status = (data_it->second.value != 0);
            
            // 更新对应级别的告警JSON对象
            switch (level) {
                case 1: this->alarm_level1[alarm_name] = status; break;
                case 2: this->alarm_level2[alarm_name] = status; break;
                case 3: this->alarm_level3[alarm_name] = status; break;
            }
            // 更新总数据中的告警状态
            this->data_to_qt[alarm_name] = status;
        }
    }
}

void IOModule::read_data(ModbusClient& mb_client)
{
    if (!mb_client.is_connected()){
        LOG_ERROR_LOC("ModbusClient is not connected.");
        return;
    }

        // 更新时间戳
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    {
        std::unique_lock<std::shared_mutex> lock(this->data_to_qt_rwlock_);
        this->data_to_qt["timestamp"] = ss.str();
    }
    
    try {
        bool total_read_success = true;

        mb_client.set_slave(this->id_);

        // 读取线圈（功能码01）
        // const auto& do_segment = this->segments_[0];
        // this->data_buffer_vec_[0].resize(do_segment.num_regs);
        
        // 使用读线圈功能码，因设备协议固定不变，直接写死地址
        bool read_success = mb_client.read_coils(
            0, 8, this->data_buffer_vec01_[0].data());
        
        if (!read_success) {
            total_read_success = false;
            // LOG_ERROR_LOC("Modbus read coils failed for IOModule " + get_name());
        }
        
        // 读取离散输入（功能码02）
        if (total_read_success) {
            // const auto& di_segment = this->segments_[1];
            // this->data_buffer_vec_[1].resize(di_segment.num_regs);
            
            // 使用读离散输入功能码,直接写死
            bool read_success = mb_client.read_input_bits(
                0, 8, this->data_buffer_vec02_[0].data());
            
            if (!read_success) {
                total_read_success = false;
                // LOG_ERROR_LOC("Modbus read discrete inputs failed for IOModule " + get_name());
            }
        }    
        if (total_read_success){
            this->data_buffer.clear();
            
            // 合并DI和DO数据
            for (const std::vector<uint8_t>& reg : this->data_buffer_vec01_){
                for (const uint8_t& bit : reg){
                    this->data_buffer.push_back(bit);
                }
            }

            for (const std::vector<uint8_t>& reg : this->data_buffer_vec02_){
                for (const uint8_t& bit : reg){
                    this->data_buffer.push_back(bit);
                }
            }
            
            parse_rawdata(this->data_buffer);

            this->data_to_qt["online_status"] = true;
            this->online_status = true;
        } else {
            this->reconnect_counter++;
            if (this->reconnect_counter>3){
                safe_set_qt_data(false);
                this->online_status = false;
                this->reconnect_counter = 0;
                LOG_ERROR_LOC("Modbus read failed: " + get_name());
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception& e) {
        LOG_ERROR_LOC("Modbus read error for IOModule " + get_name() + ": " + std::string(e.what()));
        safe_set_qt_data(false);
        this->online_status = false;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}