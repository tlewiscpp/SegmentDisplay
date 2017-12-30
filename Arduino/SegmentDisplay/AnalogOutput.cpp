#include "AnalogOutput.h"

AnalogOutput::AnalogOutput(const AnalogOutputFunction &analogOutputFunction) :
    m_analogOutputFunction{analogOutputFunction},
    m_state{0}
{
    this->analogWrite(this->m_state);
}

void AnalogOutput::analogWrite(unsigned int state) {
    this->m_state = state;
    this->m_analogOutputFunction(this->m_state);
}

unsigned int AnalogOutput::state() const {
    return this->m_state;
}

void AnalogOutput::setAnalogOutputFunction(const AnalogOutputFunction &analogOutputFunction) {
    this->m_analogOutputFunction = analogOutputFunction;
}

const AnalogOutputFunction &AnalogOutput::analogOutputFunction() const {
    return this->m_analogOutputFunction;
}

