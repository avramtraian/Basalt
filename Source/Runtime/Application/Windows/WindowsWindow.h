// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Application/Window.h"

#include <Windows.h>

namespace Basalt
{

class WindowsWindow : public Window
{
public:
    using Super = Window;

public:
    virtual bool Initialize(const WindowDescription& description) override;

    WindowsWindow() = default;
    virtual ~WindowsWindow() override;

    FORCEINLINE virtual WindowHandle GetNativeHandle() const override { return (void*)m_window_handle; }

    FORCEINLINE virtual U32 GetWidth() const { return m_width; }
    FORCEINLINE virtual U32 GetHeight() const { return m_height; }

    FORCEINLINE virtual I32 GetPositionX() const { return m_position_x; }
    FORCEINLINE virtual I32 GetPositionY() const { return m_position_y; }

private:
    virtual void ProcessMessages() override;

    void ReshapeWindow(U32 new_width, U32 new_height, I32 new_position_x, I32 new_position_y);

    void GetFullscreenDimensions(U32* out_width, U32* out_height, I32* out_x, I32* out_y) const;

private:
    static void RegisterWindowClass();

    static void ReadWindowDescription(const WindowDescription& description, DWORD& out_style, DWORD& out_ex_style);

    static LRESULT MessageCallback(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

private:
    HWND m_window_handle = NULL;

    U32 m_width = 0;
    U32 m_height = 0;

    I32 m_position_x = 0;
    I32 m_position_y = 0;

    PFN_WindowEventCallback m_event_callback = nullptr;
};

} // namespace Basalt
