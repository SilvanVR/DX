#include "keycodes.h"

/**********************************************************************
    class: None (keycodes.cpp)

    author: S. Hau
    date: November 4, 2017
**********************************************************************/

String KeyToString(Key key)
{
    switch (key)
    {
        case Key::Backspace:    return "Backspace";
        case Key::Tab:          return "Tab";
        case Key::Clear:        return "Clear";
        case Key::Enter:        return "Enter";
        case Key::Shift:        return "Shift";
        case Key::Control:      return "Control";
        case Key::Alt:          return "Alt";
        case Key::Pause:        return "Pause";
        case Key::CapsLock:     return "CapsLock";
        case Key::Escape:       return "Escape";
        case Key::Space:        return "Space";
        case Key::PageUp:       return "PageUp";
        case Key::PageDown:     return "PageDown";
        case Key::End:          return "End";
        case Key::Home:         return "Home";
        case Key::Left:         return "Left";
        case Key::Up:           return "Up";
        case Key::Right:        return "Right";
        case Key::Down:         return "Down";
        case Key::Select:       return "Select";
        case Key::Print:        return "Print";
        case Key::Execute:      return "Execute";
        case Key::PrintScreen:  return "PrintScreen";
        case Key::Insert:       return "Insert";
        case Key::Delete:       return "Delete";
        case Key::Help:         return "Help";
        case Key::Zero:         return "Zero";
        case Key::One:          return "One";
        case Key::Two:          return "Two";
        case Key::Three:        return "Three";
        case Key::Four:         return "Four";
        case Key::Five:         return "Five";
        case Key::Six:          return "Six";
        case Key::Seven:        return "Seven";
        case Key::Eight:        return "Eight";
        case Key::Nine:         return "Nine";
        case Key::A: 
        case Key::B: 
        case Key::C: 
        case Key::D: 
        case Key::E:
        case Key::F:
        case Key::G:
        case Key::H:
        case Key::I:
        case Key::J:
        case Key::K:
        case Key::L:
        case Key::M:
        case Key::N:
        case Key::O:
        case Key::P:
        case Key::Q:
        case Key::R:
        case Key::S:
        case Key::T:
        case Key::U:
        case Key::V:
        case Key::W:
        case Key::X:
        case Key::Y:
        case Key::Z:
            return String(1, (char)key);
        case Key::LeftWindowsKey:   return "LeftWindowsKey";
        case Key::RightWindowsKey:  return "RightWindowsKey";
        case Key::ApplicationsKey:  return "ApplicationsKey";
        case Key::Sleep:            return "Sleep";
        case Key::NumPad0:          return "NumPad0";
        case Key::NumPad1:          return "NumPad1";
        case Key::NumPad2:          return "NumPad2";
        case Key::NumPad3:          return "NumPad3";
        case Key::NumPad4:          return "NumPad4";
        case Key::NumPad5:          return "NumPad5";
        case Key::NumPad6:          return "NumPad6";
        case Key::NumPad7:          return "NumPad7";
        case Key::NumPad8:          return "NumPad8";
        case Key::NumPad9:          return "NumPad9";
        case Key::Multiply:         return "Multiply";
        case Key::Add:              return "Add";
        case Key::Seperator:        return "Seperator";
        case Key::Subtract:         return "Subtract";
        case Key::Decimal:          return "Decimal";
        case Key::Divide:           return "Divice";
        case Key::F1:               return "F1";
        case Key::F2:               return "F2";
        case Key::F3:               return "F3";
        case Key::F4:               return "F4";
        case Key::F5:               return "F5";
        case Key::F6:               return "F6";
        case Key::F7:               return "F7";
        case Key::F8:               return "F8";
        case Key::F9:               return "F9";
        case Key::F10:              return "F10";
        case Key::F11:              return "F11";
        case Key::F12:              return "F12";
        case Key::F13:              return "F13";
        case Key::F14:              return "F14";
        case Key::F15:              return "F15";
        case Key::F16:              return "F16";
        case Key::F17:              return "F17";
        case Key::F18:              return "F18";
        case Key::F19:              return "F19";
        case Key::F20:              return "F20";
        case Key::F21:              return "F21";
        case Key::F22:              return "F22";
        case Key::F23:              return "F23";
        case Key::F24:              return "F24";
        case Key::Numlock:          return "Numlock";
        case Key::ScrollLock:       return "ScrollLock";
        case Key::LeftShift:        return "LeftShift";
        case Key::RightShift:       return "RightShift";
        case Key::LeftControl:      return "LeftControl";
        case Key::RightContol:      return "RightControl";
        case Key::LeftMenu:         return "LeftMenu";
        case Key::RightMenu:        return "RightMenu";
        case Key::BrowserBack:      return "BrowserBack";
        case Key::BrowserForward:   return "BrowserForward";
        case Key::BrowserRefresh:   return "BrowserRefresh";
        case Key::BrowserStop:      return "BrowserStop";
        case Key::BrowserSearch:    return "BrowserSearch";
        case Key::BrowserFavorites: return "BrowserFavorites";
        case Key::BrowserHome:      return "BrowserHome";
        case Key::VolumeMute:       return "VolumeMute";
        case Key::VolumeDown:       return "VolumeDown";
        case Key::VolumeUp:         return "VolumeUp";
        case Key::NextTrack:        return "NextTrack";
        case Key::PreviousTrack:    return "PreviousTrack";
        case Key::StopMedia:        return "StopMedia";
        case Key::PlayPause:        return "PlayPause";
        case Key::LaunchMail:       return "LaunchMail";
        case Key::SelectMedia:      return "SelectMedia";
        case Key::LaunchApp1:       return "LaunchApp1";
        case Key::LaunchApp2:       return "LaunchApp2";
        case Key::OEM1:             return "OEM1";
        case Key::OEMPlus:          return "OEMPlus";
        case Key::OEMComma:         return "OEMComma";
        case Key::OEMMinus:         return "OEMMinus";
        case Key::OEMPeriod:        return "OEMPeriod";
        case Key::OEM2:             return "OEM2";
        case Key::OEM3:             return "OEM3";
        case Key::OEM4:             return "OEM4";
        case Key::OEM5:             return "OEM5";
        case Key::OEM6:             return "OEM6";
        case Key::OEM7:             return "OEM7";
        case Key::OEM8:             return "OEM8";
        case Key::OEM102:           return "OEM102";
        case Key::Process:          return "Process";
        case Key::Packet:           return "Packet";
        case Key::Attn:             return "Attn";
        case Key::CrSel:            return "CrSel";
        case Key::ExSel:            return "ExSel";
        case Key::EraseEOF:         return "EraseEOF";
        case Key::Play:             return "Play";
        case Key::Zoom:             return "Zoom";
        case Key::PA1:              return "PA1";
        case Key::OEMClear:         return "OEMClear";
        default:                    return "UNKNOWN";
    }
}