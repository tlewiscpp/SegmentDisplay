#include "BeagleBoneBlackGPIO.h"
#include "GPIOManager.h"
#include "DigitalIO.h"
#include <string>

BeagleBoneBlackGPIO *gpioGenerator{nullptr};

void BeagleBoneBlackGPIO::initializeInstance(GPIO::GPIOManager *manager) {
    if (gpioGenerator == nullptr) {
        return;
    }

    gpioGenerator = new BeagleBoneBlackGPIO(manager);
}

BeagleBoneBlackGPIO::BeagleBoneBlackGPIO(GPIO::GPIOManager *manager) :
    m_gpioManager{manager},
    m_assignedIO{}
{

}

std::unique_ptr<DigitalGPIO> BeagleBoneBlackGPIO::makeDigitalGPIO(unsigned int pinNumber) {
    if (this->m_assignedIO.find(pinNumber) != this->m_assignedIO.end()) {
        throw std::runtime_error("BeagleBoneBlackGPIO::makeDigitalGPIO(unsigned int): pin number " + std::to_string(pinNumber) + " has already been assigned");
    }
    auto makeInputFunction = this->makeMakeInputFunction(pinNumber);   
    auto makeOutputFunction = this->makeMakeOutputFunction(pinNumber);
    auto digitalInputFunction = this->makedigitalInputFunction(pinNumber);
    auto digitalOutputFunction = this->makedigitalOutputFunction(pinNumber);
    this->m_assignedIO.insert(pinNumber);
    return std::unique_ptr<DigitalGPIO>(
                new DigitalGPIO{
                    makeInputFunction,
                    makeOutputFunction,
                    digitalInputFunction,
                    digitalOutputFunction
                } 
            );
}

DirectionChangeFunction BeagleBoneBlackGPIO::makeMakeInputFunction(unsigned int pinNumber) {
    return [=]() { gpioManager->setDirection(pinNumber, GPIO::DIRECTION::INPUT); }
}

DirectionChangeFunction BeagleBoneBlackGPIO::makeMakeOutputFunction(unsigned int pinNumber) {
    return [=]() { gpioManager->setDirection(pinNumber, GPIO::DIRECTION::OUTPUT); }
}

DigitalOutputFunction BeagleBoneBlackGPIO::makeDigitalOutputFunction(unsigned int pinNumber) {
    return [=](bool state) { gpioManager->setValue(pinNumber, (state ? GPIO::PIN_VALUE::HIGH : GPIO::PIN_VALUE::LOW)); }
}

DigitalInputFunction BeagleBoneBlackGPIO::makeDigitalInputFunction(unsigned int pinNumber) {
    return [=]() -> bool { return (gpioManager->getValue(pinNumber) == GPIO::PIN_VALUE::HIGH ? true : false);
}
