#include <iostream>
#include <bitset>
#include <vector>
#include <sstream>
#include "ApplicationUtilities.h"
#include "GlobalDefinitions.h"
#include "MessageLogger.h"
#include "SegmentDisplay.h"

#include <getopt.h>
#include <fstream>
#include <forward_list>

using namespace ApplicationUtilities;
using namespace TMessageLogger;

static const uint8_t constexpr CONTRAST_CONTROL{66};
static const uint8_t constexpr REGISTER_SELECT{67};
static const uint8_t constexpr READ_WRITE_SELECT{69};
static const uint8_t constexpr DATA_0{68};
static const uint8_t constexpr DATA_1{45};
static const uint8_t constexpr DATA_2{44};
static const uint8_t constexpr DATA_3{26};
static const uint8_t constexpr DATA_4{47};
static const uint8_t constexpr DATA_5{46};
static const uint8_t constexpr DATA_6{27};
static const uint8_t constexpr DATA_7{65};

static struct option long_options[]
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

    while ( (currentOption = getopt_long(argc, argv, "hv", long_options, &optionIndex)) != -1) {
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
                LOG_WARN() << TStringFormat(R"(Unknown option "{0}", skipping)", long_options[optionIndex].name);
        }
    }
    displayVersion();
    LOG_INFO() << TStringFormat("Using LogFile {0}", ApplicationUtilities::getLogFilePath());

    segmentDisplay = std::make_shared<SegmentDisplay> (
            CONTRAST_CONTROL,
            REGISTER_SELECT,
            READ_WRITE_SELECT,
            std::array<uint8_t, 8> {
                    DATA_0,
                    DATA_1,
                    DATA_2,
                    DATA_3,
                    DATA_4,
                    DATA_5,
                    DATA_6,
                    DATA_7
            }
    );

}
