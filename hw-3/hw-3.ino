#include <Arduino.h>
#include <limits.h>

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
    Up = 0,
    Down,
    Left,
    Right,
    NumDirections,
};

/* Using declarations */
using SegmentNeighbours = uint8_t[JoystickDirection::NumDirections];

/* Classes and member functions */
class ButtonController {
public:
    enum PressType : uint8_t {
        None = 0,
        Short,
        Long,
    };

    ButtonController(const uint8_t pin);

    void init();
    uint8_t getState(const unsigned long);

    static constexpr unsigned long SHORT_PRESS_DUR = 100;
    static constexpr unsigned long LONG_PRESS_DURATION = 2000;

private:
    bool update(const unsigned long);

private:
    uint8_t pin;
    uint8_t previousValue;
    unsigned long previousTs;
    unsigned long pressDur;
};

struct DisplayController {
public:
    enum class State {
        Disengaged,
        Engaged,
    };

    DisplayController();

    void update(const unsigned long);

    static constexpr unsigned long SELECTED_BLINK_INTERVAL = 350;

public:
    Segment currentSegment;
    State currentState;
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
    [Segment::A] = { Segment::A, Segment::G, Segment::F, Segment::B },
    [Segment::B] = { Segment::A, Segment::G, Segment::F, Segment::B },
    [Segment::C] = { Segment::G, Segment::D, Segment::E, Segment::Dot },
    [Segment::D] = { Segment::G, Segment::D, Segment::E, Segment::C },
    [Segment::E] = { Segment::G, Segment::D, Segment::E, Segment::C },
    [Segment::F] = { Segment::A, Segment::G, Segment::F, Segment::B },
    [Segment::G] = { Segment::A, Segment::D, Segment::G, Segment::G },
    [Segment::Dot] = { Segment::Dot, Segment::Dot, Segment::C, Segment::Dot },
};

/* Constructors and member functions */
ButtonController::ButtonController(const uint8_t pin)
    : pin(pin)
{
}

void ButtonController::init()
{
    pinMode(pin, INPUT_PULLUP);
    previousValue = HIGH;
    previousTs = millis();
    pressDur = 0;
}

bool ButtonController::update(const unsigned long currentTs)
{
    const auto currentValue = digitalRead(pin);
    if (currentValue != previousValue) {
        previousValue = currentValue;
        pressDur = currentTs - previousTs;
        previousTs = currentTs;

        return true;
    }

    return false;
}

uint8_t ButtonController::getState(const unsigned long currentTs)
{
    const bool changed = update(currentTs);
    if (!changed || !previousValue || pressDur < SHORT_PRESS_DUR)
        return PressType::None;
    return PressType::Short + (pressDur > LONG_PRESS_DURATION);
}

DisplayController::DisplayController()
    : currentSegment(Segment::A)
    , currentState(State::Disengaged)
{
}

void DisplayController::update(const unsigned long now)
{
    const bool oddInterval = (now / SELECTED_BLINK_INTERVAL) % 2;
    digitalWrite(SEGMENT_PINS[currentSegment], oddInterval);
}

/* Global variables */
static ButtonController buttonController(BUTTON_PIN);
static DisplayController displayController;

/* Functions */
void setup()
{
    /* Init segment display */
    for (auto pin : SEGMENT_PINS)
        pinMode(pin, OUTPUT);

    /* Init button controller */
    buttonController.init();
}

void loop()
{
    const auto now = millis();
    displayController.update(now);
}

int main()
{
    init();
    setup();
    for (;;)
        loop();
}
