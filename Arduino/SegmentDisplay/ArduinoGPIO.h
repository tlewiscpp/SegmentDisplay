#ifndef SEGMENTDISPLAY_ARDUINOGPIO_H
#define SEGMENTDISPLAY_ARDUINOGPIO_H

class DigitalGPIO;
class AnalogOutput;
class AnalogInput;

#include <Arduino.h>
#include "ArduinoGPIO.h"
#include "DigitalGPIO.h"
#include "AnalogOutput.h"
#include "AnalogInput.h"

using DigitalOutputFunction = void(*)(bool);
using DigitalInputFunction = bool(*)(void);
using AnalogInputFunction = unsigned int(*)(void);
using AnalogOutputFunction = void(*)(unsigned int);
using DirectionChangeFunction = void(*)(void);

class ArduinoGPIO {
public: 
    static void initializeInstance();
    static ArduinoGPIO *getInstance();


    template <unsigned int PinNumber>
    DigitalGPIO *makeDigitalGPIO() {
        return new DigitalGPIO{
                    IODirection::Input,
                    this->makeMakeInputFunction<PinNumber>(),
                    this->makeMakeOutputFunction<PinNumber>(),
                    this->makeDigitalInputFunction<PinNumber>(),
                    this->makeDigitalOutputFunction<PinNumber>()
                };
    }
    
    template <unsigned int PinNumber>
    AnalogOutput *makeAnalogOutput() {
        return new AnalogOutput {
            this->makeAnalogOutputFunction<PinNumber>()
        };
    }
    
    template <unsigned int PinNumber>
    AnalogInput *makeAnalogInput() {
        return new AnalogInput{
            this->makeAnalogInputFunction<PinNumber>()
        };
    }

private:
    ArduinoGPIO() = default;
    ArduinoGPIO(const ArduinoGPIO &rhs) = delete;
    ArduinoGPIO(ArduinoGPIO &&rhs) = delete;
    ArduinoGPIO &operator=(const ArduinoGPIO &rhs) = delete;
    ArduinoGPIO &operator=(ArduinoGPIO &&rhs) = delete;
    

    template <unsigned int PinNumber>
    static constexpr DirectionChangeFunction makeMakeInputFunction() {
        return []() { pinMode(PinNumber, INPUT); };
    }

    template <unsigned int PinNumber>
    static constexpr DirectionChangeFunction makeMakeOutputFunction() {
        return []() { pinMode(PinNumber, OUTPUT); };
    }

    template <unsigned int PinNumber>
    static constexpr DigitalOutputFunction makeDigitalOutputFunction() {
        return [](bool state) { digitalWrite(PinNumber, state); };
    }

    template <unsigned int PinNumber>
    static constexpr DigitalInputFunction makeDigitalInputFunction() {
        return []() -> bool { return digitalRead(PinNumber); };
    }

    template <unsigned int PinNumber>
    static constexpr AnalogOutputFunction makeAnalogOutputFunction() {
        return [](unsigned int value) { return analogWrite(PinNumber, value); };
    }

    template <unsigned int PinNumber>
    static constexpr AnalogInputFunction makeAnalogInputFunction() {
        return []() -> unsigned int { return analogRead(PinNumber); };
    }
};

#endif //SEGMENTDISPLAY_BEAGLEBONEBLACKGPIO_H
