#include "globals.h"
#include "input.h"
#include "error.h"
#include "network.h"
#include "player.h"
#include "graphics.h"
#include "util.h"

extern Player madotsuki;
extern Player poniko;

namespace OS
{
    unsigned int gameFrame = 0;
    uint64_t epoch;

    HWND os_window;
    HDC os_device_context;
    HGLRC os_render_context;
    LARGE_INTEGER os_time_frequency;
    BOOL os_use_qpc;

    bool is_fullscreen = false;

    void setResolution(bool restore)
    {
        if(restore)
        {
            //Change the resolution back
            ChangeDisplaySettings(NULL, CDS_FULLSCREEN);
        }
        else
        {
            //Change the screen resolution
            DEVMODE mode;
            EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode);
            //Mode.dmBitsPerPel = 32;
            mode.dmPelsWidth = WINDOW_WIDTH;
            mode.dmPelsHeight = WINDOW_HEIGHT;
            mode.dmSize = sizeof(mode);
            mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
            ChangeDisplaySettings(&mode, CDS_FULLSCREEN);
        }
    }

    LRESULT CALLBACK handleMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch(message)
        {
        case WM_CREATE:
            return 0;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_KEYDOWN:
            if(HIWORD(lParam) & KF_REPEAT) break; //No key repeat
            if(wParam == VK_ESCAPE)
                PostQuitMessage(0);
            else
                Input::keyPress(wParam, true);
            return 0;

        case WM_KEYUP:
            Input::keyPress(wParam, false);
            return 0;

        case WM_SYSCOMMAND:
            if(wParam == SC_KEYMENU)
            {
                if(lParam == VK_RETURN)
                    toggleFullscreen();
                return 0;
            }
            break;

        case WM_SETFOCUS:
            if(is_fullscreen)
            {
                //Show the window and set desktop resolution
                ShowWindow(os_window, SW_RESTORE);
                setResolution(false);
            }
            break;

        case WM_KILLFOCUS:
            if(is_fullscreen)
            {
                //Minimize the window and restore desktop resolution
                ShowWindow(os_window, SW_MINIMIZE);
                setResolution(true);
            }
            break;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    //This is the amount of pixels taken up by the title bar
    int title_offset = 0;

    void init(HINSTANCE hInstance)
    {
        //Don't even bother with convention naming in this function

        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = WINDOW_WIDTH;
        rect.bottom = WINDOW_HEIGHT;

        //Create window class
        WNDCLASS wc;
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = handleMessages;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = WINDOW_CLASS;
        RegisterClass(&wc);

        AdjustWindowRectEx(&rect, WINDOW_STYLE, FALSE, WINDOW_STYLE_EX);

        //Create window
        os_window = CreateWindowEx(
            WINDOW_STYLE_EX,
            WINDOW_CLASS,
            WINDOW_TITLE,
            WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
            WINDOW_STYLE,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left, rect.bottom - rect.top,
            NULL, NULL, hInstance, NULL);

        title_offset = (rect.bottom - rect.top) - WINDOW_HEIGHT;

        //Enable OpenGL
        PIXELFORMATDESCRIPTOR pfd;
        int format;

        //Get the device context (DC)
        os_device_context = GetDC(os_window);

        //Set the pixel format for the DC
        ZeroMemory(&pfd, sizeof(pfd));
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 24;
        pfd.cDepthBits = 16;
        pfd.iLayerType = PFD_MAIN_PLANE;
        format = ChoosePixelFormat(os_device_context, &pfd);
        SetPixelFormat(os_device_context, format, &pfd);

        // create and enable the render context (RC)
        os_render_context = wglCreateContext(os_device_context);
        wglMakeCurrent(os_device_context, os_render_context);


        //Initialize timer
        os_use_qpc = QueryPerformanceFrequency(&os_time_frequency);
        if(os_use_qpc)
        {
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            epoch = (uint64_t)((1000LL * now.QuadPart) / os_time_frequency.QuadPart);
        }
        else
        {
            epoch = (uint64_t)GetTickCount();
        }
    }

    void deinit()
    {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(os_render_context);
        ReleaseDC(os_window, os_device_context);
    }

    unsigned long getTime()
    {
        if(os_use_qpc)
        {
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            return (unsigned long)((1000LL * now.QuadPart) / os_time_frequency.QuadPart - epoch);
        }
        else
        {
            return (unsigned long)(GetTickCount() - epoch);
        }
    }

    void setTitle(const char* szTitle)
    {
		utf16* sz = utf8to16(szTitle);
        SetWindowTextW(os_window, (LPCWSTR)sz);
		free(sz);
    }

    void show(ARGS_OS_SHOW_DEF)
    {
        ShowWindow(os_window, nCmdShow);
    }

    void getClipboard(char* b_sz, size_t size)
    {
        if(!b_sz)
			return;
        if(OpenClipboard(os_window))
        {
            HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
            utf16* data16 = (utf16*)GlobalLock(hClipboardData);
			char* data8 = utf16to8(data16);
            strncpy(b_sz, data8, size);
			free(data8);
            GlobalUnlock(hClipboardData);
            CloseClipboard();
        }
        else *b_sz = 0;
    }

    int x_window = 0;
    int y_window = 0;

    void toggleFullscreen()
    {
        if(is_fullscreen)
        {
            //Change the resolution back
            setResolution(true);

            //Restore window border
            LONG style = GetWindowLong(os_window, GWL_STYLE);
            style |= WINDOW_STYLE_WINDOWED;
            SetWindowLong(os_window, GWL_STYLE, style);

            //Restore window position
            SetWindowPos(os_window, HWND_NOTOPMOST, x_window, y_window, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE);

            //Show cursor
            ShowCursor(TRUE);

            is_fullscreen = false;
        }
        else
        {
            //Save window position
            RECT window_pos;
            GetWindowRect(os_window, &window_pos);
            x_window = window_pos.left;
            y_window = window_pos.top;

            //Change the screen resolution
            setResolution(false);

            //Remove the border
            LONG style = GetWindowLong(os_window, GWL_STYLE);
            style &= ~WINDOW_STYLE_WINDOWED;
            SetWindowLong(os_window, GWL_STYLE, style);

            //Set window position
            SetWindowPos(os_window, HWND_TOPMOST, 0, -title_offset, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE);

            //Hide the cursor
            ShowCursor(FALSE);

            is_fullscreen = true;
        }
    }

    void refresh()
    {
        SwapBuffers(os_device_context);

#ifdef GAME
        madotsuki.frameInput = 0;
        poniko.frameInput = 0;
#endif

        MSG _msg;
        while(PeekMessage(&_msg, NULL, 0, 0, PM_REMOVE))
        {
            if(_msg.message == WM_QUIT)
            {
                exit(0);
            }
            else
            {
                TranslateMessage(&_msg);
                DispatchMessage(&_msg);
            }
        }

#ifdef GAME
        Network::refresh();
        Input::refresh();
        Sound::refresh();
#endif
        Graphics::refresh();

        gameFrame++;
    }
}
