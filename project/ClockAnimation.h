#pragma once

#include <chrono>

class ShaderProgram;

// Update all clock instances using local time, including fractional seconds.
void updateClockHands(
    const ShaderProgram& program,
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now());
