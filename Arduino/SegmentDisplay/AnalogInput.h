#ifndef SEGMENTDISPLAY_ANALOGINPUT_H
#define SEGMENTDISPLAY_ANALOGINPUT_H

#include "ArduinoGPIO.h"

using AnalogInputFunction = unsigned int(*)(void);

class AnalogInput {
public:
    AnalogInput(const AnalogInputFunction &analogInputFunction);

    void setAnalogInputFunction(const AnalogInputFunction &analogInputFunction);
    const AnalogInputFunction &analogInputFunction() const;

    unsigned int analogRead();

private:
    AnalogInputFunction m_analogInputFunction;
};


#endif //SEGMENTDISPLAY_ANALOGINPUT_H
