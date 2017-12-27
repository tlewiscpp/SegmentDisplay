#ifndef SEGMENTDISPLAY_APPLICATIONUTILITIES_H
#define SEGMENTDISPLAY_APPLICATIONUTILITIES_H

#include <string>
#include <regex>
#include <vector>
#include <sstream>
#include <utility>
#include <tuple>
#include "MessageLogger.h"

namespace ApplicationUtilities
{
void globalLogHandler(TMessageLogger::LogLevel logLevel, TMessageLogger::LogContext logContext, const std::string &str);
void installSignalHandlers(void (*signalHandler)(int));
std::string getLogFilePath();
extern bool verboseLogging;
void displayVersion();
void displayHelp();

bool startsWith(const std::string &str, const std::string &start);
bool endsWith(const std::string &str, const std::string &ending);
void toLower(std::string &str);
void toUpper(std::string &str);
std::string currentTime();
std::string currentDate();

bool endsWith(const std::string &str, const std::string &ending);
bool endsWith(const std::string &str, char ending);

template <typename T> static inline std::string toStdString(T t) { return dynamic_cast<std::ostringstream &>(std::ostringstream{} << t).str(); }

template <char Delimiter>
std::vector<std::string> split(const std::string &str)
{
    std::string tempString{""};
    std::vector<std::string> returnVector{};
    std::stringstream istr{str};
    for(std::string s{""}; std::getline(istr, s, Delimiter); ( (s.length() > 0) ?  returnVector.push_back(s) : (void)returnVector.size() ));
    return returnVector;
}

std::vector<std::string> split(std::string str, const std::string &delimiter);
inline std::vector<std::string> split(const std::string &str, char delimiter) { return split(str, std::string(1, delimiter)); }

int createDirectory(const std::string &filePath, mode_t mode = 0777);
int deleteFile(const std::string &filePath);

};


#endif //SEGMENTDISPLAY_APPLICATIONUTILITIES_H
