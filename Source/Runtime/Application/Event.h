// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"

namespace Basalt
{

/**
 * Enumeration of event types.
 */
enum class EEventType : U8
{
    Unknown = 0,

    // Window events.
    WindowClosed, WindowResized, WindowMoved, WindowTitleChanged, WindowModeChanged,
    
    // Mouse events.
    MouseMoved, MouseButtonPressed, MouseButtonReleased, MouseWheelScrolled,

    // Keyboard events.
    KeyPressed, KeyReleased,

    MaxEnumValue
};

/**
 * Base class for all event types.
 */
class BASALT_API Event
{
public:
    FORCEINLINE Event(EEventType type)
        : m_type(type)
        , m_is_handled(false)
    {}

public:
    FORCEINLINE EEventType GetType() const { return m_type; }

    FORCEINLINE bool IsHandled() const { return m_is_handled; }

private:
    EEventType m_type;

    bool m_is_handled;

private:
    friend class EventDispatcher;
};

/**
 * 
 */
class BASALT_API EventDispatcher
{
public:
    FORCEINLINE EventDispatcher(Event* event_context)
        : m_event_context(event_context)
    {}

    template<typename T>
    FORCEINLINE void Dispatch(bool(*callback)(const T&))
    {
        if (m_event_context->GetType() == T::GetStaticType())
        {
            m_event_context->m_is_handled = callback(*(T*)(m_event_context));
        }
    }

private:
    Event* m_event_context;
};

} // namespace Basalt
