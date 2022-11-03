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
    static constexpr unsigned long LONG_PRESS_DURATION = 1000;

private:
    bool update(const unsigned long);

private:
    uint8_t pin;
    uint8_t previousValue;
    unsigned long previousTs;
    unsigned long pressDur;
};

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
    if (!changed | !previousValue | pressDur < SHORT_PRESS_DUR)
        return PressType::None;
    return PressType::Short + (pressDur > LONG_PRESS_DURATION);
}

/* Constants */
static constexpr uint8_t BUTTON_PIN = 2;

static constexpr uint8_t SEGMENT_OUTPUT_PINS[NumSegments] = {
    [Segment::A] = 2,
    [Segment::B] = 3,
    [Segment::C] = 4,
    [Segment::D] = 5,
    [Segment::E] = 6,
    [Segment::F] = 7,
    [Segment::G] = 8,
    [Segment::Dot] = 9,
};

/* Global variables */
static ButtonController buttonController(BUTTON_PIN);

/* Functions */
void setup()
{
    /* Init segment display */
    for (auto pin : SEGMENT_OUTPUT_PINS)
        pinMode(pin, OUTPUT);

    /* Init button controller */
    buttonController.init();
}

void loop()
{
    const auto pressType = buttonController.getState(millis());
    const auto pressed = bool(pressType);
}

int main()
{
    init();
    setup();
    for (;;)
        loop();
}
