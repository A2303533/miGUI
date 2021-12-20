// win32_test.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "win32_test.h"

#include "miGUI.h"
#include "miGUILoader.h"

#include <time.h>

mgContext* g_gui_context = 0;
mgInputContext g_input;
HRAWINPUT g_rawInputData[0xff];

mgFont* g_win32font = 0;

HDC g_dc = 0;
HWND g_hwnd = 0;
RECT g_windowRect;
HKL KEYBOARD_INPUT_HKL = 0;
unsigned int KEYBOARD_INPUT_CODEPAGE = 0;

mgPoint borderSize;

/*double buffering*/
HDC hdcMem = 0;
HBITMAP hbmMem = 0, hbmOld = 0;
void DeleteBackBuffer() {
    if (hdcMem)
        DeleteDC(hdcMem);
    if (hbmMem)
        DeleteObject(hbmMem);
}
void UpdateBackBuffer() {
    DeleteBackBuffer();
    g_dc = GetWindowDC(g_hwnd);
    hdcMem = CreateCompatibleDC(g_dc);

    GetWindowRect(g_hwnd, &g_windowRect);
    hbmMem = CreateCompatibleBitmap(g_dc,
        g_windowRect.right - g_windowRect.left,
        g_windowRect.bottom - g_windowRect.top);
    ReleaseDC(g_hwnd, g_dc);
}


void gui_beginDraw()
{
    hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
    RECT r;
    r.left = 0;
    r.top = 0;
    r.right = g_windowRect.right - g_windowRect.left;
    r.bottom =  g_windowRect.bottom - g_windowRect.top;
    FillRect(hdcMem, &r, (HBRUSH)(COLOR_WINDOW + 1));

    UINT dpi = GetDpiForWindow(g_hwnd);
    int padding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
    borderSize.x = GetSystemMetricsForDpi(SM_CXFRAME, dpi) + padding;
    borderSize.y = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + padding);
}

void gui_endDraw()
{
    g_dc = GetWindowDC(g_hwnd);
    BitBlt(g_dc,
        0, 0,
        g_windowRect.right - g_windowRect.left, g_windowRect.bottom - g_windowRect.top,
        hdcMem,
        0, 0,
        SRCCOPY);
    ReleaseDC(g_hwnd, g_dc);
}

mgTexture gui_createTexture(mgImage* img)
{
    return 0;
}

void gui_destroyTexture(mgTexture t)
{

}

mgRect gui_setClipRect(mgRect* r)
{
    static mgRect old;

    mgRect ret = old;
    old = *r;
    return ret;
}

void gui_drawRectangle(mgElement* element,mgPoint* position,mgPoint* size,mgColor* color1,
    mgColor* color2,mgTexture texture,mgVec4* UVRegion)
{

    RECT r;
    r.left = position->x + borderSize.x;
    r.top = position->y + borderSize.y;
    r.right = r.left + size->x;
    r.bottom = r.top + size->y;

    unsigned int c1 = mgColorGetAsIntegerARGB(color1);
    unsigned int c2 = mgColorGetAsIntegerARGB(color2);

    HBRUSH brsh = CreateSolidBrush(c1);
    SelectObject(hdcMem, brsh);

    /*RoundRect(hdcMem, r.left, r.top, r.right, r.bottom, 6, 6);*/
    /*or with clip region*/
    
    HRGN rgn = CreateRoundRectRgn(r.left, r.top, r.right, r.bottom, 7, 7);
    SelectClipRgn(hdcMem, rgn);

    /*FillRect(hdcMem, &r, brsh);*/
    /*or gradient*/
    {
        TRIVERTEX vertex[2];
        vertex[0].x = r.left;
        vertex[0].y = r.top;
        vertex[0].Red = (c1 & 0x000000ff) << 8;
        vertex[0].Green = (c1 & 0x0000ff00);
        vertex[0].Blue = (c1 & 0x00ff0000) >> 8;
        vertex[0].Alpha = (c1 & 0xff000000) >> 16;

        vertex[1].x = r.right;
        vertex[1].y = r.bottom;
        vertex[1].Red = (c2 & 0x000000ff) << 8;
        vertex[1].Green = (c2 & 0x0000ff00);
        vertex[1].Blue = (c2 & 0x00ff0000) >> 8;
        vertex[1].Alpha = (c2 & 0xff000000) >> 16;

        GRADIENT_RECT gRect;
        gRect.UpperLeft = 0;
        gRect.LowerRight = 1;
        GradientFill(hdcMem, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
    }

    DeleteObject(rgn);
    DeleteObject(brsh);
    SelectClipRgn(hdcMem, 0);
}

void gui_drawText(
    mgPoint* position,
    const wchar_t* text,
    int textLen,
    mgColor* color,
    mgFont* font)
{
    SelectObject(hdcMem, font->implementation);
    SetTextColor(hdcMem, mgColorGetAsIntegerRGB(color));
    SetBkMode(hdcMem, TRANSPARENT);
    TextOutW(hdcMem, position->x + borderSize.x, position->y + borderSize.y, text, textLen);
}

void gui_getTextSize(const wchar_t* text, mgFont* font, mgPoint* sz)
{
    SelectObject(hdcMem, font->implementation);
    int c = wcslen(text);
    if (!c)
        return;
    SIZE s;
    GetTextExtentPoint32W(hdcMem, text, c, &s);
    sz->x = s.cx;
    sz->y = s.cy;
}

mgFont* gui_createFont(const char* fn, unsigned int flags, int size)
{
    mgFont* f = (mgFont*)malloc(sizeof(mgFont));
    
    f->implementation = CreateFontA(
        size, 0, 0, 0,
        (flags & MG_FNTFL_BOLD) ? FW_BOLD : FW_NORMAL,
        (flags & MG_FNTFL_ITALIC) ? 1 : 0,
        (flags & MG_FNTFL_UNDERLINE) ? 1 : 0,
        (flags & MG_FNTFL_STRIKEOUT) ? 1 : 0,
        DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        VARIABLE_PITCH,
        fn);
   /* f->implementation = CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));*/
    return f;
}

void draw_gui()
{
    if (!g_gui_context)
        return;

    g_gui_context->gpu->beginDraw();
    mgPoint point;
    mgPointSet(&point, 0, 0);

    mgPoint size;
    mgPointSet(&size, 220, 180);

    mgColor color1;
    mgColor color2;
    mgColorSetAsIntegerRGB(&color1, 0xFFFF9999);
    mgColorSetAsIntegerRGB(&color2, 0xFF9999FF);

    g_gui_context->gpu->drawRectangle(0, &point, &size, &color1, &color2, 0, 0);

    mgPoint textPosition;
    mgPointSet(&textPosition, 10, 10);
    mgColor textColor;
    mgColorSetAsIntegerRGB(&textColor, 0xFF005000);
    wchar_t textBuffer[200];
    
    swprintf_s(textBuffer, L"mousePosition: %i %i", g_input.mousePosition.x, g_input.mousePosition.y);
    mgPointSet(&textPosition, 10, 10);
    gui_drawText(&textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);
    
    swprintf_s(textBuffer, L"mouseMoveDelta: %i %i", g_input.mouseMoveDelta.x, g_input.mouseMoveDelta.y);
    mgPointSet(&textPosition, 10, 20);
    gui_drawText(&textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);
    
    swprintf_s(textBuffer, L"mouseWheelDelta: %.1f", g_input.mouseWheelDelta);
    mgPointSet(&textPosition, 10, 30);
    gui_drawText(&textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);
    
    swprintf_s(textBuffer, L"mouseButtons: 0x%x 0x%x", g_input.mouseButtonFlags1, g_input.mouseButtonFlags2);
    mgPointSet(&textPosition, 10, 40);
    gui_drawText(&textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);

    swprintf_s(textBuffer, L"key char: %c 0x%x", g_input.character, g_input.character);
    mgPointSet(&textPosition, 10, 50);
    gui_drawText(&textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);

    if (mgIsKeyHit(&g_input, MG_KEY_ENTER))
    {
        swprintf_s(textBuffer, L"Hit ENTER");
        mgPointSet(&textPosition, 10, 60);
        gui_drawText(&textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);
    }

    if (mgIsKeyHold(&g_input, MG_KEY_ENTER))
    {
        swprintf_s(textBuffer, L"Hold ENTER");
        mgPointSet(&textPosition, 10, 70);
        gui_drawText(&textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);
    }

    if (mgIsKeyRelease(&g_input, MG_KEY_ENTER))
    {
        swprintf_s(textBuffer, L"Release ENTER");
        mgPointSet(&textPosition, 10, 80);
        gui_drawText(&textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);
    }

    swprintf_s(textBuffer, L"KB modifier 0x%x\n", g_input.keyboardModifier);
    mgPointSet(&textPosition, 10, 90);
    gui_drawText(&textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);

    /*draw elements*/
    mgDraw(g_gui_context);

    g_gui_context->gpu->endDraw();
}


void rect_onMouseEnter(struct mgElement_s* e){
    mgElementText* text = (mgElementText*)e->userData;
    text->text = L"Mouse enter";
    text->textLen = wcslen(text->text);
    mgSetCursor(g_gui_context, g_gui_context->defaultCursors[mgCursorType_Hand], mgCursorType_Arrow);
}
void rect_onMouseLeave(struct mgElement_s* e){
    mgElementText* text = (mgElementText*)e->userData;
    text->text = L"Mouse leave";
    text->textLen = wcslen(text->text);
    mgSetCursor(g_gui_context, g_gui_context->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);
}
void rect_onClickLMB(struct mgElement_s* e) {
    mgElementText* text = (mgElementText*)e->userData;
    text->color.setAsIntegerBGR(0xFF0000);
}
void rect_onReleaseLMB(struct mgElement_s* e) {
    mgElementText* text = (mgElementText*)e->userData;
    text->color.setAsIntegerBGR(0xFFFFFF);
}

static unsigned int LocaleIdToCodepage(unsigned int lcid);

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32TEST));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = L"migui";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    RegisterClassExW(&wcex);

    g_hwnd = CreateWindowExW(0, wcex.lpszClassName, L"migui", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 0, 0, hInstance, 0);
    if (!g_hwnd)
        return FALSE;
    
    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);

    RAWINPUTDEVICE device;
    device.usUsagePage = 0x01;
    device.usUsage = 0x02;
    device.dwFlags = 0;
    device.hwndTarget = 0;
    RegisterRawInputDevices(&device, 1, sizeof device);


    MG_LIB_HANDLE gui_lib = mgLoad();
    if (!gui_lib)
    {
        MessageBoxA(0, "Can't load migui.dll", "Error", MB_OK);
        return 0;
    }

    mgVideoDriverAPI gui_gpu;
    gui_gpu.createTexture = gui_createTexture;
    gui_gpu.destroyTexture = gui_destroyTexture;
    gui_gpu.beginDraw = gui_beginDraw;
    gui_gpu.endDraw = gui_endDraw;
    gui_gpu.drawRectangle = gui_drawRectangle;
    gui_gpu.drawText = gui_drawText;
    gui_gpu.setClipRect = gui_setClipRect;

    memset(&g_input, 0, sizeof(g_input));
    g_gui_context = mgCreateContext(&gui_gpu, &g_input);
    g_gui_context->getTextSize = gui_getTextSize;
    
    g_win32font = gui_createFont("Segoe", MG_FNTFL_BOLD, 14);
    {
        mgPoint pos, sz;
        mgColor c1, c2;
        pos.x = 300;
        pos.y = 0;
        sz.x = 100;
        sz.y = 20;
        c1.setAsIntegerRGB(0xFF0000);
        c2.setAsIntegerRGB(0x0000FF);
        mgElement* rectangle = mgCreateRectangle(g_gui_context, &pos, &sz, &c1, &c2);
        mgElement* text = mgCreateText(g_gui_context, &pos, L"Text", g_win32font);
        ((mgElementText*)text->implementation)->color.setAsIntegerBGR(0xFFFFFF);
        rectangle->userData = text->implementation;
        rectangle->onMouseEnter = rect_onMouseEnter;
        rectangle->onMouseLeave = rect_onMouseLeave;
        rectangle->onClickLMB = rect_onClickLMB;
        rectangle->onReleaseLMB = rect_onReleaseLMB;
     
        pos.y += 30;
        mgElement* button = mgCreateButton(g_gui_context, &pos, &sz, L"Button", g_win32font);
    }

    UpdateBackBuffer();
    MSG msg;
    int run = 1;
    
    Sleep(100);
    while (run)
    {
        mgStartFrame(g_gui_context);

        run = GetMessage(&msg, 0, 0, 0);
        if (!TranslateAccelerator(msg.hwnd, 0, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        mgUpdate(g_gui_context);
        draw_gui();
    }
    
    DeleteObject(g_win32font->implementation);
    free(g_win32font);

    DeleteBackBuffer();

    mgDestroyContext(g_gui_context);
    mgUnload(gui_lib);

    return (int) msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_SIZE:
    {
        UpdateBackBuffer();
        if(g_gui_context)
            g_gui_context->needRebuild = 1;
    }return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_ERASEBKGND:
    case WM_ACTIVATE:
    case WM_PAINT:
    {
        draw_gui();
            //PAINTSTRUCT ps;
            //HDC hdc = BeginPaint(hWnd, &ps);
            ////// TODO: Add any drawing code that uses hdc here...
            //EndPaint(hWnd, &ps);
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

            g_input.mouseMoveDelta.x = x;
            g_input.mouseMoveDelta.y = y;
            if (wheelDelta)
                g_input.mouseWheelDelta = (float)wheelDelta / (float)WHEEL_DELTA;

            POINT cursorPoint;
            GetCursorPos(&cursorPoint);
            ScreenToClient(hWnd, &cursorPoint);
            g_input.mousePosition.x = cursorPoint.x;
            g_input.mousePosition.y = cursorPoint.y;

            g_input.mouseButtonFlags1 = 0;

            if (flags)
            {
                if ((flags & 0x1) == 0x1)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_LMBDOWN;
                    g_input.mouseButtonFlags2 |= MG_MBFL_LMBHOLD;
                }

                if ((flags & 0x2) == 0x2)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_LMBUP;
                    if((g_input.mouseButtonFlags2 & MG_MBFL_LMBHOLD) == MG_MBFL_LMBHOLD)
                        g_input.mouseButtonFlags2 ^= MG_MBFL_LMBHOLD;
                }

                if ((flags & 0x4) == 0x4)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_RMBDOWN;
                    g_input.mouseButtonFlags2 |= MG_MBFL_RMBHOLD;
                }
                if ((flags & 0x8) == 0x8)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_RMBUP;
                    if ((g_input.mouseButtonFlags2 & MG_MBFL_RMBHOLD) == MG_MBFL_RMBHOLD)
                        g_input.mouseButtonFlags2 ^= MG_MBFL_RMBHOLD;
                }

                if ((flags & 0x10) == 0x10)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_MMBDOWN;
                    g_input.mouseButtonFlags2 |= MG_MBFL_MMBHOLD;
                }
                if ((flags & 0x20) == 0x20)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_MMBUP;
                    if ((g_input.mouseButtonFlags2 & MG_MBFL_MMBHOLD) == MG_MBFL_MMBHOLD)
                        g_input.mouseButtonFlags2 ^= MG_MBFL_MMBHOLD;
                }

                if ((flags & 0x100) == 0x100)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_X1MBDOWN;
                    g_input.mouseButtonFlags2 |= MG_MBFL_X1MBHOLD;
                }
                if ((flags & 0x200) == 0x200)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_X1MBUP;
                    if ((g_input.mouseButtonFlags2 & MG_MBFL_X1MBHOLD) == MG_MBFL_X1MBHOLD)
                        g_input.mouseButtonFlags2 ^= MG_MBFL_X1MBHOLD;
                }

                if ((flags & 0x40) == 0x40)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_X2MBDOWN;
                    g_input.mouseButtonFlags2 |= MG_MBFL_X2MBHOLD;
                }
                if ((flags & 0x80) == 0x80)
                {
                    g_input.mouseButtonFlags1 |= MG_MBFL_X2MBUP;
                    if ((g_input.mouseButtonFlags2 & MG_MBFL_X2MBHOLD) == MG_MBFL_X2MBHOLD)
                        g_input.mouseButtonFlags2 ^= MG_MBFL_X2MBHOLD;
                }

            }
        }
    }break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SETCURSOR: {
        auto id = LOWORD(lParam);
        switch (id)
        {
        default:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_Arrow], mgCursorType_Arrow);
            return TRUE;
        case HTLEFT:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeWE], mgCursorType_SizeWE);
            return TRUE;
        case HTRIGHT:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeWE], mgCursorType_SizeWE);
            return TRUE;
        case HTTOP:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeNS], mgCursorType_SizeNS);
            return TRUE;
        case HTBOTTOM:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeNS], mgCursorType_SizeNS);
            return TRUE;
        case HTTOPLEFT:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeNWSE], mgCursorType_SizeNWSE);
            return TRUE;
        case HTBOTTOMRIGHT:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeNWSE], mgCursorType_SizeNWSE);
            return TRUE;
        case HTBOTTOMLEFT:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeNESW], mgCursorType_SizeNESW);
            return TRUE;
        case HTTOPRIGHT:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_SizeNESW], mgCursorType_SizeNESW);
            return TRUE;
        case HTHELP:
            mgSetCursor(g_gui_context, g_gui_context->currentCursors[mgCursorType_Help], mgCursorType_Help);
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
                sizeof(chars), (WCHAR*)&g_input.character, 1);
        }

        if (isPress)
        {
            if (key < 256)
            {
                g_input.keyFlags[key] |= MG_KEYFL_HOLD;
                g_input.keyFlags[key] |= MG_KEYFL_HIT;
            }
        }
        else
        {
            if (key < 256)
            {
                if((g_input.keyFlags[key] & MG_KEYFL_HOLD) == MG_KEYFL_HOLD)
                    g_input.keyFlags[key] ^= MG_KEYFL_HOLD;
                
                g_input.keyFlags[key] |= MG_KEYFL_RELEASE;
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
    return 0;
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