// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Application/Event.h"
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

/** Function pointer that represents the callback that will be invoked when a message is received. */
using PFN_WindowEventCallback = void(*)(WindowHandle, Event*);

/**
 * Structure that describes the behavior and appearance of a window.
 */
struct WindowDescription
{
    /** Function pointer to the callback that will be invoked when a window event will be received. */
    PFN_WindowEventCallback event_callback = nullptr;

    /** The title of the window. Will be displayed if the window is windowed. */
    StringView title = "Basalt Window"sv;

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

    /** @return The mode of the window. */
    virtual StringView GetTitle() const = 0;

    /** @return The title of the window. */
    virtual EWindowMode GetMode() const = 0;

public:
    /**
     * Sets the width of the window client area. If the provided width is different
     * then the current dimension a window resized event will be generated.
     * 
     * @param client_width The new width of the window client area, expressed in pixels.
     */
    virtual void SetWidth(U32 client_width) = 0;
    
    /**
     * Sets the height of the window client area. If the provided height is different
     * than the current dimension a window resized event will be generated.
     *
     * @param client_height The new height of the window client area, expressed in pixels.
     */
    virtual void SetHeight(U32 client_height) = 0;
    
    /**
     * Sets the dimensions of the window client area. If either the provided width or height
     * is different than the current dimensions, a window resized event will be generated.
     *
     * @param client_width  The new width of the window client area, expressed in pixels.
     * @param client_height The new height of the window client area, expressed in pixels.
     */
    virtual void SetSize(U32 client_width, U32 client_height) = 0;

    /**
     * Sets the position of the window top-left client area corner. If the provided X position
     * is different than the current position a window moved event will be generated.
     * 
     * @param client_x The new position on the X-axis of the window client area.
     */
    virtual void SetPositionX(I32 client_x) = 0;

    /**
     * Sets the position of the window top-left client area corner. If the provided Y position
     * is different than the current position a window moved event will be generated.
     *
     * @param client_y The new position on the Y-axis of the window client area.
     */
    virtual void SetPositionY(I32 client_y) = 0;

    /**
     * Sets the position of the window top-left client area corner. If wither the provided X position
     * or the Y position is different than the current position a window moved event will be generated.
     *
     * @param client_x The new position on the X-axis of the window client area.
     * @param client_y The new position on the Y-axis of the window client area.
     */
    virtual void SetPosition(I32 client_x, I32 client_y) = 0;

    /**
     * Sets the title of the window. If the provided title is different than the current
     * title of the window, a window title changed event will be generated.
     * 
     * @param title The new window title.
     */
    virtual void SetTitle(StringView title) = 0;

    /**
     * Sets the window mode. Might generate a window resized event and a window moved event.
     * 
     * @param mode The new window mode.
     */
    virtual void SetMode(EWindowMode mode) = 0;

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
