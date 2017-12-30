#ifndef ARDUINOLINCONTROLLER_UTILITIES_H
#define ARDUINOLINCONTROLLER_UTILITIES_H

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>
#include <Arduino.h>

#define SMALL_BUFFER_SIZE 255
#define FIXED_WIDTH_BUFFER_SIZE 25
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#define MAX_SPLIT_ENTRIES 12
#define MAX_SPLIT_ENTRY_LENGTH 5
extern char splitBuffer[MAX_SPLIT_ENTRIES][MAX_SPLIT_ENTRY_LENGTH];


#define CHECK_BIT(value,bit) ((value) & (1 << (bit)))
#define SET_BIT(value,bit) ((value) |= (1 << (bit)))
#define CLEAR_BIT(value,bit) ((value) &= ~(1 << (bit)))
#define TOGGLE_BIT(value,bit) ((value) ^= (1 << (bit)))

namespace Utilities
{
    template <typename T, size_t N> inline size_t arraySize(T(&)[N]) { return N; }
    const int constexpr ASCII_WHITESPACE_MAXIMUM_VALUE{32};
    bool isEvenlyDivisibleBy(int numberToCheck, int divisor);
    bool isEven(int numberToCheck);
    bool isOdd(int numberToCheck);
    int32_t findFirstInString(const char *str, char toFind);
    bool isLineEnding(char c);
    bool startsWith(const char *str, char compare);
    bool startsWith(const char *str, const char *compare);
    bool endsWith(const char *str, const char *compare);
    bool endsWith(const char *str, char compare);
    bool isWhitespace(const char *str);
    bool isWhitespace(char charToCheck);
    int whitespace(char *out, size_t howMuch);
    char intToChar (int intToConvert);
    bool isDigit(char charToCheck);
    int8_t charToInt(char target);
    int toFixedWidth(const char *input, char *out, size_t fixedWidth);
    int leftPad(const char *input, char *out, size_t fixedWidth, char padChar = '0');
   

    uint32_t hexStringToUInt(const char *str);
    uint8_t hexStringToUChar(const char *str);
    uint32_t decStringToUInt(const char *str);
    uint8_t decStringToUChar(const char *str);
    uint32_t stringToUInt(const char *str);
    uint8_t stringToUChar(const char *str);
    int intExp(int base, int super);
    int tAbs(int lhs, int rhs);
    int tMax(int lhs, int rhs);
    int tMin(int lhs, int rhs);
    bool substringExists(const char *first, const char *second);
    bool substringExists(const char *first, char second);
    int positionOfSubstring(const char *first, const char *second);
    int positionOfSubstring(const char *first, char second);
    int substring(const char *str, size_t startPosition, char *out, size_t maximumLength);
    int substring(const char *str, size_t startPosition, size_t length, char *out, size_t maximumLength);
    bool isValidByte(char byteToCheck);
    int split(const char *str, char **out, const char *delimiter, size_t maximumElements, size_t maximumLength);
    int split(const char *str, char **out, const char delimiter, size_t maximumElements, size_t maximumLength);
    
    int arraySplit(const char *str, char out[MAX_SPLIT_ENTRIES][MAX_SPLIT_ENTRY_LENGTH], const char *delimiter);
    int arraySplit(const char *str, char out[MAX_SPLIT_ENTRIES][MAX_SPLIT_ENTRY_LENGTH], const char delimiter);
    unsigned long tMillis();

    template <typename InputType>   
    void toFixedWidthHex(char *target, InputType value, uint8_t targetLength, bool includeZeroX = true)
    {
        static char formatting[FIXED_WIDTH_BUFFER_SIZE];
        static char widthString[FIXED_WIDTH_BUFFER_SIZE];
        memset(formatting, '\0', FIXED_WIDTH_BUFFER_SIZE);
        memset(widthString, '\0', FIXED_WIDTH_BUFFER_SIZE);
        if (includeZeroX) {
            strcpy(formatting, "0x%0");
        } else {
            strcpy(formatting, "%0");
        }
        sprintf(widthString, "%i", static_cast<int>(targetLength));
        strcat(formatting, widthString);
        strcat(formatting, "x");
        sprintf(target, formatting, static_cast<int>(value));
    }

    template <typename Ptr>
    void free2D(Ptr **out, size_t elements)
    {
        for (size_t i = 0; i < elements; i++) {
            free(out[i]);
        }
        free(out);
    }

    template <typename Ptr>
    Ptr **calloc2D(size_t elements, size_t maximumLength)
    {
        Ptr **out = (Ptr **)calloc(elements, sizeof(Ptr *) * maximumLength);
        for (size_t i = 0; i < elements; i++) {
            out[i] = (Ptr *)calloc(maximumLength, sizeof(Ptr));
        }
        return out;
    }

    template <typename T>
    int toDecString(T number, char *out, size_t maximumLength)
    {
        return (snprintf(out, maximumLength, "%i", static_cast<unsigned int>(number)));
    }

    template <typename T>
    int toHexString(T number, char *out, size_t maximumLength)
    {
        return (snprintf(out, maximumLength, "%02X", static_cast<unsigned int>(number))); 
    }

    bool isAllZeroes(const char *str);
    bool isAllZeroesOrDecimal(const char *str);
    size_t stripNonDigits(char *str); 
    size_t stripNonFloatingPointDigits(char *out);
    
    template <typename T> uint8_t safeParse(const char *str) {
        int base{10};
        auto stringLength = strlen(str);
        if (stringLength >= 2) {
            if (str[0] == '0') {
                if (str[1] == 'x') {
                    base = 16;
                } else if (str[1] == 'b') {
                    base = 2;
                } else if (str[1] == 'o') {
                    base = 8;
                }
            }
        }
        return static_cast<T>(strtol(str, nullptr, base));
    }
}

#endif //ARDUINOLINCONTROLLER_UTILITIES_H

