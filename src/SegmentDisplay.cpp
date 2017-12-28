#include "SegmentDisplay.h"
#include "MessageLogger.h"

#include "DigitalGPIO.h"

#include <thread>
#include <chrono>

using GPIOPtr = std::shared_ptr<DigitalGPIO>;

const int constexpr SegmentDisplay::DATA_PIN_COUNT;
const int constexpr SegmentDisplay::DEFAULT_COLUMN_COUNT;
const int constexpr SegmentDisplay::DEFAULT_ROW_COUNT;
const uint8_t constexpr SegmentDisplay::MAX_BRIGHTNESS;

SegmentDisplay::SegmentDisplay(GPIOPtr brightnessPin, GPIOPtr controlPin, GPIOPtr readWritePin, GPIOPtr enablePin,
                               const std::array<GPIOPtr, DATA_PIN_COUNT> &dataPins) :
    m_brightnessPin{nullptr},
    m_controlPin{nullptr},
    m_readWritePin{nullptr},
    m_enablePin{nullptr},
    m_dataPins{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    m_columnCount{DEFAULT_COLUMN_COUNT},
    m_rowCount{DEFAULT_ROW_COUNT},
    m_currentColumn{0},
    m_currentRow{0},
    m_brightness{0}
{
    this->setBrightnessPin(brightnessPin);
    this->setControlPin(controlPin);
    this->setReadWritePin(readWritePin);
    this->setDataPins(dataPins);
    this->setColumn(0);
    this->setRow(0);
    this->setBrightness(std::numeric_limits<uint8_t>::max());
}

void SegmentDisplay::write(char c) {
    this->writeCharacter(c);
}

void SegmentDisplay::write(const std::string &str) {
    for (const auto &it : str) {
        this->write(str);
    }
}

void SegmentDisplay::delay(unsigned int howLong) {
    std::this_thread::sleep_for(std::chrono::milliseconds(howLong));
}

void SegmentDisplay::writeCharacter(char c) {
    if (this->m_currentColumn == (this->m_columnCount - 1) ) {
        this->shiftDisplayLeft();
    }   
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
    this->delay(1);
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
    this->delay(1);
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
    for (uint8_t i = 0; i < this->m_dataPins.size(); i++) {
        uint8_t bitPosition{static_cast<uint8_t>(this->m_dataPins.size()) - i - 1};
        bool thisLevel{(command & (1 << bitPosition) ) ? true : false};
        this->m_dataPins[i]->digitalWrite(thisLevel);
    }
}

uint8_t SegmentDisplay::digitalReadByte() {
    uint8_t returnValue{0};
    for (uint8_t i = 0; i < static_cast<uint8_t>(this->m_dataPins.size()); i++) {
        uint8_t bitPosition{this->m_dataPins.size() - i - 1};
        this->m_dataPins[i]->setPortDirection(IODirection::Input);
        if (this->m_dataPins[i]->digitalRead()) {
            returnValue |= (1 << i);    
        } else {
            returnValue &= ~(1 << i);
        }
    }
    return returnValue;
}






void SegmentDisplay::setColumn(unsigned int column) {
    if (column >= this->m_columnCount) {
        throw std::runtime_error(TMessageLogger::TStringFormat("SegmentDisplay::setColumn(unsigned int): selected column cannot be greater than or equal to column count ({0} >= {1})", column, this->m_columnCount));
    }
    while (this->m_currentColumn != column) {
        //doSetColumn
    }
}

void SegmentDisplay::setRow(unsigned int row) {
    if (row >= this->m_rowCount) {
        throw std::runtime_error(TMessageLogger::TStringFormat("SegmentDisplay::setRow(unsigned int): selected row cannot be greater than or equal to row count ({0} >= {1})", row, this->m_rowCount));
    }
    while (this->m_currentRow != row) {
        //doSetRow
    }
}

void SegmentDisplay::setColumnCount(unsigned int columns) {
    if (this->m_currentColumn >= columns) {
        this->setColumn(columns - 1);
    }
    this->m_currentColumn = columns - 1;
    this->m_columnCount = columns;
}

void SegmentDisplay::setRowCount(unsigned int rows) {
    if (this->m_currentColumn >= rows) {
        this->setRow(rows - 1);
    }
    this->m_currentRow = rows - 1;
    this->m_rowCount = rows;
}

void SegmentDisplay::setDataPins(const std::array<GPIOPtr, SegmentDisplay::DATA_PIN_COUNT> dataPins) {
    std::copy(dataPins.begin(), dataPins.begin(), this->m_dataPins.begin());
    for (const auto &it : this->m_dataPins) {
        it->setPortDirection(IODirection::Output);
        it->digitalWrite(false);
    }
}

void SegmentDisplay::setBrightnessPin(GPIOPtr brightnessPin) {
    this->m_brightnessPin = brightnessPin;
    this->m_brightnessPin->setPortDirection(IODirection::Output);
    this->m_brightnessPin->digitalWrite(this->m_brightness);
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
    this->m_brightnessPin->digitalWrite(this->m_brightness);
}


const std::array<GPIOPtr, SegmentDisplay::DATA_PIN_COUNT> &SegmentDisplay::dataPins() const { return this->m_dataPins; }
GPIOPtr SegmentDisplay::brightnessPin() const { return this->m_brightnessPin; }
GPIOPtr SegmentDisplay::controlPin() const { return this->m_controlPin; }
GPIOPtr SegmentDisplay::readWritePin() const { return this->m_readWritePin; }
GPIOPtr SegmentDisplay::enablePin() const { return this->m_enablePin; }
unsigned int SegmentDisplay::columnCount() const { return this->m_columnCount; }
unsigned int SegmentDisplay::rowCount() const { return this->m_rowCount; }
uint8_t SegmentDisplay::brightness() const { return this->m_brightness; }

