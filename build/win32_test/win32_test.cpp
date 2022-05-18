// win32_test.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "win32_test.h"

#include "miGUI.h"
#include "miGUILoader.h"

#include <time.h>
#include <stdio.h>

#include <objidl.h>
#include <gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")
Gdiplus::GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR           gdiplusToken;

mgContext* g_gui_context = 0;
mgInputContext g_input;
HRAWINPUT g_rawInputData[0xff];

mgFont* g_win32font = 0;
mgIcons* g_icons = 0;

mgWindow* test_guiWindow1 = 0;
mgWindow* test_guiWindow2 = 0;
mgWindow* test_guiWindow3 = 0;
mgWindow* test_guiWindow4 = 0;
mgWindow* test_guiWindow5 = 0;

#define WindowID_1 1
#define WindowID_2 2
#define WindowID_3 3
#define WindowID_4 4
#define WindowID_5 5

#define ButtonID_Button 1
#define ButtonID_SaveDock 2
#define ButtonID_LoadDock 3

mgSystemWindowOSData* g_mainSystemWindowOSData = 0; // mgContext has this.

mgSystemWindowOSData* g_currSystemWindowOSData = 0;
mgSystemWindowOSData* gui_setCurrentWindow(mgSystemWindowOSData* data)
{
    mgSystemWindowOSData* prev = g_currSystemWindowOSData;
    g_currSystemWindowOSData = data;

    return prev;
}

//HDC g_dc = 0; // now I have g_currSystemWindowOSData
//HWND g_hwnd = 0;

HKL KEYBOARD_INPUT_HKL = 0;
unsigned int KEYBOARD_INPUT_CODEPAGE = 0;

mgPoint borderSize;
mgPoint borderSizeMainWindow;

int g_fps = 0;
int g_run = 1;

class MyImage
{
public:
    MyImage() {}
    ~MyImage() {
        if (m_gdiimage)
            delete m_gdiimage;
    }
    void load(const wchar_t* f)
    {
        m_gdiimage = new Gdiplus::Image(f);
    }

    Gdiplus::Image * m_gdiimage = 0;
};
//MyImage g_iconsImage;
mgTexture* g_iconsImage = 0;

/*double buffering*/
//HDC hdcMem = 0;
//HBITMAP hbmMem = 0, hbmOld = 0;
void DeleteBackBuffer(mgSystemWindowOSData* data) {
    if (data)
    {
        if (data->hdcMem)
            DeleteDC((HDC)data->hdcMem);
        if (data->hbmMem)
            DeleteObject(data->hbmMem);
    }
}
void UpdateBackBuffer() {
    if (!g_currSystemWindowOSData)
        return;

    DeleteBackBuffer(g_currSystemWindowOSData);

    HWND hWnd = (HWND)g_currSystemWindowOSData->hWnd;

    HDC dc = GetWindowDC(hWnd);
    g_currSystemWindowOSData->hdcMem = CreateCompatibleDC(dc);

    RECT windowRect;
    GetWindowRect(hWnd, &windowRect);

    g_currSystemWindowOSData->position.x = windowRect.left;
    g_currSystemWindowOSData->position.y = windowRect.top;
    g_currSystemWindowOSData->size.x = windowRect.right - windowRect.left;
    g_currSystemWindowOSData->size.y = windowRect.bottom - windowRect.top;

    g_currSystemWindowOSData->hbmMem = CreateCompatibleBitmap(dc,
        g_currSystemWindowOSData->size.x,
        g_currSystemWindowOSData->size.y);

    ReleaseDC(hWnd, dc);
}


void gui_beginDraw(int reason)
{
    g_currSystemWindowOSData->hbmOld = (HBITMAP)SelectObject((HDC)g_currSystemWindowOSData->hdcMem, g_currSystemWindowOSData->hbmMem);
    RECT r;
    r.left = 0;
    r.top = 0;
    r.right = g_currSystemWindowOSData->size.x;
    r.bottom = g_currSystemWindowOSData->size.y;

    int padding = GetSystemMetrics(SM_CXPADDEDBORDER);

    switch (reason)
    {
    default:
        borderSize.x = GetSystemMetrics(SM_CXFRAME) + padding;
        borderSize.y = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + padding);
        borderSizeMainWindow = borderSize;
        FillRect((HDC)g_currSystemWindowOSData->hdcMem, &r, (HBRUSH)(COLOR_WINDOW + 1));
        break;
    case mgBeginDrawReason_popupWindow:
        borderSize.x = 0;
        borderSize.y = 0;
        FillRect((HDC)g_currSystemWindowOSData->hdcMem, &r, (HBRUSH)(COLOR_WINDOW + 3));
        break;
    }
}

void gui_endDraw()
{
    HWND hWnd = (HWND)g_currSystemWindowOSData->hWnd;
    HDC dc = GetWindowDC(hWnd);
    BitBlt(dc,
        0, 0,
        g_currSystemWindowOSData->size.x, 
        g_currSystemWindowOSData->size.y,
        (HDC)g_currSystemWindowOSData->hdcMem,
        0, 0,
        SRCCOPY);
    ReleaseDC(hWnd, dc);
}

mgTexture* gui_createTexture(mgImage* img)
{
    return 0;
}

void gui_destroyTexture(mgTexture* t)
{

}

RECT g_clipRect;
mgRect gui_setClipRect(mgRect* r)
{
    static mgRect old;
    g_clipRect.left = r->left;
    g_clipRect.top = r->top;
    g_clipRect.right = r->right;
    g_clipRect.bottom = r->bottom;
    mgRect ret = old;
    old = *r;
    return ret;
}

void gui_drawRectangle(
    int reason,
    void * object,
    mgRect* rct,
    mgColor* color1,
    mgColor* color2,
    mgTexture* texture, /*optional*/
    mgVec4* UVRegion)
{
    HDC hdcMem = (HDC)g_currSystemWindowOSData->hdcMem;

    if (texture)
    {
        if (reason == mgDrawRectangleReason_windowCloseButton
            || reason == mgDrawRectangleReason_windowCollapseButton
            || reason == mgDrawRectangleReason_popupNextIcon)
        {
            Gdiplus::Image* myimg = (Gdiplus::Image*)texture->implementation;
            Gdiplus::Graphics graphics(hdcMem);
            Gdiplus::Rect gdirct;
            gdirct.X = rct->left + borderSize.x;
            gdirct.Y = rct->top + borderSize.y;
            gdirct.Width = rct->right - rct->left;
            gdirct.Height = rct->bottom - rct->top;
            graphics.DrawImage(myimg, 
                gdirct,
                g_gui_context->currentIcon->lt.x, 
                g_gui_context->currentIcon->lt.y, 
                g_gui_context->currentIcon->sz.x, 
                g_gui_context->currentIcon->sz.y, 
                Gdiplus::UnitPixel);
        }
        return;
    }

    RECT r;
    r.left = rct->left + borderSize.x;
    r.top = rct->top + borderSize.y;
    r.right = rct->right + borderSize.x;
    r.bottom = rct->bottom + borderSize.y;

    unsigned int c1 = mgColorGetAsIntegerARGB(color1);
    unsigned int c2 = mgColorGetAsIntegerARGB(color2);
    HBRUSH brsh = CreateSolidBrush(RGB(c1 & 0xff, (c1 & 0xff00) >> 8, (c1 & 0xff0000) >> 16));
    SelectObject(hdcMem, brsh);
    HRGN rgn = 0;
    if (reason == mgDrawRectangleReason_rectangle)
    {
        rgn = CreateRectRgn(g_clipRect.left + borderSize.x, g_clipRect.top + borderSize.y, g_clipRect.right + borderSize.x, g_clipRect.bottom + borderSize.y);
        SelectClipRgn(hdcMem, rgn);
        FillRect(hdcMem, &r, brsh);
       /* Gdiplus::Graphics graphics(hdcMem);
        Gdiplus::SolidBrush br(Gdiplus::Color(c1 & 0xff, (c1 & 0xff00) >> 8, (c1 & 0xff0000) >> 16));
        Gdiplus::Rect gdirct;
        gdirct.X = r.left;
        gdirct.Y = r.top;
        gdirct.Width = size->x;
        gdirct.Height = size->y;
        graphics.FillRectangle(&br, gdirct);*/
    }
    else if (reason == mgDrawRectangleReason_buttonBG
        || reason == mgDrawRectangleReason_popupBG
        || reason == mgDrawRectangleReason_windowScrollbarBG
        || reason == mgDrawRectangleReason_dockElementBG
        || reason == mgDrawRectangleReason_dockSplitterBGHor
        || reason == mgDrawRectangleReason_dockSplitterBGVert
        || reason == mgDrawRectangleReason_dockPanelSplitterBGHor
        || reason == mgDrawRectangleReason_dockPanelSplitterBGVert
        || reason == mgDrawRectangleReason_dockBG
        || reason == mgDrawRectangleReason_tooltip
        || reason == mgDrawRectangleReason_windowMenuBG
        || reason == mgDrawRectangleReason_popupSeparator
        || reason == mgDrawRectangleReason_dockTabWindowTitle
        || reason == mgDrawRectangleReason_dockTabBG)
    {
        if (reason == mgDrawRectangleReason_popupBG
            || reason == mgDrawRectangleReason_tooltip)
        {
            r.left += 1;
            r.top += 1;
            r.right -= 1;
            r.bottom -= 1;

            // BORDER for popup and tooltip
            int sz = 1;
            RECT r2 = r;
            r2.left -= sz;
            r2.top -= sz;
            r2.bottom += sz;
            r2.right += sz;
            rgn = CreateRectRgn(
                g_clipRect.left + borderSize.x- sz,
                g_clipRect.top + borderSize.y- sz,
                g_clipRect.right + borderSize.x+ sz,
                g_clipRect.bottom + borderSize.y+ sz);
            HBRUSH brsh2 = CreateSolidBrush(RGB(0, 0, 0));
            SelectObject(hdcMem, brsh2);
            FillRect(hdcMem, &r2, brsh2);
            DeleteObject(brsh2);

            SelectObject(hdcMem, brsh);
        }
        else if (reason == mgDrawRectangleReason_popupSeparator)
        {
            r.top += 2;
            r.bottom -= 2;
            r.left += 2;
            r.right -= 2;
        }
        else
        {
            rgn = CreateRectRgn(g_clipRect.left + borderSize.x, g_clipRect.top + borderSize.y, g_clipRect.right + borderSize.x, g_clipRect.bottom + borderSize.y);
        }
        SelectClipRgn(hdcMem, rgn);
        FillRect(hdcMem, &r, brsh);
    }
    else
    {
        /*RoundRect(hdcMem, r.left, r.top, r.right, r.bottom, 6, 6);*/
        /*or with clip region*/

        int roundRectBtm = r.bottom+1;
        if (reason == mgDrawRectangleReason_windowTitlebar)
            roundRectBtm += 7;

        rgn = CreateRoundRectRgn(r.left, r.top, r.right+1, roundRectBtm, 7, 7);

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
    }

    DeleteObject(rgn);
    DeleteObject(brsh);
    SelectClipRgn(hdcMem, 0);
}

void gui_drawText(
    int reason,
    void* object,
    mgPoint* position,
    const wchar_t* text,
    int textLen,
    mgColor* color,
    mgFont* font)
{
    HDC hdcMem = (HDC)g_currSystemWindowOSData->hdcMem;
    
    SelectObject(hdcMem, font->implementation);
    SetTextColor(hdcMem, mgColorGetAsIntegerBGR(color));
    SetBkMode(hdcMem, TRANSPARENT);

    HRGN rgn = CreateRectRgn(g_clipRect.left + borderSize.x, g_clipRect.top + borderSize.y, g_clipRect.right + borderSize.x, g_clipRect.bottom + borderSize.y);
    SelectClipRgn(hdcMem, rgn);
    TextOutW(hdcMem, position->x + borderSize.x, position->y + borderSize.y, text, textLen);
    DeleteObject(rgn);
    SelectClipRgn(hdcMem, 0);
}

void gui_getTextSize(const wchar_t* text, mgFont* font, mgPoint* sz)
{
    HDC hdcMem = (HDC)g_currSystemWindowOSData->hdcMem;
    
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
    HWND hWnd = (HWND)g_mainSystemWindowOSData->hWnd;
    
    mgFont* f = (mgFont*)malloc(sizeof(mgFont));
    HDC dc = GetWindowDC(hWnd);
    f->implementation = CreateFontA(
        -MulDiv(size, GetDeviceCaps(dc, LOGPIXELSY), 72),
        0, 0, 0,
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
    ReleaseDC(hWnd, dc);
    return f;
}

void gui_drawLine(
    int reason,
    void* object,  /*depends on reason*/
    mgPoint* position,
    mgPoint* where,
    mgColor*,
    int size)
{

}

void draw_gui()
{
    if (!g_gui_context)
        return;

    g_gui_context->gpu->beginDraw(mgBeginDrawReason_systemWindow);

    /*draw elements*/
    mgDraw(g_gui_context);

    borderSize = borderSizeMainWindow;

    static int drawSomeInfo = 1;
    if (mgIsKeyHit(g_gui_context->input, MG_KEY_F1))
        drawSomeInfo = drawSomeInfo == 1 ? 0 : 1;

    if (drawSomeInfo)
    {

        mgRect rect;
        mgRectSet(&rect, 0, 0, 220, 180);

        mgColor color1;
        mgColor color2;
        mgColorSetAsIntegerRGB(&color1, 0xFFFF9999);
        mgColorSetAsIntegerRGB(&color2, 0xFF9999FF);

        {
            mgRect r;
            r.left = 0;
            r.top = 0;
            r.right = 800;
            r.bottom = 600;
            gui_setClipRect(&r);
        }

        g_gui_context->gpu->drawRectangle(0, 0, &rect, &color1, &color2, 0, 0);

        mgPoint textPosition;
        mgPointSet(&textPosition, 10, 10);
        mgColor textColor;
        mgColorSetAsIntegerRGB(&textColor, 0xFF005000);
        wchar_t textBuffer[200];


        swprintf_s(textBuffer, L"FPS: %i", g_fps);
        mgPointSet(&textPosition, 0, 0);
        gui_drawText(0, 0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);

        swprintf_s(textBuffer, L"mousePosition: %i %i", g_input.mousePosition.x, g_input.mousePosition.y);
        mgPointSet(&textPosition, 10, 10);
        gui_drawText(0, 0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);

        swprintf_s(textBuffer, L"mouseMoveDelta: %i %i", g_input.mouseMoveDelta.x, g_input.mouseMoveDelta.y);
        mgPointSet(&textPosition, 10, 20);
        gui_drawText(0, 0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);

        swprintf_s(textBuffer, L"mouseWheelDelta: %.1f", g_input.mouseWheelDelta);
        mgPointSet(&textPosition, 10, 30);
        gui_drawText(0, 0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);

        swprintf_s(textBuffer, L"mouseButtons: 0x%x 0x%x", g_input.mouseButtonFlags1, g_input.mouseButtonFlags2);
        mgPointSet(&textPosition, 10, 40);
        gui_drawText(0, 0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);

        swprintf_s(textBuffer, L"key char: %c 0x%x", g_input.character, g_input.character);
        mgPointSet(&textPosition, 10, 50);
        gui_drawText(0, 0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);

        if (mgIsKeyHit(&g_input, MG_KEY_ENTER))
        {
            swprintf_s(textBuffer, L"Hit ENTER");
            mgPointSet(&textPosition, 10, 60);
            gui_drawText(0, 0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);
        }

        if (mgIsKeyHold(&g_input, MG_KEY_ENTER))
        {
            swprintf_s(textBuffer, L"Hold ENTER");
            mgPointSet(&textPosition, 10, 70);
            gui_drawText(0, 0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);
        }

        if (mgIsKeyRelease(&g_input, MG_KEY_ENTER))
        {
            swprintf_s(textBuffer, L"Release ENTER");
            mgPointSet(&textPosition, 10, 80);
            gui_drawText(0, 0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);
        }

        swprintf_s(textBuffer, L"KB modifier 0x%x\n", g_input.keyboardModifier);
        mgPointSet(&textPosition, 10, 90);
        gui_drawText(0, 0, &textPosition, textBuffer, wcslen(textBuffer), &textColor, g_win32font);
    }
    

    g_gui_context->gpu->endDraw();
}

void saveDock(struct mgElement_s* e)
{
    int saveDataSize = 0;
    int* saveData = mgDockGetSaveData(g_gui_context, &saveDataSize);
    if (saveData)
    {
        //hWnd f = CreateFile(L"dock.data", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
        FILE* f = fopen("dock.data", "wb");
        if (f)
        {
            //WriteFile(f, (void*)saveData, saveDataSize * sizeof(int), 0, 0);
            //ClosehWnd(f);
            fwrite(saveData, saveDataSize * sizeof(int), 1, f);
            fclose(f);
        }
        free(saveData);
    }
}

mgWindow* loadDockCallback(int windowID)
{
    switch (windowID)
    {
    case WindowID_1:
        return test_guiWindow1;
    case WindowID_2:
        return test_guiWindow2;
    case WindowID_3:
        return test_guiWindow3;
    case WindowID_4:
        return test_guiWindow4;
    case WindowID_5:
        return test_guiWindow5;
    }
    return 0;
}
void loadDock(struct mgElement_s* e)
{
    FILE* f = fopen("dock.data", "rb");
    if (f)
    {
        fseek(f, 0, SEEK_END);
        int fileSize = ftell(f);
        fseek(f, 0, SEEK_SET);

        int* data = (int*)malloc(fileSize);
        if (data)
        {
            fread((void*)data, fileSize, 1, f);
        }

        fclose(f);

        mgDockLoadData(g_gui_context, data, fileSize / 4, loadDockCallback);

        if (data)
            free(data);
    }
}

mgFont* Button_onFont(struct mgElement_s* e)
{
    return g_win32font;
}

const wchar_t* Button_onText(struct mgElement_s* e, size_t* textLen)
{
    mgElementButton* btn = (mgElementButton*)e->implementation;

    switch (e->id)
    {
    case ButtonID_Button:
        *textLen = 6;
        return L"Button";
    case ButtonID_SaveDock:
        *textLen = 10;
        return L"Save dock";
    case ButtonID_LoadDock:
        *textLen = 10;
        return L"Load dock";
    }

    *textLen = 0;
    return 0;
}

mgColor* Text_onColor(struct mgElement_s* e)
{
    static mgColor c;
    c.setAsIntegerRGB(0xFF1144);
    return &c;
}

mgFont* Text_onFont(struct mgElement_s*)
{
    return g_win32font;
}

const wchar_t* Text_onText(struct mgElement_s*, size_t* textLen)
{
    *textLen = 4;
    return L"Text";
}

static unsigned int LocaleIdToCodepage(unsigned int lcid);

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR    lpCmdLine, int       nCmdShow)
{
    //char buf[1024];
    //memset(buf, 0, 1024);
    //FILE * myf = fopen("data1", "wb");
    //setvbuf(myf, buf, _IOFBF, 1024);
    //fprintf(myf, "hello world!");

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    
    AllocConsole();
    freopen("CONOUT$", "wb", stdout);

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

    HWND hwnd = CreateWindowExW(0, wcex.lpszClassName, L"migui", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 0, 0, hInstance, 0);
    if (!hwnd)
        return FALSE;
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

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
    gui_gpu.drawLine = gui_drawLine;
    gui_gpu.setClipRect = gui_setClipRect;
    gui_gpu.setCurrentWindow = gui_setCurrentWindow;
    gui_gpu.updateBackBuffer = UpdateBackBuffer;

    memset(&g_input, 0, sizeof(g_input));
    g_gui_context = mgCreateContext(&gui_gpu, &g_input);
    g_gui_context->getTextSize = gui_getTextSize;

    g_mainSystemWindowOSData = &g_gui_context->systemWindowData;

    g_mainSystemWindowOSData->hWnd = hwnd; // for multiple windows. save main window
    g_currSystemWindowOSData = g_mainSystemWindowOSData; // for multiple windows. set current
    UpdateBackBuffer(); // need to create hdcMem and hbmMem before font creation

    {
        RECT rc;
        GetClientRect(hwnd, &rc);
        g_gui_context->needRebuild = 1;
        g_gui_context->windowSize.x = rc.right - rc.left;
        g_gui_context->windowSize.y = rc.bottom - rc.top;
    }

    MyImage* myim = new MyImage;
    myim->load(L"../data/icons.png");
    g_iconsImage = new mgTexture;
    g_iconsImage->implementation = myim->m_gdiimage;

    mgInitDefaultIcons(g_gui_context, g_iconsImage);


    g_win32font = gui_createFont("Segoe", 0, 10);
    g_gui_context->defaultPopupFont = g_win32font;
    g_gui_context->tooltipFont = g_win32font;
    {
        mgDockPanelElementCreationInfo dckElmts[] = {
            {1, 20, 20, 1000},
            {3, 20, 20, 1000},
            {0, 200, 20, 1000},
            {2, 20, 20, 1000},
            {1, 20, 20, 1000},
            {1, 20, 20, 1000},
        };
        
        mgInitDockPanel(g_gui_context, 0, 30, 0, 0, dckElmts, sizeof(dckElmts) / sizeof(mgDockPanelElementCreationInfo));

       // mgColorSet(&g_gui_context->activeStyle->dockpanelBGColor, 0.9f, 0.9f, 0.9f, 1.f);
        g_gui_context->dockPanel->flags |= mgDockPanelFlag_drawBG;
        for (int i = 0; i < g_gui_context->dockPanel->elementsSize; ++i)
        {
            //g_gui_context->dockPanel->elements[i].flags |= mgDockPanelElementFlag_drawBG;
        }

        test_guiWindow1 = mgCreateWindow(g_gui_context, 10, 10, 300, 180);
        test_guiWindow1->titlebarFont = g_win32font;
        test_guiWindow1->titlebarHeight = 30;
        test_guiWindow1->flags ^= mgWindowFlag_closeButton;
        test_guiWindow1->flags |= mgWindowFlag_canDock;
        test_guiWindow1->id = WindowID_1;
        mgSetWindowTitle(test_guiWindow1, L"Window1");
        
        mgPoint pos, sz;
        mgColor c1, c2;
        mgPointSet(&pos, 30, 30);
        mgPointSet(&sz, 50, 500);
        c1.setAsIntegerRGB(0xff0000);
        c2.setAsIntegerRGB(0x0000FF);
       // mgElement* er = mgCreateRectangle(guiWindow1, &pos, &sz, &c1, &c2);
       // er->align = mgAlignment_center;

        mgPointSet(&sz, 100, 40);
        mgPointSet(&pos, 100, 60);
        mgElement* eb = mgCreateButton(test_guiWindow1, &pos, &sz);
        eb->id = ButtonID_Button;
        eb->align = mgAlignment_rightBottom;
        ((mgElementButton*)eb->implementation)->pushButton = 1;
        ((mgElementButton*)eb->implementation)->onFont = Button_onFont;
        ((mgElementButton*)eb->implementation)->onText = Button_onText;

        mgPointSet(&pos, 100, 120);
        eb = mgCreateButton(test_guiWindow1, &pos, &sz);
        eb->id = ButtonID_SaveDock;
        eb->onReleaseLMB = saveDock;
        eb->enabled = 1;
        ((mgElementButton*)eb->implementation)->onFont = Button_onFont;
        ((mgElementButton*)eb->implementation)->onText = Button_onText;

        mgPointSet(&pos, 100, 160);
        eb = mgCreateButton(test_guiWindow1, &pos, &sz);
        eb->id = ButtonID_LoadDock;
        eb->onReleaseLMB = loadDock;
        ((mgElementButton*)eb->implementation)->onFont = Button_onFont;
        ((mgElementButton*)eb->implementation)->onText = Button_onText;

        mgPointSet(&pos, 100, 100);
        mgElement* et = mgCreateText(test_guiWindow1, &pos);
        et->align = mgAlignment_rightBottom;
        ((mgElementText*)et->implementation)->onColor = Text_onColor;
        ((mgElementText*)et->implementation)->onFont = Text_onFont;
        ((mgElementText*)et->implementation)->onText = Text_onText;

        test_guiWindow2 = mgCreateWindow(g_gui_context, 30, 30, 300, 180);
        test_guiWindow2->titlebarFont = g_win32font;
        test_guiWindow2->flags ^= mgWindowFlag_collapseButton;
        test_guiWindow2->flags |= mgWindowFlag_canDock;
        test_guiWindow2->sizeMinimum.x = 200;
        test_guiWindow2->sizeMinimum.y = 100;
        test_guiWindow2->id = WindowID_2;
        mgSetWindowTitle(test_guiWindow2, L"_canDock");
        {
            mgPointSet(&sz, 160, 20);
            mgPointSet(&pos, 0, 0);
            mgElement* btn = mgCreateButton(test_guiWindow2, &pos, &sz);
            btn->id = ButtonID_Button;
            ((mgElementButton*)btn->implementation)->onFont = Button_onFont;
            ((mgElementButton*)btn->implementation)->onText = Button_onText;
        }

        test_guiWindow3 = mgCreateWindow(g_gui_context, 30, 30, 300, 180);
        test_guiWindow3->titlebarFont = g_win32font;
        test_guiWindow3->flags ^= mgWindowFlag_collapseButton;
        test_guiWindow3->flags |= mgWindowFlag_canDock;
        test_guiWindow3->sizeMinimum.x = 200;
        test_guiWindow3->sizeMinimum.y = 100;
        test_guiWindow3->id = WindowID_3;
        mgSetWindowTitle(test_guiWindow3, L"_canDock3");
        {
            mgPointSet(&sz, 160, 20);
            mgPointSet(&pos, 0, 0);
            mgElement* btn = mgCreateButton(test_guiWindow3, &pos, &sz);
            btn->id = ButtonID_Button;
            ((mgElementButton*)btn->implementation)->onFont = Button_onFont;
            ((mgElementButton*)btn->implementation)->onText = Button_onText;
        }

        test_guiWindow4 = mgCreateWindow(g_gui_context, 30, 30, 300, 180);
        test_guiWindow4->titlebarFont = g_win32font;
        test_guiWindow4->flags ^= mgWindowFlag_collapseButton;
        test_guiWindow4->flags |= mgWindowFlag_canDock;
        test_guiWindow4->sizeMinimum.x = 200;
        test_guiWindow4->sizeMinimum.y = 100;
        test_guiWindow4->id = WindowID_4;
        mgSetWindowTitle(test_guiWindow4, L"_canDock4");
        {
            mgPointSet(&sz, 160, 20);
            mgPointSet(&pos, 0, 0);
            mgElement* btn = mgCreateButton(test_guiWindow4, &pos, &sz);
            btn->id = ButtonID_Button;
            ((mgElementButton*)btn->implementation)->onFont = Button_onFont;
            ((mgElementButton*)btn->implementation)->onText = Button_onText;
        }

        test_guiWindow5 = mgCreateWindow(g_gui_context, 30, 30, 300, 180);
        test_guiWindow5->titlebarFont = g_win32font;
        test_guiWindow5->flags ^= mgWindowFlag_collapseButton;
        test_guiWindow5->flags |= mgWindowFlag_canDock;
        test_guiWindow5->sizeMinimum.x = 200;
        test_guiWindow5->sizeMinimum.y = 100;
        test_guiWindow5->id = WindowID_5;
        mgSetWindowTitle(test_guiWindow5, L"_canDock5");
        {
            mgPointSet(&sz, 160, 20);
            mgPointSet(&pos, 0, 0);
            mgElement* btn = mgCreateButton(test_guiWindow5, &pos, &sz);
            btn->id = ButtonID_Button;
            ((mgElementButton*)btn->implementation)->onFont = Button_onFont;
            ((mgElementButton*)btn->implementation)->onText = Button_onText;
        }
        
        int popupFlags = 0;
        popupFlags |= mgPopupFlags_systemWindow;

        {
            mgPopupItemInfo items_file_popup[] =
            {
                {0, L"Open", 0, 0, mgPopupItemType_default, 0, L"Ctrl+O", 1},
                {0, L"New", 0, 0, mgPopupItemType_default, 0, 0, 1},
                {0, 0, 0, 0, mgPopupItemType_separator, 0, 0, 1},
                {0, L"Save", 0, 0, mgPopupItemType_default, 0, L"Ctrl+S", 1},
                {0, L"Save ass...", 0, 0, mgPopupItemType_default, 0, L"Ctrl+Shift+S", 1},
                {0, 0, 0, 0, mgPopupItemType_separator, 0, 0, 1},
                {0, L"Exit", 0, 0, mgPopupItemType_default, 0, L"Alt+F4", 1},
            };
            mgPopup* file_popup = mgCreatePopup(g_gui_context, items_file_popup, 7, g_win32font, popupFlags);

            mgPopupItemInfo items_submenu1_popup[] =
            {
                {0, L"Item1", 0, 0, mgPopupItemType_default, 0, L"Ctrl+O", 1},
                {0, L"Item2", 0, 0, mgPopupItemType_default, 0, 0, 1},
                {0, L"Item3", 0, 0, mgPopupItemType_default, 0, L"Ctrl+S", 1},
                {0, L"Sub menu2", 0, 0, mgPopupItemType_default, 0, 0, 1},
            };
            mgPopup* submenu1_popup = mgCreatePopup(g_gui_context, items_submenu1_popup, 4, g_win32font, popupFlags);

            mgPopupItemInfo items_edit_popup[] =
            {
                {0, L"Copy", 0, 0, mgPopupItemType_default, 0, L"Ctrl+O", 1},
                {0, L"Paste", 0, 0, mgPopupItemType_default, 0, 0, 1},
                {0, 0, 0, 0, mgPopupItemType_separator, 0, 0, 1},
                {0, L"Cut", 0, 0, mgPopupItemType_default, 0, L"Ctrl+S", 1},
                {0, 0, 0, 0, mgPopupItemType_separator, 0, 0, 1},
                {0, L"Sub menu1", submenu1_popup, 0, mgPopupItemType_default, 0, 0, 1},
            };
            mgPopup* edit_popup = mgCreatePopup(g_gui_context, items_edit_popup, 6, g_win32font, popupFlags);

            mgMenuItemInfo menu_items[] =
            {
                {L"File", file_popup},
                {L"Edit", edit_popup},
                {L"Window", 0},
                {L"View", 0},
                {L"Extension", 0},
            };
            mgMenu* menu = mgCreateMenu(g_gui_context, menu_items, 5, g_win32font);
            test_guiWindow1->menu = menu;
        }
    }

    g_gui_context->needRebuild = 1; //

    UpdateBackBuffer();
    MSG msg;
    
    int fps_counter = 0;
    float fps_time = 0.f;

    Sleep(100);
    while (g_run)
    {
        mgStartFrame(g_gui_context);

        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            GetMessage(&msg, NULL, 0, 0);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        fps_counter += 1;
        fps_time += g_gui_context->deltaTime;
        if (fps_time > 1.f)
        {
            fps_time = 0.f;
            g_fps = fps_counter;
            fps_counter = 0;
        }

        mgUpdate(g_gui_context);
        draw_gui();
    }
    
    if (g_icons)
        mgDestroyIcons(g_icons);

    if (g_iconsImage)
        delete g_iconsImage;

    DeleteObject(g_win32font->implementation);
    free(g_win32font);

    DeleteBackBuffer(g_mainSystemWindowOSData);

    mgDestroyContext(g_gui_context);
    mgUnload(gui_lib);

    if (myim)
        delete myim;
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return (int) msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = 800;
        lpMMI->ptMinTrackSize.y = 600;
    }
    break;
    case WM_SIZE:
    {
        if (g_gui_context)
        {
            RECT rc;
            GetClientRect(hWnd, &rc);
            mgOnWindowSize(g_gui_context, rc.right - rc.left, rc.bottom - rc.top);
            g_mainSystemWindowOSData->size.x = rc.right - rc.left;
            g_mainSystemWindowOSData->size.y = rc.bottom - rc.top;
            /*g_gui_context->needRebuild = 1;
            g_gui_context->windowSize.x = rc.right - rc.left;
            g_gui_context->windowSize.y = rc.bottom - rc.top;*/
        }
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
            HANDLE devicehWnd = raw->header.hDevice;

            const RAWMOUSE& mouseData = raw->data.mouse;

            USHORT flags = mouseData.usButtonFlags;
            short wheelDelta = (short)mouseData.usButtonData;
            LONG x = mouseData.lLastX, y = mouseData.lLastY;

            /*wprintf(
                L"Mouse: Device=0x%08X, Flags=%04x, WheelDelta=%d, X=%d, Y=%d\n",
                devicehWnd, flags, wheelDelta, x, y);*/

            //g_input.mouseMoveDelta.x = x;
            //g_input.mouseMoveDelta.y = y;
            if (wheelDelta)
                g_input.mouseWheelDelta = (float)wheelDelta / (float)WHEEL_DELTA;

            POINT cursorPoint;
            GetCursorPos(&cursorPoint);
            ScreenToClient(hWnd, &cursorPoint);
            g_input.mousePosition.x = cursorPoint.x;
            g_input.mousePosition.y = cursorPoint.y;

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
        g_run = 0;
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