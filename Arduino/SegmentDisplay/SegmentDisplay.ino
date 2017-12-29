#include "DigitalGPIO.h"
#include "SegmentDisplay.h"
#include "AnalogOutput.h"
#include "ArduinoGPIO.h"
#include "Utilities.h"


#define CONTRAST_CONTROL 44
#define REGISTER_SELECT 22
#define READ_WRITE_SELECT 23
#define COMMAND_ENABLE 24
#define DATA_0 25
#define DATA_1 26
#define DATA_2 27
#define DATA_3 28
#define DATA_4 29
#define DATA_5 30
#define DATA_6 31
#define DATA_7 32

#define SERIAL_BAUD_RATE 115200

#define MESSAGE_BUFFER_MAX 64
#define IO_BUFFER_MAX 128

static char messageBuffer[MESSAGE_BUFFER_MAX];
static char ioBuffer[IO_BUFFER_MAX];

bool readSerialIO();

using namespace Utilities;

SegmentDisplay *segmentDisplay{nullptr};

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

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
}

const char *ALL_WORK_STRING{"AllWorkAndNoPlayMakesTylerADullBoy"};
void doCharacterWrite(const char *str);

void loop() {
    /*
    for (unsigned int i = 0; i < strlen(ALL_WORK_STRING); i++) {
        segmentDisplay->write(ALL_WORK_STRING[i]);
        delay(100);
    }
    delay(500);
    segmentDisplay->clearDisplay();
    return;

  
    if (!readSerialIO()) {
        return;
    }
    if (ioBuffer[0] == '\t') {
        uint8_t commandCharacter{safeParse<uint8_t>(ioBuffer + 1)};
        Serial.print("Writing command ");
        toFixedWidthHex(messageBuffer, commandCharacter, 2);
        Serial.println(messageBuffer);       
    } else {
        auto stringLength = strlen(ioBuffer);
        for (unsigned int i = 0; i < stringLength; i++) {
            auto it = ioBuffer[i];
            if (isPrintable(it)) {
                Serial.print("Writing character ");
                Serial.println(it);
                segmentDisplay->write(it);
            } else {
                Serial.print("Skipping non-printable character (code = ");
                toFixedWidthHex(messageBuffer, it, 2); 
                Serial.print(messageBuffer);
                Serial.println(')');    
            }
            delay(10);
            auto brightness = static_cast<unsigned int>(255.0 / stringLength);
            //segmentDisplay->setBrightness(brightness);
        }
    }
    memset(ioBuffer, '\0', IO_BUFFER_MAX);

    */
    if (!readSerialIO()) {
        return;
    }
    auto stringLength = strlen(ioBuffer);
    for (unsigned int i = 0; i < stringLength; i++) {
        auto it = ioBuffer[i];
        switch(it) {
          case '<':
             segmentDisplay->decrementCursor();
             break;
          case '>':
             segmentDisplay->incrementCursor();
             break;
          case 'd':
             segmentDisplay->shiftDisplayRight();
             break;
          case 'a':
             segmentDisplay->shiftDisplayLeft();
             break;
          case 'c':
             doCharacterWrite(ioBuffer + 1);
             break;
        }
    }
    memset(ioBuffer, '\0', IO_BUFFER_MAX);

}

void doCharacterWrite(const char *str) {
    auto stringLength = strlen(str);
    for (unsigned int i = 0; i < stringLength; i++) {
      auto it = str[i];
      if (isPrintable(it)) {
          Serial.print("Writing character ");
          Serial.println(it);
          segmentDisplay->write(it);
      } else {
          Serial.print("Skipping non-printable character (code = ");
          toFixedWidthHex(messageBuffer, it, 2); 
          Serial.print(messageBuffer);
          Serial.println(')');    
      }
      delay(10);
    }
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
