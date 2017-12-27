#ifndef SEGMENTDISPLAY_GPIO_H
#define SEGMENTDISPLAY_GPIO_H

class DigitalGPIO {
public:
    using std::function<void(bool)> = DigitalOutputFunction;
    using std::function<bool(void)> = DigitalInputFunction;
    using std::function<void(void)> = DirectionChangeFunction;

    void setMakeInputFunction(const DirectionChangeFunction &function);
    void setMakeOutputFunction(const DirectionChangeFunction &function);
    void setInputFunction(const DigitalInputFunction &function);
    void setOutputFunction(const DigitalOutputFunction &function);
    void setDirection(IODirection direction);
    void digitalWrite(bool state);
    void digitalRead(bool state);
    bool state();
    const DigitalInputFunction &inputFunction() const;
    const DigitalOutputFunction &outputFunction() const;
    const DirectionChangeFunction &makeInputFunction() const;
    const DirectionChangeFunction &makeOutputFunction() const;
private:
    
    
    bool DigitalIO::updateState();
};

#endif //SEGMENTDISPLAY_GPIO_H
