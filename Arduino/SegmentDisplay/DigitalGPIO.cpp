#include "DigitalGPIO.h"

DigitalGPIO::DigitalGPIO(IODirection direction,
        const DirectionChangeFunction &makeInputCallback,
        const DirectionChangeFunction &makeOutputCallback,
        const DigitalInputFunction &inputCallback, 
        const DigitalOutputFunction &outputCallback) :
    m_portDirection{direction},
    m_makeInputFunction{makeInputCallback},
    m_makeOutputFunction{makeOutputCallback},
    m_inputFunction{inputCallback},
    m_outputFunction{outputCallback},
    m_state{false}
{
    this->setPortDirection(direction);
}

void DigitalGPIO::setMakeInputFunction(const DirectionChangeFunction &function) {
    this->m_makeInputFunction = function;
}

void DigitalGPIO::setMakeOutputFunction(const DirectionChangeFunction &function) {
    this->m_makeOutputFunction = function;
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
        this->m_makeInputFunction();
        this->m_state = this->m_inputFunction();
    } else {
        this->m_makeOutputFunction();
        this->m_state = false;
        this->m_outputFunction(this->m_state);
    }
}

void DigitalGPIO::digitalWrite(bool state) {
    if (this->m_portDirection != IODirection::Output) {
        this->setPortDirection(IODirection::Output);
    }
    this->m_state = state;
    this->m_outputFunction(this->m_state);
}

bool DigitalGPIO::digitalRead() {
    if (this->m_portDirection != IODirection::Input) {
        this->setPortDirection(IODirection::Input);
    }
    return this->updateState();
}

bool DigitalGPIO::state() {
    return this->updateState();
}

bool DigitalGPIO::updateState() {
    if (this->m_portDirection == IODirection::Input) {
        this->m_state = this->m_inputFunction();
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

