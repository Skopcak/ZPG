#include "ClockAnimation.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <glm/trigonometric.hpp>

#include "Rotation.h"
#include "ShaderProgram.h"

void updateClockHands(
    const ShaderProgram& program,
    std::chrono::system_clock::time_point now)
{
    const auto wholeSeconds = std::chrono::floor<std::chrono::seconds>(now);
    const std::time_t timestamp =
        std::chrono::system_clock::to_time_t(wholeSeconds);

    std::tm localTime{};
    if (localtime_s(&localTime, &timestamp) != 0) {
        std::cerr << "Unable to read local time\n";
        std::exit(EXIT_FAILURE);
    }

    const float seconds = static_cast<float>(localTime.tm_sec) + std::chrono::duration<float>(now - wholeSeconds).count();
    const float minutes = static_cast<float>(localTime.tm_min) + seconds / 60.0f;
    const float hours = static_cast<float>(localTime.tm_hour % 12) + minutes / 60.0f;

    const float clockwiseTurn = glm::radians(-360.0f);
    const Rotation hourHandRotation(clockwiseTurn * (hours / 12.0f), 0.0f, 0.0f, 1.0f);
    const Rotation minuteHandRotation(clockwiseTurn * (minutes / 60.0f), 0.0f, 0.0f, 1.0f);
    const Rotation secondHandRotation(clockwiseTurn * (seconds / 60.0f), 0.0f, 0.0f, 1.0f);

    program.setUniform("hourMatrix", hourHandRotation.getMatrix());
    program.setUniform("minuteMatrix", minuteHandRotation.getMatrix());
    program.setUniform("secondMatrix", secondHandRotation.getMatrix());
}
