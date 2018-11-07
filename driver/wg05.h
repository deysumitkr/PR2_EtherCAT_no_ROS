#ifndef WG05_H
#define WG05_H

#include "master_driver.h"
#include <boost/static_assert.hpp>

#include <stdio.h>
#include <sys/time.h>
#include <math.h>

#define PI 3.14159265
#define WG05_PRODUCT_CODE 6805005

namespace kul {


    pthread_mutex_t mutex_buffer_access = PTHREAD_MUTEX_INITIALIZER;

    int start_address = 0x00010000;

    const unsigned COMMAND_PHY_ADDR = 0x1000;
    const unsigned STATUS_PHY_ADDR = 0x2000;

    const unsigned MBX_COMMAND_PHY_ADDR = 0x1400;
    const unsigned MBX_COMMAND_SIZE = 512;
    const unsigned MBX_STATUS_PHY_ADDR = 0x2400;
    const unsigned MBX_STATUS_SIZE = 512;

    const unsigned MBX_COMMAND_SYNCMAN_NUM = 2;
    const unsigned MBX_STATUS_SYNCMAN_NUM  = 3; 

    double current_amplitude = 10.0;
    unsigned int target_slave = 13;
    bool current_disp = true;

    struct timeval now;


    enum {
      MODE_OFF = 0x00,
      MODE_ENABLE = (1 << 0),
      MODE_CURRENT = (1 << 1),
      MODE_SAFETY_RESET = (1 << 4),
      MODE_SAFETY_LOCKOUT = (1 << 5),
      MODE_UNDERVOLTAGE = (1 << 6),
      MODE_RESET = (1 << 7)
    };


    struct WG0XStatus{
    uint8_t mode_;
    uint8_t digital_out_;
    int16_t programmed_pwm_value_;
    int16_t programmed_current_;
    int16_t measured_current_;
    uint32_t timestamp_;
    int32_t encoder_count_;
    int32_t encoder_index_pos_;
    uint16_t num_encoder_errors_;
    uint8_t encoder_status_;
    uint8_t calibration_reading_;
    int32_t last_calibration_rising_edge_;
    int32_t last_calibration_falling_edge_;
    uint16_t board_temperature_;
    uint16_t bridge_temperature_;
    uint16_t supply_voltage_;
    int16_t motor_voltage_;
    uint16_t packet_count_;
    uint8_t pad_;
    uint8_t checksum_;

    static const unsigned SIZE=44;
    }__attribute__ ((__packed__));


    struct WG0XCommand {
    uint8_t mode_;
    uint8_t digital_out_;
    int16_t programmed_pwm;
    int16_t programmed_current_;
    uint8_t pad_;
    uint8_t checksum_;
    }__attribute__ ((__packed__));


    std::vector<EtherCAT_SlaveHandler*> wg05_slave_handles;

    int initializePr2EthercatSlaves(const char * const ethercatPortName);
    int findWG05slaves(std::vector<EtherCAT_SlaveHandler*> slave_handles);
    int configureWG05fmmu(EtherCAT_SlaveHandler *sh);
    int configureWG05pd(EtherCAT_SlaveHandler *sh);

    int update();
    int monitorChanges(int threshold=100);

}

#endif // WG05_H

// wg05.h {L#316}
//! Different possible states for application ram on device. 
  //  Application ram is non-volitile memory that application can use to store temporary
  //  data between runs.
  //  Currently app ram is used to store zero offset of joint after calibration
  //    PRESENT - App ram is available
  //    MISSING - App ram is missing but would typically be availble
  //    NOT_APPLICABLE - App ram is not availble, but is is not needed for this type of device 
//   enum AppRamStatus { APP_RAM_PRESENT=1, APP_RAM_MISSING=2, APP_RAM_NOT_APPLICABLE=3 };