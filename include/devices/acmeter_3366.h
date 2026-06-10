#ifndef ACMETER_3366_H
#define ACMETER_3366_H

#include "device.h"

class ACMeter_3366 : public Device {
    public:
        ACMeter_3366(const std::string& name, int com, int id);
        ~ACMeter_3366()=default;

        void read_data(ModbusClient& mb_client) override;
        void parse_rawdata(const std::vector<uint16_t>& data_list) override;
        void init_config(const std::string& config_file) override;

        
};


#endif