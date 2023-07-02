// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#include "Window.h"

#include "Windows/WindowsWindow.h"

namespace Basalt
{

Unique<Window> Window::Create()
{
#if BT_PLATFORM_WINDOWS
    return Unique<WindowsWindow>::Create().As<Window>();
#endif // BT_PLATFORM_WINDOWS
}

bool Window::Initialize(const WindowDescription& description)
{
    return true;
}

Window::~Window()
{
}

} // namespace Basalt
