/*
  Copyright (C) 2022 Basov Artyom
  The authors can be contacted at <artembasov@outlook.com>
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
	 notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
	 notice, this list of conditions and the following disclaimer in
	 the documentation and/or other materials provided with the
	 distribution.
  3. The names of the authors may not be used to endorse or promote
	 products derived from this software without specific prior
	 written permission.
  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "miGUI.h"

#include "framework/mgf.h"
#include "framework/Framework.h"
#include "framework/SystemWindowImpl.h"
#include "framework/Context.h"

#include <wchar.h>

using namespace mgf;

#ifdef MG_PLATFORM_WINDOWS
HRAWINPUT g_rawInputData[0xff];
HKL KEYBOARD_INPUT_HKL = 0;
int KEYBOARD_INPUT_CODEPAGE = 0;
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
static unsigned int LocaleIdToCodepage(unsigned int lcid);
#endif

extern Framework* g_mgf;


SystemWindowImpl::SystemWindowImpl(int windowFlags, const mgPoint& windowPosition, const mgPoint& windowSize)
{
#ifdef MG_PLATFORM_WINDOWS
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(0);
    wcex.hIcon = 0;// LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_WIN32TEST));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = 0;

    static int windowCount = 0;
    wsprintf(m_className, L"w%i", windowCount);

    wcex.lpszClassName = m_className;
    wcex.hIconSm = 0;// LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    RegisterClassExW(&wcex);

    m_hWnd = CreateWindowExW(
        WS_EX_ACCEPTFILES,
        m_className, 
        L"mgf", 
        windowFlags,
        windowPosition.x,
        windowPosition.y,
        windowSize.x,
        windowSize.y,
        0, 
        0, 
        wcex.hInstance, 
        this
    );
    
    m_OSData.handle = m_hWnd;

    KEYBOARD_INPUT_HKL = GetKeyboardLayout(0);
    KEYBOARD_INPUT_CODEPAGE = LocaleIdToCodepage(LOWORD(KEYBOARD_INPUT_HKL));

    UINT dpi = GetDpiForWindow(m_hWnd);
    int padding = GetSystemMetrics(SM_CXPADDEDBORDER);
    m_borderSize.x = GetSystemMetrics(SM_CXFRAME) + padding;
    m_borderSize.y = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + padding);

    RAWINPUTDEVICE device;
    device.usUsagePage = 0x01;
    device.usUsage = 0x02;
    device.dwFlags = 0;
    device.hwndTarget = 0;
    RegisterRawInputDevices(&device, 1, sizeof device);
#endif
}

SystemWindowImpl::~SystemWindowImpl()
{
#ifdef MG_PLATFORM_WINDOWS
    if (m_hWnd)
    {
        if (m_hWnd)
        {
            if (m_hdcMem)
                DeleteDC(m_hdcMem);
            if (m_hbmMem)
                DeleteObject(m_hbmMem);

            //ReleaseDC(m_hWnd, m_dc);
            DestroyWindow(m_hWnd);
        }
        UnregisterClass(m_className, GetModuleHandle(0));
    }
#endif
}

const SystemWindowOSData& SystemWindowImpl::GetOSData()
{
    return m_OSData;
}

void SystemWindowImpl::Show()
{
#ifdef MG_PLATFORM_WINDOWS
    ShowWindow(m_hWnd, SW_SHOW);
#endif
}

void SystemWindowImpl::Hide()
{
#ifdef MG_PLATFORM_WINDOWS
    ShowWindow(m_hWnd, SW_HIDE);
#endif
}

void SystemWindowImpl::SetTitle(const wchar_t* t)
{
#ifdef MG_PLATFORM_WINDOWS
    SetWindowTextW(m_hWnd, t);
#endif
}

const mgPoint& SystemWindowImpl::GetSize()
{
    return m_size;
}

void SystemWindowImpl::UpdateBackbuffer()
{
    m_context->m_backend->UpdateBackbuffer();
}

void SystemWindowImpl::SetOnClose(SystemWindowOnClose c)
{
    m_onClose = c;
}

void SystemWindowImpl::SetOnSize(SystemWindowOnSize c)
{
    m_onSize = c;
}

void SystemWindowImpl::SetOnDropFiles(SystemWindowOnDropFiles c)
{
    m_onDropFiles = c;
}

void SystemWindowImpl::OnSize()
{
    RECT rc;
    GetClientRect(m_hWnd, &rc);
    m_size.x = rc.right - rc.left;
    m_size.y = rc.bottom - rc.top;
    if (m_onSize)
        m_onSize(this);

    UpdateBackbuffer();
    m_context->OnWindowSize();
}

bool SystemWindowImpl::IsVisible()
{
    return m_isVisible;
}

#ifdef MG_PLATFORM_WINDOWS
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    SystemWindowImpl* pW = 0;
    if (message == WM_NCCREATE)
    {
        pW = static_cast<SystemWindowImpl*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
        SetLastError(0);
        if (!SetWindowLongPtr(hWnd, -21, reinterpret_cast<LONG_PTR>(pW)))
        {
            if (GetLastError() != 0)
                return FALSE;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    else
        pW = reinterpret_cast<SystemWindowImpl*>(GetWindowLongPtr(hWnd, -21));

    switch (message)
    {
    case WM_DROPFILES:
    {
        if (pW->m_onDropFiles)
        {
            //printf("DROP FILES\n");
            HDROP hDrop = (HDROP)wParam;
            if (hDrop)
            {
                UINT num = DragQueryFileW(hDrop, 0xFFFFFFFF, 0, 0);
                //printf("%u FILES\n", num);
                
                wchar_t* buf = new wchar_t[0xffff];
                for (UINT i = 0; i < num; ++i)
                {
                    DragQueryFileW(hDrop, i, buf, 0xffff);
                    buf[0xffff - 1] = 0;

                    POINT pt;
                    DragQueryPoint(hDrop, &pt);

                    pW->m_onDropFiles(num, i, buf, pt.x, pt.y);
                    //wprintf(L"FILE: %s\n", buf);
                }
                delete[] buf;

                DragFinish(hDrop);
            }
        }

        return 0;
    }break;
    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = 800;
        lpMMI->ptMinTrackSize.y = 600;
    }
    break;
    case WM_SIZE:
    {
        if (pW)
        {
            pW->OnSize();
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case SIZE_MINIMIZED:
                pW->m_isVisible = false;
                break;
            case SIZE_RESTORED:
            case SIZE_MAXIMIZED:
                pW->m_isVisible = true;
                break;
            }
        }
    }return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_COMMAND:
    {
        /*int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }*/
    }
    break;

    case WM_ERASEBKGND:
    case WM_ACTIVATE:
    case WM_PAINT:
    {
        if (pW)
            pW->m_context->DrawAll();
        /*draw_gui();*/
        //if (pW)
        //{
        //    PAINTSTRUCT ps;
        //    HDC hdc = BeginPaint(hWnd, &ps);
        //    ////// TODO: Add any drawing code that uses hdc here...
        //    //pW->m_context->

        //    if (pW->m_context->m_gui_context)
        //    {
        //        pW->m_context->m_gui_context->gpu->beginDraw();
        //        mgDraw(pW->m_context->m_gui_context);
        //        pW->m_context->m_gui_context->gpu->endDraw();
        //    }

        //    EndPaint(hWnd, &ps);
        //}
    }break;
    case WM_INPUT:
    {
        HRAWINPUT hRawInput = (HRAWINPUT)lParam;
        UINT dataSize;
        GetRawInputData(hRawInput, RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

        if (dataSize == 0 || dataSize > 0xff)
            break;

        void* dataBuf = &g_rawInputData[0];
        GetRawInputData(hRawInput, RID_INPUT, dataBuf, &dataSize, sizeof(RAWINPUTHEADER));

        const RAWINPUT* raw = (const RAWINPUT*)dataBuf;
        if (raw->header.dwType == RIM_TYPEMOUSE)
        {
            HANDLE deviceHandle = raw->header.hDevice;

            const RAWMOUSE& mouseData = raw->data.mouse;

            USHORT flags = mouseData.usButtonFlags;
            short wheelDelta = (short)mouseData.usButtonData;
            LONG x = mouseData.lLastX, y = mouseData.lLastY;

            /*wprintf(
                L"Mouse: Device=0x%08X, Flags=%04x, WheelDelta=%d, X=%d, Y=%d\n",
                deviceHandle, flags, wheelDelta, x, y);*/

            if (wheelDelta)
                pW->m_context->m_input->mouseWheelDelta = (float)wheelDelta / (float)WHEEL_DELTA;

            POINT cursorPoint;
            GetCursorPos(&cursorPoint);
            ScreenToClient(hWnd, &cursorPoint);
            pW->m_context->m_input->mousePosition.x = cursorPoint.x;
            pW->m_context->m_input->mousePosition.y = cursorPoint.y;

            if (flags)
            {
                if ((flags & 0x1) == 0x1)
                {
                    pW->m_context->m_input->mouseButtonFlags1 |= MG_MBFL_LMBDOWN;
                    pW->m_context->m_input->mouseButtonFlags2 |= MG_MBFL_LMBHOLD;
                }

                if ((flags & 0x2) == 0x2)
                {
                    pW->m_context->m_input->mouseButtonFlags1 |= MG_MBFL_LMBUP;
                    if ((pW->m_context->m_input->mouseButtonFlags2 & MG_MBFL_LMBHOLD) == MG_MBFL_LMBHOLD)
                        pW->m_context->m_input->mouseButtonFlags2 ^= MG_MBFL_LMBHOLD;
                }

                if ((flags & 0x4) == 0x4)
                {
                    pW->m_context->m_input->mouseButtonFlags1 |= MG_MBFL_RMBDOWN;
                    pW->m_context->m_input->mouseButtonFlags2 |= MG_MBFL_RMBHOLD;
                }
                if ((flags & 0x8) == 0x8)
                {
                    pW->m_context->m_input->mouseButtonFlags1 |= MG_MBFL_RMBUP;
                    if ((pW->m_context->m_input->mouseButtonFlags2 & MG_MBFL_RMBHOLD) == MG_MBFL_RMBHOLD)
                        pW->m_context->m_input->mouseButtonFlags2 ^= MG_MBFL_RMBHOLD;
                }

                if ((flags & 0x10) == 0x10)
                {
                    pW->m_context->m_input->mouseButtonFlags1 |= MG_MBFL_MMBDOWN;
                    pW->m_context->m_input->mouseButtonFlags2 |= MG_MBFL_MMBHOLD;
                }
                if ((flags & 0x20) == 0x20)
                {
                    pW->m_context->m_input->mouseButtonFlags1 |= MG_MBFL_MMBUP;
                    if ((pW->m_context->m_input->mouseButtonFlags2 & MG_MBFL_MMBHOLD) == MG_MBFL_MMBHOLD)
                        pW->m_context->m_input->mouseButtonFlags2 ^= MG_MBFL_MMBHOLD;
                }

                if ((flags & 0x100) == 0x100)
                {
                    pW->m_context->m_input->mouseButtonFlags1 |= MG_MBFL_X1MBDOWN;
                    pW->m_context->m_input->mouseButtonFlags2 |= MG_MBFL_X1MBHOLD;
                }
                if ((flags & 0x200) == 0x200)
                {
                    pW->m_context->m_input->mouseButtonFlags1 |= MG_MBFL_X1MBUP;
                    if ((pW->m_context->m_input->mouseButtonFlags2 & MG_MBFL_X1MBHOLD) == MG_MBFL_X1MBHOLD)
                        pW->m_context->m_input->mouseButtonFlags2 ^= MG_MBFL_X1MBHOLD;
                }

                if ((flags & 0x40) == 0x40)
                {
                    pW->m_context->m_input->mouseButtonFlags1 |= MG_MBFL_X2MBDOWN;
                    pW->m_context->m_input->mouseButtonFlags2 |= MG_MBFL_X2MBHOLD;
                }
                if ((flags & 0x80) == 0x80)
                {
                    pW->m_context->m_input->mouseButtonFlags1 |= MG_MBFL_X2MBUP;
                    if ((pW->m_context->m_input->mouseButtonFlags2 & MG_MBFL_X2MBHOLD) == MG_MBFL_X2MBHOLD)
                        pW->m_context->m_input->mouseButtonFlags2 ^= MG_MBFL_X2MBHOLD;
                }

            }

            /*if (pW->m_context->m_gui_context)
            {
                pW->m_context->m_gui_context->gpu->beginDraw();
                mgDraw(pW->m_context->m_gui_context);
                pW->m_context->m_gui_context->gpu->endDraw();
            }*/
        }
        //if (pW->m_context->m_gui_context)
        //{
        //    mgUpdate(pW->m_context->m_gui_context);
        //    //printf("u");
        //}
    }break;
   case WM_CLOSE:
       if (pW)
       {
           if (pW->m_onClose)
           {
               if (pW->m_onClose(pW))
               {
                   g_mgf->m_run = false;
                   PostQuitMessage(0);
               }
           }
           else
           {
               g_mgf->m_run = false;
               PostQuitMessage(0);
           }
       }
       else
       {
           g_mgf->m_run = false;
           PostQuitMessage(0);
       }
       return 0;
    case WM_SETCURSOR: {
        auto id = LOWORD(lParam);
        switch (id)
        {
        default:
            mgSetCursor(pW->m_context->m_gui_context, pW->m_context->m_gui_context->currentCursors[mgCursorType_Arrow], mgCursorType_Arrow);
            return TRUE;
        case HTLEFT:
            mgSetCursor(pW->m_context->m_gui_context, pW->m_context->m_gui_context->currentCursors[mgCursorType_SizeWE], mgCursorType_SizeWE);
            return TRUE;
        case HTRIGHT:
            mgSetCursor(pW->m_context->m_gui_context, pW->m_context->m_gui_context->currentCursors[mgCursorType_SizeWE], mgCursorType_SizeWE);
            return TRUE;
        case HTTOP:
            mgSetCursor(pW->m_context->m_gui_context, pW->m_context->m_gui_context->currentCursors[mgCursorType_SizeNS], mgCursorType_SizeNS);
            return TRUE;
        case HTBOTTOM:
            mgSetCursor(pW->m_context->m_gui_context, pW->m_context->m_gui_context->currentCursors[mgCursorType_SizeNS], mgCursorType_SizeNS);
            return TRUE;
        case HTTOPLEFT:
            mgSetCursor(pW->m_context->m_gui_context, pW->m_context->m_gui_context->currentCursors[mgCursorType_SizeNWSE], mgCursorType_SizeNWSE);
            return TRUE;
        case HTBOTTOMRIGHT:
            mgSetCursor(pW->m_context->m_gui_context, pW->m_context->m_gui_context->currentCursors[mgCursorType_SizeNWSE], mgCursorType_SizeNWSE);
            return TRUE;
        case HTBOTTOMLEFT:
            mgSetCursor(pW->m_context->m_gui_context, pW->m_context->m_gui_context->currentCursors[mgCursorType_SizeNESW], mgCursorType_SizeNESW);
            return TRUE;
        case HTTOPRIGHT:
            mgSetCursor(pW->m_context->m_gui_context, pW->m_context->m_gui_context->currentCursors[mgCursorType_SizeNESW], mgCursorType_SizeNESW);
            return TRUE;
        case HTHELP:
            mgSetCursor(pW->m_context->m_gui_context, pW->m_context->m_gui_context->currentCursors[mgCursorType_Help], mgCursorType_Help);
            return TRUE;
        }
    }break;
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        bool isPress = true;

        unsigned char key = (unsigned char)wParam;

        if (message == WM_SYSKEYUP) isPress = false;
        if (message == WM_KEYUP) isPress = false;

        const UINT MY_MAPVK_VSC_TO_VK_EX = 3;

        if (key == MG_KEY_SHIFT)
        { // shift -> lshift rshift
            key = static_cast<unsigned char>(MapVirtualKey((static_cast<UINT>(lParam >> 16) & 255u), MY_MAPVK_VSC_TO_VK_EX));
        }
        if (key == MG_KEY_CTRL)
        { // ctrl -> lctrl rctrl
            key = static_cast<unsigned char>(MapVirtualKey((static_cast<UINT>(lParam >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX));
            if (lParam & 0x1000000)
                key = static_cast<unsigned char>(163);
        }

        if (key == MG_KEY_ALT)
        { // alt -> lalt ralt
            key = static_cast<unsigned char>(MapVirtualKey((static_cast<UINT>(lParam >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX));
            if (lParam & 0x1000000)
                key = static_cast<unsigned char>(165);
            //printf("alt = %i\n",(int)ev.keyboardEvent.key);
        }

        static unsigned char keys[256u];
        GetKeyboardState(keys);
        WORD chars[2];

        if (ToAsciiEx((UINT)wParam, HIWORD(lParam), keys, chars, 0, KEYBOARD_INPUT_HKL) == 1)
        {
            MultiByteToWideChar(KEYBOARD_INPUT_CODEPAGE, MB_PRECOMPOSED, (LPCSTR)chars,
                sizeof(chars), (WCHAR*)&pW->m_context->m_input->character, 1);
        }

        if (isPress)
        {
            if (key < 256)
            {
                pW->m_context->m_input->keyFlags[key] |= MG_KEYFL_HOLD;
                pW->m_context->m_input->keyFlags[key] |= MG_KEYFL_HIT;
            }
        }
        else
        {
            if (key < 256)
            {
                if ((pW->m_context->m_input->keyFlags[key] & MG_KEYFL_HOLD) == MG_KEYFL_HOLD)
                    pW->m_context->m_input->keyFlags[key] ^= MG_KEYFL_HOLD;

                pW->m_context->m_input->keyFlags[key] |= MG_KEYFL_RELEASE;
            }
        }

        if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP)
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        else
        {
            return 0;
        }
    }break;

    case WM_INPUTLANGCHANGE:
        KEYBOARD_INPUT_HKL = GetKeyboardLayout(0);
        KEYBOARD_INPUT_CODEPAGE = LocaleIdToCodepage(LOWORD(KEYBOARD_INPUT_HKL));
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_SCREENSAVE ||
            (wParam & 0xFFF0) == SC_MONITORPOWER ||
            (wParam & 0xFFF0) == SC_KEYMENU
            )
        {
            return 0;
        }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// IrrLicht
// Get the codepage from the locale language id
// Based on the table from http://www.science.co.il/Language/Locale-Codes.asp?s=decimal
static unsigned int LocaleIdToCodepage(unsigned int lcid)
{
    switch (lcid)
    {
    case 1098:  // Telugu
    case 1095:  // Gujarati
    case 1094:  // Punjabi
    case 1103:  // Sanskrit
    case 1111:  // Konkani
    case 1114:  // Syriac
    case 1099:  // Kannada
    case 1102:  // Marathi
    case 1125:  // Divehi
    case 1067:  // Armenian
    case 1081:  // Hindi
    case 1079:  // Georgian
    case 1097:  // Tamil
        return 0;
    case 1054:  // Thai
        return 874;
    case 1041:  // Japanese
        return 932;
    case 2052:  // Chinese (PRC)
    case 4100:  // Chinese (Singapore)
        return 936;
    case 1042:  // Korean
        return 949;
    case 5124:  // Chinese (Macau S.A.R.)
    case 3076:  // Chinese (Hong Kong S.A.R.)
    case 1028:  // Chinese (Taiwan)
        return 950;
    case 1048:  // Romanian
    case 1060:  // Slovenian
    case 1038:  // Hungarian
    case 1051:  // Slovak
    case 1045:  // Polish
    case 1052:  // Albanian
    case 2074:  // Serbian (Latin)
    case 1050:  // Croatian
    case 1029:  // Czech
        return 1250;
    case 1104:  // Mongolian (Cyrillic)
    case 1071:  // FYRO Macedonian
    case 2115:  // Uzbek (Cyrillic)
    case 1058:  // Ukrainian
    case 2092:  // Azeri (Cyrillic)
    case 1092:  // Tatar
    case 1087:  // Kazakh
    case 1059:  // Belarusian
    case 1088:  // Kyrgyz (Cyrillic)
    case 1026:  // Bulgarian
    case 3098:  // Serbian (Cyrillic)
    case 1049:  // Russian
        return 1251;
    case 8201:  // English (Jamaica)
    case 3084:  // French (Canada)
    case 1036:  // French (France)
    case 5132:  // French (Luxembourg)
    case 5129:  // English (New Zealand)
    case 6153:  // English (Ireland)
    case 1043:  // Dutch (Netherlands)
    case 9225:  // English (Caribbean)
    case 4108:  // French (Switzerland)
    case 4105:  // English (Canada)
    case 1110:  // Galician
    case 10249:  // English (Belize)
    case 3079:  // German (Austria)
    case 6156:  // French (Monaco)
    case 12297:  // English (Zimbabwe)
    case 1069:  // Basque
    case 2067:  // Dutch (Belgium)
    case 2060:  // French (Belgium)
    case 1035:  // Finnish
    case 1080:  // Faroese
    case 1031:  // German (Germany)
    case 3081:  // English (Australia)
    case 1033:  // English (United States)
    case 2057:  // English (United Kingdom)
    case 1027:  // Catalan
    case 11273:  // English (Trinidad)
    case 7177:  // English (South Africa)
    case 1030:  // Danish
    case 13321:  // English (Philippines)
    case 15370:  // Spanish (Paraguay)
    case 9226:  // Spanish (Colombia)
    case 5130:  // Spanish (Costa Rica)
    case 7178:  // Spanish (Dominican Republic)
    case 12298:  // Spanish (Ecuador)
    case 17418:  // Spanish (El Salvador)
    case 4106:  // Spanish (Guatemala)
    case 18442:  // Spanish (Honduras)
    case 3082:  // Spanish (International Sort)
    case 13322:  // Spanish (Chile)
    case 19466:  // Spanish (Nicaragua)
    case 2058:  // Spanish (Mexico)
    case 10250:  // Spanish (Peru)
    case 20490:  // Spanish (Puerto Rico)
    case 1034:  // Spanish (Traditional Sort)
    case 14346:  // Spanish (Uruguay)
    case 8202:  // Spanish (Venezuela)
    case 1089:  // Swahili
    case 1053:  // Swedish
    case 2077:  // Swedish (Finland)
    case 5127:  // German (Liechtenstein)
    case 1078:  // Afrikaans
    case 6154:  // Spanish (Panama)
    case 4103:  // German (Luxembourg)
    case 16394:  // Spanish (Bolivia)
    case 2055:  // German (Switzerland)
    case 1039:  // Icelandic
    case 1057:  // Indonesian
    case 1040:  // Italian (Italy)
    case 2064:  // Italian (Switzerland)
    case 2068:  // Norwegian (Nynorsk)
    case 11274:  // Spanish (Argentina)
    case 1046:  // Portuguese (Brazil)
    case 1044:  // Norwegian (Bokmal)
    case 1086:  // Malay (Malaysia)
    case 2110:  // Malay (Brunei Darussalam)
    case 2070:  // Portuguese (Portugal)
        return 1252;
    case 1032:  // Greek
        return 1253;
    case 1091:  // Uzbek (Latin)
    case 1068:  // Azeri (Latin)
    case 1055:  // Turkish
        return 1254;
    case 1037:  // Hebrew
        return 1255;
    case 5121:  // Arabic (Algeria)
    case 15361:  // Arabic (Bahrain)
    case 9217:  // Arabic (Yemen)
    case 3073:  // Arabic (Egypt)
    case 2049:  // Arabic (Iraq)
    case 11265:  // Arabic (Jordan)
    case 13313:  // Arabic (Kuwait)
    case 12289:  // Arabic (Lebanon)
    case 4097:  // Arabic (Libya)
    case 6145:  // Arabic (Morocco)
    case 8193:  // Arabic (Oman)
    case 16385:  // Arabic (Qatar)
    case 1025:  // Arabic (Saudi Arabia)
    case 10241:  // Arabic (Syria)
    case 14337:  // Arabic (U.A.E.)
    case 1065:  // Farsi
    case 1056:  // Urdu
    case 7169:  // Arabic (Tunisia)
        return 1256;
    case 1061:  // Estonian
    case 1062:  // Latvian
    case 1063:  // Lithuanian
        return 1257;
    case 1066:  // Vietnamese
        return 1258;
    }
    return 65001;   // utf-8
}
#endif

