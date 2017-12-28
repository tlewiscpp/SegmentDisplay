#ifndef SEGMENTDISPLAY_BEAGLEBONEBLACKGPIO_H
#define SEGMENTDISPLAY_BEAGLEBONEBLACKGPIO_H

namespace GPIO {
    class GPIOManager;
}
class DigitalGPIO;

#include <memory>
#include <unordered_set>

class BeagleBoneBlackGPIO {
public:
    using DigitalOutputFunction = std::function<void(bool)>;
    using DigitalInputFunction = std::function<bool(void)>;
    using DirectionChangeFunction = std::function<void(void)>;
 
    static void initializeInstance(GPIO::GPIOManager *manager);
    static BeagleBoneBlackGPIO *getInstance();
    std::unique_ptr<DigitalGPIO> makeDigitalGPIO(unsigned int pinNumber);

private:
    std::unordered_set<unsigned int> m_assignedIO;
    BeagleBoneBlackGPIO(GPIO::GPIOManager *manager);
    DirectionChangeFunction makeMakeInputFunction(unsigned int pinNumber);
    DirectionChangeFunction makeMakeOutputFunction(unsigned int pinNumber);
    DigitalOutputFunction makeDigitalOutputFunction(unsigned int pinNumber);
    DigitalInputFunction makeDigitalInputFunction(unsigned int pinNumber);
};

#endif //SEGMENTDISPLAY_BEAGLEBONEBLACKGPIO_H
