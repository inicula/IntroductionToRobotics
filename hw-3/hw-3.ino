#include "utils.h"
#include <Arduino.h>
#include <limits.h>

/* Macros */
#define UNREACHABLE __builtin_unreachable()

/* Enums */
enum Segment : uint8_t {
    A = 0,
    B,
    C,
    D,
    E,
    F,
    G,
    Dot,
    NumSegments,
};

enum JoystickDirection : uint8_t {
    None = 0,
    Up,
    Down,
    Left,
    Right,
    NumDirections,
};

/* Using declarations */
using SegmentNeighbours = Segment[JoystickDirection::NumDirections];
using Bitfield = uint16_t;

/* Classes and member functions */
class JoystickController {
public:
    enum PressType : uint8_t {
        None = 0,
        Short,
        Long,
    };

    enum DirectionState : uint8_t {
        Ok = 0,
        NeedsReset,
    };

    JoystickController(uint8_t, uint8_t, uint8_t);

    void init();
    uint8_t getButtonValue(unsigned long);
    uint8_t getDirectionValue();

private:
    bool updateButton(unsigned long);

private:
    struct {
        uint8_t pin;
        uint8_t previousValue;
        unsigned long previousTs;
        unsigned long pressDur;
    } button;
    struct {
        uint8_t pin; /* Analog input */
    } xAxis, yAxis;
    uint8_t directionState;
};

class DisplayController {
public:
    enum class State {
        Disengaged = 0,
        Engaged,
    };

    DisplayController() = default;

    void update(unsigned long, JoystickController&);

    static constexpr unsigned long SELECTED_BLINK_INTERVAL = 350;
    static constexpr Bitfield ALL_SEGMENTS_OFF = 0;

private:
    static void drawSegments(Bitfield);

private:
    Segment currentSegment;
    State currentState;
    Bitfield segmentStates;
};

/* Compile-time constants */
static constexpr uint8_t BUTTON_PIN = 2;

static constexpr uint8_t SEGMENT_PINS[NumSegments] = {
    [Segment::A] = 4,
    [Segment::B] = 5,
    [Segment::C] = 6,
    [Segment::D] = 7,
    [Segment::E] = 8,
    [Segment::F] = 9,
    [Segment::G] = 10,
    [Segment::Dot] = 11,
};

static constexpr SegmentNeighbours SEGMENTS_NEIGHBOURS[NumSegments] = {
/*  Source segment                       Neighbour through move type                       */
/*                     None          Up            Down          Left        Right         */
    [Segment::A]   = { Segment::A,   Segment::A,   Segment::G,   Segment::F, Segment::B   },
    [Segment::B]   = { Segment::B,   Segment::A,   Segment::G,   Segment::F, Segment::B   },
    [Segment::C]   = { Segment::C,   Segment::G,   Segment::D,   Segment::E, Segment::Dot },
    [Segment::D]   = { Segment::D,   Segment::G,   Segment::D,   Segment::E, Segment::C   },
    [Segment::E]   = { Segment::E,   Segment::G,   Segment::D,   Segment::E, Segment::C   },
    [Segment::F]   = { Segment::F,   Segment::A,   Segment::G,   Segment::F, Segment::B   },
    [Segment::G]   = { Segment::G,   Segment::A,   Segment::D,   Segment::G, Segment::G   },
    [Segment::Dot] = { Segment::Dot, Segment::Dot, Segment::Dot, Segment::C, Segment::Dot },
};

/* Constructors and member functions */
JoystickController::JoystickController(
    const uint8_t buttonPin, const uint8_t xPin, const uint8_t yPin)
{
    button.pin = buttonPin;
    xAxis.pin = xPin;
    yAxis.pin = yPin;
}

void JoystickController::init()
{
    pinMode(button.pin, INPUT_PULLUP);
    button.previousValue = HIGH;
    button.previousTs = millis();
}

bool JoystickController::updateButton(const unsigned long currentTs)
{
    const auto currentValue = digitalRead(button.pin);
    if (currentValue != button.previousValue) {
        button.previousValue = currentValue;
        button.pressDur = currentTs - button.previousTs;
        button.previousTs = currentTs;

        return true;
    }

    return false;
}

uint8_t JoystickController::getButtonValue(const unsigned long currentTs)
{
    /* Button thresholds */
    static constexpr unsigned long SHORT_PRESS_DUR = 100;
    static constexpr unsigned long LONG_PRESS_DURATION = 2000;

    const bool changed = updateButton(currentTs);
    if (!changed || !button.previousValue || button.pressDur < SHORT_PRESS_DUR)
        return PressType::None;
    return PressType::Short + (button.pressDur > LONG_PRESS_DURATION);
}

uint8_t JoystickController::getDirectionValue()
{
    /* Axis thresholds */
    static constexpr Tiny::Pair<unsigned, unsigned> INPUT_RANGE = { 0, 1023 };
    static constexpr unsigned INPUT_MIDDLE = INPUT_RANGE.second / 2;
    static constexpr unsigned AXIS_DELTA_THRESHOLD = 400;
    static constexpr unsigned AXIS_MIN_THRESHOLD = INPUT_MIDDLE - AXIS_DELTA_THRESHOLD;
    static constexpr unsigned AXIS_MAX_THRESHOLD = INPUT_MIDDLE + AXIS_DELTA_THRESHOLD;
    static constexpr unsigned RESET_THRESHOLD = 30;
    static constexpr Tiny::Pair<unsigned, unsigned> RESET_RANGE
        = { INPUT_MIDDLE - RESET_THRESHOLD, INPUT_MIDDLE + RESET_THRESHOLD };

    const unsigned xVal = analogRead(xAxis.pin);
    const unsigned yVal = analogRead(yAxis.pin);

    switch (directionState) {
    case Ok: {
        const uint8_t xDir = xVal < AXIS_MIN_THRESHOLD
            ? JoystickDirection::Left
            : (xVal > AXIS_MAX_THRESHOLD ? JoystickDirection::Right : JoystickDirection::None);

        const uint8_t yDir = yVal < AXIS_MIN_THRESHOLD
            ? JoystickDirection::Down
            : (yVal > AXIS_MAX_THRESHOLD ? JoystickDirection::Up : JoystickDirection::None);

        if (xDir || yDir)
            directionState = DirectionState::NeedsReset;

        if (xDir && yDir) {
            /* Both values are past the threshold, so choose the one closer to the limits */
            const auto xRemaining = min(xVal - INPUT_RANGE.first, INPUT_RANGE.second - xVal);
            const auto yRemaining = min(yVal - INPUT_RANGE.first, INPUT_RANGE.second - yVal);

            return xRemaining <= yRemaining ? xDir : yDir;
        }

        return xDir + yDir;
    }
    case NeedsReset:
        directionState = !(
            xVal == Tiny::clamp(xVal, RESET_RANGE) && yVal == Tiny::clamp(yVal, RESET_RANGE));
        return JoystickDirection::None;
    default:
        UNREACHABLE;
    }
}

void DisplayController::update(const unsigned long now, JoystickController& joystickController)
{
    const auto joystickDir = joystickController.getDirectionValue();
    const auto joyPressType = joystickController.getButtonValue(now);
    const bool joyPressed = joyPressType != JoystickController::PressType::None;

    const Bitfield segmentMask = 1 << currentSegment;
    switch (currentState) {
    case State::Disengaged: {
        /* Handle directional input */
        currentSegment = SEGMENTS_NEIGHBOURS[currentSegment][joystickDir];

        /* Handle button input */
        if (joyPressType == JoystickController::PressType::Short)
            currentState = State::Engaged;
        else if (joyPressType == JoystickController::PressType::Long) {
            segmentStates = ALL_SEGMENTS_OFF;
            currentSegment = Segment::Dot;
        }

        /* Set the current segment on the appropriate blink phase */
        const bool oddInterval = (now / SELECTED_BLINK_INTERVAL) % 2;
        const Bitfield intervalSegmentStates
            = (segmentStates & ~segmentMask) | (oddInterval << currentSegment);

        /* Draw the resulting segment states */
        drawSegments(intervalSegmentStates);
        break;
    }
    case State::Engaged:
        if (joystickDir == JoystickDirection::Left)
            segmentStates ^= segmentMask; /* Toggle the current segment */

        if (joyPressed)
            currentState = State::Disengaged;

        drawSegments(segmentStates);
        break;
    }
}

void DisplayController::drawSegments(const Bitfield segmentStates)
{
    static constexpr Bitfield MASK = 1;
    for (unsigned i = 0; i < NumSegments; ++i) {
        const bool segValue = segmentStates & (MASK << i);
        digitalWrite(SEGMENT_PINS[i], segValue);
    }
}

/* Global variables */
static JoystickController joystickController(BUTTON_PIN, A1, A0);
static DisplayController displayController;

/* Functions */
void setup()
{
    /* Init segment display */
    for (auto pin : SEGMENT_PINS)
        pinMode(pin, OUTPUT);

    /* Init button controller */
    joystickController.init();
}

void loop() { displayController.update(millis(), joystickController); }

int main()
{
    init();
    setup();
    for (;;)
        loop();
}
