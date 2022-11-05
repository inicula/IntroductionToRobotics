#include "DisplayController.h"
#include <Arduino.h>

constexpr DisplayController::SegNeighbours DisplayController::SEGMENTS_NEIGHBOURS[NumSegments];
constexpr u8 DisplayController::SEGMENT_PINS[NumSegments];

void DisplayController::update(const u32 currentTs, JoystickController& joystickController)
{
    const auto joystickDir = joystickController.getDirection();
    const auto joyPressType = joystickController.getButtonValue(currentTs);
    const bool joyPressed = joyPressType != JoystickController::Press::None;

    const Bitfield16 segmentMask = 1 << currentSegment;
    switch (currentState) {
    case State::Disengaged: {
        /* Handle directional input */
        currentSegment = SEGMENTS_NEIGHBOURS[currentSegment][u8(joystickDir)];

        /* Handle button input */
        if (joyPressType == JoystickController::Press::Short)
            currentState = State::Engaged;
        else if (joyPressType == JoystickController::Press::Long) {
            segmentStates = ALL_SEGMENTS_OFF;
            currentSegment = Segment::DP;
        }

        /* Calculate the current segment's appropriate blink phase */
        const bool oddInterval = (currentTs / SELECTED_BLINK_INTERVAL) % 2;

        /* Odd interval -> treat segment as ON. Even interval -> treat segment as OFF */
        const Bitfield16 intervalSegmentStates
            = (segmentStates & ~segmentMask) | (oddInterval << currentSegment);

        drawSegments(intervalSegmentStates);
        break;
    }
    case State::Engaged:
        if (joystickDir == JoystickController::Direction::Left)
            segmentStates ^= segmentMask; /* Toggle the current segment */

        if (joyPressed)
            currentState = State::Disengaged;

        drawSegments(segmentStates);
        break;
    default:
        UNREACHABLE;
    }
}

void DisplayController::drawSegments(const Bitfield16 segmentStates)
{
    for (u32 i = 0; i < NumSegments; ++i) {
        const Bitfield16 mask = 1 << i;
        digitalWrite(SEGMENT_PINS[i], bool(segmentStates & mask));
    }
}

void DisplayController::init() const
{
    for (auto pin : SEGMENT_PINS)
        pinMode(pin, OUTPUT);
}
