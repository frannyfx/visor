#include "Window.h"
#include <utility>
#include <tchar.h>
#include <iostream>

using namespace std;

Window::Window(std::string windowClassName) : temp_window(nullptr), window_class_name(std::move(windowClassName))
{
    ZeroMemory(&window_class, sizeof(WNDCLASSEX));

    window_class.cbSize = sizeof(WNDCLASSEX);
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = DefWindowProc;
    window_class.lpszClassName = window_class_name.c_str();

    window_class.hInstance = GetModuleHandle(nullptr);
    if (window_class.hInstance == nullptr) {
        cout << "KUSH" << endl;
        return;
    }

    if (!RegisterClassEx(&window_class)) {
        cout << "JUUL" << endl;
        return;
    }
        
    temp_window = CreateWindow(window_class.lpszClassName, "Visor", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, window_class.hInstance, NULL);
    if (temp_window == nullptr) {
        cout << GetLastError() << endl;
        return;
    }

    cout << "Created temporary window" << endl;
}

Window::~Window()
{
    if (temp_window)
    {
        DestroyWindow(temp_window);
        UnregisterClass(window_class.lpszClassName, window_class.hInstance);
    }
}

HWND Window::windowHandle() const
{
    return temp_window;
}