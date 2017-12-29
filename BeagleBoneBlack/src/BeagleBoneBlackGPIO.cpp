#include "BeagleBoneBlackGPIO.h"
#include "GPIOManager.h"
#include "DigitalGPIO.h"

static BeagleBoneBlackGPIO *gpioGenerator{nullptr};

void BeagleBoneBlackGPIO::initializeInstance(GPIO::GPIOManager *manager) {
    if (gpioGenerator == nullptr) {
        gpioGenerator = new BeagleBoneBlackGPIO(manager);
    }
}

BeagleBoneBlackGPIO *BeagleBoneBlackGPIO::getInstance() {
    if (gpioGenerator == nullptr) {
        throw std::runtime_error("BeagleBoneBlackGPIO::getInstance(): Singleton instance is a nullptr. Call BeagleBoneBlackGPIO::initializeInstance(GPIO::GPIOManager *) first");
    }
    return gpioGenerator;
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
    auto digitalInputFunction = this->makeDigitalInputFunction(pinNumber);
    auto digitalOutputFunction = this->makeDigitalOutputFunction(pinNumber);
    this->m_assignedIO.insert(pinNumber);
    return std::unique_ptr<DigitalGPIO>(
                new DigitalGPIO{
                        IODirection::Input,
                    makeInputFunction,
                    makeOutputFunction,
                    digitalInputFunction,
                    digitalOutputFunction
                } 
            );
}

DirectionChangeFunction BeagleBoneBlackGPIO::makeMakeInputFunction(unsigned int pinNumber) {
    return [=]() { this->m_gpioManager->setDirection(pinNumber, GPIO::DIRECTION::INPUT); };
}

DirectionChangeFunction BeagleBoneBlackGPIO::makeMakeOutputFunction(unsigned int pinNumber) {
    return [=]() { this->m_gpioManager->setDirection(pinNumber, GPIO::DIRECTION::OUTPUT); };
}

DigitalOutputFunction BeagleBoneBlackGPIO::makeDigitalOutputFunction(unsigned int pinNumber) {
    return [=](bool state) { this->m_gpioManager->setValue(pinNumber, (state ? GPIO::PIN_VALUE::HIGH : GPIO::PIN_VALUE::LOW)); };
}

DigitalInputFunction BeagleBoneBlackGPIO::makeDigitalInputFunction(unsigned int pinNumber) {
    return [=]() -> bool { return this->m_gpioManager->getValue(pinNumber) == GPIO::PIN_VALUE::HIGH; };
}
