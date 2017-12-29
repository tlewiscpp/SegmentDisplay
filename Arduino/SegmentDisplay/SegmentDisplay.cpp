#include "SegmentDisplay.h"

#include "DigitalGPIO.h"
#include "AnalogOutput.h"

using GPIOPtr = DigitalGPIO *;
using AOPtr = AnalogOutput *;

const int constexpr SegmentDisplay::DATA_PIN_COUNT;
const uint8_t constexpr SegmentDisplay::MAX_BRIGHTNESS;

SegmentDisplay::SegmentDisplay(AOPtr brightnessPin, GPIOPtr controlPin, GPIOPtr readWritePin, GPIOPtr enablePin, GPIOPtr *dataPins) :
    m_brightnessPin{nullptr},
    m_controlPin{nullptr},
    m_readWritePin{nullptr},
    m_enablePin{nullptr},
    m_dataPins{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    m_brightness{0}
{
    this->setBrightnessPin(brightnessPin);
    this->setControlPin(controlPin);
    this->setReadWritePin(readWritePin);
    this->setEnablePin(enablePin);
    this->setDataPins(dataPins);
    this->turnOnDisplay();
    this->setBrightness(MAX_BRIGHTNESS);
    this->clearDisplay();
    this->incrementCursor();
}

void SegmentDisplay::clearDisplay() {
    this->doClearDisplay();
    this->returnCursorHome();
}

void SegmentDisplay::doClearDisplay() {
   this->writeGenericCommand(Command::ClearDisplay);
}

void SegmentDisplay::returnCursorHome() {
    this->writeGenericCommand(Command::ReturnHome);
}

void SegmentDisplay::write(char c) {
    this->writeCharacter(c);
}

void SegmentDisplay::write(const char *str) {
    for (unsigned int i = 0; i < strlen(str); i++) {
        this->write(str);
    }
}

void SegmentDisplay::writeCharacter(char c) {
    this->writeGenericCharacter(c);
    this->incrementCursor();
}

void SegmentDisplay::incrementCursor() {
    this->writeGenericCommand(Command::IncrementCursor);
}

void SegmentDisplay::decrementCursor() {
    this->writeGenericCommand(Command::DecrementCursor);
}

void SegmentDisplay::shiftDisplayLeft() {
    this->writeGenericCommand(Command::ShiftDisplayLeft);
}

void SegmentDisplay::shiftDisplayRight() {
    this->writeGenericCommand(Command::ShiftDisplayLeft);
}

uint8_t SegmentDisplay::readGenericCommand() {
    this->setCommandMode(CommandMode::Data);
    this->setCommandDirection(CommandDirection::Read);
    this->m_enablePin->digitalWrite(true);
    uint8_t returnByte{this->digitalReadByte()};
    this->m_enablePin->digitalWrite(false);
    return returnByte;
}

void SegmentDisplay::writeGenericCommand(Command command) {
    this->setCommandMode(CommandMode::Command);
    this->setCommandDirection(CommandDirection::Write);
    this->digitalWriteByte(static_cast<uint8_t>(command));
    this->m_enablePin->digitalWrite(true);
    this->m_enablePin->digitalWrite(false);
}

char SegmentDisplay::readGenericCharacter() {
    this->setCommandMode(CommandMode::Data);
    this->setCommandDirection(CommandDirection::Read);
    this->m_enablePin->digitalWrite(true);
    uint8_t returnByte{this->digitalReadByte()};
    this->m_enablePin->digitalWrite(false);
    return static_cast<char>(returnByte);
}

void SegmentDisplay::writeGenericCharacter(char c) {
    this->setCommandMode(CommandMode::Data);
    this->setCommandDirection(CommandDirection::Write);
    this->digitalWriteByte(static_cast<uint8_t>(c));
    this->m_enablePin->digitalWrite(true);
    this->m_enablePin->digitalWrite(false);
}


void SegmentDisplay::setCursorBlinking(bool blinking) {
    if (blinking) {
        this->writeGenericCommand(Command::LcdOnCursorOn);
    } else {
        this->writeGenericCommand(Command::LcdOnCursorBlinking);
    }
}

void SegmentDisplay::setCursorVisible(bool visible) {
    if (visible) {
        this->writeGenericCommand(Command::LcdOnCursorOn);
    } else {
        this->writeGenericCommand(Command::LcdOnCursorOff);
    }
}

void SegmentDisplay::turnOnDisplay() {
    this->writeGenericCommand(Command::LcdOnCursorOn);
}  

void SegmentDisplay::turnOffDisplay() {
    this->writeGenericCommand(Command::LcdOffCursorOff);
}

void SegmentDisplay::setCommandDirection(CommandDirection commandDirection) {
    this->m_readWritePin->digitalWrite(static_cast<bool>(commandDirection));
}

void SegmentDisplay::setCommandMode(CommandMode commandMode) {
    this->m_controlPin->digitalWrite(static_cast<bool>(commandMode));
}

void SegmentDisplay::digitalWriteByte(uint8_t command) {
    for (uint8_t i = 0; i < DATA_PIN_COUNT; i++) {
        this->m_dataPins[i]->setPortDirection(IODirection::Output);
        //uint8_t bitPosition{static_cast<uint8_t>(this->m_dataPins.size() - i - 1)};
        uint8_t bitPosition{static_cast<uint8_t>(i)};
        bool thisLevel{(command & (1 << bitPosition) ) ? true : false};
        this->m_dataPins[i]->digitalWrite(thisLevel);
    }
}

uint8_t SegmentDisplay::digitalReadByte() {
    uint8_t returnValue{0};
    for (uint8_t i = 0; i < DATA_PIN_COUNT; i++) {
        this->m_dataPins[i]->setPortDirection(IODirection::Input);
        if (this->m_dataPins[i]->digitalRead()) {
            returnValue |= (1 << i);    
        } else {
            returnValue &= ~(1 << i);
        }
    }
    return returnValue;
}

void SegmentDisplay::setDataPins(GPIOPtr *dataPins) {
    for (int i = 0; i < DATA_PIN_COUNT; i++) {
        if (this->m_dataPins[i]) {
            delete this->m_dataPins[i];
        }
        this->m_dataPins[i] = dataPins[i];
    }
    for (const auto &it : this->m_dataPins) {
        it->setPortDirection(IODirection::Output);
        it->digitalWrite(false);
    }
}

void SegmentDisplay::setBrightnessPin(AOPtr brightnessPin) {
    this->m_brightnessPin = brightnessPin;
    this->setBrightness(this->m_brightness);
}

void SegmentDisplay::setControlPin(GPIOPtr controlPin) {
    this->m_controlPin = controlPin;
    this->m_controlPin->setPortDirection(IODirection::Output);
}

void SegmentDisplay::setReadWritePin(GPIOPtr readWritePin) {
    this->m_readWritePin = readWritePin;
    this->m_readWritePin->setPortDirection(IODirection::Output);
}

void SegmentDisplay::setEnablePin(GPIOPtr enablePin) {
    this->m_enablePin = enablePin;
    this->m_enablePin->setPortDirection(IODirection::Output);
    this->m_enablePin->digitalWrite(false);
}

void SegmentDisplay::setBrightness(uint8_t brightness) {
    this->m_brightness = brightness;
    this->m_brightnessPin->analogWrite(this->m_brightness);
}

SegmentDisplay::~SegmentDisplay() {
    delete this->m_brightnessPin;
    delete this->m_enablePin;
    delete this->m_controlPin;
    delete this->m_readWritePin;
    for (int i = 0; i < DATA_PIN_COUNT; i++) {
        delete this->m_dataPins[i];
    }
}

const GPIOPtr *SegmentDisplay::dataPins() const { return this->m_dataPins; }
AOPtr SegmentDisplay::brightnessPin() const { return this->m_brightnessPin; }
GPIOPtr SegmentDisplay::controlPin() const { return this->m_controlPin; }
GPIOPtr SegmentDisplay::readWritePin() const { return this->m_readWritePin; }
GPIOPtr SegmentDisplay::enablePin() const { return this->m_enablePin; }
uint8_t SegmentDisplay::brightness() const { return this->m_brightness; }

