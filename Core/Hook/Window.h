#pragma once

#include <string>
#include <Windows.h>

class Window
{
    WNDCLASSEX window_class{};
    HWND temp_window;
    std::string window_class_name;
public:
    Window(std::string windowClassName);
    HWND windowHandle() const;
    ~Window();
};
