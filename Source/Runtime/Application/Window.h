// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"

namespace Basalt
{

/**
 * Enumeration of the modes that a window can be in.
 */
enum class EWindowMode
{
    Unknown = 0,

    Windowed,
    Fullscreen,

    MaxEnumValue
};

/** Native window handle. */
using WindowHandle = void*;

/**
 * Structure that describes the behavior and appearance of a window.
 */
struct WindowDescription
{

    /** The title of the window. Will be displayed if the window is windowed. */
    String title = "Basalt Window"sv;

    /**
     * The width of the window client area.
     * Unless the window is in fullscreen mode, the actual extent of the window will be
     * larger, as this doesn't include the borders.
     */
    U32 client_width = 1280;

    /**
     * The height of the window client area.
     * Unless the window is in fullscreen mode, the actual extent of the window will be
     * larger, as this doesn't include the borders.
     */
    U32 client_height = 720;

    /**
     * The position on the X-axis of the top-left window client area corner.
     * Unless the window is in fullscreen mode, this position doesn't account
     * for the window borders.
     */
    I32 client_x = 300;

    /**
     * The position on the Y-axis of the top-left window client area corner.
     * Unless the window is in fullscreen mode, this position doesn't account
     * for the window borders.
     */
    I32 client_y = 300;

    /** The mode the window will be created in. */
    EWindowMode mode = EWindowMode::Windowed;

    /** Whether or not the window can be maximized by the user. */
    bool supports_maximize = true;
    
    /** Whether or not the window can be minimized by the user. */
    bool supports_minimize = true;

    /** Whether or not the window can be resized by the used. */
    bool supports_resizing = true;

    /** Whether or not the window will appear in the taskbar. */
    bool appears_in_taskbar = true;
};

/**
 * Abstract interface responsible for managing a window.
 * This is the base for all platform-specific window implementations.
 */
class Window
{
public:
    static Unique<Window> Create();

public:
    Window() = default;
    virtual ~Window();

public:
    virtual bool Initialize(const WindowDescription& description);

public:
    /** @return The platform-specific window native handle. */
    virtual WindowHandle GetNativeHandle() const = 0;

    /** @return True if the window will be destroyed at the end of this frame; False otherwise. */
    FORCEINLINE bool IsPendingClose() const { return m_is_pending_close; }

    /** @return The width of the window client area. */
    virtual U32 GetWidth() const = 0;

    /** @return The height of the window client area. */
    virtual U32 GetHeight() const = 0;

    /** @return The position on the X-axis of the window client area top-left corner. */
    virtual I32 GetPositionX() const = 0;

    /** @return The position on the Y-axis of the window client area top-left corner. */
    virtual I32 GetPositionY() const = 0;

private:
    /**
     * Queries the window for messages and dispatches them through the engine event system.
     * This function is called at the start of every frame.
     */
    virtual void ProcessMessages() = 0;

protected:
    /** Whether or not the window has been marked for destruction at the end of the frame. */
    bool m_is_pending_close = false;

protected:
    // The engine class is responsible for managing when the window should process messages.
    friend class Engine;
};

/**
 * Converts a window mode to a readable string.
 * 
 * @param window_mode The window mode to convert.
 * @return The window mode as a readable string.
 */
FORCEINLINE static StringView WindowModeToString(EWindowMode window_mode)
{
    switch (window_mode)
    {
        case EWindowMode::Unknown:      return "Unknown"sv;
        case EWindowMode::Windowed:     return "Windowed"sv;
        case EWindowMode::Fullscreen:   return "Fullscreen"sv;
    }
    
    Check(false); // Invalid window mode.
    return "Invalid"sv;
}

} // namespace Basalt
