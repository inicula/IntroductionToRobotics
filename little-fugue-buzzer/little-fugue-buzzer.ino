/*
 *  Simple method for playing note sequences on a buzzer.
 *  In this case: Little Fugue in G Minor (BWV 578).
 */

#include "notes.h"
#include <Arduino.h>

struct Note {
    unsigned int freq;
    unsigned long slice;
};

using Melody = const Note*;

template <typename T> static unsigned get_total_slices(const T& notes)
{
    unsigned sum = 0;
    for (const auto& note : notes)
        sum += note.slice;
    return sum;
}

class MelodyPlayer {
public:
    template <typename T>
    constexpr MelodyPlayer(const T& notes, unsigned total_duration)
        : mel(&notes[0])
        , num_notes(sizeof(notes) / sizeof(notes[0]))
        , ms_per_slice(total_duration / get_total_slices(notes))
        , i(num_notes)
        , past(0)
    {
    }

    void play(const uint8_t buzzer_pin)
    {
        const auto current_ts = millis();

        if (i == num_notes) {
            i = 0;
            past = current_ts;
        }

        const auto note = mel[i].freq;
        if (note)
            tone(buzzer_pin, note);
        else
            noTone(buzzer_pin);

        if (current_ts - past > mel[i].slice * ms_per_slice) {
            past = current_ts;
            ++i;
        }
    }

private:
    Melody mel;
    unsigned num_notes;
    unsigned long ms_per_slice;
    unsigned i;
    unsigned long past;
};

static constexpr Note NOTES[] = {
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
static MelodyPlayer mc(NOTES, 4000);

void setup() { pinMode(3, OUTPUT); }

void loop() { mc.play(3); }

int main()
{
    init();
    setup();
    for (;;)
        loop();
}
