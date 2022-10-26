/*
 *  Simple method for playing some notes on a buzzer.
 *  In this case: Little Fugue in G Minor (BWV 578).
 */

#include "notes.h"
#include <Arduino.h>

struct Note {
    unsigned int freq;
    unsigned long slice;
};

using Melody = const Note*;

struct MelodyController {
public:
    void play(const uint8_t buzzer_pin)
    {
        if (idx == N) {
            idx = 0;
            past = millis();
        }

        auto note = mel[idx].freq;
        if (note)
            tone(buzzer_pin, note);
        else
            noTone(buzzer_pin);

        const auto current = millis();
        if (current - past > mel[idx].slice * ms_per_unit) {
            past = current;
            ++idx;
        }
    }

public:
    Melody mel;
    unsigned N;
    unsigned long ms_per_unit;
    unsigned idx;
    unsigned long past;
};

template <typename T> static unsigned get_total_units(const T& notes)
{
    unsigned sum = 0;
    for (const auto& note : notes)
        sum += note.slice;

    return sum;
}

#define N 12
static constexpr Note NOTES[N] = {
    { NOTE_G5, 4 },
    { NOTE_D4, 4 },
    { NOTE_AS3, 5 },
    { 0, 1 },
    { NOTE_A3, 4 },
    { NOTE_G3, 4 },
    { NOTE_AS3, 4 },
    { NOTE_A3, 4 },
    { NOTE_G3, 4 },
    { NOTE_FS3, 4 },
    { NOTE_A3, 4 },
    { NOTE_D3, 4 },
};
static MelodyController mc = { &NOTES[0], N, 6000 / get_total_units(NOTES), N, 0 };

void setup() { pinMode(3, OUTPUT); }

void loop() { mc.play(3); }

int main()
{
    init();
    setup();
    for (;;)
        loop();
}
