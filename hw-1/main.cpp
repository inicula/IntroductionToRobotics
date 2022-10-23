/*
 *  Main source file for homework #1
 */

#include "utils.h"
#include <Arduino.h>

struct LedController {
public:
    static constexpr Tiny::Pair<unsigned, unsigned> INPUT_RANGE = { 0, 1023 };
    static constexpr Tiny::Pair<unsigned, unsigned> OUTPUT_RANGE = { 0, 255 };

    LedController(const int inputPin, const int outputPin);
    void update() const;

public:
    int inputPin;
    int outputPin;
};

LedController::LedController(const int inputPin, const int outputPin)
    : inputPin(inputPin)
    , outputPin(outputPin)
{
    pinMode(inputPin, INPUT);
    pinMode(outputPin, OUTPUT);
}

void LedController::update() const
{
    auto inputValue = analogRead(inputPin);
    auto outputValue = map(inputValue, INPUT_RANGE.first, INPUT_RANGE.second,
        OUTPUT_RANGE.first, OUTPUT_RANGE.second);

    Serial.println(outputValue);
    analogWrite(outputPin, outputValue);
}

static const LedController LED_CONTROLLERS[3] = {
    { A0, 9 },
    { A1, 10 },
    { A2, 11 },
};

void setup()
{
    static constexpr unsigned BAUD = 9600;
    Serial.begin(BAUD);
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
