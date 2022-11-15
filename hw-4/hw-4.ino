#include "DisplayController.h"

/* Global variables */
static JoystickController joystickController;
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
