#ifndef SEGMENTDISPLAY_SEGMENTDISPLAY_H
#define SEGMENTDISPLAY_SEGMENTDISPLAY_H

#include <array>
#include <limits>
#include <algorithm>

class SegmentDisplay {
private:
    static const int constexpr DATA_PIN_COUNT{8};
    static const int constexpr DEFAULT_COLUMN_COUNT{16};
    static const int constexpr DEFAULT_ROW_COUNT{2};
    static const uint8_t constexpr MAX_BRIGHTNESS{std::numeric_limits<uint8_t>::max()};


public:
    SegmentDisplay(uint8_t brightnessPin, uint8_t controlPin, uint8_t readWritePin, const std::array<uint8_t, DATA_PIN_COUNT> &dataPins);

    void setColumnCount(unsigned int columns);
    void setRowCount(unsigned int rows);
    void setDataPins(const std::array<uint8_t, 8> dataPins);
    void setBrightnessPin(uint8_t brightnessPin);
    void setControlPin(uint8_t controlPin);
    void setReadWritePin(uint8_t readWritePin);

    void setBrightness(uint8_t brightness);

    const std::array<uint8_t, DATA_PIN_COUNT> &dataPins() const;;
    uint8_t brightnessPin() const;
    uint8_t controlPin() const;
    uint8_t readWritePin() const;
    unsigned int columnCount() const;
    unsigned int rowCount() const;
    uint8_t brightness() const;
private:
    uint8_t m_brightnessPin;
    uint8_t m_controlPin;
    uint8_t m_readWritePin;
    std::array<uint8_t, DATA_PIN_COUNT> m_dataPins;

    unsigned int m_columnCount;
    unsigned int m_rowCount;

    unsigned int m_currentColumn;
    unsigned int m_currentRow;
    uint8_t m_brightness;

    void setRow(unsigned int row);

    void setColumn(unsigned int column);
};


#endif //SEGMENTDISPLAY_SEGMENTDISPLAY_H
