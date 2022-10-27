#include "common/utils.h"
#include <Arduino.h>

#define NUM_LEDS 3

struct LedController {
public:
    void init() const;
    void update() const;

public:
    uint8_t inputPin; /* Analog pin */
    uint8_t outputPin; /* PWM pin */
};

void LedController::init() const
{
    pinMode(inputPin, INPUT);
}

void LedController::update() const
{
    static constexpr Tiny::Pair<unsigned, unsigned> INPUT_RANGE = { 0, 1023 };
    static constexpr Tiny::Pair<unsigned, unsigned> OUTPUT_RANGE = { 0, 255 };

    auto inputValue = analogRead(inputPin);
    auto outputValue = map(inputValue, INPUT_RANGE.first, INPUT_RANGE.second,
        OUTPUT_RANGE.first, OUTPUT_RANGE.second);

    analogWrite(outputPin, outputValue);
}

static constexpr LedController LED_CONTROLLERS[NUM_LEDS] = {
    { A0, 9 },
    { A1, 10 },
    { A2, 11 },
};

void setup()
{
    for (auto& lc : LED_CONTROLLERS)
        lc.init();
}

void loop()
{
    for (auto& lc : LED_CONTROLLERS)
        lc.update();
}

int main()
{
    init();
    setup();
    for (;;)
        loop();
}
