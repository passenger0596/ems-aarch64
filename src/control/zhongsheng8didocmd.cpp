#include "command.h"
#include <iostream>
#include "qtcontroller.h"
#include "utils.h"
#include "log.h"

static auto& cmd = QtController::getInstance;

Zhongsheng8didoCmd::Zhongsheng8didoCmd(std::unordered_map<int, std::shared_ptr<ModbusClient>> modbus_clients,std::unordered_map<std::string, std::shared_ptr<Device>> device_map) 
: modbus_clients_(modbus_clients)
,device_map_(device_map) {
}

void Zhongsheng8didoCmd::board_8di8do_operate(const std::string& switch_state,const int do_num,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    uint16_t value = (switch_state == "close") ? 1 : 0;
    uint16_t addr = static_cast<uint16_t>(do_num - 1);
    device->writeCmdToDevice(mb_client,"05",addr,value);
    LOG_INFO_LOC("8DI8DO板 " + device->get_name() + " 操作DO" + std::to_string(do_num) + "，值:" + switch_state + "，模式:" + mode);
}

void Zhongsheng8didoCmd::board_8di8do_multi_operate(const std::vector<uint16_t>& switch_list,const std::string& mode,std::shared_ptr<Device>& device) {
    int com = device->get_com();
    std::shared_ptr<ModbusClient>& mb_client = this->modbus_clients_[com];
    device->multiWriteCmdToDevice(mb_client,"15",0,switch_list);
    LOG_INFO_LOC("8DI8DO板 " + device->get_name() + " 批量操作DO，数量:" + std::to_string(switch_list.size()) + "，模式:" + mode);
}

void Zhongsheng8didoCmd::process_board_8di8do_commands(const std::string& device_name) {
    try {
        auto& device_commands = cmd()->cmd_from_qt[device_name];
        
        if (device_map_.find(device_name) == device_map_.end()) {

            return;
        }
        std::shared_ptr<Device>& device = device_map_.at(device_name);
        
        for (int i = 1; i <= 8; ++i) {
            std::string key = "do" + std::to_string(i);
            if (device_commands.contains(key) && !device_commands[key].is_null()) {
                bool bool_value = device_commands[key].get<bool>();
                std::string switch_state = bool_value ? "close" : "open";
                LOG_INFO_LOC("准备执行 " + device_name + " DO" + std::to_string(i) + " 命令: " + switch_state);
                board_8di8do_operate(switch_state, i, "手动", device);
                device_commands[key] = nullptr;
            }
        }
        
    } catch (const std::exception& e) {
        LOG_ERROR_LOC("Error processing " + device_name + " commands: " + std::string(e.what()));
    }
}

void Zhongsheng8didoCmd::board_8di8do_manual_control(const std::string& device_name) {
    process_board_8di8do_commands(device_name);
}