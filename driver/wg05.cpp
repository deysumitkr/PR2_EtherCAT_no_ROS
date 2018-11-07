#include "wg05.h"

namespace kul {

    int configureWG05fmmu(EtherCAT_SlaveHandler *sh){

        if(sh == NULL){
            fprintf(stderr, "Slave is NULL\n");
            return -1;
        }

        if (sh->get_product_code() != WG05_PRODUCT_CODE){
            fprintf(stderr, "Configuring a non-WG05 slave with WG05 configuration. This is a fatal error. Program will terminate.\n");
            // exit(-1);
            return -2;
        }

        unsigned int base_status = sizeof(WG0XStatus);
        unsigned long command_size_ = sizeof(WG0XCommand);
        // unsigned long status_size_ = sizeof(WG0XStatus);

        // As good a place as any for making sure that compiler actually packed these structures correctly
        BOOST_STATIC_ASSERT(sizeof(WG0XStatus) == WG0XStatus::SIZE);

        EtherCAT_FMMU_Config *fmmu = new EtherCAT_FMMU_Config(2);
        //ROS_DEBUG("device %d, command  0x%X = 0x10000+%d", (int)sh->get_ring_position(), start_address, start_address-0x10000);
        (*fmmu)[0] = EC_FMMU(start_address, // Logical start address
                            command_size_,// Logical length
                            0x00, // Logical StartBit
                            0x07, // Logical EndBit
                            COMMAND_PHY_ADDR, // Physical Start address
                            0x00, // Physical StartBit
                            false, // Read Enable
                            true, // Write Enable
                            true); // Enable

        start_address += command_size_;

        //ROS_DEBUG("device %d, status   0x%X = 0x10000+%d", (int)sh->get_ring_position(), start_address, start_address-0x10000);
        (*fmmu)[1] = EC_FMMU(start_address, // Logical start address
                            base_status, // Logical length
                            0x00, // Logical StartBit
                            0x07, // Logical EndBit
                            STATUS_PHY_ADDR, // Physical Start address
                            0x00, // Physical StartBit
                            true, // Read Enable
                            false, // Write Enable
                            true); // Enable

        start_address += base_status;

        sh->set_fmmu_config(fmmu);

        return 0;
    }


    int configureWG05pd(EtherCAT_SlaveHandler *sh){

        if(sh == NULL){
            fprintf(stderr, "Slave is NULL\n");
            return -1;
        }

        if (sh->get_product_code() != WG05_PRODUCT_CODE){
            fprintf(stderr, "Configuring a non-WG05 slave with WG05 configuration. This is a fatal error. Program will terminate.\n");
            // exit(-1);
            return -2;
        }

        unsigned int base_status = sizeof(WG0XStatus);
        unsigned long command_size_ = sizeof(WG0XCommand);
        // unsigned long status_size_ = sizeof(WG0XStatus);

        EtherCAT_PD_Config *pd = new EtherCAT_PD_Config(4);

        // Sync managers
        (*pd)[0] = EC_SyncMan(COMMAND_PHY_ADDR, command_size_, EC_BUFFERED, EC_WRITTEN_FROM_MASTER);
        (*pd)[0].ChannelEnable = true;
        (*pd)[0].ALEventEnable = true;

        (*pd)[1] = EC_SyncMan(STATUS_PHY_ADDR, base_status);
        (*pd)[1].ChannelEnable = true;

        (*pd)[2] = EC_SyncMan(MBX_COMMAND_PHY_ADDR, MBX_COMMAND_SIZE, EC_QUEUED, EC_WRITTEN_FROM_MASTER);
        (*pd)[2].ChannelEnable = true;
        (*pd)[2].ALEventEnable = true;

        (*pd)[3] = EC_SyncMan(MBX_STATUS_PHY_ADDR, MBX_STATUS_SIZE, EC_QUEUED);
        (*pd)[3].ChannelEnable = true;

        sh->set_pd_config(pd);

        return 0;
    }

    
    int findWG05slaves(std::vector<EtherCAT_SlaveHandler*> slave_handles){
        for(unsigned int i=0; i < slave_handles.size(); i++){
            if(slave_handles[i] != NULL) {
                if (slave_handles[i]->get_product_code() == WG05_PRODUCT_CODE){
                    wg05_slave_handles.push_back(slave_handles[i]);
                }
            }
        }
        return 0;
    }


    int setMasterBuffer(){
        if(slave_handles.size() == 0){
            fprintf(stderr, "slave_handles are empty\n");
            return -1;
        }

        // [NOTE] slaves must be configured (fmmu & pd) before changing state

        // change state of all slaves

        // // INIT to PRE_OP
        // for(unsigned int i=0; i < slave_handles.size(); i++){
        //     changeState(slave_handles[i], EC_PREOP_STATE);
        // }

        // // PRE_OP to SAFE_OP
        // for(unsigned int i=0; i < slave_handles.size(); i++){
        //     changeState(slave_handles[i], EC_SAFEOP_STATE);
        // }

        // // SAFE_OP to OP
        // for(unsigned int i=0; i < slave_handles.size(); i++){
        //     changeState(slave_handles[i], EC_OP_STATE);
        // }

        // change state of only WG05 slaves

        // INIT to PRE_OP
        for(unsigned int i=0; i < wg05_slave_handles.size(); i++){
            changeState(wg05_slave_handles[i], EC_PREOP_STATE);
        }

        // PRE_OP to SAFE_OP
        for(unsigned int i=0; i < wg05_slave_handles.size(); i++){
            changeState(wg05_slave_handles[i], EC_SAFEOP_STATE);
        }

        // SAFE_OP to OP
        for(unsigned int i=0; i < wg05_slave_handles.size(); i++){
            changeState(wg05_slave_handles[i], EC_OP_STATE);
        }

        Master_Buffer_Size_ = (sizeof(WG0XStatus) + sizeof(WG0XCommand)) * num_slaves; 
        Master_Buffer_ = new unsigned char[2*Master_Buffer_Size_]; // 2 x buffer size to send and receive

        memset(Master_Buffer_, 0, 2 * Master_Buffer_Size_);
        txandrx_PD(Master_Buffer_Size_, Master_Buffer_, 20);

        this_buffer_ = Master_Buffer_;
        prev_buffer_ = Master_Buffer_ + Master_Buffer_Size_;

        // prev_buffer should contain valid status data when update function is first used
        memcpy(prev_buffer_, this_buffer_, Master_Buffer_Size_);

        // after initialization reduce socket timeout
        //set_socket_timeout();

        return 0;
    }

    int configureWG05slaves(){
        for(unsigned int i=0; i<wg05_slave_handles.size(); i++){
            configureWG05fmmu(wg05_slave_handles[i]);
            configureWG05pd(wg05_slave_handles[i]);
        }
    }

    int initializePr2EthercatSlaves(const char * const ethercatPortName){
        initializeECatMaster(ethercatPortName);
        getSlaveHandles();

        findWG05slaves(slave_handles);
        configureWG05slaves();

        setMasterBuffer();

        return 0;
    }

    unsigned int rotateRight8(unsigned in) {
        in &= 0xff;
        in = (in >> 1) | (in << 7);
        in &= 0xff;
        return in;
    }

    unsigned computeChecksum(void const *data, unsigned length){
        const unsigned char *d = (const unsigned char *)data;
        unsigned int checksum = 0x42;
        for (unsigned int i = 0; i < length; ++i)
        {
            checksum = rotateRight8(checksum);
            checksum ^= d[i];
            checksum &= 0xff;
        }
        return checksum;
    }

    int packCommand(){
        unsigned int command_size = sizeof(WG0XCommand);
        unsigned int status_size = sizeof(WG0XStatus);

        unsigned char *this_buffer;
        this_buffer = this_buffer_;

        for(unsigned int i=0; i<slave_handles.size(); i++){
            if(slave_handles[i] == NULL) {
                fprintf(stderr, "[FATAL] Slave handler for %u slave is NULL\n", i);
                // exit(-1);
            }

            WG0XCommand *c = (WG0XCommand *)this_buffer;
            memset(c, 0, command_size);
            
            if(i == target_slave){
                gettimeofday(&now, NULL);

                double currentTime_in_sec = ((double)now.tv_sec + ((double)now.tv_usec)/(1000.0*1000.0));
                double frequency = 1.0;
                
                // c->programmed_current_ = int(current / config_info_.nominal_current_scale_);
                c->programmed_current_ = (int)((current_amplitude/2.0) * (sin(2.0 * PI * frequency *  currentTime_in_sec) + 1.0));
                c->mode_ = (MODE_ENABLE | MODE_CURRENT); 
                // c->mode_ = MODE_OFF;
                //c->mode_ |= (reset ? MODE_SAFETY_RESET : 0);
                c->digital_out_ = 0; //digital_out_.command_.data_;
                c->checksum_ = rotateRight8(computeChecksum(c, command_size - 1));

                // double c_tm = (double)now.tv_sec * 1.0;
                // if(current_disp){
                //     printf("[RT Thread] Current set: %d, %f, %.2f, %.2f\n", c->programmed_current_, 
                //         sin(2.0 * PI * frequency *  currentTime_in_sec)+1.0,
                //         (current_amplitude/2.0),
                //         (current_amplitude/2.0)*(sin(2.0 * PI * frequency *  currentTime_in_sec)+1.0)
                //          );
                //     // current_disp = false;
                // }
            }

            this_buffer += command_size + status_size;
        }
        return 0;
    }

    int unpackCommand(){
        if(slave_handles.size() == 0){
            fprintf(stderr, "No slaves initialized\n");
            return -1;
        }
        unsigned int command_size = sizeof(WG0XCommand);
        unsigned int status_size = sizeof(WG0XStatus);

        unsigned char *this_buffer;
        this_buffer = this_buffer_;

        for(unsigned int i=0; i<slave_handles.size(); i++){
            if(slave_handles[i] == NULL) {
                fprintf(stderr, "Slave handler %u is NULL\n", i);
                // return -2;
            }
                
            WG0XStatus *status = (WG0XStatus *)(this_buffer+command_size);
            fprintf(stdout, "Slave No.: %u\t", i);
            //fprintf(stdout, "Digital Out: %u\t", status->digital_out_);
            fprintf(stdout, "Encoder Count: %d\t", status->encoder_count_);
            // fprintf(stdout, "Encoder Index pos: %d\t", status->encoder_index_pos_);
            // // state.position_ = double(this_status->encoder_count_) / actuator_info_.pulses_per_revolution_ * 2 * M_PI - state.zero_offset_;
            // fprintf(stdout, "Encoder errors: %d\t", status->num_encoder_errors_);
            // fprintf(stdout, "Packet Count: %d\t", status->packet_count_);
            // fprintf(stdout, "Timestamp: %u\t", status->timestamp_);

            fprintf(stdout, "\n");
            this_buffer += command_size + status_size;
        }

        return 0;
    }


    int update(){
        // unsigned char *this_buffer, *prev_buffer;
        // this_buffer = this_buffer_;
        // prev_buffer = prev_buffer_;

        pthread_mutex_lock(&mutex_buffer_access);
        
            packCommand();
            bool success = txandrx_PD(Master_Buffer_Size_, this_buffer_, max_pd_retries_);
            // if(success) {
            //     fprintf(stdout, "PD txrx succeeded\n");
            // } else {
            //     fprintf(stderr, "PD txrx failed\n");
            // }
            // unpackCommand();
            
            unsigned char *tmp = this_buffer_;
            this_buffer_ = prev_buffer_;
            prev_buffer_ = tmp;

        pthread_mutex_unlock(&mutex_buffer_access);

        return 0;
    }

    int monitorChanges(int threshold){
        static bool monitor_delta_init = false;
        static unsigned char *monitor_backup = new unsigned char [Master_Buffer_Size_];

        if(monitor_delta_init && prev_buffer_){

            unsigned int command_size = sizeof(WG0XCommand);
            unsigned int status_size = sizeof(WG0XStatus);

            unsigned char *prev_buffer, *back_buffer;
            prev_buffer = prev_buffer_;
            back_buffer = monitor_backup;

            pthread_mutex_lock(&mutex_buffer_access);

                for(unsigned int i=0; i<slave_handles.size(); i++){
                    if(slave_handles[i] == NULL) {
                        fprintf(stderr, "Slave handler %u is NULL\n", i);
                        // return -2;
                    }
                        
                    WG0XStatus *status_p = (WG0XStatus *)(prev_buffer + command_size);
                    WG0XStatus *status_b = (WG0XStatus *)(back_buffer + command_size);

                    //fprintf(stdout, "Slave No.: %u\t", i);
                    //fprintf(stdout, "Digital Out: %u\t", status->digital_out_);
                    if(abs(status_p->encoder_count_ - status_b->encoder_count_) > threshold) fprintf(stdout, "Slave No.: %u\tEncoder Count: %d\n", i, status_p->encoder_count_);
                    // fprintf(stdout, "Encoder Index pos: %d\t", status->encoder_index_pos_);
                    // // state.position_ = double(this_status->encoder_count_) / actuator_info_.pulses_per_revolution_ * 2 * M_PI - state.zero_offset_;
                    // fprintf(stdout, "Encoder errors: %d\t", status->num_encoder_errors_);
                    // fprintf(stdout, "Packet Count: %d\t", status->packet_count_);
                    // fprintf(stdout, "Timestamp: %u\t", status->timestamp_);

                    // fprintf(stdout, "\n");
                    prev_buffer += command_size + status_size;
                    back_buffer += command_size + status_size;
                }

            pthread_mutex_unlock(&mutex_buffer_access);


        }

        if(prev_buffer_){ // *** lock/lock-free
            
            pthread_mutex_lock(&mutex_buffer_access);
                memcpy(monitor_backup, prev_buffer_, Master_Buffer_Size_);
            pthread_mutex_unlock(&mutex_buffer_access);

            monitor_delta_init = true;
        }

        return 0;
    }
}

// static const int PWM_MAX = 0x4000; // wg05.h {L#347}

// ethercat_hardware.cpp {L#219}
// buffer_size_ += slaves_[slave]->command_size_ + slaves_[slave]->status_size_;

// wg05.cpp {L#133}
// bool poor_measured_motor_voltage = (board_major_ <= 2);
// double max_pwm_ratio = double(0x3C00) / double(PWM_MAX);
// double board_resistance = 0.8;