// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "WindowsWindow.h"

#include "Application/WindowEvents.h"
#include "Engine/Engine.h"

namespace Basalt
{

constexpr const wchar_t* WindowClassName = L"BasaltWindowClass";

bool WindowsWindow::Initialize(const WindowDescription& description)
{
    if (!Super::Initialize(description))
    {
        // Failed to create the base window.
        return false;
    }

    // The platform call is performed only when the first window is created.
    WindowsWindow::RegisterWindowClass();

    m_width = description.client_width;
    m_height = description.client_height;

    m_position_x = description.client_x;
    m_position_y = description.client_y;

    DWORD window_ex_style = 0;
    DWORD window_style = 0;
    WindowsWindow::ReadWindowDescription(description, window_style, window_ex_style);

    // NOTE(traian): Creating the window will post events to the message queue. But since
    //               the window handle hasn't been acquired yet, they will be ignored.
    m_window_handle = ::CreateWindowEx(
        window_ex_style,
        WindowClassName,
        L"Basalt Editor - Win64 - Untitled*",
        window_style,
        0, 0, 0, 0, // The window size and position will be set later.
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (m_window_handle == nullptr)
    {
        BT_LOG_ERROR(Engine, "Failed to create the window with title: '%s'.", *description.title);
        return false;
    }

    if (description.mode == EWindowMode::Fullscreen)
    {
        U32 width, height;
        I32 x, y;

        GetFullscreenDimensions(&width, &height, &x, &y);
        // Since the window is in fullscreen mode, there are no borders.
        SetWindowPos(m_window_handle, NULL, x, y, width, height, 0);
        ::ShowWindow(m_window_handle, SW_SHOWMAXIMIZED);
    }
    else
    {
        ReshapeWindow(m_width, m_height, m_position_x, m_position_y);
        ::ShowWindow(m_window_handle, SW_SHOW);
    }

    return true;
}

WindowsWindow::~WindowsWindow()
{
    if (m_window_handle)
    {
        ::DestroyWindow(m_window_handle);
    }
}

void WindowsWindow::ProcessMessages()
{
    if (m_window_handle)
    {
        MSG message = {};
        while (::PeekMessage(&message, m_window_handle, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&message);
            ::DispatchMessage(&message);
        }
    }
}

void WindowsWindow::ReshapeWindow(U32 new_width, U32 new_height, I32 new_position_x, I32 new_position_y)
{
    // The window info is required for the window styles.
    WINDOWINFO window_info = {};
    ::GetWindowInfo(m_window_handle, &window_info);

    RECT border_rect = { 0, 0, 0, 0 };
    // Calling this function with a zeroed rect will determine the dimensions of the OS-native window.
    ::AdjustWindowRectEx(&border_rect, window_info.dwStyle, false, window_info.dwExStyle);

    // Adjust the dimensions in order to take into account the window border.
    // NOTE(traian): The stored width and height represent the dimensions of the client window area.
    new_width += (border_rect.right - border_rect.left);
    new_height += (border_rect.bottom - border_rect.top);

    // Adjust the position in order to take into account the window border.
    // NOTE(traian): The stored X position and Y position represent the coordinate of the top-left client window area corner.
    new_position_x += border_rect.left;
    new_position_y += border_rect.top;

    // This function will internally post an event to the message queue.
    // When that message will be processed, the dimension and position variables of the
    // window will be also set.
    ::SetWindowPos(m_window_handle, NULL, new_position_x, new_position_y, new_width, new_height, 0);
}

void WindowsWindow::GetFullscreenDimensions(U32* out_width, U32* out_height, I32* out_x, I32* out_y) const
{
    HMONITOR monitor_handle = ::MonitorFromWindow(m_window_handle, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFO monitor_info = {};
    monitor_info.cbSize = sizeof(MONITORINFO);
    ::GetMonitorInfo(monitor_handle, &monitor_info);

    *out_width = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
    *out_height = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;

    *out_x = monitor_info.rcMonitor.left;
    *out_y = monitor_info.rcMonitor.top;
}

void WindowsWindow::ReadWindowDescription(const WindowDescription& description, DWORD& style, DWORD& ex_style)
{
    if (description.mode == EWindowMode::Fullscreen)
    {
        style |= WS_POPUP;
    }
    else
    {
        style = WS_CAPTION | WS_SYSMENU;

        if (description.supports_maximize)
        {
            style |= WS_MAXIMIZEBOX;
        }

        if (description.supports_minimize)
        {
            style |= WS_MINIMIZEBOX;
        }

        if (description.supports_resizing)
        {
            style |= WS_THICKFRAME;
        }
        else
        {
            style |= WS_BORDER;
        }
    }

    if (description.appears_in_taskbar)
    {
        ex_style |= WS_EX_APPWINDOW;
    }
    else
    {
        ex_style |= WS_EX_TOOLWINDOW;
    }
}

static bool s_is_window_class_registered = false;
void WindowsWindow::RegisterWindowClass()
{
    if (!s_is_window_class_registered)
    {
        s_is_window_class_registered = true;

        WNDCLASSEX window_class = {};

        window_class.lpfnWndProc = WindowsWindow::MessageCallback;
        window_class.lpszClassName = WindowClassName;
        window_class.cbSize = sizeof(WNDCLASSEX);
        
        window_class.hIcon = NULL;
        window_class.hIconSm = NULL;

        ::RegisterClassEx(&window_class);
    }
}

LRESULT WindowsWindow::MessageCallback(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    return ::DefWindowProc(window_handle, message, w_param, l_param);
}

} // namespace Basalt
