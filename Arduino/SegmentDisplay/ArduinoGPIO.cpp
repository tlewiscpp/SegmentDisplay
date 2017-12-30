
#include "ArduinoGPIO.h"

static ArduinoGPIO *gpioGenerator{nullptr};

void ArduinoGPIO::initializeInstance() {
    if (gpioGenerator == nullptr) {
        gpioGenerator = new ArduinoGPIO();
    }
}

ArduinoGPIO *ArduinoGPIO::getInstance() {
    if (gpioGenerator == nullptr) {
        ArduinoGPIO::initializeInstance();
    }
    return gpioGenerator;
}


