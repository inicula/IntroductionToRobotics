#include "DisplayController.h"

using i8 = int8_t;

constexpr u8 DisplayController::SECTION_PINS[NumSections];
constexpr u8 DisplayController::DIGIT_NODE_STATES[NUM_DIGITS];

void DisplayController::init()
{
    pinMode(DATA_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);

    for (auto pin : SECTION_PINS)
        pinMode(pin, OUTPUT);
}

void DisplayController::update(const u32 currentTs, JoystickController& joystickController)
{
    static constexpr u32 SELECTED_BLINK_INTERVAL = 256;

    const auto joystickDir = joystickController.getDirection();
    const auto joyPress = joystickController.getButtonValue(currentTs);

    Bitset8 nodeStates;
    switch (currentState) {
    case State::Disengaged: {
        /* Handle directional input */
        const i8 delta = joystickDir == JoystickController::Direction::Right
            ? 1
            : (joystickDir == JoystickController::Direction::Left ? -1 : 0);

        currentSection = u8(currentSection + delta) % NumSections;

        /* Handle button input */
        if (joyPress == JoystickController::Press::Short)
            currentState = State::Engaged;
        else if (joyPress == JoystickController::Press::Long) {
            sectionDigits = {};
            currentSection = Section::D1;
        }

        /* Calculate DP's appropriate blink phase */
        const bool oddInterval = (currentTs / SELECTED_BLINK_INTERVAL) % 2;

        /* Odd interval -> treat DP as ON. Even interval -> treat DP as OFF */
        nodeStates = Bitset8(
            DIGIT_NODE_STATES[sectionDigits[currentSection]] | (oddInterval << Node::DP));

        break;
    }
    case State::Engaged: {
        /* Handle directional input */
        const i8 delta = joystickDir == JoystickController::Direction::Up
            ? 1
            : (joystickDir == JoystickController::Direction::Down ? -1 : 0);

        sectionDigits[currentSection] = u8(sectionDigits[currentSection] + delta) % NUM_DIGITS;

        /* Handle button input */
        if (joyPress != JoystickController::Press::None)
            currentState = State::Disengaged;

        nodeStates
            = Bitset8(DIGIT_NODE_STATES[sectionDigits[currentSection]] | (1 << Node::DP));

        break;
    }
    default:
        UNREACHABLE;
    }

    drawDigit(nodeStates);
}

void DisplayController::drawDigit(const Bitset8 nodeStates)
{
    static constexpr u8 MULTIPLEXING_DELAY_DUR = 5;

    for (u8 sectionIter = 0; sectionIter < NumSections; ++sectionIter) {
        digitalWrite(LATCH_PIN, LOW);
        shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST,
            sectionIter == currentSection ? nodeStates
                                          : DIGIT_NODE_STATES[sectionDigits[sectionIter]]);
        digitalWrite(LATCH_PIN, HIGH);

        for (u8 i = 0; i < NumSections; ++i)
            digitalWrite(SECTION_PINS[i], HIGH);
        digitalWrite(SECTION_PINS[sectionIter], LOW);

        delay(MULTIPLEXING_DELAY_DUR);
    }
}
