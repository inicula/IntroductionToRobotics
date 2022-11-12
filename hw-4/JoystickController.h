#pragma once
#include "utils.h"
#include <Arduino.h>

class JoystickController {
public:
    enum class Direction : u8 {
        None = 0,
        Up,
        Down,
        Left,
        Right,
        NumDirections,
    };
    enum class Press : u8 {
        None = 0,
        Short,
        Long,
    };
    enum class MoveState : u8 {
        Ok = 0,
        NeedsReset,
    };

    JoystickController(u8, u8, u8);
    void init();
    Press getButtonValue(u32);
    Direction getDirection();

    static constexpr auto NUM_DIRECTIONS = u8(Direction::NumDirections);

private:
    bool updateButton(u32);

private:
    struct {
        u8 pin;
        bool previousValue;
        u32 previousTs;
        u32 pressDur;
    } button;
    struct {
        u8 pin; /* Analog input */
    } xAxis, yAxis;
    MoveState moveState;
};
