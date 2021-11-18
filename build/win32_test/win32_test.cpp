// win32_test.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "win32_test.h"

#include "miGUI.h"

HDC g_dc = 0;
HWND g_hwnd = 0;

void gui_beginDraw()
{
    g_dc = GetWindowDC(g_hwnd);
}

void gui_endDraw()
{
    ReleaseDC(g_hwnd, g_dc);
}

mgTexture gui_createTexture(mgImage* img)
{
    return 0;
}

void gui_destroyTexture(mgTexture t)
{

}

static int
win32_dpi_scale(
    int value,
    UINT dpi
) {
    return (int)((float)value * dpi / 96);
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
    auto brsh = CreateSolidBrush( mgColorGetAsIntegerARGB(color1) );
    SelectObject(g_dc, brsh);
    FillRect(g_dc, &r, brsh);
    DeleteObject(brsh);
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

    g_hwnd = CreateWindowW(wcex.lpszClassName, L"migui", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!g_hwnd)
        return FALSE;

    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);


    auto gui_lib = mgLoad();
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


    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, 0, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        gui_context->m_gpu->beginDraw();
        mgPoint point;
        mgPointSet(&point, 0, 0); /*C style*/

        mgPoint size(100, 100);/*C++ style*/

        mgColor color;
        mgColorSetAsIntegerRGB(&color, 0xFFFF42E2);
        
        gui_context->m_gpu->drawRectangle(0, &point, &size, &color, &color, 0, 0);
        gui_context->m_gpu->endDraw();
    }

    mgDestroyContext(gui_context);
    mgUnload(gui_lib);

    return (int) msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
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
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
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
