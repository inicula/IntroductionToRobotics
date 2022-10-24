#include <Arduino.h>

/* Enums */
enum {
    L_PED_RED = 0,
    L_PED_GREEN,
    L_CAR_RED,
    L_CAR_YELLOW,
    L_CAR_GREEN,
    NUM_LEDS,
};

enum {
    S_RED_LIGHT = 0,
    S_RED_LIGHT_ENDING,
    S_YELLOW_LIGHT,
    S_GREEN_LIGHT,
    S_GREEN_LIGHT_ENDING,
    NUM_SEM_STATES,
};

/* Structs */
struct Led {
    int outputPin;
};

/* Constants */
static constexpr Led LEDS[NUM_LEDS] = {
    { 2 },
    { 3 },
    { 4 },
    { 5 },
    { 6 },
};

static constexpr unsigned DURATIONS[NUM_SEM_STATES] = {
    [S_RED_LIGHT] = (unsigned)-1,
    [S_RED_LIGHT_ENDING] = 10,
    [S_YELLOW_LIGHT] = 3,
    [S_GREEN_LIGHT] = 10,
    [S_GREEN_LIGHT_ENDING] = 5,
};

static constexpr unsigned LED_STATES[NUM_SEM_STATES] = {
    [S_RED_LIGHT] = 0b10001,
    [S_RED_LIGHT_ENDING] = 0b10001,
    [S_YELLOW_LIGHT] = 0b01001,
    [S_GREEN_LIGHT] = 0b00110,
    [S_GREEN_LIGHT_ENDING] = 0b00110,
};

static constexpr unsigned long GREEN_LIGHT_BLINK_INTERVAL = 100;

/* Semaphore state */
static int currentSemState;
static unsigned long prevTs;

/* Functions */
static void updateLeds(const unsigned ledStates)
{
    static constexpr unsigned mask = 1;
    for (unsigned i = 0; i < NUM_LEDS; ++i) {
        const unsigned value = ledStates & (mask << i);
        digitalWrite(LEDS[i].outputPin, bool(value));
    }
}

void setup()
{
    static constexpr unsigned BAUD = 9600;
    Serial.begin(BAUD);

    /* Init semaphore state */
    currentSemState = S_RED_LIGHT;
    prevTs = millis();

    /* Init output LED pins */
    for (auto& led : LEDS)
        pinMode(led.outputPin, OUTPUT);
}

void loop()
{
    const int oldSemState = currentSemState;

    switch (currentSemState) {
    case S_RED_LIGHT: /* Fallthrough */
        if (true) {
            prevTs = millis();
            currentSemState = S_RED_LIGHT_ENDING;
        }
    case S_RED_LIGHT_ENDING: /* Fallthrough */
    case S_YELLOW_LIGHT:
        /* No further action necessary */
        break;
    case S_GREEN_LIGHT:
        break;
    case S_GREEN_LIGHT_ENDING:
        break;
    }

    const auto currentTs = millis();
    if (currentTs - prevTs > DURATIONS[currentSemState]) {
        prevTs = currentTs;
        currentSemState = (currentSemState + 1) % NUM_SEM_STATES;
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
