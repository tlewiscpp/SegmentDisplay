#ifndef SEGMENTDISPLAY_SEGMENTDISPLAYCHARACTER_H
#define SEGMENTDISPLAY_SEGMENTDISPLAYCHARACTER_H

class SegmentDisplay;
#include <stdint.h>

class SegmentDisplayCharacter 
{
public:
    SegmentDisplayCharacter(uint8_t m_column, uint8_t m_row, char c);
    SegmentDisplayCharacter();

    SegmentDisplayCharacter &operator=(char c);
    operator char();
private:
    uint8_t m_column;
    uint8_t m_row;
        char m_character;

    static SegmentDisplay *segmentDisplay;
    friend class SegmentDisplay;

};

#endif //SEGMENTDISPLAY_SEGMENTDISPLAYCHARACTER_H
