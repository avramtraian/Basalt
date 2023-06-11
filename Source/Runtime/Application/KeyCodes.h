// Copyright (c) 2023 Basalt Technology Group. All rights reserved.

#pragma once

#include "Core/Core.h"

namespace Basalt
{

enum class EMouseButton : U8
{
    Unknown = 0,
    Left, Middle, Right,
    MaxEnumValue
};

enum class EKey : U16
{
    Unknown = 0,

    Zero,
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,

    A, B, C, D, E, F, G,
    H, I, J, K, L, M, N,
    O, P, Q, R, S, T, U,
    V, W, X, Y, Z,

    Shift,
    Control,
    Alt,
    Escape,
    Space,
    Enter,
    Backspace,
    Tab,
    CapsLock,
    NumLock,

    Left,
    Right,
    Up,
    Down,

    Plus,
    Minus,
    Comma,
    Period,

    Semicolon,
    ForwardSlash,
    Backslash,
    BackQuote,
    LeftBracket,
    RightBracket,
    Apostrophe,

    Insert,
    Delete,

    Numpad0,
    Numpad1,
    Numpad2,
    Numpad3,
    Numpad4,
    Numpad5,
    Numpad6,
    Numpad7,
    Numpad8,
    Numpad9,

    Multiply,
    Add,
    Subtract,
    Divide,
    Decimal,

    PageUp,
    PageDown,
    End,
    Home,

    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,

    MaxEnumValue
};

FORCEINLINE static StringView KeyCodeToString(EKey key_code)
{
    switch (key_code)
    {
        case EKey::Unknown:         return "Unknown"sv;

        case EKey::Zero:            return "Zero"sv;
        case EKey::One:             return "One"sv;
        case EKey::Two:             return "Two"sv;
        case EKey::Three:           return "Three"sv;
        case EKey::Four:            return "Four"sv;
        case EKey::Five:            return "Five"sv;
        case EKey::Six:             return "Six"sv;
        case EKey::Seven:           return "Seven"sv;
        case EKey::Eight:           return "Eight"sv;
        case EKey::Nine:            return "Nine"sv;
        case EKey::A:               return "A"sv;
        case EKey::B:               return "B"sv;
        case EKey::C:               return "C"sv;
        case EKey::D:               return "D"sv;
        case EKey::E:               return "E"sv;
        case EKey::F:               return "F"sv;
        case EKey::G:               return "G"sv;
        case EKey::H:               return "H"sv;
        case EKey::I:               return "I"sv;
        case EKey::J:               return "J"sv;
        case EKey::K:               return "K"sv;
        case EKey::L:               return "L"sv;
        case EKey::M:               return "M"sv;
        case EKey::N:               return "N"sv;
        case EKey::O:               return "O"sv;
        case EKey::P:               return "P"sv;
        case EKey::Q:               return "Q"sv;
        case EKey::R:               return "R"sv;
        case EKey::S:               return "S"sv;
        case EKey::T:               return "T"sv;
        case EKey::U:               return "U"sv;
        case EKey::V:               return "V"sv;
        case EKey::W:               return "W"sv;
        case EKey::X:               return "X"sv;
        case EKey::Y:               return "Y"sv;
        case EKey::Z:               return "Z"sv;
        case EKey::Shift:           return "Shift"sv;
        case EKey::Control:         return "Control"sv;
        case EKey::Alt:             return "Alt"sv;
        case EKey::Escape:          return "Escape"sv;
        case EKey::Space:           return "Space"sv;
        case EKey::Enter:           return "Enter"sv;
        case EKey::Backspace:       return "Backspace"sv;
        case EKey::Tab:             return "Tab"sv;
        case EKey::CapsLock:        return "CapsLock"sv;
        case EKey::NumLock:         return "NumLock"sv;
        case EKey::Left:            return "Left"sv;
        case EKey::Right:           return "Right"sv;
        case EKey::Up:              return "Up"sv;
        case EKey::Down:            return "Down"sv;
        case EKey::Plus:            return "Plus"sv;
        case EKey::Minus:           return "Minus"sv;
        case EKey::Comma:           return "Comma"sv;
        case EKey::Period:          return "Period"sv;
        case EKey::Semicolon:       return "Semicolon"sv;
        case EKey::ForwardSlash:    return "ForwardSlash"sv;
        case EKey::Backslash:       return "Backslash"sv;
        case EKey::BackQuote:       return "BackQuote"sv;
        case EKey::LeftBracket:     return "LeftBracket"sv;
        case EKey::RightBracket:    return "RightBracket"sv;
        case EKey::Apostrophe:      return "Apostrophe"sv;
        case EKey::Insert:          return "Insert"sv;
        case EKey::Delete:          return "Delete"sv;
        case EKey::Numpad0:         return "Numpad0"sv;
        case EKey::Numpad1:         return "Numpad1"sv;
        case EKey::Numpad2:         return "Numpad2"sv;
        case EKey::Numpad3:         return "Numpad3"sv;
        case EKey::Numpad4:         return "Numpad4"sv;
        case EKey::Numpad5:         return "Numpad5"sv;
        case EKey::Numpad6:         return "Numpad6"sv;
        case EKey::Numpad7:         return "Numpad7"sv;
        case EKey::Numpad8:         return "Numpad8"sv;
        case EKey::Numpad9:         return "Numpad9"sv;
        case EKey::Multiply:        return "Multiply"sv;
        case EKey::Add:             return "Add"sv;
        case EKey::Subtract:        return "Subtract"sv;
        case EKey::Divide:          return "Divide"sv;
        case EKey::Decimal:         return "Decimal"sv;
        case EKey::PageUp:          return "PageUp"sv;
        case EKey::PageDown:        return "PageDown"sv;
        case EKey::End:             return "End"sv;
        case EKey::Home:            return "Home"sv;
        case EKey::F1:              return "F1"sv;
        case EKey::F2:              return "F2"sv;
        case EKey::F3:              return "F3"sv;
        case EKey::F4:              return "F4"sv;
        case EKey::F5:              return "F5"sv;
        case EKey::F6:              return "F6"sv;
        case EKey::F7:              return "F7"sv;
        case EKey::F8:              return "F8"sv;
        case EKey::F9:              return "F9"sv;
        case EKey::F10:             return "F10"sv;
        case EKey::F11:             return "F11"sv;
        case EKey::F12:             return "F12"sv;
        case EKey::F13:             return "F13"sv;
        case EKey::F14:             return "F14"sv;
        case EKey::F15:             return "F15"sv;
        case EKey::F16:             return "F16"sv;
        case EKey::F17:             return "F17"sv;
        case EKey::F18:             return "F18"sv;
        case EKey::F19:             return "F19"sv;
        case EKey::F20:             return "F20"sv;
    }
}

} // namespace Basalt
