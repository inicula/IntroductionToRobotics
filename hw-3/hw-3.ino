#include "DisplayController.h"
#include "utils.h"
#include <Arduino.h>
#include <limits.h>

/* Compile-time constants */
static constexpr u8 BUTTON_PIN = 2;

/* Global variables */
static JoystickController joystickController(BUTTON_PIN, A1, A0);
static DisplayController displayController;

/* Functions */
void setup()
{
    displayController.init();
    joystickController.init();
}

void loop() { displayController.update(millis(), joystickController); }

int main()
{
    init();
    setup();
    for (;;)
        loop();
}
