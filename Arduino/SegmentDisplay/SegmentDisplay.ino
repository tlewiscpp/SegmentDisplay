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
    
    segmentDisplay->clearDisplay();
    segmentDisplay->returnCursorHome();
}

const char *ALL_WORK_STRING{"AllWorkAndNoPlayMakesTylerADullBoy"};
void doCharacterWrite(const char *str);

void doCommandWrite(const char *str);

void loop() {
  /*
    for (unsigned int i = 0; i < strlen(ALL_WORK_STRING); i++) {
        segmentDisplay->write(ALL_WORK_STRING[i]);
        delay(100);
    }
    delay(500);
    segmentDisplay->clearDisplay();
    return;
 */
    
    if (!readSerialIO()) {
        return;
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
             segmentDisplay->clearDisplay();
             break;
        }
        if (it == 'w') {
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
