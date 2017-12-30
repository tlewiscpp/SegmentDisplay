#include "AnalogInput.h"

AnalogInput::AnalogInput(const AnalogInputFunction &analogInputFunction) :
    m_analogInputFunction{analogInputFunction}
{

}

void AnalogInput::setAnalogInputFunction(const AnalogInputFunction &analogInputFunction) {
    this->m_analogInputFunction = analogInputFunction;
}

const AnalogInputFunction &AnalogInput::analogInputFunction() const {
    return this->m_analogInputFunction;
}

unsigned int AnalogInput::analogRead() {
    return this->m_analogInputFunction();
}

