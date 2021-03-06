#ifndef SEGMENTDISPLAY_DIGITALGPIO_H
#define SEGMENTDISPLAY_DIGITALGPIO_H

enum class IODirection {
    Output,
    Input
};

using DigitalOutputFunction = void(*)(bool);
using DigitalInputFunction = bool(*)(void);
using DirectionChangeFunction = void(*)(void);

class DigitalGPIO {
public:
    DigitalGPIO(IODirection direction,
                 const DirectionChangeFunction &makeInputCallback,
                 const DirectionChangeFunction &makeOutputCallback,
                 const DigitalInputFunction &inputCallback,
                 const DigitalOutputFunction &outputCallback);


    void setMakeInputFunction(const DirectionChangeFunction &function);
    void setMakeOutputFunction(const DirectionChangeFunction &function);
    void setInputFunction(const DigitalInputFunction &function);
    void setOutputFunction(const DigitalOutputFunction &function);
    void setPortDirection(IODirection direction);
    void digitalWrite(bool state);
    bool digitalRead();
    bool state();
    IODirection portDirection() const;
    const DigitalInputFunction &inputFunction() const;
    const DigitalOutputFunction &outputFunction() const;
    const DirectionChangeFunction &makeInputFunction() const;
    const DirectionChangeFunction &makeOutputFunction() const;
private:
    IODirection m_portDirection;
    DirectionChangeFunction m_makeInputFunction;
    DirectionChangeFunction m_makeOutputFunction;  
    DigitalInputFunction m_inputFunction;
    DigitalOutputFunction m_outputFunction;
    bool m_state;
  
    bool updateState();
};

#endif //SEGMENTDISPLAY_GPIO_H

