#pragma once
#include "JoystickController.h"

class DisplayController {
public:
    enum class State : u8 {
        Disengaged = 0,
        Engaged,
    };
    enum Section : u8 {
        D1 = 0,
        D2,
        D3,
        D4,
        NumSections,
    };
    enum Node : u8 {
        A = 0,
        B,
        C,
        D,
        E,
        F,
        G,
        DP,
        NumNodes,
    };

    using Bitset8 = u8;

    void init();
    void update(u32, JoystickController&);

    static constexpr u8 DATA_PIN = 12;
    static constexpr u8 LATCH_PIN = 11;
    static constexpr u8 CLOCK_PIN = 10;
    static constexpr u8 NUM_DIGITS = 16;
    static constexpr u8 SECTION_PINS[NumSections] = {
        [Section::D1] = 7,
        [Section::D2] = 6,
        [Section::D3] = 5,
        [Section::D4] = 4,
    };
    static constexpr Bitset8 SECTION_STATES[NumSections] = {
        [Section::D1] = 0b0001,
        [Section::D2] = 0b0010,
        [Section::D3] = 0b0100,
        [Section::D4] = 0b1000,
    };
    static constexpr Bitset8 DIGIT_NODE_STATES[NUM_DIGITS] = {
        [0x0] = 0b00111111,
        [0x1] = 0b00000110,
        [0x2] = 0b01011011,
        [0x3] = 0b01001111,
        [0x4] = 0b01100110,
        [0x5] = 0b01101101,
        [0x6] = 0b01111101,
        [0x7] = 0b00000111,
        [0x8] = 0b01111111,
        [0x9] = 0b01101111,
        [0xA] = 0b01110111,
        [0xB] = 0b01111100,
        [0xC] = 0b00111001,
        [0xD] = 0b01011110,
        [0xE] = 0b01111001,
        [0xF] = 0b01110001,
    };

private:
    void drawDigit(Bitset8);

private:
    Tiny::Array<u8, NumSections> sectionDigits;
    u8 currentSection;
    State currentState;
};
