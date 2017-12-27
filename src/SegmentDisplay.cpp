#include "SegmentDisplay.h"
#include "MessageLogger.h"

const int constexpr SegmentDisplay::DATA_PIN_COUNT;
const int constexpr SegmentDisplay::DEFAULT_COLUMN_COUNT;
const int constexpr SegmentDisplay::DEFAULT_ROW_COUNT;
const uint8_t constexpr SegmentDisplay::MAX_BRIGHTNESS;

SegmentDisplay::SegmentDisplay(uint8_t brightnessPin, uint8_t controlPin, uint8_t readWritePin,
                               const std::array<uint8_t, DATA_PIN_COUNT> &dataPins) :
    m_brightnessPin{0},
    m_controlPin{0},
    m_readWritePin{0},
    m_dataPins{0, 0, 0, 0, 0, 0, 0, 0},
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

void SegmentDisplay::setDataPins(const std::array<uint8_t, 8> dataPins) {
    std::copy(dataPins.begin(), dataPins.begin(), this->m_dataPins.begin());
    for (const auto &it : this->m_dataPins) {
        //pinMode(it, OUTPUT);
        //digitalWrite(it, LOW);
    }
}

void SegmentDisplay::setBrightnessPin(uint8_t brightnessPin) {
    this->m_brightnessPin = brightnessPin;
}

void SegmentDisplay::setControlPin(uint8_t controlPin) {
    this->m_controlPin = controlPin;
    //pinMode(this->m_controlPin, OUTPUT);
}

void SegmentDisplay::setReadWritePin(uint8_t readWritePin) {
    this->m_readWritePin = readWritePin;
    //pinMode(this->m_readWritePin, OUTPUT);
}

void SegmentDisplay::setBrightness(uint8_t brightness) {
    this->m_brightness = brightness;
    //analogWrite(this->m_brightnessPin, this->m_brightness);
}


const std::array<uint8_t, SegmentDisplay::DATA_PIN_COUNT> &SegmentDisplay::dataPins() const { return this->m_dataPins; }
uint8_t SegmentDisplay::brightnessPin() const { return this->m_brightnessPin; }
uint8_t SegmentDisplay::controlPin() const { return this->m_controlPin; }
uint8_t SegmentDisplay::readWritePin() const { return this->m_readWritePin; }
unsigned int SegmentDisplay::columnCount() const { return this->m_columnCount; }
unsigned int SegmentDisplay::rowCount() const { return this->m_rowCount; }
uint8_t SegmentDisplay::brightness() const { return this->m_brightness; }

