#include "SegmentDisplay.h"

#include "DigitalGPIO.h"
#include "AnalogOutput.h"
#include "Utilities.h"
#include "SegmentDisplayCharacter.h"

#include <util/delay.h>

using GPIOPtr = DigitalGPIO *;
using AOPtr = AnalogOutput *;

const int constexpr SegmentDisplay::DATA_PIN_COUNT;
const uint8_t constexpr SegmentDisplay::MAX_BRIGHTNESS;
const uint16_t constexpr SegmentDisplay::READY_TIMEOUT;
const uint8_t constexpr SegmentDisplay::COLUMN_COUNT;
const uint8_t constexpr SegmentDisplay::ROW_COUNT;

SegmentDisplay::SegmentDisplay(AOPtr brightnessPin, GPIOPtr controlPin, GPIOPtr readWritePin, GPIOPtr enablePin, GPIOPtr *dataPins) :
    m_brightnessPin{nullptr},
    m_controlPin{nullptr},
    m_readWritePin{nullptr},
    m_enablePin{nullptr},
    m_dataPins{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    m_column{0},
    m_row{0},
    m_brightness{0}
{
        
    this->setBrightnessPin(brightnessPin);
    this->setControlPin(controlPin);
    this->setReadWritePin(readWritePin);
    this->setEnablePin(enablePin);
    this->setDataPins(dataPins);
    this->enter8BitMode();
    this->setRowCount();
    this->turnOnDisplay();
    this->setBrightness(MAX_BRIGHTNESS);
    this->clearDisplay();  
    this->returnCursorHome();
    SegmentDisplayCharacter::segmentDisplay = this;
}

void SegmentDisplay::setCursorPosition(uint8_t column, uint8_t row) {
     if (column > COLUMN_COUNT) {
          column = COLUMN_COUNT;
     }
     if (row > ROW_COUNT) {
        row = ROW_COUNT;
     }
     while (row > this->m_row) {
        this->incrementCursor(); 
     }
     while (row < this->m_row) {
        this->decrementCursor();
     }
     while (column > this->m_column) {
        this->incrementCursor();
     }
     while (column < this->m_column) {
        this->decrementCursor();
     }
}

bool SegmentDisplay::waitForDisplayReady(unsigned long timeout) {
    auto startTime = millis();
    do {
        if (!this->checkBusyFlag()) {
            return true;
        }
    } while ( (millis() - startTime) <= static_cast<unsigned long>(timeout));
   
    return false;
}

bool SegmentDisplay::checkBusyFlag() {
  return false;
    /*
    auto returnByte = this->readGenericCommand();
    return (returnByte & (1 << 7));
    */
}

void SegmentDisplay::setRowCount() {
    this->digitalWriteByte(0b00111000);
 }

void SegmentDisplay::enter8BitMode() {
    for (int i = 0; i < 3; i++) {
        this->digitalWriteByte(0b00110000);
    }
}

void SegmentDisplay::clearDisplay() {
  this->writeGenericCommand(Command::ClearDisplay);
    this->m_column = 0;
  this->m_row = 0;
}
  
void SegmentDisplay::returnCursorHome() {
    this->writeGenericCommand(Command::ReturnHome);
    this->m_column = 0;
    this->m_row = 0;
    delay(100);
}

bool SegmentDisplay::write(char c) {
    return this->writeCharacter(c);
}

uint8_t SegmentDisplay::write(const char *str) {
    uint8_t returnValue{0};
    for (unsigned int i = 0; i < strlen(str); i++) {
        if (this->write(str)) {
            returnValue++;
        }
    }
    return returnValue;
}

bool SegmentDisplay::cursorIsAtEndOfTravel() {
    return ( (this->m_column == COLUMN_COUNT) && (this->m_row == ROW_COUNT) );
}

bool SegmentDisplay::cursorIsAtBeginningOfTravel() {
    return ( (this->m_column == 0) && (this->m_row == 0) );
}

bool SegmentDisplay::writeCharacter(char c) {
    if (this->cursorIsAtEndOfTravel()) {
        return false;
    }
    this->writeGenericCharacter(c);
    return true;
}

void SegmentDisplay::internalIncrementCursor() {
   if (this->m_column == COLUMN_COUNT) {
       if (this->m_row < ROW_COUNT) {
          this->m_row++;
           this->m_column = 0;
       }
    } else {
       this->m_column++;
    }
}

void SegmentDisplay::internalDecrementCursor() {
    if (this->m_column == 0) {
       if (this->m_row == ROW_COUNT) {
          this->m_row--;
          this->m_column = COLUMN_COUNT;
       }
    } else {
       this->m_column--;
    }
}


void SegmentDisplay::incrementCursor() {
      if (this->cursorIsAtEndOfTravel()) {
        return;
    }
    this->writeGenericCommand(Command::IncrementCursor);
    this->internalIncrementCursor();
}

void SegmentDisplay::decrementCursor() {
    if (this->cursorIsAtBeginningOfTravel()) {
        return;
    }
    this->writeGenericCommand(Command::DecrementCursor);
    this->internalDecrementCursor();
}

void SegmentDisplay::shiftDisplayLeft() {
    if (this->cursorIsAtBeginningOfTravel()) {
        return;
    }
    this->writeGenericCommand(Command::ShiftDisplayLeft);
    this->internalDecrementCursor();
}

void SegmentDisplay::shiftDisplayRight() {
    if (this->cursorIsAtEndOfTravel()) {
        return;
    }
    this->writeGenericCommand(Command::ShiftDisplayRight);
    this->internalIncrementCursor();
}

uint8_t SegmentDisplay::readGenericCommand() {
    this->setCommandMode(CommandMode::Data);
    this->setCommandDirection(CommandDirection::Read);
    this->m_enablePin->digitalWrite(true);
    uint8_t returnByte{this->digitalReadByte()};
    this->m_enablePin->digitalWrite(false);
    _delay_us(50);
    return returnByte;
}

void SegmentDisplay::writeGenericCommand(Command command) {
    return this->writeGenericCommand(static_cast<uint8_t>(command));
}

void SegmentDisplay::writeGenericCommand(uint8_t command) {
    this->setCommandMode(CommandMode::Command);
    this->setCommandDirection(CommandDirection::Write);
    this->digitalWriteByte(static_cast<uint8_t>(command));
    this->m_enablePin->digitalWrite(true);
    this->m_enablePin->digitalWrite(false);
    _delay_us(50);
}


char SegmentDisplay::readGenericCharacter() {
    this->setCommandMode(CommandMode::Data);
    this->setCommandDirection(CommandDirection::Read);
    this->m_enablePin->digitalWrite(true);
    this->m_enablePin->digitalWrite(false);
    uint8_t returnByte{this->digitalReadByte()};
    _delay_us(50);
    return static_cast<char>(returnByte);
}

void SegmentDisplay::writeGenericCharacter(char c) {
    this->setCommandMode(CommandMode::Data);
    this->setCommandDirection(CommandDirection::Write);
    this->digitalWriteByte(static_cast<uint8_t>(c));
    this->m_enablePin->digitalWrite(true);
    this->m_enablePin->digitalWrite(false);
    _delay_us(50);
    this->internalIncrementCursor();
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

void SegmentDisplay::characterAssigned(uint8_t column, uint8_t row, char character) {
   this->setCursorPosition(column, row);
   this->write(character);
}


const GPIOPtr *SegmentDisplay::dataPins() const { return this->m_dataPins; }
AOPtr SegmentDisplay::brightnessPin() const { return this->m_brightnessPin; }
GPIOPtr SegmentDisplay::controlPin() const { return this->m_controlPin; }
GPIOPtr SegmentDisplay::readWritePin() const { return this->m_readWritePin; }
GPIOPtr SegmentDisplay::enablePin() const { return this->m_enablePin; }
uint8_t SegmentDisplay::brightness() const { return this->m_brightness; }


