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
    PedRedLight = 0,     /* Red for pedestrians. Green for cars. Ends on button press.      */
    PedRedLightEnding,   /* Same as above but ends after 8 seconds.                         */
    CarYellowLight,      /* Red for pedestrians. Yellow for cars. Ends after 3 seconds.     */
    PedGreenLight,       /* Green for pedestrians. Frequent buzz. Ends after 8 seconds.     */
    PedGreenLightEnding, /* Blinking green light. Faster buzz rate. Ends after 4 seconds.   */
    NumCrossStates,      /* When a state `S` ends, state `(S + 1) % NumCrossStates` begins. */
};

/* Compile-time constants */
static constexpr unsigned long GREEN_LIGHT_BLINK_INTERVAL = 100;
static constexpr unsigned long GREEN_LIGHT_BEEP_INTERVAL = 500;
static constexpr uint8_t BUTTON_PIN = 2;
static constexpr uint8_t BUZZER_PIN = 3; /* PWM pin */
static constexpr unsigned NOTE_FS5 = 784;
static constexpr unsigned NOTE_FS4 = 370;

static constexpr uint8_t LED_OUTPUT_PINS[NumLeds] = {
    [Led::PedRed] = 4,
    [Led::PedGreen] = 5,
    [Led::CarRed] = 6,
    [Led::CarYellow] = 7,
    [Led::CarGreen] = 8,
};

static constexpr unsigned long DURATIONS[NumCrossStates] = {
    /* State durations in milliseconds */
    [CrossState::PedRedLight] = ULONG_MAX,
    [CrossState::PedRedLightEnding] = 8000,
    [CrossState::CarYellowLight] = 3000,
    [CrossState::PedGreenLight] = 8000,
    [CrossState::PedGreenLightEnding] = 4000,
};

static constexpr uint8_t LED_STATES[NumCrossStates] = {
    /*
     *  LED values for a specific crosswalk state. The bits (from right to left) reference
     *  the LEDs in the order that they appear in `enum Led` (i.e. the 1st bit is for the
     *  pedestrians' red light, the 2nd is for the pedestrians' green light and so on).
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

/* Functions */
static bool buttonIsPressed() { return digitalRead(BUTTON_PIN) == LOW; }

static void updateLeds(const uint8_t ledStates)
{
    static constexpr uint8_t MASK = 1;
    for (uint8_t i = 0; i < NumLeds; ++i) {
        const uint8_t value = ledStates & (MASK << i);
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

    /* Init LED states */
    updateLeds(LED_STATES[CrossState::PedRedLight]);
}

void loop()
{
    const uint8_t oldCrossState = currentCrossState;
    const auto currentTs = millis();

    bool oddInterval;

    /* Handle the current state */
    switch (currentCrossState) {
    case CrossState::PedRedLight: /* Fallthrough */
        prevTs = currentTs;
        currentCrossState
            = buttonIsPressed() ? CrossState::PedRedLightEnding : currentCrossState;
    case CrossState::PedRedLightEnding: /* Fallthrough */
    case CrossState::CarYellowLight:
        /* No further action necessary */
        break;
    case CrossState::PedGreenLight:
        oddInterval = (currentTs / GREEN_LIGHT_BEEP_INTERVAL) % 2;
        if (oddInterval)
            tone(BUZZER_PIN, NOTE_FS4);
        else
            noTone(BUZZER_PIN);

        break;
    case CrossState::PedGreenLightEnding: /* Fallthrough */
        oddInterval = (currentTs / GREEN_LIGHT_BLINK_INTERVAL) % 2;
        if (oddInterval)
            tone(BUZZER_PIN, NOTE_FS5);
        else
            noTone(BUZZER_PIN);

        digitalWrite(LED_OUTPUT_PINS[Led::PedGreen], oddInterval);
    default:
        break;
    }

    if (currentTs - prevTs > DURATIONS[currentCrossState]) {
        /* Go to the next state in the cycle */
        prevTs = currentTs;
        currentCrossState = (currentCrossState + 1) % NumCrossStates;
    }

    if (currentCrossState != oldCrossState) {
        /* Handle state transition */
        switch (currentCrossState) {
        case CrossState::PedRedLight: /* Fallthrough */
        case CrossState::PedGreenLightEnding: /* Fallthrough */
            noTone(BUZZER_PIN);
        default:
            updateLeds(LED_STATES[currentCrossState]);
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
