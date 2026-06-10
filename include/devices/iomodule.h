#ifndef IOMODULE_H
#define IOMODULE_H

#include "device.h"
#include <string>
#include <map>
#include <memory>
#include "json.hpp"
#include <vector>

using json = nlohmann::json;

class IOModule : public Device {
public:
    IOModule(const std::string& name, int com, int id);
    ~IOModule() = default;
    
    void read_data(ModbusClient& mb_client) override;
    void parse_rawdata(const std::vector<uint16_t>& data_list) override;
    void init_config(const std::string& config_file) override;

private:
    void update_di_status();

};

#endif