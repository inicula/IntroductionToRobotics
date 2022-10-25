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

enum CrossState {
    PedRedLight = 0,
    PedRedLightEnding,
    CarYellowLight,
    PedGreenLight,
    PedGreenLightEnding,
    NumCrossStates,
};

/* Structs */
struct LedController {
    uint8_t outputPin;
};

/* Constants */
static constexpr unsigned long GREEN_LIGHT_BLINK_INTERVAL = 100;
static constexpr uint8_t BUTTON_PIN = 2;
static constexpr uint8_t BUZZER_PIN = 3; /* PWM pin */
static constexpr unsigned NOTE_FS5 = 784;

static constexpr LedController LED_CONTROLLERS[NumLeds] = {
    [Led::PedRed] = { 4 },
    [Led::PedGreen] = { 5 },
    [Led::CarRed] = { 6 },
    [Led::CarYellow] = { 7 },
    [Led::CarGreen] = { 8 },
};

static constexpr unsigned DURATIONS[NumCrossStates] = {
    [CrossState::PedRedLight] = (unsigned)-1,
    [CrossState::PedRedLightEnding] = 10,
    [CrossState::CarYellowLight] = 3,
    [CrossState::PedGreenLight] = 10,
    [CrossState::PedGreenLightEnding] = 5,
};

static constexpr uint8_t LED_STATES[NumCrossStates] = {
    [CrossState::PedRedLight] = 0b10001,
    [CrossState::PedRedLightEnding] = 0b10001,
    [CrossState::CarYellowLight] = 0b01001,
    [CrossState::PedGreenLight] = 0b00110,
    [CrossState::PedGreenLightEnding] = 0b00110,
};

/* Crosswalk state */
static uint8_t currentCrossState;
static unsigned long prevTs;

/* Macros */
#define BUTTON_PRESSED (digitalRead(BUTTON_PIN) == HIGH)

/* Functions */
static void updateLeds(const uint8_t ledStates)
{
    static constexpr uint8_t mask = 1;
    for (uint8_t i = 0; i < NumLeds; ++i) {
        const uint8_t value = ledStates & (mask << i);
        digitalWrite(LED_CONTROLLERS[i].outputPin, bool(value));
    }
}

void setup()
{
    /* Init semaphore state */
    currentCrossState = CrossState::PedRedLight;
    prevTs = millis();

    /* Init output LED pins */
    for (auto& lc : LED_CONTROLLERS)
        pinMode(lc.outputPin, OUTPUT);

    /* Init button pin */
    pinMode(BUTTON_PIN, INPUT);

    /* Init buzzer pin */
    pinMode(BUZZER_PIN, OUTPUT);
}

void loop()
{
    const uint8_t oldSemState = currentCrossState;
    const auto currentTs = millis();

    /* Handle the current state */
    switch (currentCrossState) {
    case CrossState::PedRedLight: /* Fallthrough */
        if (BUTTON_PRESSED) {
            prevTs = currentTs;
            currentCrossState = CrossState::PedRedLightEnding;
        }
    case CrossState::PedRedLightEnding: /* Fallthrough */
    case CrossState::CarYellowLight: /* Fallthrough */
    case CrossState::PedGreenLight:
        /* No further action necessary */
        break;
    case CrossState::PedGreenLightEnding:
        if ((currentTs / GREEN_LIGHT_BLINK_INTERVAL) % 2) {
            tone(BUZZER_PIN, NOTE_FS5);
            digitalWrite(LED_CONTROLLERS[Led::PedGreen].outputPin, HIGH);
        } else {
            noTone(BUZZER_PIN);
            digitalWrite(LED_CONTROLLERS[Led::PedGreen].outputPin, LOW);
        }
        break;
    default:
        __builtin_unreachable();
    }

    if (currentTs - prevTs > DURATIONS[currentCrossState]) {
        prevTs = currentTs;
        currentCrossState = (currentCrossState + 1) % NumCrossStates;
    }

    if (currentCrossState != oldSemState) {
        /* Handle state transition */
        updateLeds(LED_STATES[currentCrossState]);

        switch (currentCrossState) {
        case CrossState::PedGreenLight:
            tone(BUZZER_PIN, NOTE_FS5);
            break;
        case CrossState::PedGreenLightEnding:
            noTone(BUZZER_PIN);
        default:
            break;
        }
    }
}

int main()
{
    init();
    setup();
    for (;;)
        loop();
}
