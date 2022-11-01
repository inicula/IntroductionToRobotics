#include <Arduino.h>

enum Segment {
    A = 0,
    B,
    C,
    D,
    E,
    F,
    G,
    Dot,
    NumLeds,
};

static constexpr uint8_t SEGMENT_OUTPUT_PINS[NumLeds] = {
    [Segment::A] = 2,
    [Segment::B] = 3,
    [Segment::C] = 4,
    [Segment::D] = 5,
    [Segment::E] = 6,
    [Segment::F] = 7,
    [Segment::G] = 8,
    [Segment::Dot] = 9,
};

void setup()
{
    /* Init segment display */
    for (auto pin : SEGMENT_OUTPUT_PINS)
        pinMode(pin, OUTPUT);
}

void loop() { }

int main()
{
    init();
    setup();
    for (;;)
        loop();
}
