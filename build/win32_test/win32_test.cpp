// win32_test.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "win32_test.h"

#include "miGUI.h"

HDC g_dc = 0;
HWND g_hwnd = 0;
RECT g_windowRect;

/*double buffering*/
HDC hdcMem = 0;
HBITMAP hbmMem = 0, hbmOld = 0;
void DeleteBackBuffer() {
    if (hdcMem)
        DeleteDC(hdcMem);
    if (CreateCompatibleBitmap)
        DeleteObject(CreateCompatibleBitmap);
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

    UINT dpi = GetDpiForWindow(g_hwnd);
    int padding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);


    RECT r;
    r.left = position->x + GetSystemMetricsForDpi(SM_CXFRAME, dpi) + padding;
    r.top = position->y + (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + padding);
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

    mgInputContext input;
    mgContext* gui_context = mgCreateContext(&gui_gpu, &input);
    
    mgFont* win32font = gui_createFont("Impact", 0, 22);

    UpdateBackBuffer();
    MSG msg;
    while (GetMessage(&msg, 0, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, 0, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        gui_context->m_gpu->beginDraw();
        mgPoint point;
        mgPointSet(&point, 0, 0);

        mgPoint size;
        mgPointSet(&size, 100, 100);

        mgColor color1;
        mgColor color2;
        mgColorSetAsIntegerRGB(&color1, 0xFFFF0000);
        mgColorSetAsIntegerRGB(&color2, 0xFF0000FF);
        
        gui_context->m_gpu->drawRectangle(0, &point, &size, &color1, &color2, 0, 0);

        mgPointSet(&point, 200, 100);
        mgColorSetAsIntegerRGB(&color1, 0xFF004242);
        mgColorSetAsIntegerRGB(&color2, 0xFF00FFE2);
        gui_context->m_gpu->drawRectangle(0, &point, &size, &color1, &color2, 0, 0);

        SelectObject(hdcMem, win32font->implementation);
        SetTextColor(hdcMem, RGB(0, 0, 0));
        SetBkMode(hdcMem, TRANSPARENT);
        TextOut(hdcMem, 30, 50, L"Hello", 5);

        gui_context->m_gpu->endDraw();
    }
    DeleteObject(win32font->implementation);
    DeleteBackBuffer();

    mgDestroyContext(gui_context);
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
    case WM_PAINT:
    {
            //PAINTSTRUCT ps;
            //HDC hdc = BeginPaint(hWnd, &ps);
            ////// TODO: Add any drawing code that uses hdc here...
            //EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
