//#include "driver/master_driver.h"
#include "driver/wg05.h"
#include <stdio.h>
#include <sys/time.h>
#include <sched.h>
#include <pthread.h>

#define ENCODER_THRESHOLD_MOVEMENT 1 // non-negative integer

#define RT_LOOP_WAIT_TIME 900 // in micro-seconds (us)
#define MONITOR_LOOP_WAIT_TIME 500000 // in micro-seconds (us)

static int __RUN_THREADS__;

pthread_mutex_t rt_loop_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  rt_loop_cond  = PTHREAD_COND_INITIALIZER;

pthread_mutex_t monitor_loop_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  monitor_loop_cond  = PTHREAD_COND_INITIALIZER;

void * realTimeLoop(void *){
    struct timespec timer_interrupt;
    struct timeval now;

    pthread_mutex_lock(&rt_loop_mutex);

    while(__RUN_THREADS__) {
        gettimeofday(&now, NULL);
        timer_interrupt.tv_sec = now.tv_sec;
        timer_interrupt.tv_nsec = (now.tv_usec + RT_LOOP_WAIT_TIME) * 1000;
        
        pthread_cond_timedwait(&rt_loop_cond, &rt_loop_mutex, &timer_interrupt);
        kul::update();
    }

    pthread_mutex_unlock(&rt_loop_mutex);
    return NULL;
}

void * monitorChanges(void *){
    struct timespec timer_interrupt;
    struct timeval now;

    pthread_mutex_lock(&monitor_loop_mutex);

    while(__RUN_THREADS__) {
        gettimeofday(&now, NULL);
        timer_interrupt.tv_sec = now.tv_sec;
        timer_interrupt.tv_nsec = (now.tv_usec + MONITOR_LOOP_WAIT_TIME) * 1000;
        
        pthread_cond_timedwait(&monitor_loop_cond, &monitor_loop_mutex, &timer_interrupt);
        kul::monitorChanges(ENCODER_THRESHOLD_MOVEMENT);
    }

    pthread_mutex_unlock(&monitor_loop_mutex);
    return NULL;
}

int main(){
    // testLinking();
    
    // kul::initializeECatMaster("eno1");
    // kul::getSlaveHandles();
    // // kul::slaveInfo(kul::slave_handles);

    // kul::findWG05slaves(kul::slave_handles);
    // // kul::slaveInfo(kul::wg05_slave_handles);

    // kul::configureWG05fmmu(kul::wg05_slave_handles[2]);
    // // kul::slaveInfo(kul::wg05_slave_handles);

    // kul::configureWG05pd(kul::wg05_slave_handles[2]);
    // kul::slaveInfo(kul::wg05_slave_handles);

    // ------------------------------------

    kul::initializePr2EthercatSlaves("eno1");
    //kul::slaveInfo(kul::wg05_slave_handles);

    pthread_t rt_thread, monitor_thread;

    struct sched_param monitor_thread_param;
    struct sched_param rt_thread_param;
    struct sched_param main_thread_param;

    memset(&monitor_thread_param, 0, sizeof(monitor_thread_param));
    memset(&rt_thread_param, 0, sizeof(rt_thread_param));
    memset(&main_thread_param, 0, sizeof(main_thread_param));

    /* do not set priority above 49, otherwise sockets are starved */
    main_thread_param.sched_priority = 20;
    rt_thread_param.sched_priority = 40;
    monitor_thread_param.sched_priority = 30;

    // set current thread priority
    sched_setscheduler(0, SCHED_FIFO, &main_thread_param);

    __RUN_THREADS__ = 1;

    // run real-time thread with higher priority
    pthread_create( &rt_thread, NULL, &realTimeLoop, NULL);
    pthread_setschedparam(rt_thread, SCHED_OTHER, &rt_thread_param);

    // run monitoring thread
    pthread_create( &monitor_thread, NULL, &monitorChanges, NULL);
    pthread_setschedparam(monitor_thread, SCHED_OTHER, &monitor_thread_param);

    // getchar();   // [ENTER] to stop

    int human_loop=1;
    double amplitude=10.0;

    printf("READY\n");
    printf("Target Slave: %d\n", kul::target_slave);

    printf("Slave Number: ");
    scanf("%d", &kul::target_slave);

    while(human_loop){
        printf("Amplitude: ");
        scanf("%lf", &amplitude);
        printf("[USER INPUT] %lf\n", amplitude);
        if(amplitude > 0){
            kul::current_amplitude = amplitude; 
            kul::current_disp = true;
        }
        else { human_loop = 0; }
    }

    __RUN_THREADS__ = 0;

    pthread_join(rt_thread, NULL);
    pthread_join(monitor_thread, NULL);

    
    //realTimeLoop();

    kul::closeOperationalSlaves();

    return 0;
}