#include "command.h"
#include <iostream>
#include "qtcontroller.h"
#include "utils.h"
#include "log.h"

static auto& cmd = QtController::getInstance;

HengduAcCmd::HengduAcCmd(std::unordered_map<int, std::shared_ptr<ModbusClient>> modbus_clients,std::unordered_map<std::string, std::shared_ptr<Device>> device_map) 
: modbus_clients_(modbus_clients)
,device_map_(device_map) {
}

void HengduAcCmd::ac_on_off(const std::string& switch_state,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    uint16_t value = (switch_state == "on") ? 1 : 0;
    device->writeCmdToDevice(mb_client,"05",0,value);
    LOG_INFO_LOC("空调 " + device->get_name() + " " + switch_state + "，模式:" + mode);
}

void HengduAcCmd::force_cooling(const std::string& switch_state,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    uint16_t value = (switch_state == "on") ? 1 : 0;
    device->writeCmdToDevice(mb_client,"05",2,value);
    LOG_INFO_LOC("空调 " + device->get_name() + " 强制制冷" + switch_state + "，模式:" + mode);
}

void HengduAcCmd::force_heating(const std::string& switch_state,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    uint16_t value = (switch_state == "on") ? 1 : 0;
    device->writeCmdToDevice(mb_client,"05",3,value);
    LOG_INFO_LOC("空调 " + device->get_name() + " 强制加热" + switch_state + "，模式:" + mode);
}

void HengduAcCmd::set_cooling_temp(const int16_t& temp,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    uint16_t raw_value = static_cast<uint16_t>(temp * 10);
    device->writeCmdToDevice(mb_client,"06",0,raw_value);
    LOG_INFO_LOC("空调 " + device->get_name() + " 设置制冷温度:" + std::to_string(temp) + "℃，模式:" + mode);
}

void HengduAcCmd::set_cooling_temp_rd(const int16_t& temp_rd,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    uint16_t raw_value = static_cast<uint16_t>(temp_rd * 10);
    device->writeCmdToDevice(mb_client,"06",1,raw_value);
    LOG_INFO_LOC("空调 " + device->get_name() + " 设置制冷回差温度:" + std::to_string(temp_rd) + "℃，模式:" + mode);
}

void HengduAcCmd::set_heating_temp(const int16_t& temp,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    uint16_t raw_value = static_cast<uint16_t>(temp * 10);
    device->writeCmdToDevice(mb_client,"06",2,raw_value);
    LOG_INFO_LOC("空调 " + device->get_name() + " 设置制热温度:" + std::to_string(temp) + "℃，模式:" + mode);
}

void HengduAcCmd::set_heating_temp_rd(const int16_t& temp_rd,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    uint16_t raw_value = static_cast<uint16_t>(temp_rd * 10);
    device->writeCmdToDevice(mb_client,"06",3,raw_value);
    LOG_INFO_LOC("空调 " + device->get_name() + " 设置制热回差温度:" + std::to_string(temp_rd) + "℃，模式:" + mode);
}

void HengduAcCmd::set_high_temp_alarm(const int16_t& temp,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    uint16_t raw_value = static_cast<uint16_t>(temp * 10);
    device->writeCmdToDevice(mb_client,"06",4,raw_value);
    LOG_INFO_LOC("空调 " + device->get_name() + " 设置高温告警温度:" + std::to_string(temp) + "℃，模式:" + mode);
}

void HengduAcCmd::set_low_temp_alarm(const int16_t& temp,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    uint16_t raw_value = static_cast<uint16_t>(temp * 10);
    device->writeCmdToDevice(mb_client,"06",5,raw_value);
    LOG_INFO_LOC("空调 " + device->get_name() + " 设置低温告警温度:" + std::to_string(temp) + "℃，模式:" + mode);
}

void HengduAcCmd::set_drying_open_humidity(const uint16_t& humidity,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    uint16_t raw_value = humidity * 10;
    device->writeCmdToDevice(mb_client,"06",6,raw_value);
    LOG_INFO_LOC("空调 " + device->get_name() + " 设置除湿开启湿度:" + std::to_string(humidity) + "%，模式:" + mode);
}

void HengduAcCmd::set_drying_humidity_rd(const uint16_t& humidity_rd,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    uint16_t raw_value = humidity_rd * 10;
    device->writeCmdToDevice(mb_client,"06",7,raw_value);
    LOG_INFO_LOC("空调 " + device->get_name() + " 设置除湿回差湿度:" + std::to_string(humidity_rd) + "%，模式:" + mode);
}

void HengduAcCmd::set_high_humidity_alarm(const uint16_t& humidity,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    uint16_t raw_value = humidity * 10;
    device->writeCmdToDevice(mb_client,"06",8,raw_value);
    LOG_INFO_LOC("空调 " + device->get_name() + " 设置高湿告警值:" + std::to_string(humidity) + "%，模式:" + mode);
}

void HengduAcCmd::set_drying_enable(const uint16_t& enable,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    device->writeCmdToDevice(mb_client,"06",9,enable);
    LOG_INFO_LOC("空调 " + device->get_name() + " 设置除湿使能:" + std::to_string(enable) + "，模式:" + mode);
}

void HengduAcCmd::process_hengdu_ac_commands(const std::string& device_name) {
    try {
        auto& device_commands = cmd()->cmd_from_qt[device_name];
        
        if (device_map_.find(device_name) == device_map_.end()) {
            LOG_WARNING_LOC("Device " + device_name + " not found.");
            return;
        }
        std::shared_ptr<Device>& device = device_map_.at(device_name);
        
        if (device_commands.contains("setOnOff") && !device_commands["setOnOff"].is_null()) {
            std::string switch_state = device_commands["setOnOff"].get<bool>() ? "on" : "off";
            LOG_INFO_LOC("准备执行 " + device_name + " 开关命令: " + switch_state);
            ac_on_off(switch_state, "手动", device);
            device_commands["setOnOff"] = nullptr;
        }
        
        if (device_commands.contains("setForceCooling") && !device_commands["setForceCooling"].is_null()) {
            std::string switch_state = device_commands["setForceCooling"].get<bool>() ? "on" : "off";
            LOG_INFO_LOC("准备执行 " + device_name + " 强制制冷命令: " + switch_state);
            force_cooling(switch_state, "手动", device);
            device_commands["setForceCooling"] = nullptr;
        }
        
        if (device_commands.contains("setForceHeating") && !device_commands["setForceHeating"].is_null()) {
            std::string switch_state = device_commands["setForceHeating"].get<bool>() ? "on" : "off";
            LOG_INFO_LOC("准备执行 " + device_name + " 强制加热命令: " + switch_state);
            force_heating(switch_state, "手动", device);
            device_commands["setForceHeating"] = nullptr;
        }
        
        if (device_commands.contains("setCoolingTemp") && !device_commands["setCoolingTemp"].is_null()) {
            int16_t temp = device_commands["setCoolingTemp"].get<int16_t>();
            LOG_INFO_LOC("准备执行 " + device_name + " 设置制冷温度命令: " + std::to_string(temp) + "℃");
            set_cooling_temp(temp, "手动", device);
            device_commands["setCoolingTemp"] = nullptr;
        }
        
        if (device_commands.contains("setCoolingTemp_rd") && !device_commands["setCoolingTemp_rd"].is_null()) {
            int16_t temp_rd = device_commands["setCoolingTemp_rd"].get<int16_t>();
            LOG_INFO_LOC("准备执行 " + device_name + " 设置制冷回差温度命令: " + std::to_string(temp_rd) + "℃");
            set_cooling_temp_rd(temp_rd, "手动", device);
            device_commands["setCoolingTemp_rd"] = nullptr;
        }
        
        if (device_commands.contains("setHeatingTemp") && !device_commands["setHeatingTemp"].is_null()) {
            int16_t temp = device_commands["setHeatingTemp"].get<int16_t>();
            LOG_INFO_LOC("准备执行 " + device_name + " 设置制热温度命令: " + std::to_string(temp) + "℃");
            set_heating_temp(temp, "手动", device);
            device_commands["setHeatingTemp"] = nullptr;
        }
        
        if (device_commands.contains("setHeatingTemp_rd") && !device_commands["setHeatingTemp_rd"].is_null()) {
            int16_t temp_rd = device_commands["setHeatingTemp_rd"].get<int16_t>();
            LOG_INFO_LOC("准备执行 " + device_name + " 设置制热回差温度命令: " + std::to_string(temp_rd) + "℃");
            set_heating_temp_rd(temp_rd, "手动", device);
            device_commands["setHeatingTemp_rd"] = nullptr;
        }
        
        if (device_commands.contains("setHighTempAlarm") && !device_commands["setHighTempAlarm"].is_null()) {
            int16_t temp = device_commands["setHighTempAlarm"].get<int16_t>();
            LOG_INFO_LOC("准备执行 " + device_name + " 设置高温告警温度命令: " + std::to_string(temp) + "℃");
            set_high_temp_alarm(temp, "手动", device);
            device_commands["setHighTempAlarm"] = nullptr;
        }
        
        if (device_commands.contains("setLowTempAlarm") && !device_commands["setLowTempAlarm"].is_null()) {
            int16_t temp = device_commands["setLowTempAlarm"].get<int16_t>();
            LOG_INFO_LOC("准备执行 " + device_name + " 设置低温告警温度命令: " + std::to_string(temp) + "℃");
            set_low_temp_alarm(temp, "手动", device);
            device_commands["setLowTempAlarm"] = nullptr;
        }
        
        if (device_commands.contains("setDryingOpenHumidity") && !device_commands["setDryingOpenHumidity"].is_null()) {
            uint16_t humidity = device_commands["setDryingOpenHumidity"].get<uint16_t>();
            LOG_INFO_LOC("准备执行 " + device_name + " 设置除湿开启湿度命令: " + std::to_string(humidity) + "%");
            set_drying_open_humidity(humidity, "手动", device);
            device_commands["setDryingOpenHumidity"] = nullptr;
        }
        
        if (device_commands.contains("setDryingHumidity_rd") && !device_commands["setDryingHumidity_rd"].is_null()) {
            uint16_t humidity_rd = device_commands["setDryingHumidity_rd"].get<uint16_t>();
            LOG_INFO_LOC("准备执行 " + device_name + " 设置除湿回差湿度命令: " + std::to_string(humidity_rd) + "%");
            set_drying_humidity_rd(humidity_rd, "手动", device);
            device_commands["setDryingHumidity_rd"] = nullptr;
        }
        
        if (device_commands.contains("setHighHumidityAlarm") && !device_commands["setHighHumidityAlarm"].is_null()) {
            uint16_t humidity = device_commands["setHighHumidityAlarm"].get<uint16_t>();
            LOG_INFO_LOC("准备执行 " + device_name + " 设置高湿告警值命令: " + std::to_string(humidity) + "%");
            set_high_humidity_alarm(humidity, "手动", device);
            device_commands["setHighHumidityAlarm"] = nullptr;
        }
        
        if (device_commands.contains("setDryingEnable") && !device_commands["setDryingEnable"].is_null()) {
            uint16_t enable = device_commands["setDryingEnable"].get<bool>() ? 1 : 0;
            LOG_INFO_LOC("准备执行 " + device_name + " 设置除湿使能命令: " + std::to_string(enable));
            set_drying_enable(enable, "手动", device);
            device_commands["setDryingEnable"] = nullptr;
        }
        
    } catch (const std::exception& e) {
        LOG_ERROR_LOC("Error processing " + device_name + " commands: " + std::string(e.what()));
    }
}

void HengduAcCmd::hengdu_ac_manual_control(const std::string& device_name) {
    process_hengdu_ac_commands(device_name);
}