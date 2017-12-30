#include "Utilities.h"

char splitBuffer[MAX_SPLIT_ENTRIES][MAX_SPLIT_ENTRY_LENGTH];

namespace Utilities
{
    
    int8_t charToInt(char target)
    {
        if (!isDigit(target)) {
            return -1;
        }
        return target - '0';
    }

    int32_t findFirstInString(const char *str, char toFind)
    {
        char *occurance{strchr(str, toFind)};
        if (occurance) {
            return occurance - str;
        } else {
            return -1;
        }
    }
    
    bool isEvenlyDivisibleBy(int numberToCheck, int divisor)
    {
        if ((numberToCheck == 0) || (divisor == 0)) {
            return false;
        }
        return ((numberToCheck % divisor) == 0);
    }

    bool isEven(int numberToCheck)
    {
        return isEvenlyDivisibleBy(numberToCheck, 2);
    }

    bool isOdd(int numberToCheck) 
    {
        return !isEven(numberToCheck);
    }

    bool isLineEnding(char c)
    {
        return ((c == '\r') || (c == '\n'));
    }


    bool startsWith(const char *str, const char *compare)
    {
        return (strncmp(str, compare, strlen(compare)) == 0);
    }

    bool startsWith(const char *str, char compare)
    {
        if (strlen(str) == 0) {
            return false;
        } else {
            return (str[0] == compare);
        }
        return false;
    }

    bool endsWith(const char *str, const char *compare)
    {
        if ((!str) || (!compare)) {
            return false;
        }
        size_t stringLength = strlen(str);
        size_t endingStringLength = strlen(compare);
        if (endingStringLength > stringLength) {
            return false;
        } else {
            return strncmp(str + stringLength - endingStringLength, compare, endingStringLength) == 0;
        }
    }

    bool endsWith(const char *str, char compare)
    {
        if (!str) {
            return false;
        }
        size_t endOfStringPosition{strlen(str)-1};
        return (*(str + endOfStringPosition) == compare);
    }

    bool isWhitespace(const char *stringToCheck)
    {
        if (!stringToCheck) {
            return false;
        }
        for (unsigned int i = 0; i < strlen(stringToCheck); i++) {
            char tempChar{*(stringToCheck + i)};
            if ((tempChar > ASCII_WHITESPACE_MAXIMUM_VALUE) || (tempChar == '\r') || (tempChar == '\n')) {
                return false;
            }
        }
        return true;
    }

    bool isWhitespace(char charToCheck)
    {
        return ((charToCheck < ASCII_WHITESPACE_MAXIMUM_VALUE) || (charToCheck == '\r') || (charToCheck == '\n')); 
    }

    int whitespace(char *out, size_t howMuch) 
    {
        if (!out) {
            return -1;
        }
        char temp[howMuch + 1]{' '};
        strncpy(out, temp, howMuch);
        return strlen(out);
    }

    char intToChar (int intToConvert)
    {
        switch(intToConvert) {
        case 0: return '0';
        case 1: return '1';
        case 2: return '2';
        case 3: return '3';
        case 4: return '4';
        case 5: return '5';
        case 6: return '6';
        case 7: return '7';
        case 8: return '8';
        case 9: return '9';
        default: return '?';
        }
    }

    bool isDigit(char charToCheck)
    {
        return ((charToCheck == '0') || (charToCheck == '1') || (charToCheck == '2') || (charToCheck == '3') || (charToCheck == '4') || (charToCheck == '5') || (charToCheck == '6') || (charToCheck == '7') || (charToCheck == '8') || (charToCheck == '9'));
    }

    int toFixedWidth(const char *inputString, char *out, size_t fixedWidth)
    {

        size_t ssize{strlen(inputString)};
        size_t bits{fixedWidth * 8};
        out = (char *) malloc(bits + 1);
        memset(out, '0', bits - ssize);
        strcpy(out + bits - ssize, inputString);
        return strlen(out);
    }

    int leftPad(const char *inputString, char *out, size_t width, char padChar)
    {
        if (strlen(inputString) >= width) {
            strncpy(out, inputString, strlen(inputString));
            out[strlen(inputString)] = '\0';
            return strlen(out);
        }
        for (unsigned int i = 0; i < width - strlen(inputString); i++) {
            out[i] = padChar;
        }
        out[width - strlen(inputString)] = '\0';
        strcat(out, inputString);
        return strlen(out);
    }

    uint32_t hexStringToUInt(const char *str)
    {
        if (!str) {
            return 0;
        }
        return (uint32_t)strtol(str, NULL, 16);
    }

    uint8_t hexStringToUChar(const char *str)
    {
        return (uint8_t)hexStringToUInt(str);
    }


    uint32_t decStringToUInt(const char *str)
    {
        if (!str) {
            return 0;
        }
        return (uint32_t)strtol(str, NULL, 10);
    }

    uint8_t decStringToUChar(const char *str)
    {
        return (uint8_t)decStringToUInt(str);
    }
    
    uint32_t stringToUInt(const char *str)
    {
        if (!str) {
            return 0;
        }
        return (uint32_t)strtol(str, NULL, 0);
    }

    uint8_t stringToUChar(const char *str)
    {
        return (uint8_t)strtol(str, NULL, 0);
    }

    int intExp(int base, int super)
    {
        if (super == 0) {
            return 1;
        }
        int total{base};
        for (int i = 1; i < super; i++) {
            total = total * base;
        }
        return total;
    }

    int tAbs(int lhs, int rhs)
    {
        return (lhs-rhs >= 0 ) ? (lhs-rhs) : -1*(lhs-rhs);
    }

    int tMax(int lhs, int rhs)
    {
        return (lhs >= rhs) ? lhs : rhs;
    }

    int tMin(int lhs, int rhs)
    {
        return (lhs <= rhs) ? lhs : rhs;
    }

    bool substringExists(const char *first, const char *second)
    {
        if ((!first) || (!second)) {
            return false;
        }
        return (strstr(first, second) != NULL);
    }

    bool substringExists(const char *first, char second)
    {
        return findFirstInString(first, second) != -1;
    }

    int positionOfSubstring(const char *first, const char *second)
    {
        if ((!first) || (!second)) {
            return -1;
        }
        const char *pos{strstr(first, second)};
        if (!pos) {
            return -1;
        }
        return (pos - first);
    }

    int positionOfSubstring(const char *first, char second)
    {
        return findFirstInString(first, second);
    }

    int substring(const char *str, size_t startPosition, char *out, size_t maximumLength)
    {
        if ((!str) || (!out)) {
            return -1;
        }
        size_t stringLength{strlen(str)};
        size_t numberToCopy{stringLength - startPosition};
        if (numberToCopy > maximumLength) {
            return -1;
        }
        memcpy(out, &(*(str + startPosition)), numberToCopy);
        *(out + numberToCopy) = '\0';
        return numberToCopy;
    }

    int substring(const char *str, size_t startPosition, size_t length, char *out, size_t maximumLength)
    {
        if ((!str) || (!out)) {
            return -1;
        }
        size_t stringLength{strlen(str)};
        (void)stringLength;
        size_t numberToCopy{length};
        if (numberToCopy > maximumLength) {
            return -1;
        }
        memcpy(out, &(*(str + startPosition)), numberToCopy);
        *(out + numberToCopy) = '\0';
        return numberToCopy;
    }

    int split(const char *str, char **out, const char *delimiter, size_t maximumElements, size_t maximumLength)
    {
        size_t outputCount{0};
        char *token = strtok(const_cast<char *>(str), delimiter);
        while ( (token != NULL) && (outputCount < maximumElements) ) {
            strncpy(out[outputCount++], token, maximumLength);
            token = strtok(NULL, delimiter);
        }
        return static_cast<int>(outputCount);
    }


    int split(const char *str, char **out, char delimiter, size_t maximumElements, size_t maximumLength)
    {
        const char delimiterString[2]{delimiter, '\0'};
        return split(str, out, delimiterString, maximumElements, maximumLength);
    }

    int arraySplit(const char *str, char out[MAX_SPLIT_ENTRIES][MAX_SPLIT_ENTRY_LENGTH], const char *delimiter)
    {
        size_t outputCount{0};
        char *token = strtok(const_cast<char *>(str), delimiter);
        while ( (token != NULL) && (outputCount < MAX_SPLIT_ENTRIES) ) {
            strncpy(out[outputCount++], token, MAX_SPLIT_ENTRY_LENGTH);
            token = strtok(NULL, delimiter);
        }
        return static_cast<int>(outputCount);
    }


    int arraySplit(const char *str, char out[MAX_SPLIT_ENTRIES][MAX_SPLIT_ENTRY_LENGTH], char delimiter)
    {
        const char delimiterString[2]{delimiter, '\0'};
        return arraySplit(str, out, delimiterString);
    }

    bool isValidByte(char byteToCheck)
    {
        return (isprint(byteToCheck) || (byteToCheck == '\r') || (byteToCheck == '\n'));
    }

    bool isAllZeroes(const char *str)
    {
        for (size_t i = 0; i < strlen(str); i++) {
            if (str[i] != '0') {
                return false;
            }
        }
        return true;
    }

    bool isAllZeroesOrDecimal(const char *str)
    {
        for (size_t i = 0; i < strlen(str); i++) {
            if ((str[i] != '0') && (str[i] != '.')) {
                return false;
            }
        }
        return true;
    }

    void genericSplitCast(const char *str, const char *delimiter)
    {
        (void)str;
        (void)delimiter;
        return;
    }

    void genericSplitCast(const char *str, const char delimiter)
    {
        (void)str;
        (void)delimiter;
        return;
    }

    size_t stripNonDigits(char *out)
    {
        char *temp{new char[strlen(out) + 1]};
        size_t stringIndex{0};
        for (size_t i = 0; i < strlen(out); i++) {
            if (isDigit(out[i])) {
                temp[stringIndex++] = out[i];
            }
        }
        temp[stringIndex] = '\0';
        strcpy(out, temp);
        delete temp;
        return stringIndex;
    }

    size_t stripNonFloatingPointDigits(char *out)
    {
        char *temp{new char[strlen(out) + 1]};
        size_t stringIndex{0};
        for (size_t i = 0; i < strlen(out); i++) {
            if ((isDigit(out[i])) || (out[i] == '.')) {
                temp[stringIndex++] = out[i];
            }
        }
        temp[stringIndex] = '\0';
        strcpy(out, temp);
        delete temp;
        return stringIndex;
    }
}

