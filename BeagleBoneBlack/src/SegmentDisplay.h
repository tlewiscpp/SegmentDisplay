#ifndef SEGMENTDISPLAY_SEGMENTDISPLAY_H
#define SEGMENTDISPLAY_SEGMENTDISPLAY_H

#include <array>
#include <memory>
#include <limits>
#include <algorithm>

enum class CommandDirection {
    Write = 0,
    Read = 1
};

enum class CommandMode {
    Command = 0,
    Data = 1
};

enum class Command {
    LcdOnCursorOn            = 0x0F,
    ClearDisplay             = 0x01,
    ReturnHome               = 0x02,
    DecrementCursor          = 0x04,
    IncrementCursor          = 0x06,
    ShiftDisplayRight        = 0x05,
    ShiftDisplayLeft         = 0x07,
    LcdOnCursorBlinking      = 0x0E,
    FirstLineCarriageReturn  = 0x80,
    SecondLineCarriageReturn = 0xC0,
    LcdOnCursorOff           = 0x0C,
    LcdOffCursorOff          = 0x08

//0x38 - 2 lines and 5×7 matrix
//0x83 - Cursor line 1 position 3
//0x3C - Activate second line
//0xC1 - Jump to second line, position 1
//0xC1 - Jump to second line, position 1
//0xC2 - Jump to second line, position 2
};

class DigitalGPIO;

class SegmentDisplay {
private:
    static const int constexpr DEFAULT_COLUMN_COUNT{16};
    static const int constexpr DEFAULT_ROW_COUNT{2};
    static const uint8_t constexpr MAX_BRIGHTNESS{std::numeric_limits<uint8_t>::max()};


public:
    static const int constexpr DATA_PIN_COUNT{8};
    using GPIOPtr = std::shared_ptr<DigitalGPIO>;

    SegmentDisplay(GPIOPtr brightnessPin, 
                    GPIOPtr controlPin, 
                    GPIOPtr readWritePin, 
                    GPIOPtr enablePin,
                    const std::array<GPIOPtr, DATA_PIN_COUNT> &dataPins);

    SegmentDisplay(const SegmentDisplay &rhs) = delete;
    SegmentDisplay(SegmentDisplay &&rhs) = delete;
    SegmentDisplay &operator=(const SegmentDisplay &rhs) = delete;
    SegmentDisplay &operator=(SegmentDisplay &&rhs) = delete;
    ~SegmentDisplay() = default;

    void turnOnDisplay();
    void turnOffDisplay();
    void setCursorBlinking(bool blinking);
    void setCursorVisible(bool visible);
    void write(const std::string &str);
    void write(char c);




    //Member access
    void setColumnCount(unsigned int columns);
    void setRowCount(unsigned int rows);
    void setDataPins(const std::array<GPIOPtr, DATA_PIN_COUNT> dataPins);
    void setBrightnessPin(GPIOPtr brightnessPin);
    void setControlPin(GPIOPtr controlPin);
    void setReadWritePin(GPIOPtr readWritePin);
    void setEnablePin(GPIOPtr enablePin);
    void setBrightness(uint8_t brightness);

    const std::array<GPIOPtr, DATA_PIN_COUNT> &dataPins() const;;
    GPIOPtr brightnessPin() const;
    GPIOPtr controlPin() const;
    GPIOPtr readWritePin() const;
    GPIOPtr enablePin() const;
    unsigned int columnCount() const;
    unsigned int rowCount() const;
    uint8_t brightness() const;
private:
    GPIOPtr m_brightnessPin;
    GPIOPtr m_controlPin;
    GPIOPtr m_readWritePin;
    GPIOPtr m_enablePin;
    std::array<GPIOPtr, DATA_PIN_COUNT> m_dataPins;
    unsigned int m_columnCount;
    unsigned int m_rowCount;
    unsigned int m_currentColumn;
    unsigned int m_currentRow;
    uint8_t m_brightness;

    void setRow(unsigned int row);
    void setColumn(unsigned int column);
    void delay(unsigned int howLong);
    uint8_t readGenericCommand();
    void writeGenericCommand(Command command);
    char readGenericCharacter();
    void writeGenericCharacter(char c);
    void writeCharacter(char c);
    void incrementCursor();
    void decrementCursor();
    void shiftDisplayLeft();
    void shiftDisplayRight();
    void setCommandDirection(CommandDirection commandDirection);
    void setCommandMode(CommandMode commandMode);
    void digitalWriteByte(uint8_t command);
    uint8_t digitalReadByte();
};


#endif //SEGMENTDISPLAY_SEGMENTDISPLAY_H
