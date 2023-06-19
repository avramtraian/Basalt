// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "WindowsWindow.h"

#include "Application/KeyEvents.h"
#include "Application/MouseEvents.h"
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

    m_event_callback = description.event_callback;

    m_width = description.client_width;
    m_height = description.client_height;

    m_position_x = description.client_x;
    m_position_y = description.client_y;

    m_window_mode = description.mode;

    m_title = description.title;
    
    DWORD window_ex_style = 0;
    DWORD window_style = 0;
    WindowsWindow::ReadWindowDescription(description, window_style, window_ex_style);

    ScopedBuffer title_buffer;
    StringBuilder::ToUTF16Dynamic(m_title.ToView(), title_buffer.RawBuffer(), true);

    // NOTE(traian): Creating the window will post events to the message queue. But since
    //               the window handle hasn't been acquired yet, they will be ignored.
    m_window_handle = ::CreateWindowEx(
        window_ex_style,
        WindowClassName,
        title_buffer.As<wchar_t>(),
        window_style,
        0, 0, 0, 0, // The window size and position will be set later.
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (m_window_handle == nullptr)
    {
        BT_LOG_ERROR(Engine, "Failed to create the window with title: '%s'.", *description.title);
        return false;
    }

    if (m_window_mode == EWindowMode::Fullscreen)
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

void WindowsWindow::SetWidth(U32 client_width)
{
    if (m_width == client_width)
    {
        return;
    }

    ReshapeWindow(client_width, m_height, m_position_x, m_position_y);
}

void WindowsWindow::SetHeight(U32 client_height)
{
    if (m_height == client_height)
    {
        return;
    }

    ReshapeWindow(m_width, client_height, m_position_x, m_position_y);
}

void WindowsWindow::SetSize(U32 client_width, U32 client_height)
{
    if (m_width == client_width && m_height == client_height)
    {
        return;
    }

    ReshapeWindow(client_width, client_height, m_position_x, m_position_y);
}

void WindowsWindow::SetPositionX(I32 client_x)
{
    if (m_position_x == client_x)
    {
        return;
    }

    ReshapeWindow(m_width, m_height, client_x, m_position_y);
}

void WindowsWindow::SetPositionY(I32 client_y)
{
    if (m_position_y == client_y)
    {
        return;
    }

    ReshapeWindow(m_width, m_height, m_position_x, client_y);
}

void WindowsWindow::SetPosition(I32 client_x, I32 client_y)
{
    if (m_position_x == client_x && m_position_y == client_y)
    {
        return;
    }

    ReshapeWindow(m_width, m_height, client_x, client_y);
}

void WindowsWindow::SetTitle(StringView title)
{
    Buffer title_buffer;
    StringBuilder::ToUTF16Dynamic(title, title_buffer, true);

    ::SetWindowText(m_window_handle, title_buffer.As<wchar_t>());
    title_buffer.Release();
}

void WindowsWindow::SetMode(EWindowMode mode)
{
    if (mode == m_window_mode)
    {
        return;
    }

    m_window_mode = mode;

    if (m_window_mode == EWindowMode::Fullscreen)
    {
        U32 width, height;
        I32 position_x, position_y;
        GetFullscreenDimensions(&width, &height, &position_x, &position_y);

        ::SetWindowLong(m_window_handle, GWL_STYLE, WS_POPUP);
        ::SetWindowPos(m_window_handle, HWND_TOP, position_x, position_y, width, height, 0);
    }
    else if (m_window_mode == EWindowMode::Windowed)
    {
        // TODO(traian): Implement!
        Check(false);
    }

    if (m_event_callback)
    {
        WindowModeChangedEvent ev;
        m_event_callback(m_window_handle, &ev);
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

namespace Utils
{

FORCEINLINE static EKey TranslateKeyCode(WPARAM win32_code)
{
    switch (win32_code)
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            return (EKey)((U16)EKey::Zero + (win32_code - '0'));
        }

        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        {
            return (EKey)((U16)EKey::A + (win32_code - 'A'));
        }

        case VK_SHIFT:      return EKey::Shift;
        case VK_CONTROL:    return EKey::Control;
        case VK_MENU:       return EKey::Alt;
        case VK_ESCAPE:     return EKey::Escape;
        case VK_SPACE:      return EKey::Space;
        case VK_RETURN:     return EKey::Enter;
        case VK_BACK:       return EKey::Backspace;
        case VK_TAB:        return EKey::Tab;
        case VK_CAPITAL:    return EKey::CapsLock;
        case VK_NUMLOCK:    return EKey::NumLock;

        case VK_LEFT:       return EKey::Left;
        case VK_RIGHT:      return EKey::Right;
        case VK_UP:         return EKey::Up;
        case VK_DOWN:       return EKey::Down;

        case VK_OEM_PLUS:   return EKey::Plus;
        case VK_OEM_MINUS:  return EKey::Minus;
        case VK_OEM_COMMA:  return EKey::Comma;
        case VK_OEM_PERIOD: return EKey::Period;

        case VK_OEM_1:      return EKey::Semicolon;
        case VK_OEM_2:      return EKey::ForwardSlash;
        case VK_OEM_5:      return EKey::Backslash;
        case VK_OEM_3:      return EKey::BackQuote;
        case VK_OEM_4:      return EKey::LeftBracket;
        case VK_OEM_6:      return EKey::RightBracket;
        case VK_OEM_7:      return EKey::Apostrophe;

        case VK_INSERT:     return EKey::Insert;
        case VK_DELETE:     return EKey::Delete;

        case VK_NUMPAD0:    return EKey::Numpad0;
        case VK_NUMPAD1:    return EKey::Numpad1;
        case VK_NUMPAD2:    return EKey::Numpad2;
        case VK_NUMPAD3:    return EKey::Numpad3;
        case VK_NUMPAD4:    return EKey::Numpad4;
        case VK_NUMPAD5:    return EKey::Numpad5;
        case VK_NUMPAD6:    return EKey::Numpad6;
        case VK_NUMPAD7:    return EKey::Numpad7;
        case VK_NUMPAD8:    return EKey::Numpad8;
        case VK_NUMPAD9:    return EKey::Numpad9;

        case VK_MULTIPLY:   return EKey::Multiply;
        case VK_ADD:        return EKey::Add;
        case VK_SUBTRACT:   return EKey::Subtract;
        case VK_DIVIDE:     return EKey::Divide;
        case VK_DECIMAL:    return EKey::Decimal;

        case VK_PRIOR:      return EKey::PageUp;
        case VK_NEXT:       return EKey::PageDown;
        case VK_END:        return EKey::End;
        case VK_HOME:       return EKey::Home;

        case VK_F1:         return EKey::F1;
        case VK_F2:         return EKey::F2;
        case VK_F3:         return EKey::F3;
        case VK_F4:         return EKey::F4;
        case VK_F5:         return EKey::F5;
        case VK_F6:         return EKey::F6;
        case VK_F7:         return EKey::F7;
        case VK_F8:         return EKey::F8;
        case VK_F9:         return EKey::F9;
        case VK_F10:        return EKey::F10;
        case VK_F11:        return EKey::F11;
        case VK_F12:        return EKey::F12;
        case VK_F13:        return EKey::F13;
        case VK_F14:        return EKey::F14;
        case VK_F15:        return EKey::F15;
        case VK_F16:        return EKey::F16;
        case VK_F17:        return EKey::F17;
        case VK_F18:        return EKey::F18;
        case VK_F19:        return EKey::F19;
        case VK_F20:        return EKey::F20;

        // Unsupported key codes.
        case VK_OEM_102:
        {
            return EKey::Unknown;
        }
    }

    BT_LOG_WARN(Engine, "Unknown key code encountered: %d", (int)win32_code);
    return EKey::Unknown;
}

} // namespace Basalt::Utils

LRESULT WindowsWindow::MessageCallback(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    switch (message)
    {
        case WM_CLOSE:
        {
            WindowsWindow* window = (WindowsWindow*)g_engine->GetWindowByHandle(window_handle);
            if (!window)
            {
                break;
            }

            window->m_is_pending_close = true;

            if (window->m_event_callback)
            {
                WindowClosedEvent ev;
                window->m_event_callback(window->GetNativeHandle(), &ev);
            }
            
            return 0;
        }

        case WM_SIZE:
        {
            const U32 width = (U32)LOWORD(l_param);
            const U32 height = (U32)HIWORD(l_param);

            WindowsWindow* window = (WindowsWindow*)g_engine->GetWindowByHandle(window_handle);
            if (!window)
            {
                break;
            }

            window->m_width = width;
            window->m_height = height;

            if (window->m_event_callback)
            {
                WindowResizedEvent ev(width, height);
                window->m_event_callback(window->GetNativeHandle(), &ev);
            }

            return 0;
        }

        case WM_MOVE:
        {
            POINTS point = MAKEPOINTS(l_param);

            WindowsWindow* window = (WindowsWindow*)g_engine->GetWindowByHandle(window_handle);
            if (!window)
            {
                break;
            }

            window->m_position_x = point.x;
            window->m_position_y = point.y;

            if (window->m_event_callback)
            {
                WindowMovedEvent ev((I32)point.x, (I32)point.y);
                window->m_event_callback(window->GetNativeHandle(), &ev);
            }

            return 0;
        }

        case WM_SETTEXT:
        {
            const wchar_t* title = (const wchar_t*)l_param;

            WindowsWindow* window = (WindowsWindow*)g_engine->GetWindowByHandle(window_handle);
            if (!window)
            {
                break;
            }

            {
                ScopedBuffer title_buffer = ScopedBuffer(32);
                Usize title_size = StringBuilder::FromUTF16Dynamic(title, title_buffer.RawBuffer(), false);
                window->m_title = StringView(title_buffer.As<char>(), title_size);
            }

            if (window->m_event_callback)
            {
                WindowTitleChangedEvent ev(window->m_title.ToView());
                window->m_event_callback(window->GetNativeHandle(), &ev);
            }

            break;
        }

        case WM_MOUSEMOVE:
        {
            POINTS point = MAKEPOINTS(l_param);

            WindowsWindow* window = (WindowsWindow*)g_engine->GetWindowByHandle(window_handle);
            if (!window || !window->m_event_callback)
            {
                break;
            }

            MouseMovedEvent ev((I32)point.x, (I32)point.y);
            window->m_event_callback(window->GetNativeHandle(), &ev);
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            WindowsWindow* window = (WindowsWindow*)g_engine->GetWindowByHandle(window_handle);
            if (!window || !window->m_event_callback)
            {
                break;
            }

            MouseButtonPressedEvent ev(EMouseButton::Left);
            window->m_event_callback(window->GetNativeHandle(), &ev);
            return 0;
        }

        case WM_LBUTTONUP:
        {
            WindowsWindow* window = (WindowsWindow*)g_engine->GetWindowByHandle(window_handle);
            if (!window || !window->m_event_callback)
            {
                break;
            }

            MouseButtonReleasedEvent ev(EMouseButton::Left);
            window->m_event_callback(window->GetNativeHandle(), &ev);
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            WindowsWindow* window = (WindowsWindow*)g_engine->GetWindowByHandle(window_handle);
            if (!window || !window->m_event_callback)
            {
                break;
            }

            MouseButtonPressedEvent ev(EMouseButton::Right);
            window->m_event_callback(window->GetNativeHandle(), &ev);
            return 0;
        }

        case WM_RBUTTONUP:
        {
            WindowsWindow* window = (WindowsWindow*)g_engine->GetWindowByHandle(window_handle);
            if (!window || !window->m_event_callback)
            {
                break;
            }

            MouseButtonReleasedEvent ev(EMouseButton::Right);
            window->m_event_callback(window->GetNativeHandle(), &ev);
            return 0;
        }

        case WM_MBUTTONDOWN:
        {
            WindowsWindow* window = (WindowsWindow*)g_engine->GetWindowByHandle(window_handle);
            if (!window || !window->m_event_callback)
            {
                break;
            }

            MouseButtonPressedEvent ev(EMouseButton::Middle);
            window->m_event_callback(window->GetNativeHandle(), &ev);
            return 0;
        }

        case WM_MBUTTONUP:
        {
            WindowsWindow* window = (WindowsWindow*)g_engine->GetWindowByHandle(window_handle);
            if (!window || !window->m_event_callback)
            {
                break;
            }

            MouseButtonReleasedEvent ev(EMouseButton::Middle);
            window->m_event_callback(window->GetNativeHandle(), &ev);
            return 0;
        }

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            WindowsWindow* window = (WindowsWindow*)g_engine->GetWindowByHandle(window_handle);
            if (!window || !window->m_event_callback)
            {
                break;
            }

            KeyPressedEvent ev(Utils::TranslateKeyCode(w_param));
            window->m_event_callback(window->GetNativeHandle(), &ev);
            return 0;
        }

        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            WindowsWindow* window = (WindowsWindow*)g_engine->GetWindowByHandle(window_handle);
            if (!window || !window->m_event_callback)
            {
                break;
            }

            KeyReleasedEvent ev(Utils::TranslateKeyCode(w_param));
            window->m_event_callback(window->GetNativeHandle(), &ev);
            return 0;
        }
    }

    return ::DefWindowProc(window_handle, message, w_param, l_param);
}

} // namespace Basalt
