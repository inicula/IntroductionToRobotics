/*
 *  Simple method for playing note sequences on a buzzer.
 */

#include "common/music.h"

static MelodyPlayer mc(CONTRAPUNCTUS_1, 10000);

void setup() { pinMode(3, OUTPUT); }

void loop() { mc.play(3); }

int main()
{
    init();
    setup();
    for (;;)
        loop();
}
