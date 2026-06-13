#ifndef AC_HENGDU_H
#define AC_HENGDU_H

#include "device.h"

class AcHengdu : public Device {
    public:
        AcHengdu(const std::string& name, int com, int id);
        ~AcHengdu()=default;

        void read_data(ModbusClient& mb_client) override;
        void parse_rawdata(const std::vector<uint16_t>& data_list) override;
        void init_config(const std::string& config_file) override;

    private:
        void update_alarm_status();

        
};


#endif