#include "ultrasonic.h"
#include "log.h"
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

// bool processed = false;

void add()
{
   std::cout << "Include any code you want here to be invoked in the Ultra object" << '\n';
//    return 0;
}

int main()
{
    GPIO::gpioInitialise();
    // Need to instantiate 8 ultrasonic sensors
    Ultra N, S;
    // , E, W, NE, NW, SW, SE;
    S.setup(14,18, 1, &add);
    // N.setup(17,22, 2, &add);
    // E.setup(15,18);
    // W.setup(15,18);
    // NE.setup(15,18);
    // NW.setup(15,18);
    // SE.setup(15,18);
    // SW.setup(15,18);
    // a.main_test();  
    // here we use invoke as a callback function, which calls a function add that is written to achieve a specific task. 
    // this then allows main_test to be reused for all 8 ultrasonics. 
    // S.ultra_setcallback(&add);
    // N.ultra_setcallback(&add);
    // S.invoke(&add);
    // S.invoke(&add);
    

    // std::mutex m;
    // std::condition_variable cv;
    // std::string data;
    // bool ready = false;
    
    // std::thread worker(&Ultra::run_ultrasonic, &a);

    // while(1)
    // {
    //     // wait for the worker
        
    //         std::unique_lock<std::mutex> lk(m);
    //         cv.wait(lk, []{return processed;}); 
        
        
    //     // This is where we can do other stuff in response to the ultrasonic interrupt.
    //     std::cout << "Back in main(), data = " << data << '\n';

    //     //

    //     processed = false;
    // }

    // worker.join();
    return 0;
};