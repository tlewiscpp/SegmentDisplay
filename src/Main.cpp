#include <iostream>
#include <bitset>
#include <vector>
#include <sstream>
#include "ApplicationUtilities.h"
#include "GlobalDefinitions.h"
#include "MessageLogger.h"
#include "SegmentDisplay.h"

#include "../../BeagleBoneBlack-GPIO/GPIO/GPIOManager.h"
#include "BeagleBoneBlackGPIO.h"
#include "DigitalGPIO.h"

#include <fstream>
#include <forward_list>

#include <unistd.h>
#include <sys/types.h>
#include <getopt.h>

using namespace ApplicationUtilities;
using namespace TMessageLogger;

using GPIOPtr = std::shared_ptr<DigitalGPIO>;

static const int constexpr DATA_PIN_COUNT{8};

static const uint8_t constexpr CONTRAST_CONTROL{66};
static const uint8_t constexpr REGISTER_SELECT{67};
static const uint8_t constexpr READ_WRITE_SELECT{69};
static const uint8_t constexpr COMMAND_ENABLE{61};
static const uint8_t constexpr DATA_0{68};
static const uint8_t constexpr DATA_1{45};
static const uint8_t constexpr DATA_2{44};
static const uint8_t constexpr DATA_3{26};
static const uint8_t constexpr DATA_4{47};
static const uint8_t constexpr DATA_5{46};
static const uint8_t constexpr DATA_6{27};
static const uint8_t constexpr DATA_7{65};

static struct option longOptions[]
{
    {"verbose",  no_argument,   nullptr, 'e'},
    {"help",     no_argument,   nullptr, 'h'},
    {"version",  no_argument,   nullptr, 'v'},
    {nullptr, 0, nullptr, 0}
};

std::shared_ptr<SegmentDisplay> segmentDisplay{nullptr};

int main(int argc, char *argv[])
{
    MessageLogger::initializeInstance();
    (void)messageLogger->installLogHandler(ApplicationUtilities::globalLogHandler);

    int currentOption{0};
    int optionIndex{0};
    opterr = 0;

    while ( (currentOption = getopt_long(argc, argv, "hv", longOptions, &optionIndex)) != -1) {
        switch (currentOption) {
            case 'h':
                displayHelp();
                exit(EXIT_SUCCESS);
            case 'v':
                displayVersion();
                exit(EXIT_SUCCESS);
            case 'e':
                ApplicationUtilities::verboseLogging = true;
                break;
            default:
                LOG_WARN() << TStringFormat(R"(Unknown option "{0}", skipping)", longOptions[optionIndex].name);
        }
    }
    displayVersion();
    LOG_INFO() << TStringFormat("Using LogFile {0}", ApplicationUtilities::getLogFilePath());
    auto euid = geteuid();
    if (euid != 0) {
        LOG_FATAL() << TStringFormat("This program must be run as root ({0} != 0)", euid);
    }
    auto gpioManager = GPIO::GPIOManager::getInstance();
    BeagleBoneBlackGPIO::initializeInstance(gpioManager);
    auto gpioGenerator = BeagleBoneBlackGPIO::getInstance();
    gpioManager->exportPin(CONTRAST_CONTROL);
    gpioManager->exportPin(REGISTER_SELECT);
    gpioManager->exportPin(READ_WRITE_SELECT);
    gpioManager->exportPin(COMMAND_ENABLE);
    gpioManager->exportPin(DATA_0);
    gpioManager->exportPin(DATA_1);
    gpioManager->exportPin(DATA_2);
    gpioManager->exportPin(DATA_3);
    gpioManager->exportPin(DATA_4);
    gpioManager->exportPin(DATA_5);
    gpioManager->exportPin(DATA_6);
    gpioManager->exportPin(DATA_7);
    GPIOPtr contrastControl = gpioGenerator->makeDigitalGPIO(CONTRAST_CONTROL);
    GPIOPtr registerSelect = gpioGenerator->makeDigitalGPIO(REGISTER_SELECT);
    GPIOPtr readWriteSelect = gpioGenerator->makeDigitalGPIO(READ_WRITE_SELECT);
    GPIOPtr commandEnable = gpioGenerator->makeDigitalGPIO(COMMAND_ENABLE);
    std::array<GPIOPtr, DATA_PIN_COUNT> dataPinArray{
        gpioGenerator->makeDigitalGPIO(DATA_0),
        gpioGenerator->makeDigitalGPIO(DATA_1),
        gpioGenerator->makeDigitalGPIO(DATA_2),
        gpioGenerator->makeDigitalGPIO(DATA_3),
        gpioGenerator->makeDigitalGPIO(DATA_4),
        gpioGenerator->makeDigitalGPIO(DATA_5),
        gpioGenerator->makeDigitalGPIO(DATA_6),
        gpioGenerator->makeDigitalGPIO(DATA_7)
    };

    segmentDisplay = std::make_shared<SegmentDisplay> (
            contrastControl,
            registerSelect,
            readWriteSelect,
            commandEnable,
            dataPinArray
    );
    segmentDisplay->turnOnDisplay();
    segmentDisplay->setCursorBlinking(true);
    std::string readLine{""};
    while (true) {
        std::getline(std::cin, readLine);
        if (readLine.empty()) {
            continue;
        }
        if (readLine[0] == '\t') {
            uint8_t commandCharacter{safeParse<uint8_t>(readLine.substr(1))};
            LOG_INFO() << TStringFormat("Writing command {0}", toFixedWidthHex(commandCharacter, 2));
        } else {
            for (const auto &it : readLine) {
                if (::isalnum(it) && !::isspace(it)) {
                    LOG_INFO() << TStringFormat("Writing character {0}", std::string(1, it));
                    segmentDisplay->write(it);
                } else {
                    LOG_INFO() << TStringFormat("Skipping non-printable character (code = {0})", toFixedWidthHex(it, 2));
                }
                delay(500);
            }
        }
    }

}

