#pragma once

struct GLFWwindow;

class Application
{
private:
    GLFWwindow* window;

public:
    Application();
    ~Application();

    void run();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
};