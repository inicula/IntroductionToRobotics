#include "DisplayController.h"

constexpr u8 DisplayController::SECTION_PINS[NumSections];
constexpr u8 DisplayController::SECTION_STATES[NumSections];
constexpr u8 DisplayController::DIGIT_NODE_STATES[NUM_DIGITS];

using i8 = int8_t;

void DisplayController::init()
{
    for (auto pin : SECTION_PINS)
        pinMode(pin, OUTPUT);
}

void DisplayController::update(const u32 currentTs, JoystickController& joystickController)
{
    static constexpr u32 SELECTED_BLINK_INTERVAL = 256;

    const auto joystickDir = joystickController.getDirection();
    const auto joyPress = joystickController.getButtonValue(currentTs);

    switch (currentState) {
    case State::Disengaged: {
        /* Handle directional input */
        const i8 delta = joystickDir == JoystickController::Direction::Right
            ? 1
            : (joystickDir == JoystickController::Direction::Left ? -1 : 0);

        currentSection = u8(currentSection + delta);
        currentSection %= NumSections;

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
        const auto intervalNodeStates = Bitset8(
            DIGIT_NODE_STATES[sectionDigits[currentSection]] | (oddInterval << Node::DP));

        drawDigit(currentSection, intervalNodeStates);
        break;
    }
    case State::Engaged: {
        /* Handle directional input */
        const i8 delta = joystickDir == JoystickController::Direction::Right
            ? 1
            : (joystickDir == JoystickController::Direction::Left ? -1 : 0);

        sectionDigits[currentSection] = u8(sectionDigits[currentSection] + delta);
        currentSection %= NumSections;

        if (joyPress != JoystickController::Press::None)
            currentState = State::Disengaged;

        const auto nodeStates
            = Bitset8(DIGIT_NODE_STATES[sectionDigits[currentSection]] | (1 << Node::DP));

        drawDigit(currentSection, nodeStates);
        break;
    }
    default:
        UNREACHABLE;
    }
}

void DisplayController::drawDigit(const u8 section, const Bitset8 nodeStates)
{
    const Bitset8 sectionStates = SECTION_STATES[section];

    for (u32 i = 0; i < NumSections; ++i) {
        const auto sectionValue = u8((sectionStates >> i) & 1);
        digitalWrite(SECTION_PINS[i], sectionValue);
    }

    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, nodeStates);
    digitalWrite(LATCH_PIN, HIGH);
}
