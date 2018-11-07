#include "master_driver.h"

// int testLinking(){
//     printf("from driver\n");
//     return 0;
// }

namespace kul {

    EtherCAT_Master *em;
    uint32_t num_slaves;
    std::vector<EtherCAT_SlaveHandler*> slave_handles;
    EthercatOobCom *oob_com_;

    unsigned char *Master_Buffer_ = NULL;
    unsigned int Master_Buffer_Size_; 

    unsigned int max_pd_retries_ = 20;
    unsigned char *this_buffer_ = NULL, *prev_buffer_ = NULL;


    int pdInfo(const EtherCAT_PD_Config * const pdconfig) {
        if(!pdconfig){
            fprintf(stdout, "PD has NOT been initialized\n");
            // Check this: ethercat_harware/wg05.cpp #L53 -> WG05::construct()
            return -1;
        }
        // fprintf(stdout, "PD Config:\n");
        uint32_t num_sms = pdconfig->get_num_used_sms();
        fprintf(stdout, "Number of SMs: %3u\n", num_sms);

        for(unsigned int i=0; i<num_sms; i++){
            // al/ethercat_slave_conf.h #L76
            // dll/ethercat_slave_memory.h #L610
            // TODO: disp Sync Manager array
            fprintf(stdout, "SM: %u\n", i);
            
            fprintf(stdout, "\tLength: %u\n", (*pdconfig)[i].Length);
            fprintf(stdout, "\tPhysical Address: %u\n", (*pdconfig)[i].PhysicalStartAddress);
            fprintf(stdout, "\tBuffer Type: %s\n", NameBufferType((*pdconfig)[i].BufferType));
            fprintf(stdout, "\tBuffer State: %s\n", NameBufferState((*pdconfig)[i].BufferedState));
            fprintf(stdout, "\tDirection: %s\n", NameDirection((*pdconfig)[i].Direction));
            
            fprintf(stdout, "\tALEventEnable: %s\n", yesno((*pdconfig)[i].ALEventEnable));
            fprintf(stdout, "\tECATEventEnable: %s\n", yesno((*pdconfig)[i].ECATEventEnable));
            fprintf(stdout, "\tWatchdogEnable: %s\n", yesno((*pdconfig)[i].WatchdogEnable));
            fprintf(stdout, "\tWatchdogTrigger: %s\n", yesno((*pdconfig)[i].WatchdogTrigger));
            fprintf(stdout, "\tQueuedState: %s\n", yesno((*pdconfig)[i].QueuedState));
            fprintf(stdout, "\tReadEvent: %s\n", yesno((*pdconfig)[i].ReadEvent));
            fprintf(stdout, "\tWriteEvent: %s\n", yesno((*pdconfig)[i].WriteEvent));
            fprintf(stdout, "\tChannelEnable: %s\n", yesno((*pdconfig)[i].ChannelEnable));
        }
        
        return 0;
    }

    int mbxInfo(const EtherCAT_MbxConfig * const mbxConfig){
        if(!mbxConfig){
            fprintf(stdout, "MBX has NOT been initialized\n");
            return -1;
        }
        
        fprintf(stdout, "MBX initialized\n");
        return 0;
        
    }

    int fmmuInfo(const EtherCAT_FMMU_Config * const fmmuConfig){
        if(!fmmuConfig){
            fprintf(stdout, "FMMU has NOT been initialized\n");
            // Check this: ethercat_harware/wg05.cpp #L53 -> WG05::construct()
            return -1;
        }
        uint32_t num_fmmus = fmmuConfig->get_num_used_fmmus();
        fprintf(stdout, "Number of FMMU: %3u\n", num_fmmus);

        for(unsigned int i=0; i < num_fmmus; i++ ){
            fprintf(stdout, "FMMU %u\n", i);
            fprintf(stdout, "\tLength: %u\n", (*fmmuConfig)[i].Length);

            fprintf(stdout, "\tPhysical Addr: %u\n", (*fmmuConfig)[i].PhysicalStartAddress);
            fprintf(stdout, "\tPhysical Start Bit: %u\n", (uint8_t)(*fmmuConfig)[i].PhysicalStartBit);
            
            fprintf(stdout, "\tLogical Start Addr: %u\n", (*fmmuConfig)[i].LogicalStartAddress);
            fprintf(stdout, "\tLogical Start Bit: %u\n", (uint16_t)(*fmmuConfig)[i].LogicalStartBit);
            fprintf(stdout, "\tLogical End Bit: %u\n", (uint16_t)(*fmmuConfig)[i].LogicalEndBit);

            fprintf(stdout, "\tRead Enable: %s\n", yesno((*fmmuConfig)[i].ReadEnable));
            fprintf(stdout, "\tWrite Enable: %s\n", yesno((*fmmuConfig)[i].WriteEnable));
            fprintf(stdout, "\tChannel Enable: %s\n", yesno((*fmmuConfig)[i].ChannelEnable));
        }
        return 0;
    }

    int slaveInfo(std::vector<EtherCAT_SlaveHandler*> sh){
        for (unsigned int i = 0; i < sh.size(); ++i) {
            fprintf(stdout, "Slave: %u\n", i+1);
            slaveInfo(sh[i]);
        }
        return 0;
    }

    int slaveInfo(EtherCAT_SlaveHandler* sh){
        fprintf(stdout, "\tSerial Number: %u\n", sh->get_serial());
        fprintf(stdout, "\tProduct Code: %u\n", sh->get_product_code());
        fprintf(stdout, "\tSlave Revision: %u\n", sh->get_revision());
        
        fprintf(stdout, "\tStation Addr: %u\n", sh->get_station_address()-1);
        fprintf(stdout, "\tRing Position: %u\n", sh->get_ring_position());
        fprintf(stdout, "\tSlave State: %s\n", NameState(sh->get_state()));
        
        fprintf(stdout, "\tConfigured? %s\n", (sh->is_used())? "yes" : "no");
        fprintf(stdout, "\tComplex slave? %s\n", (sh->is_complex())? "yes" : "no");

        fprintf(stdout, "\tMbx Counter: %u\n", sh->get_mbx_counter());
        // increments on subsequent calls
        // fprintf(stdout, "Slave #%3u: Mbx Counter: %12u\n", i, sh->get_mbx_counter()); 

        fprintf(stdout, "\t[Valid for pr2 slaves] WG0%d (%#08x)\n", sh->get_product_code() % 100, sh->get_product_code());
        
        // WG EtherCAT devices (WG05,WG06,WG21) revisioning scheme
        // uint8_t fw_major_ = (sh->get_revision() >> 8) & 0xff;
        // uint8_t fw_minor_ = sh->get_revision() & 0xff;
        // uint8_t board_major_ = ((sh->get_revision() >> 24) & 0xff) - 1;
        // uint8_t board_minor_ = (sh->get_revision() >> 16) & 0xff;
        
        fmmuInfo(sh->get_fmmu_config());
        pdInfo(sh->get_pd_config());
        mbxInfo(sh->get_mbx_config());

        fprintf(stdout, "\n\n");
        return 0;
    }

    int getSlaveHandles(){
        
        for (unsigned int slave = 0; slave < num_slaves; ++slave) {

            EC_FixedStationAddress fsa(slave + 1);
            EtherCAT_SlaveHandler *sh = em->get_slave_handler(fsa);
            if (sh == NULL) {
                fprintf(stderr, "Unable to get slave handler #%d\n", slave);
                return -1;
            }
            
            slave_handles.push_back(sh);
        }
        fprintf(stdout, "Slave handles gathered: %lu\n", slave_handles.size());
        return 0;
    }
    
    // ethercatHardware.cpp -> EthercatHardware::init
    int initializeECatMaster(const char * const interface){
        int error = 0;

        // open temporary socket to use with ioctl
        int sock = socket(PF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {    
            fprintf(stderr,"Couldn't open temp : %s", strerror(error));
            exit(-1);
        }
        
        struct ifreq ifr;
        strncpy(ifr.ifr_name, interface, IFNAMSIZ);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
            fprintf(stderr,"Cannot get interface flags for %s: %s\n", interface, strerror(error));
            exit(-1);
        }

        error = close(sock);
        if(error){
            fprintf(stderr,"Cannot close the socket for %s: %s\n", interface, strerror(error));
        }

        sock = -1;

        if (!(ifr.ifr_flags & IFF_UP)) {
            fprintf(stderr,"Interface %s is not UP. Try : ifup %s\n", interface, interface);
            exit(-1);
        }
        if (!(ifr.ifr_flags & IFF_RUNNING)) {
            fprintf(stderr,"Interface %s is not RUNNING. Is cable plugged in and device powered?\n", interface);
            exit(-1);
        }

        struct netif *ni;

        // Initialize network interface
        if ((ni = init_ec(interface)) == NULL)
        {
            fprintf(stderr, "Unable to initialize interface: %s\n", interface);
            exit(-1);
        }

        oob_com_ = new EthercatOobCom(ni);

        // Initialize Application Layer (AL)
        EtherCAT_DataLinkLayer::instance()->attach(ni);
        EtherCAT_AL *al;
        if ((al = EtherCAT_AL::instance()) == NULL)
        {
            fprintf(stderr, "Unable to initialize Application Layer (AL): %p\n", al);
            exit(-1);
        }

        //   uint32_t num_slaves = al->get_num_slaves();
        num_slaves = al->get_num_slaves();
        if (num_slaves == 0)
        {
            fprintf(stderr, "Unable to locate any slaves\n");
            exit(-1);
        }
        else {
            fprintf(stdout, "Located %u slave(s)\n", num_slaves);
        }

        // Initialize Master
        //EtherCAT_Master *em;
        if ((em = EtherCAT_Master::instance()) == NULL)
        {
            fprintf(stderr, "Unable to initialize EtherCAT_Master: %p", em);
            exit(-1);
        }

        return 0;
    }

    bool txandrx_PD(unsigned buffer_size, unsigned char* buffer, unsigned tries){
        // Try multiple times to get proccess data to device
        bool success = false;
        for (unsigned i=0; i<tries && !success; ++i) {
            // Try transmitting process data
            success = em->txandrx_PD(Master_Buffer_Size_, Master_Buffer_);
        }
        if(!success){
            // Transmit new OOB data
            fprintf(stderr, "TxRx of PD failed after %u tries. Sending as OOB data\n", tries);
            oob_com_->tx();
        }
        return success;
    }

    int changeState(EtherCAT_SlaveHandler *sh, EC_State next_state){
        fprintf(stdout, "Slave at ring position %u:\tState change from %s to %s:\t",
            sh->get_ring_position(), NameState(sh->get_state()), NameState(next_state));
        if(! sh->to_state(next_state)){
            fprintf(stdout, "[Success]\n");
            return 0;
        }
        else {
            fprintf(stdout, "[Fail]\n");
            return -1;
        }
    }

    int closeOperationalSlaves(){
        int error = 0;
        for(unsigned int i=0; i< slave_handles.size(); i++){
            if(slave_handles[i]->get_state() == EC_OP_STATE){
                if(changeState(slave_handles[i], EC_SAFEOP_STATE) < 0) --error;
            }
        }
        return error;
    }

    
}