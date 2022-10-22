/*
 *  Compile: `$ make`
 *  Upload:  `$ make upload`
 *  Monitor: `$ make monitor`. In order to kill the monitor press: CTRL-A k (furthermore, see
 * `man 1 screen` for other commands).
 *
 *  The purpose of the program is to show the things needed to compile/upload/monitor/etc. code
 *  without using the Arduino IDE (and without the Arduino boilerplate).
 *
 *  The original makefile (from `arduino-mk`) is modified such that it uses the Arduino Core
 *  provided by the Arduino IDE (newer `avr-g++` version compared to the Debian Bullseye
 *  package).
 *
 *  Expected program behavior: alternate between LED_BUILTIN being OFF/ON with a 100ms
 *  interval.
 */

#include <Arduino.h>
#include "utils.h"

static constexpr unsigned BAUD = 9600;

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
