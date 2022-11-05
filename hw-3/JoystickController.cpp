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
    static constexpr Tiny::Pair<u16, u16> INPUT_RANGE = { 0, 1023 };
    static constexpr u16 INPUT_MIDDLE = INPUT_RANGE.second / 2;
    static constexpr u16 AXIS_DELTA_THRESHOLD = 400;
    static constexpr u16 AXIS_MIN_THRESHOLD = INPUT_MIDDLE - AXIS_DELTA_THRESHOLD;
    static constexpr u16 AXIS_MAX_THRESHOLD = INPUT_MIDDLE + AXIS_DELTA_THRESHOLD;
    static constexpr u16 RESET_THRESHOLD = 30;
    static constexpr Tiny::Pair<u16, u16> RESET_RANGE
        = { INPUT_MIDDLE - RESET_THRESHOLD, INPUT_MIDDLE + RESET_THRESHOLD };

    const u16 xVal = analogRead(xAxis.pin);
    const u16 yVal = analogRead(yAxis.pin);

    switch (moveState) {
    case MoveState::Ok: {
        const auto xDir = xVal < AXIS_MIN_THRESHOLD
            ? Direction::Left
            : (xVal > AXIS_MAX_THRESHOLD ? Direction::Right : Direction::None);

        const auto yDir = yVal < AXIS_MIN_THRESHOLD
            ? Direction::Down
            : (yVal > AXIS_MAX_THRESHOLD ? Direction::Up : Direction::None);

        if (u8(xDir) || u8(yDir))
            moveState = MoveState::NeedsReset;

        if (u8(xDir) && u8(yDir)) {
            /* Both values are past the threshold, so choose the one closer to the limits */
            const auto xRemaining = min(xVal - INPUT_RANGE.first, INPUT_RANGE.second - xVal);
            const auto yRemaining = min(yVal - INPUT_RANGE.first, INPUT_RANGE.second - yVal);

            return xRemaining <= yRemaining ? xDir : yDir;
        }

        return Direction(u8(xDir) | u8(yDir));
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
