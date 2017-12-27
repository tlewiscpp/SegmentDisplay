#ifndef SEGMENTDISPLAY_BEAGLEBONEBLACKGPIO_H
#define SEGMENTDISPLAY_BEAGLEBONEBLACKGPIO_H

namespace GPIO {
    class GPIOManager;
}
class DigitalGPIO;

#include <memory>
#include <set>

class BeagleBoneBlackGPIO {
    void initializeInstance(GPIO::GPIOManager *manager);
    std::unique_ptr<DigitalGPIO> makeGPIO(unsigned int pinNumber);

private:
    std::unordered_set<unsigned int> m_assignedIO;
    BeagleBoneBlackGPIO(GPIO::GPIOManager *manager);
    DirectionChangeFunction makeMakeInputFunction(unsigned int pinNumber);
    DirectionChangeFunction makeMakeOutputFunction(unsigned int pinNumber);
    DigitalOutputFunction makeDigitalOutputFunction(unsigned int pinNumber);
    DigitalInputFunction makeDigitalInputFunction(unsigned int pinNumber);
};

extern BeagleBoneBlackGPIO *gpioGenerator;

#endif //SEGMENTDISPLAY_BEAGLEBONEBLACKGPIO_H
