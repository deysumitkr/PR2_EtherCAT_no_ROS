#ifndef xyz
#define xyz

#include <iostream>
#include <stdio.h>
#include <vector>
#include <stdint.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <iomanip>
#include <unistd.h>

#include <al/ethercat_AL.h>
#include <al/ethercat_master.h>
#include <al/ethercat_slave_handler.h>

#include <dll/ethercat_dll.h>

#include <ethercat/ethercat_defs.h>
#include <ethercat/ethercat_xenomai_drv.h>

#include <ethercat_hardware/ethercat_com.h>

#define NameState(X) (X == EC_INIT_STATE)? "EC_INIT_STATE" :\
                        (X == EC_PREOP_STATE)? "EC_PREOP_STATE" :\
                        (X == EC_BOOTSTRAP_STATE)? "EC_BOOTSTRAP_STATE" :\
                        (X == EC_SAFEOP_STATE)? "EC_SAFEOP_STATE" :\
                        (X == EC_OP_STATE)? "EC_OP_STATE" : ("UNKNOWN_STATE: " + std::to_string(X)).c_str()

#define yesno(X) (X)? "yes" : "no"

#define NameBufferType(X) (X == EC_BUFFERED)? "EC_BUFFERED" :\
                            (X == EC_QUEUED)? "EC_QUEUED" : "UNKNOWN"

#define NameBufferState(X) (X == EC_FIRST_BUFFER)? "EC_FIRST_BUFFER" :\
                            (X == EC_SECOND_BUFFER)? "EC_SECOND_BUFFER" :\
                            (X == EC_THIRD_BUFFER)? "EC_THIRD_BUFFER" :\
                            (X == EC_LOCKED_BUFFER)? "EC_LOCKED_BUFFER" : "UNKNOWN"

#define NameDirection(X) (X == EC_READ_FROM_MASTER)? "EC_READ_FROM_MASTER":\
                            (X == EC_WRITTEN_FROM_MASTER)? "EC_WRITTEN_FROM_MASTER" : "UNKNOWN"

// int testLinking();

namespace kul {

    extern EtherCAT_Master *em;
    extern uint32_t num_slaves;
    extern std::vector<EtherCAT_SlaveHandler*> slave_handles;
    extern EthercatOobCom *oob_com_; 

    extern unsigned char *Master_Buffer_;
    extern unsigned int Master_Buffer_Size_;

    extern unsigned char *this_buffer_, *prev_buffer_;

    extern unsigned int max_pd_retries_;

    // this_buffer_ = buffers_;
    // prev_buffer_ = buffers_ + buffer_size_;

    // static int start_address = 0x00010000;

    int initializeECatMaster(const char * const interface);
    int getSlaveHandles();
    int slaveInfo(EtherCAT_SlaveHandler* sh);
    int slaveInfo(std::vector<EtherCAT_SlaveHandler*> sh);

    bool txandrx_PD(unsigned buffer_size, unsigned char* buffer, unsigned tries);
    int changeState(EtherCAT_SlaveHandler *sh, EC_State next_state);
    int closeOperationalSlaves();
}

#endif // PR2_DRIVER_H