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
    Device::init_config(config_file);
    
}

void IOModule::parse_rawdata(const std::vector<uint16_t>& data_list)
{
    parse_rawdata_generic(data_list);
    
    update_di_status();
}

void IOModule::update_di_status()
{
    // 检查DI状态并更新告警（与DCDC保持一致）
    // 例如：系统急停DI触发时，设置相应告警
    for (size_t i = 0; i < this->alarm_map.size(); ++i) {
        const std::string& alarm_name = this->alarm_map[i].first;
        const uint8_t& level = this->alarm_map[i].second;

        bool status = getValue<bool>(alarm_name);
        std::string now = Utils::getCurrentTimeString();
        Device::handle_alarm(alarm_name, level, status, now);

    }
}

void IOModule::read_data(ModbusClient& mb_client)
{
    if (!mb_client.is_connected()){
            LOG_ERROR_LOC("ModbusClient is not connected.");
            return;
        }
        
        try {
            if (read_all_registers(mb_client)) {
                parse_rawdata(this->data_buffer);
                this->reconnect_counter = 0;
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
            LOG_ERROR_LOC("Modbus read error for ACMeter_3366 " + get_name() + ": " + std::string(e.what()));
            safe_set_qt_data(false);
            this->online_status = false;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

    // if (!mb_client.is_connected()){
    //     LOG_ERROR_LOC("ModbusClient is not connected.");
    //     return;
    // }

    //     // 更新时间戳
    // auto now = std::chrono::system_clock::now();
    // auto time_t_now = std::chrono::system_clock::to_time_t(now);
    // std::stringstream ss;
    // ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    // {
    //     std::unique_lock<std::shared_mutex> lock(this->data_to_qt_rwlock_);
    //     this->data_to_qt["timestamp"] = ss.str();
    // }
    
    // try {
    //     bool total_read_success = true;

    //     mb_client.set_slave(this->id_);

    //     // 读取线圈（功能码01）
    //     // const auto& do_segment = this->segments_[0];
    //     // this->data_buffer_vec_[0].resize(do_segment.num_regs);
        
    //     // 使用读线圈功能码，因设备协议固定不变，直接写死地址
    //     bool read_success = mb_client.read_coils(
    //         0, 8, this->data_buffer_vec01_[0].data());
        
    //     if (!read_success) {
    //         total_read_success = false;
    //         // LOG_ERROR_LOC("Modbus read coils failed for IOModule " + get_name());
    //     }
        
    //     // 读取离散输入（功能码02）
    //     if (total_read_success) {
    //         // const auto& di_segment = this->segments_[1];
    //         // this->data_buffer_vec_[1].resize(di_segment.num_regs);
            
    //         // 使用读离散输入功能码,直接写死
    //         bool read_success = mb_client.read_input_bits(
    //             0, 8, this->data_buffer_vec02_[0].data());
            
    //         if (!read_success) {
    //             total_read_success = false;
    //             // LOG_ERROR_LOC("Modbus read discrete inputs failed for IOModule " + get_name());
    //         }
    //     }    
    //     if (total_read_success){
    //         this->data_buffer.clear();
            
    //         // 合并DI和DO数据
    //         for (const std::vector<uint8_t>& reg : this->data_buffer_vec01_){
    //             for (const uint8_t& bit : reg){
    //                 this->data_buffer.push_back(bit);
    //             }
    //         }

    //         for (const std::vector<uint8_t>& reg : this->data_buffer_vec02_){
    //             for (const uint8_t& bit : reg){
    //                 this->data_buffer.push_back(bit);
    //             }
    //         }
            
    //         parse_rawdata(this->data_buffer);

    //         this->data_to_qt["online_status"] = true;
    //         this->online_status = true;
    //     } else {
    //         this->reconnect_counter++;
    //         if (this->reconnect_counter>3){
    //             safe_set_qt_data(false);
    //             this->online_status = false;
    //             this->reconnect_counter = 0;
    //             LOG_ERROR_LOC("Modbus read failed: " + get_name());
    //         }
    //         std::this_thread::sleep_for(std::chrono::seconds(1));
    //     }
    // } catch (const std::exception& e) {
    //     LOG_ERROR_LOC("Modbus read error for IOModule " + get_name() + ": " + std::string(e.what()));
    //     safe_set_qt_data(false);
    //     this->online_status = false;
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    // }
}