#include <Arduino.h>
#include <limits.h>

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

/* Constants */
static constexpr unsigned long GREEN_LIGHT_BLINK_INTERVAL = 100;
static constexpr uint8_t BUTTON_PIN = 2;
static constexpr uint8_t BUZZER_PIN = 3; /* PWM pin */
static constexpr unsigned NOTE_FS5 = 784;

static constexpr uint8_t LED_OUTPUT_PINS[NumLeds] = {
    [Led::PedRed] = 4,
    [Led::PedGreen] = 5,
    [Led::CarRed] = 6,
    [Led::CarYellow] = 7,
    [Led::CarGreen] = 8,
};

static constexpr unsigned long DURATIONS[NumCrossStates] = {
    [CrossState::PedRedLight] = ULONG_MAX,
    [CrossState::PedRedLightEnding] = 10000,
    [CrossState::CarYellowLight] = 3000,
    [CrossState::PedGreenLight] = 10000,
    [CrossState::PedGreenLightEnding] = 5000,
};

static constexpr uint8_t LED_STATES[NumCrossStates] = {
    /*
     *  The LED values for a specific crosswalk state. The bits (from right to left) reference
     *  the LEDs in the order that they appear in `enum Led`.
     */
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
#define BUTTON_PRESSED() (digitalRead(BUTTON_PIN) == LOW)

/* Functions */
static void updateLeds(const uint8_t ledStates)
{
    static constexpr uint8_t mask = 1;
    for (uint8_t i = 0; i < NumLeds; ++i) {
        const uint8_t value = ledStates & (mask << i);
        digitalWrite(LED_OUTPUT_PINS[i], bool(value));
    }
}

void setup()
{
    /* Init crosswalk state */
    currentCrossState = CrossState::PedRedLight;
    prevTs = millis();

    /* Init output LED pins */
    for (auto pin : LED_OUTPUT_PINS)
        pinMode(pin, OUTPUT);

    /* Init button pin */
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    /* Init buzzer pin */
    pinMode(BUZZER_PIN, OUTPUT);
}

void loop()
{
    const uint8_t oldSemState = currentCrossState;
    const auto currentTs = millis();

    bool odd_interval;

    /* Handle the current state */
    switch (currentCrossState) {
    case CrossState::PedRedLight: /* Fallthrough */
        prevTs = currentTs;
        currentCrossState
            = BUTTON_PRESSED() ? CrossState::PedRedLightEnding : currentCrossState;
    case CrossState::PedRedLightEnding: /* Fallthrough */
    case CrossState::CarYellowLight: /* Fallthrough */
    case CrossState::PedGreenLight:
        /* No further action necessary */
        break;
    case CrossState::PedGreenLightEnding: /* Fallthrough */
        odd_interval = (currentTs / GREEN_LIGHT_BLINK_INTERVAL) % 2;

        if (odd_interval)
            tone(BUZZER_PIN, NOTE_FS5);
        else
            noTone(BUZZER_PIN);

        digitalWrite(LED_OUTPUT_PINS[Led::PedGreen], odd_interval);
    default:
        break;
    }

    if (currentTs - prevTs > DURATIONS[currentCrossState]) {
        prevTs = currentTs;
        currentCrossState = (currentCrossState + 1) % NumCrossStates;
    }

    if (currentCrossState != oldSemState) {
        /* Handle state transition */
        updateLeds(LED_STATES[currentCrossState]);

        switch (currentCrossState) {
        case CrossState::PedRedLight: /* Fallthrough */
        case CrossState::PedGreenLightEnding:
            noTone(BUZZER_PIN);
            break;
        case CrossState::PedGreenLight: /* Fallthrough */
            tone(BUZZER_PIN, NOTE_FS5);
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
