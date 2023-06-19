// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Event.h"

namespace Basalt
{

class BASALT_API WindowClosedEvent : public Event
{
public:
    FORCEINLINE static EEventType GetStaticType() { return EEventType::WindowClosed; }

public:
    FORCEINLINE WindowClosedEvent()
        : Event(GetStaticType())
    {}
};

class BASALT_API WindowResizedEvent : public Event
{
public:
    FORCEINLINE static EEventType GetStaticType() { return EEventType::WindowResized; }

public:
    FORCEINLINE WindowResizedEvent(U32 width, U32 height)
        : Event(GetStaticType())
        , m_width(width)
        , m_height(height)
    {}

    FORCEINLINE U32 GetWidth() const { return m_width; }
    FORCEINLINE U32 GetHeight() const { return m_height; }

private:
    U32 m_width;
    U32 m_height;
};

class BASALT_API WindowMovedEvent : public Event
{
public:
    FORCEINLINE static EEventType GetStaticType() { return EEventType::WindowMoved; }

public:
    FORCEINLINE WindowMovedEvent(I32 position_x, I32 position_y)
        : Event(GetStaticType())
        , m_position_x(position_x)
        , m_position_y(position_y)
    {}

    FORCEINLINE I32 GetPositionX() const { return m_position_x; }
    FORCEINLINE I32 GetPositionY() const { return m_position_y; }

private:
    I32 m_position_x;
    I32 m_position_y;
};

class BASALT_API WindowTitleChangedEvent : public Event
{
public:
    FORCEINLINE static EEventType GetStaticType() { return EEventType::WindowTitleChanged; }

public:
    FORCEINLINE WindowTitleChangedEvent(StringView title)
        : Event(GetStaticType())
        , m_title(title)
    {}

    FORCEINLINE StringView GetTitle() const { return m_title; }

private:
    StringView m_title;
};

class BASALT_API WindowModeChangedEvent : public Event
{
public:
    FORCEINLINE static EEventType GetStaticType() { return EEventType::WindowModeChanged; }

public:
    FORCEINLINE WindowModeChangedEvent()
        : Event(GetStaticType())
    {}
};

} // namespace Basalt
