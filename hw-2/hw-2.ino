#include <Arduino.h>

/* Enums */
enum Led {
    PedRed = 0,
    PedGreen,
    CarRed,
    CarYellow,
    CarGreen,
    NumLeds,
};

enum SemState {
    RedLight = 0,
    RedLightEnding,
    YellowLight,
    GreenLight,
    GreenLightEnding,
    NumSemStates,
};

/* Structs */
struct LedController {
    int outputPin;
};

/* Constants */
static constexpr LedController LED_CONTROLLERS[NumLeds] = {
    [Led::PedRed] = { 2 },
    [Led::PedGreen] = { 3 },
    [Led::CarRed] = { 4 },
    [Led::CarYellow] = { 5 },
    [Led::CarGreen] = { 6 },
};

static constexpr unsigned DURATIONS[NumSemStates] = {
    [SemState::RedLight] = (unsigned)-1,
    [SemState::RedLightEnding] = 10,
    [SemState::YellowLight] = 3,
    [SemState::GreenLight] = 10,
    [SemState::GreenLightEnding] = 5,
};

static constexpr unsigned LED_STATES[NumSemStates] = {
    [SemState::RedLight] = 0b10001,
    [SemState::RedLightEnding] = 0b10001,
    [SemState::YellowLight] = 0b01001,
    [SemState::GreenLight] = 0b00110,
    [SemState::GreenLightEnding] = 0b00110,
};

static constexpr unsigned long GREEN_LIGHT_BLINK_INTERVAL = 100;

/* Semaphore state */
static int currentSemState;
static unsigned long prevTs;

/* Functions */
static void updateLeds(const unsigned ledStates)
{
    static constexpr unsigned mask = 1;
    for (unsigned i = 0; i < NumLeds; ++i) {
        const unsigned value = ledStates & (mask << i);
        digitalWrite(LED_CONTROLLERS[i].outputPin, bool(value));
    }
}

void setup()
{
    static constexpr unsigned BAUD = 9600;
    Serial.begin(BAUD);

    /* Init semaphore state */
    currentSemState = SemState::RedLight;
    prevTs = millis();

    /* Init output LED pins */
    for (auto& lc : LED_CONTROLLERS)
        pinMode(lc.outputPin, OUTPUT);
}

void loop()
{
    const int oldSemState = currentSemState;

    switch (currentSemState) {
    case SemState::RedLight: /* Fallthrough */
        if (true) {
            prevTs = millis();
            currentSemState = SemState::RedLightEnding;
        }
    case SemState::RedLightEnding: /* Fallthrough */
    case SemState::YellowLight:
        /* No further action necessary */
        break;
    case SemState::GreenLight:
        break;
    case SemState::GreenLightEnding:
        break;
    }

    const auto currentTs = millis();
    if (currentTs - prevTs > DURATIONS[currentSemState]) {
        prevTs = currentTs;
        currentSemState = (currentSemState + 1) % NumSemStates;
    }

    if (currentSemState != oldSemState)
        updateLeds(LED_STATES[currentSemState]);
}

int main()
{
    init();
    setup();
    for (;;)
        loop();
}
