// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Event.h"
#include "KeyCodes.h"

namespace Basalt
{

class KeyPressedEvent : public Event
{
public:
    FORCEINLINE static EEventType GetStaticType() { return EEventType::KeyPressed; }

public:
    FORCEINLINE KeyPressedEvent(EKey key)
        : Event(GetStaticType())
        , m_key(key)
    {}

    FORCEINLINE EKey GetKey() const { return m_key; }

private:
    EKey m_key;
};

class KeyReleasedEvent : public Event
{
public:
    FORCEINLINE static EEventType GetStaticType() { return EEventType::KeyReleased; }

public:
    FORCEINLINE KeyReleasedEvent(EKey key)
        : Event(GetStaticType())
        , m_key(key)
    {}

    FORCEINLINE EKey GetKey() const { return m_key; }

private:
    EKey m_key;
};

} // namespace Basalt
