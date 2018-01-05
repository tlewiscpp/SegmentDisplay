#ifndef SEGMENTDISPLAY_SEGMENTDISPLAY_H
#define SEGMENTDISPLAY_SEGMENTDISPLAY_H

#include <stdint.h>

enum class CommandDirection {
    Write = 0,
    Read = 1
};

enum class CommandMode {
    Command = 0,
    Data = 1
};

//0   0   0   1   S/C   R/L   *   *

enum class Command {
    LcdOnCursorOn            = 0x0F,
    ClearDisplay             = 0x01,
    ReturnHome               = 0x02,
    DecrementCursor          = 0b00010000,
    IncrementCursor          = 0b00010100,
    ShiftDisplayLeft         = 0b00011000,
    ShiftDisplayRight        = 0b00011100,
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
class AnalogOutput;
class SegmentDisplayCharacter;

class SegmentDisplay {
  friend class SegmentDisplayCharacter;
private:
    static const uint8_t constexpr MAX_BRIGHTNESS{255};
    static const uint16_t constexpr READY_TIMEOUT{1000};
    static const uint8_t constexpr COLUMN_COUNT{19}; //20 columns
    static const uint8_t constexpr ROW_COUNT{1}; //2 rows

public:
    static const int constexpr DATA_PIN_COUNT{8};
    using GPIOPtr = DigitalGPIO *;
    using AOPtr = AnalogOutput *;

    SegmentDisplay(AOPtr brightnessPin, 
                    GPIOPtr controlPin, 
                    GPIOPtr readWritePin, 
                    GPIOPtr enablePin,
                    GPIOPtr *dataPins);

    SegmentDisplay(const SegmentDisplay &rhs) = delete;
    SegmentDisplay(SegmentDisplay &&rhs) = delete;
    SegmentDisplay &operator=(const SegmentDisplay &rhs) = delete;
    SegmentDisplay &operator=(SegmentDisplay &&rhs) = delete;
    ~SegmentDisplay();

    void turnOnDisplay();
    void turnOffDisplay();
    void setCursorBlinking(bool blinking);
    void setCursorVisible(bool visible);
    uint8_t write(const char *str);
    bool write(char c);
    void clearDisplay();
    void returnCursorHome();
    void incrementCursor();
    void decrementCursor();
    void shiftDisplayLeft();
    void shiftDisplayRight();
    void writeGenericCommand(Command command);
    void writeGenericCommand(uint8_t command);
    void setCursorPosition(uint8_t row, uint8_t column);

    //Member access
    void setDataPins(GPIOPtr *dataPins);
    void setBrightnessPin(AOPtr brightnessPin);
    void setControlPin(GPIOPtr controlPin);
    void setReadWritePin(GPIOPtr readWritePin);
    void setEnablePin(GPIOPtr enablePin);
    void setBrightness(uint8_t brightness);

    const GPIOPtr *dataPins() const;
    AOPtr brightnessPin() const;
    GPIOPtr controlPin() const;
    GPIOPtr readWritePin() const;
    GPIOPtr enablePin() const;
    uint8_t brightness() const;
private:
    AOPtr m_brightnessPin;
    GPIOPtr m_controlPin;
    GPIOPtr m_readWritePin;
    GPIOPtr m_enablePin;
    GPIOPtr m_dataPins[DATA_PIN_COUNT];
    uint8_t m_column;
    uint8_t m_row;
    uint8_t m_brightness;

    
    bool cursorIsAtEndOfTravel();
    bool cursorIsAtBeginningOfTravel();
    void enter8BitMode();

    bool waitForDisplayReady(unsigned long timeout = 1000);
    bool checkBusyFlag();
    void setRowCount();
    uint8_t readGenericCommand();
    char readGenericCharacter();
    void writeGenericCharacter(char c);
    bool writeCharacter(char c);
    void setCommandDirection(CommandDirection commandDirection);
    void setCommandMode(CommandMode commandMode);
    void digitalWriteByte(uint8_t command);
    uint8_t digitalReadByte();

    void characterAssigned(uint8_t column, uint8_t row, char character);

    void internalIncrementCursor();
    void internalDecrementCursor();
};


#endif //SEGMENTDISPLAY_SEGMENTDISPLAY_H

