#include "DigitalGPIO.h"

DigitalGPIO::DigitalGPIO(IODirection direction,
        const DirectionChangeFunction &makeInputCallback,
        const DirectionChangeFunction &makeOutputCallback,
        const DigitalInputFunction &inputCallback, 
        const DigitalOutputFunction &outputCallback) :
    m_portDirection{direction},
    m_makeInputCallback{makeInputCallback},
    m_makeOutputCallback{makeOutputCallback},
    m_inputFunction{inputFunction},
    m_outputFunction{outputFunction}
    m_state{false}
{
    this->setDirection(direction);
}

void DigitalGPIO::setMakeInputFunction(const DirectionChangeFunction &function) {
    this->m_makeInputCallback = function;
}

void DigitalGPIO::setMakeOutputFunction(const DirectionChangeFunction &function) {
    this->m_makeOutputCallback = function;
}

void DigitalGPIO::setInputFunction(const DigitalInputFunction &function) {
    this->m_inputFunction = function;
}

void DigitalGPIO::setOutputFunction(const DigitalOutputFunction &function) {
    this->m_outputFunction = function;
}

void DigitalGPIO::setPortDirection(IODirection direction) {
    this->m_portDirection = direction;
    if (this->m_portDirection == IODirection::Input) {
        this->m_makeInputCallback.operator()();
        this->m_state = this->m_inputFunction.operator()();
    } else {
        this->m_makeOutputCallback.operator()();
        this->m_state = false;
        this->m_outputFunction.operator()(this->m_state);
    }
}

void DigitalGPIO::digitalWrite(bool state) {
    if (this->m_portDirection != IODirection::Output) {
        this->setDirection(IODirection::Output);
    }
    this->m_state = state;
    this->m_outputFunction.operator()(this->m_state);
}

bool DigitalGPIO::digitalRead() {
    if (this->m_portDirection != IODirection::Input) {
        this->setDirection(IODirection::Input);
    }
    return this->updateState();
}

bool DigitalGPIO::state() {
    return this->updateState();
}

bool DigitalGPIO::updateState() {
    if (this->m_portDirection == IODirection::Input) {
        this->m_state = this->m_inputFunction.operator()();
    }
    return this->m_state;
}

IODirection DigitalGPIO::portDirection() const {
    return this->m_portDirection;
}

const DigitalInputFunction &DigitalGPIO::inputFunction() const {
    return this->m_inputFunction;
}

const DigitalOutputFunction &DigitalGPIO::outputFunction() const {
    return this->m_outputFunction;
}

const DirectionChangeFunction &DigitalGPIO::makeInputFunction() const {
    return this->m_makeInputFunction;
}

const DirectionChangeFunction &DigitalGPIO::makeOutputFunction() const {
    return this->m_makeOutputFunction;
}
