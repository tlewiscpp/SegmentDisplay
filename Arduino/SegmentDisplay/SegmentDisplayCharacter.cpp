#include "SegmentDisplayCharacter.h"
#include "SegmentDisplay.h"

SegmentDisplay *SegmentDisplayCharacter::segmentDisplay{nullptr};

SegmentDisplayCharacter::SegmentDisplayCharacter(uint8_t column, uint8_t row, char c) :
    m_column{column},
    m_row{row},
    m_character{c}
{
    
}

SegmentDisplayCharacter::SegmentDisplayCharacter() :
  SegmentDisplayCharacter{0, 0, 0}
{
  
}

SegmentDisplayCharacter &SegmentDisplayCharacter::operator=(char c) {
    this->m_character = c;
    if (SegmentDisplayCharacter::segmentDisplay) {
        segmentDisplay->characterAssigned(this->m_column, this->m_row, this->m_character);
    }
    return *this;
}

SegmentDisplayCharacter::operator char() {
    return this->m_character;
}

