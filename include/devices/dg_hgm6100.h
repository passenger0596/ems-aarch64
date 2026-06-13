#ifndef DG_HGM6100N_H
#define DG_HGM6100N_H

#include "device.h"

class DgHgm6100n : public Device {
    public:
        DgHgm6100n(const std::string& name, int com, int id);
        ~DgHgm6100n()=default;

        void read_data(ModbusClient& mb_client) override;
        void parse_rawdata(const std::vector<uint16_t>& data_list) override;
        void init_config(const std::string& config_file) override;

    private:
        void update_alarm_status();

        
};


#endif