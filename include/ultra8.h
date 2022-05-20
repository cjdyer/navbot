#include "ultrasonic.h"
#include <functional>

// Struct because code is simple and short, without .cpp file. Callback function created so that each sensor can do something specific
// when it is triggered. Only one instance of Ultra8 is currently in place because only tested on one sensor atm. Need to decide what 
// actions are to be taken when the sensor is within a set distance. Chose to pass argument through method UltraCreate else every function 
// would require a setup method. 

struct Ultra8
{
    using callback_t = std::function<void (void)>;  // void (newUltra8::*)()" to "std::function<void (void)

    callback_t callback;

    void actionNorth() {
        std::cout << "Include any code you want here to be invoked in the Ultra object" << '\n';
    }

// Include more functions to control each direction of sensor:

    // void actionSouth() {
    //     std::cout << "Include any code you want here to be invoked in the Ultra object" << '\n';
    // }

    // void actionEast() {
    //     std::cout << "Include any code you want here to be invoked in the Ultra object" << '\n';
    // }

    // void actionWest() {
    //     std::cout << "Include any code you want here to be invoked in the Ultra object" << '\n';
    // }

    int UltraCreate()
    {
        GPIO::gpioInitialise();
        // Need to instantiate 8 ultrasonic sensors
        Ultra N(14,18,1,std::bind(&Ultra8::actionNorth, this));
        // Ultra S(14,18,1,std::bind(&newUltra8::add, this));
        // Ultra E(14,18,1,std::bind(&newUltra8::add, this));
        // Ultra W(14,18,1,std::bind(&newUltra8::add, this));
        // Ultra NE(14,18,1,std::bind(&newUltra8::add, this));
        // Ultra NW(14,18,1,std::bind(&newUltra8::add, this));
        // Ultra SE(14,18,1,std::bind(&newUltra8::add, this));
        // Ultra SW(14,18,1,std::bind(&newUltra8::add, this));
    
        return 0;
};

};