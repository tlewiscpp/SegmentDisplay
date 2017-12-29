#ifndef SEGMENTDISPLAY_ANALOGOUTPUT_H
#define SEGMENTDISPLAY_ANALOGOUTPUT_H

#include "ArduinoGPIO.h"

using AnalogOutputFunction = void(*)(unsigned int);

class AnalogOutput {
public:
    AnalogOutput(const AnalogOutputFunction &analogOutputFunction);

    void setAnalogOutputFunction(const AnalogOutputFunction &analogOutputFunction);
    const AnalogOutputFunction &analogOutputFunction() const;

    void analogWrite(unsigned int state);
    unsigned int state() const;

private:
    AnalogOutputFunction m_analogOutputFunction;
    unsigned int m_state;
};


#endif //SEGMENTDISPLAY_ANALOGOUTPUT_H
