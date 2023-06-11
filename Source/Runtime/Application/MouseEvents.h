// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Event.h"
#include "KeyCodes.h"

namespace Basalt
{

class MouseMovedEvent : public Event
{
public:
    FORCEINLINE static EEventType GetStaticType() { return EEventType::MouseMoved; }

public:
    FORCEINLINE MouseMovedEvent(I32 mouse_x, I32 mouse_y)
        : Event(GetStaticType())
        , m_mouse_x(mouse_x)
        , m_mouse_y(mouse_y)
    {}

    FORCEINLINE I32 GetMouseX() const { return m_mouse_x; }
    FORCEINLINE I32 GetMouseY() const { return m_mouse_y; }

private:
    I32 m_mouse_x;
    I32 m_mouse_y;
};

class MouseButtonPressedEvent : public Event
{
public:
    FORCEINLINE static EEventType GetStaticType() { return EEventType::MouseButtonPressed; }

public:
    FORCEINLINE MouseButtonPressedEvent(EMouseButton mouse_button)
        : Event(GetStaticType())
        , m_mouse_button(mouse_button)
    {}

    FORCEINLINE EMouseButton GetMouseButton() const { return m_mouse_button; }

private:
    EMouseButton m_mouse_button;
};

class MouseButtonReleasedEvent : public Event
{
public:
    FORCEINLINE static EEventType GetStaticType() { return EEventType::MouseButtonReleased; }

public:
    FORCEINLINE MouseButtonReleasedEvent(EMouseButton mouse_button)
        : Event(GetStaticType())
        , m_mouse_button(mouse_button)
    {}

    FORCEINLINE EMouseButton GetMouseButton() const { return m_mouse_button; }

private:
    EMouseButton m_mouse_button;
};

class MouseWheelScrolledEvent : public Event
{
public:
    FORCEINLINE static EEventType GetStaticType() { return EEventType::MouseWheelScrolled; }

public:
    FORCEINLINE MouseWheelScrolledEvent(float wheel_delta)
        : Event(GetStaticType())
        , m_wheel_delta(wheel_delta)
    {}

    FORCEINLINE float GetWheelDelta() const { return m_wheel_delta; }

private:
    float m_wheel_delta;
};

} // namespace Basalt
