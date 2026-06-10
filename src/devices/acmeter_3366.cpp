#include "acmeter_3366.h"
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
#include <sstream>
#include "log.h"

ACMeter_3366::ACMeter_3366(const std::string& name, int com, int id)
    : Device(name, com, id) {
    this->name_ = name;
    this->id_ = id;
    this->com_ = com;
    // 初始化JSON数据结构
    Device::init_json_structure(name);
    // 加载配置，使用ACMeter专用的配置文件路径
    init_config(Config::AC_METER_3366_COMMUNICATION_FILEPATH);
    
    // 初始化寄存器段配置（根据DTSD3366D协议XML）
    std::vector<uint16_t> all_addresses;
    for (const auto& pair : this->fc03_nameToAddr_map) {
        all_addresses.push_back(pair.second);
    }
    this->segments03_ = Device::generate_segments_from_addresses(all_addresses, 100);

    init_useful_indexes_from_map(this->fc03_nameToAddr_map, this->segments03_);
    
    // 预分配数据缓冲区（避免每次重新分配）
    this->data_buffer_vec03_.resize(this->segments03_.size());
}



void ACMeter_3366::init_config(const std::string& config_file) {
    // 直接使用父类的默认实现，ACMeter_3366不需要特殊处理
    Device::init_config(config_file);
    
    LOG_INFO_LOC("ACMeter_3366 Config loaded successfully: " + 
                 std::to_string(this->data_dict_.size()) + " registers.");
}

void ACMeter_3366::parse_rawdata(const std::vector<uint16_t>& data_list)
{
    parse_rawdata_generic(data_list);
}

void ACMeter_3366::read_data(ModbusClient& mb_client)
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
                {
                    std::unique_lock<std::shared_mutex> lock(this->data_to_qt_rwlock_);
                    this->data_to_qt["online_status"] = false;
                }
                this->online_status = false;
                this->reconnect_counter = 0;
                LOG_ERROR_LOC("Modbus read failed: " + get_name());
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception& e) {
        LOG_ERROR_LOC("Modbus read error for ACMeter_3366 " + get_name() + ": " + std::string(e.what()));
        {
            std::unique_lock<std::shared_mutex> lock(this->data_to_qt_rwlock_);
            this->data_to_qt["online_status"] = false;
        }
        this->online_status = false;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}