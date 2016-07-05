#include "stdafx.h"
#include "Engine.h"
#include "KeyboardEvent.h"
#include "MouseEvent.h"

namespace GlEngine
{
    LRESULT CALLBACK Window::WndProc(unsigned message, WPARAM wParam, LPARAM lParam)
    {
        auto &events = Engine::GetInstance().GetEventQueue();
        unsigned vkCode;
        switch (message)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT)
            {
                SetCursor(nullptr);
                break;
            }
            goto default_wnd_proc;

        case WM_KEYDOWN:
            vkCode = (unsigned)wParam;
            if (vkCode == VK_F11)
            {
                SetFullscreen(!GetFullscreen());
                break;
            }
            if ((lParam & (1 << 30)) == 0) events.PushEvent(new Events::KeyboardEvent(vkCode, Events::KeyboardEventType::KeyPressed));
            events.PushEvent(new Events::KeyboardEvent(vkCode, Events::KeyboardEventType::KeyTyped));
            break;

        case WM_KEYUP:
            vkCode = (unsigned)wParam;
            events.PushEvent(new Events::KeyboardEvent(vkCode, Events::KeyboardEventType::KeyReleased));
            break;

        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            {
                auto mousePos = Vector<2>(LOWORD(lParam), HIWORD(lParam));
                bool shift = (wParam & MK_SHIFT) > 0,
                     control = (wParam & MK_CONTROL) > 0,
                     alt = (GetKeyState(VK_MENU) & ~1) > 0;
                Events::MouseButton btn = Events::MouseButton::None;
                if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP) btn |= Events::MouseButton::Left;
                if (message == WM_MBUTTONDOWN || message == WM_MBUTTONUP) btn |= Events::MouseButton::Middle;
                if (message == WM_RBUTTONDOWN || message == WM_RBUTTONUP) btn |= Events::MouseButton::Right;
                if (message == WM_XBUTTONDOWN || message == WM_XBUTTONUP) btn |= (HIWORD(wParam) == 0x0001) ? Events::MouseButton::X1 : Events::MouseButton::X2;
                auto type = (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN) ? Events::MouseEventType::Pressed :
                                                                                       (message == WM_MOUSEMOVE)  ? Events::MouseEventType::Moved :
                                                                                                                    Events::MouseEventType::Released;
                events.PushEvent(new Events::MouseEvent(mousePos, control, shift, alt, type, btn));
            }
            break;

        //case WM_MOUSEWHEEL:

        //    break;

        default_wnd_proc:
        default:
            return DefWindowProc(_windowHandle, message, wParam, lParam);
        }
        return 0;
    }
}
