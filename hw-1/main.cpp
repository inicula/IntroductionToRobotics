/*
 *  Main source file for homework #1
 */

#include <Arduino.h>
#include "utils.h"

struct Led {
public:
        static constexpr tiny::pair<unsigned, unsigned> INPUT_RANGE = {0, 1023};
        static constexpr tiny::pair<unsigned, unsigned> OUTPUT_RANGE = {0, 255};

        Led(const int input_pin, const int output_pin);

        void update();

public:
        int input_pin;
        int output_pin;
};

Led::Led(const int input_pin, const int output_pin)
    : input_pin(input_pin)
    , output_pin(output_pin)
{
        pinMode(input_pin, INPUT);
        pinMode(output_pin, OUTPUT);
}

void
Led::update()
{
        auto input_value = analogRead(input_pin);
        auto output_value = map(input_value,
                                INPUT_RANGE.first,
                                INPUT_RANGE.second,
                                OUTPUT_RANGE.first,
                                OUTPUT_RANGE.second);

        Serial.println(output_value);
        analogWrite(output_pin, output_value);
}

/* clang-format off */
static const tiny::array<Led, 3> LEDS = {{
     {A0,  9},
     {A1, 10},
     {A2, 11},
}};
/* clang-format on */

void
setup()
{
        static constexpr unsigned BAUD = 9600;
        Serial.begin(BAUD);
}

void
loop()
{
        tiny::for_each(LEDS, [](auto& led) { led.update(); });
}

int
main()
{
        init();
        setup();
        for (;;)
                loop();
}
