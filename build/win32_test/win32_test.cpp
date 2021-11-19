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
    
    HRGN rgn = CreateRoundRectRgn(r.left, r.top, r.right, r.bottom, 6, 6);
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
    SetTextColor(hdcMem, mgColorGetAsIntegerARGB(color));
    SetBkMode(hdcMem, TRANSPARENT);
    TextOutW(hdcMem, position->x + borderSize.x, position->y + borderSize.y, text, textLen);
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

    g_gui_context->m_gpu->beginDraw();
    mgPoint point;
    mgPointSet(&point, 0, 0);

    mgPoint size;
    mgPointSet(&size, 220, 100);

    mgColor color1;
    mgColor color2;
    mgColorSetAsIntegerRGB(&color1, 0xFFFF9999);
    mgColorSetAsIntegerRGB(&color2, 0xFF9999FF);

    g_gui_context->m_gpu->drawRectangle(0, &point, &size, &color1, &color2, 0, 0);

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
    swprintf_s(textBuffer, L"mouseButtons: %x %x", g_input.mouseButtonFlags1, g_input.mouseButtonFlags2);
    mgPointSet(&textPosition, 10, 40);
    gui_drawText(&textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);

    g_gui_context->m_gpu->endDraw();
}

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

    g_gui_context = mgCreateContext(&gui_gpu, &g_input);
    
    g_win32font = gui_createFont("Segoe", MG_FNTFL_BOLD, 14);

    UpdateBackBuffer();
    MSG msg;
    int run = 1;
    
    Sleep(100);
    while (run)
    {
        g_gui_context->startFrame(g_gui_context);

        run = GetMessage(&msg, 0, 0, 0);
        if (!TranslateAccelerator(msg.hwnd, 0, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        g_gui_context->update(g_gui_context);
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
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
