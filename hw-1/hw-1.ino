/*
 *  Main source file for homework #1
 */

#include "common/utils.h"
#include <Arduino.h>

#define NUM_LEDS 3

struct LedController {
public:
    void initPins() const;
    void update() const;

public:
    int inputPin;
    int outputPin;
};

void LedController::initPins() const
{
    pinMode(inputPin, INPUT);
    pinMode(outputPin, OUTPUT);
}

void LedController::update() const
{
    static constexpr Tiny::Pair<int, int> INPUT_RANGE = { 0, 1023 };
    static constexpr Tiny::Pair<int, int> OUTPUT_RANGE = { 0, 255 };

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
    static constexpr int BAUD = 9600;
    Serial.begin(BAUD);

    for (auto& lc : LED_CONTROLLERS)
        lc.initPins();
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
