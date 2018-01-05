#include "DigitalGPIO.h"
#include "SegmentDisplay.h"
#include "AnalogOutput.h"
#include "ArduinoGPIO.h"
#include "Utilities.h"
#include "SegmentDisplayCharacter.h"
#include "EEPROM.h"

#if defined(ARDUINO_AVR_MEGA2560)
#    define CONTRAST_CONTROL 44
#    define REGISTER_SELECT 22
#    define READ_WRITE_SELECT 23
#    define COMMAND_ENABLE 24
#    define DATA_0 25
#    define DATA_1 26
#    define DATA_2 27
#    define DATA_3 28
#    define DATA_4 29
#    define DATA_5 30
#    define DATA_6 31
#    define DATA_7 32
#    define EEPROM_RESET_PIN A5
#    define SEGMENT_BRIGHTNESS 128
#elif defined(ARDUINO_AVR_UNO)
#    define CONTRAST_CONTROL 11
#    define REGISTER_SELECT 12
#    define READ_WRITE_SELECT 10
#    define COMMAND_ENABLE 9
#    define DATA_0 2
#    define DATA_1 3
#    define DATA_2 4
#    define DATA_3 5
#    define DATA_4 6
#    define DATA_5 7
#    define DATA_6 8
#    define DATA_7 A0
#    define EEPROM_RESET_PIN A5
#    define SEGMENT_BRIGHTNESS 128
#else
#    error "Unsupported AVR"
#endif //defined(AVR_MEGA2560)


#define SERIAL_BAUD_RATE 115200

#define MESSAGE_BUFFER_MAX 64
#define IO_BUFFER_MAX 128
#define COLUMN_COUNT 20
#define ROW_COUNT 2
#define DISPLAY_BUFFER_SIZE ((COLUMN_COUNT*ROW_COUNT)+1)
#define EEPROM_MAGIC_VALUE 0x69
#define DISPLAY_REFRESH_TIMEOUT 1000
#define DISPLAY_REFRESH_WRITE_DELAY 100

static char messageBuffer[MESSAGE_BUFFER_MAX];
static char ioBuffer[IO_BUFFER_MAX];
static char displayBuffer[DISPLAY_BUFFER_SIZE];
static uint8_t displayBufferIndex{0};
static unsigned long displayRefreshStartTime{millis()};

bool readSerialIO();
bool readCharactersFromEEPROM();
void doCharacterWrite(const char *str);
void initializeEEPROM();
void doCommandWrite(const char *str);
void doImAliveBlink();
bool writeToEEPROM(uint16_t index, uint8_t character);
bool doDisplayRefresh();
bool delayCallback(unsigned long howLong, bool (*func)());

using namespace Utilities;

SegmentDisplay *segmentDisplay{nullptr};

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    pinMode(EEPROM_RESET_PIN, INPUT_PULLUP);

    ArduinoGPIO::initializeInstance();
    auto gpioGenerator = ArduinoGPIO::getInstance();
    auto contrastControl = gpioGenerator->makeAnalogOutput<CONTRAST_CONTROL>();
    auto registerSelect = gpioGenerator->makeDigitalGPIO<REGISTER_SELECT>();
    auto readWriteSelect = gpioGenerator->makeDigitalGPIO<READ_WRITE_SELECT>();
    auto commandEnable = gpioGenerator->makeDigitalGPIO<COMMAND_ENABLE>();
    DigitalGPIO *dataPins[SegmentDisplay::DATA_PIN_COUNT] {
        gpioGenerator->makeDigitalGPIO<DATA_0>(),
        gpioGenerator->makeDigitalGPIO<DATA_1>(),
        gpioGenerator->makeDigitalGPIO<DATA_2>(),
        gpioGenerator->makeDigitalGPIO<DATA_3>(),
        gpioGenerator->makeDigitalGPIO<DATA_4>(),
        gpioGenerator->makeDigitalGPIO<DATA_5>(),
        gpioGenerator->makeDigitalGPIO<DATA_6>(),
        gpioGenerator->makeDigitalGPIO<DATA_7>()
    };

    segmentDisplay = new SegmentDisplay{
        contrastControl,
        registerSelect,
        readWriteSelect,
        commandEnable,
        dataPins
    };
    segmentDisplay->setCursorBlinking(true);
    segmentDisplay->clearDisplay();
    segmentDisplay->returnCursorHome();
    segmentDisplay->setBrightness(SEGMENT_BRIGHTNESS);


    if (readCharactersFromEEPROM()) {
        for (int i = 0; i < displayBufferIndex; i++) {
            segmentDisplay->write(displayBuffer[i]);
        }
    }
    
}

void loop() {
    doImAliveBlink();
    if (!readSerialIO()) {
        if (!doDisplayRefresh()) {
            return;  
        }
    }
    auto stringLength = strlen(ioBuffer);
    for (unsigned int i = 0; i < stringLength; i++) {
        auto it = ioBuffer[i];
        switch(it) {
          case '<':
             Serial.println("Decrementing cursor");
             segmentDisplay->decrementCursor();
             break;
          case '>':
             Serial.println("Incrementing cursor");
             segmentDisplay->incrementCursor();
             break;
          case 'd':
            Serial.println("Shifting display right");
             segmentDisplay->shiftDisplayRight();
             break;
          case 'a':
             Serial.println("Shifting display left");
             segmentDisplay->shiftDisplayLeft();
             break;
          case 'w':
             doCharacterWrite(ioBuffer + 1);
             break;
          case 'x':
             doCommandWrite(ioBuffer + 1);
             break;
          case 'h':
            Serial.println("Returning cursor home");
            segmentDisplay->returnCursorHome();
            break;
          case 'c':
             Serial.println("Clearing display");
             doClearSegmentDisplay();
             break;
          case '(':
             doCursorPosition(ioBuffer + 1);
             break;
          default:
             if (isDigit(it)) {
                doCursorPosition(ioBuffer);
             }
             break;
             
        }
        if ( (it == 'w') || (isDigit(it)) || (it == '(') ) {
           break;
        }
    }
    memset(ioBuffer, '\0', IO_BUFFER_MAX);
}

bool delayCallback(unsigned long howLong, bool (*func)()) {
  auto startTime = millis();
    do {
       if ( (func) && (*func)() ) return true;
    } while ( (millis() - startTime) < howLong);
    return false;
}

bool doDisplayRefresh() {
    if ( (millis() - displayRefreshStartTime) >= DISPLAY_REFRESH_TIMEOUT) {
        segmentDisplay->clearDisplay();
        delay(200);
        for (unsigned int i = 0; i < displayBufferIndex; i++) {
            segmentDisplay->write(displayBuffer[i]);
            if (delayCallback(DISPLAY_REFRESH_WRITE_DELAY, readSerialIO)) {
               return true;
            }
        }
        displayRefreshStartTime = millis();
    }
    return false;
}

void doClearSegmentDisplay() { 
    segmentDisplay->clearDisplay();
    memset(displayBuffer, '\0', DISPLAY_BUFFER_SIZE);
    for (unsigned int i = 0; i <= displayBufferIndex; i++) {
        writeToEEPROM(i, 0x00); 
    }
    displayBufferIndex = 0;
}

void initializeEEPROM() {
  //Clear EEPROM, as it hasn't been written yet
  for (unsigned int i = 0; i < EEPROM.length(); i++) {
      //If i is at the last EEPROM address, write the magic number. Otherwise, write 0x00
      EEPROM.write(i, ( (i == (EEPROM.length() - 1)) ? EEPROM_MAGIC_VALUE : 0x00) ); 
  }      
  Serial.println("Initialization of EEPROM successful");

}

bool readCharactersFromEEPROM() {
    if (digitalRead(EEPROM_RESET_PIN) == LOW) {
        initializeEEPROM();
        return false;
    }
    auto magicValue = EEPROM.read(EEPROM.length() - 1);
    Serial.print("Read ");
    Serial.print(static_cast<int>(magicValue));
    Serial.println(" from last EEPROM address");
    if (magicValue != EEPROM_MAGIC_VALUE) {
        initializeEEPROM();
        return false;
    } else {
        for ( auto readValue = EEPROM.read(displayBufferIndex); readValue != 0x00; readValue = EEPROM.read(++displayBufferIndex)) {
            auto maybeCharacter = static_cast<char>(readValue);
            displayBuffer[displayBufferIndex] = maybeCharacter;
            if (displayBufferIndex == (DISPLAY_BUFFER_SIZE - 1)) {
                break;
            }
        }
        displayBuffer[displayBufferIndex] = '\0';
        Serial.print("Read character string \"");
        Serial.print(displayBuffer);
        Serial.println("\" from EEPROM");
        return true;
    }
}

bool writeToEEPROM(uint16_t index, uint8_t character) {
    if (index >= EEPROM.length()) {
       return false;
    }
    EEPROM.write(index, character);
    return true;
}

void doCursorPosition(const char *str) {
    size_t splitSize{arraySplit(str, splitBuffer, ",")};
    if (splitSize != 2) {
        Serial.print("Invalid cursor position, wrong number of arguments (expected 2, got ");
        Serial.print(splitSize);
        Serial.print(", str = ");
        Serial.println(str);
        return;
    }
    auto stringLength = strlen(splitBuffer[1]);
    if (splitBuffer[1][stringLength - 1] == ')') {
       splitBuffer[1][stringLength - 1] = '\0';
    }
    uint8_t first{safeParse<uint8_t>(splitBuffer[0])};
    uint8_t second{safeParse<uint8_t>(splitBuffer[1])};
    if (!isAllZeroes(splitBuffer[0]) && (first == 0) ) {
        Serial.print("Invalid cursor position argument \"");
        Serial.print(splitBuffer[0]);
        Serial.println("\"");
        return;
    }
    if (!isAllZeroes(splitBuffer[1]) && (second == 0) ) {
        Serial.print("Invalid cursor position argument \"");
        Serial.print(splitBuffer[1]);
        Serial.println("\"");
        return;
    }
    Serial.print("Setting cursor position to (");
    Serial.print(static_cast<int>(first));
    Serial.print(", ");
    Serial.print(static_cast<int>(second));
    Serial.println(')');
    segmentDisplay->setCursorPosition(first, second);
}

void doCharacterWrite(const char *str) {
    doClearSegmentDisplay();
    auto stringLength = strlen(str);
    for (unsigned int i = 0; i < stringLength; i++) {
      auto it = str[i];
      if (isPrintable(it)) {
          Serial.print("Writing character ");
          Serial.println(it);
          if (!segmentDisplay->write(it)) {
              return;
          }
          displayBuffer[displayBufferIndex] = it;
          writeToEEPROM(displayBufferIndex, it);
          displayBufferIndex++;
      } else {
          Serial.print("Skipping non-printable character (code = ");
          toFixedWidthHex(messageBuffer, it, 2); 
          Serial.print(messageBuffer);
          Serial.println(')');    
      }
    }
}

void doCommandWrite(const char *str) {
    auto command = Utilities::safeParse<uint8_t>(str);
    Serial.print("Writing command ");
    toFixedWidthHex(messageBuffer, command, 2); 
    Serial.print(messageBuffer);
    Serial.println(')');    
    segmentDisplay->writeGenericCommand(command);
}

bool readSerialIO() {
    bool lineEndingRead{false};
    static int currentStringLength{0};
    while (Serial.available()) {
        char readCharacter{static_cast<char>(Serial.read())};
        if ((isPrintable(readCharacter)) || (readCharacter == '\n')) {
            ioBuffer[currentStringLength] = readCharacter;
            ioBuffer[currentStringLength + 1] = '\0';
            
            if ( (currentStringLength+2) == IO_BUFFER_MAX) {
                currentStringLength = 0;
            } else {
                currentStringLength++;
            }
        }
        if (readCharacter == '\n') {
            lineEndingRead = true;
            //Strip newline
            ioBuffer[currentStringLength - 1] = '\0';
            break;
        }
    }
    if (lineEndingRead) {
        currentStringLength = 0; 
    }
    return lineEndingRead;
}

void doImAliveBlink() {
    #define BLINK_TIMEOUT 750
    static auto startTime = millis();
    static bool state{false};
    if ( (millis() - startTime) <= BLINK_TIMEOUT) {
        state = !state;
        digitalWrite(LED_BUILTIN, state);
        startTime = millis();
    }
}

