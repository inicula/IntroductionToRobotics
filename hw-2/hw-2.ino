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
    PedRedLight = 0,
    PedRedLightEnding,
    CarYellowLight,
    PedGreenLight,
    PedGreenLightEnding,
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
    [SemState::PedRedLight] = (unsigned)-1,
    [SemState::PedRedLightEnding] = 10,
    [SemState::CarYellowLight] = 3,
    [SemState::PedGreenLight] = 10,
    [SemState::PedGreenLightEnding] = 5,
};

static constexpr unsigned LED_STATES[NumSemStates] = {
    [SemState::PedRedLight] = 0b10001,
    [SemState::PedRedLightEnding] = 0b10001,
    [SemState::CarYellowLight] = 0b01001,
    [SemState::PedGreenLight] = 0b00110,
    [SemState::PedGreenLightEnding] = 0b00110,
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
    currentSemState = SemState::PedRedLight;
    prevTs = millis();

    /* Init output LED pins */
    for (auto& lc : LED_CONTROLLERS)
        pinMode(lc.outputPin, OUTPUT);
}

void loop()
{
    const int oldSemState = currentSemState;
    const auto currentTs = millis();

    switch (currentSemState) {
    case SemState::PedRedLight: /* Fallthrough */
        if (true) {
            prevTs = currentTs;
            currentSemState = SemState::PedRedLightEnding;
        }
    case SemState::PedRedLightEnding: /* Fallthrough */
    case SemState::CarYellowLight:
        /* No further action necessary */
        break;
    case SemState::PedGreenLight:
        break;
    case SemState::PedGreenLightEnding:
        break;
    }

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
