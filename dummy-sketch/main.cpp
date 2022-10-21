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

static constexpr unsigned long INTERVAL = 100;
static auto previous_ts = millis();
static auto led_state = HIGH;

void
setup()
{
        pinMode(LED_BUILTIN, OUTPUT);
}

void
loop()
{
        if (auto current_ts = millis(); current_ts - previous_ts > INTERVAL) {
                previous_ts = current_ts;
                led_state = !led_state;
                digitalWrite(LED_BUILTIN, led_state);
        }
}

int
main()
{
        init();
        setup();
        for (;;)
                loop();
}
