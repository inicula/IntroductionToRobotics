#include "JoystickController.h"

JoystickController::JoystickController(const u8 buttonPin, const u8 xPin, const u8 yPin)
{
    button.pin = buttonPin;
    xAxis.pin = xPin;
    yAxis.pin = yPin;
}

void JoystickController::init()
{
    pinMode(button.pin, INPUT_PULLUP);
    button.previousValue = HIGH;
    button.previousTs = millis();
}

JoystickController::Press JoystickController::getButtonValue(const u32 currentTs)
{
    /* Button thresholds */
    static constexpr u32 SHORT_PRESS_DUR = 50;
    static constexpr u32 LONG_PRESS_DURATION = 2000;

    const bool changed = updateButton(currentTs);
    if (!changed || !button.previousValue || button.pressDur < SHORT_PRESS_DUR)
        return Press::None;
    return Press(u8(Press::Short) + (button.pressDur > LONG_PRESS_DURATION));
}

JoystickController::Direction JoystickController::getDirection()
{
    /* Axis thresholds */
    static constexpr Tiny::Pair<u16, u16> INPUT_RANGE = {
        0,
        1023,
    };
    static constexpr u16 INPUT_MIDDLE = INPUT_RANGE.second / 2;
    static constexpr u16 AXIS_DELTA_THRESHOLD = 400;
    static constexpr u16 RESET_DELTA_THRESHOLD = 80;
    static constexpr u16 NON_CONFLICT_DELTA_THRESHOLD = 200;
    static constexpr u16 AXIS_MIN_THRESHOLD = INPUT_MIDDLE - AXIS_DELTA_THRESHOLD;
    static constexpr u16 AXIS_MAX_THRESHOLD = INPUT_MIDDLE + AXIS_DELTA_THRESHOLD;
    static constexpr Tiny::Pair<u16, u16> RESET_RANGE = {
        INPUT_MIDDLE - RESET_DELTA_THRESHOLD,
        INPUT_MIDDLE + RESET_DELTA_THRESHOLD,
    };
    static constexpr Tiny::Pair<u16, u16> NON_CONFLICT_RANGE = {
        INPUT_MIDDLE - NON_CONFLICT_DELTA_THRESHOLD,
        INPUT_MIDDLE + NON_CONFLICT_DELTA_THRESHOLD,
    };

    const auto xVal = u16(analogRead(xAxis.pin));
    const auto yVal = u16(analogRead(yAxis.pin));

    switch (moveState) {
    case MoveState::Ok: {
        const auto xDir = xVal < AXIS_MIN_THRESHOLD
            ? Direction::Left
            : (xVal > AXIS_MAX_THRESHOLD ? Direction::Right : Direction::None);

        const auto yDir = yVal < AXIS_MIN_THRESHOLD
            ? Direction::Down
            : (yVal > AXIS_MAX_THRESHOLD ? Direction::Up : Direction::None);

        moveState = MoveState::NeedsReset;
        if (u8(xDir) && yVal == Tiny::clamp(yVal, NON_CONFLICT_RANGE))
            return xDir;
        if (u8(yDir) && xVal == Tiny::clamp(xVal, NON_CONFLICT_RANGE))
            return yDir;

        moveState = MoveState::Ok;
        return Direction::None;
    }
    case MoveState::NeedsReset:
        if (xVal == Tiny::clamp(xVal, RESET_RANGE) && yVal == Tiny::clamp(yVal, RESET_RANGE))
            moveState = MoveState::Ok;
        return Direction::None;
    default:
        UNREACHABLE;
    }
}

bool JoystickController::updateButton(const u32 currentTs)
{
    const bool currentValue = digitalRead(button.pin);
    if (currentValue != button.previousValue) {
        button.previousValue = currentValue;
        button.pressDur = currentTs - button.previousTs;
        button.previousTs = currentTs;

        return true;
    }

    return false;
}
