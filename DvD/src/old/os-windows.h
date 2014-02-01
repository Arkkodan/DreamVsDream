//No header guards for a reason

#ifndef WINVER
#define WINVER 0x0500
#endif
#include <windows.h>

#define MAIN int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#define INIT void init(HINSTANCE hInstance, int nCmdShow)
void init(HINSTANCE, int);
#define ARGS_INIT hInstance, nCmdShow

#define ARGS_OS_INIT_DEF HINSTANCE hInstance
#define ARGS_OS_SHOW_DEF int nCmdShow
#define ARGS_OS_INIT hInstance
#define ARGS_OS_SHOW nCmdShow

#define WINDOW_CLASS "ARCAMECH"
#define WINDOW_STYLE_WINDOWED (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define WINDOW_STYLE (WS_OVERLAPPED | WINDOW_STYLE_WINDOWED)
#define WINDOW_STYLE_EX (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE)

#define SLEEP(x) Sleep(x)

#define OS_KEY_LEFT     VK_LEFT
#define OS_KEY_RIGHT    VK_RIGHT
#define OS_KEY_UP       VK_UP
#define OS_KEY_DOWN     VK_DOWN
#define OS_KEY_A        0x41
#define OS_KEY_B        0x42
#define OS_KEY_C        0x43
#define OS_KEY_D        0x44
#define OS_KEY_E        0x45
#define OS_KEY_F        0x46
#define OS_KEY_G        0x47
#define OS_KEY_H        0x48
#define OS_KEY_I        0x49
#define OS_KEY_J        0x4A
#define OS_KEY_K        0x4B
#define OS_KEY_L        0x4C
#define OS_KEY_M        0x4D
#define OS_KEY_N        0x4E
#define OS_KEY_O        0x4F
#define OS_KEY_P        0x50
#define OS_KEY_Q        0x51
#define OS_KEY_R        0x52
#define OS_KEY_S        0x53
#define OS_KEY_T        0x54
#define OS_KEY_U        0x55
#define OS_KEY_V        0x56
#define OS_KEY_W        0x57
#define OS_KEY_X        0x58
#define OS_KEY_Y        0x59
#define OS_KEY_Z        0x5A
#define OS_KEY_CTRL     VK_CONTROL
#define OS_KEY_SHIFT    VK_SHIFT
#define OS_KEY_ADD      VK_ADD
#define OS_KEY_SUBTRACT VK_SUBTRACT
#define OS_KEY_RETURN   VK_RETURN
#define OS_KEY_MULTIPLY VK_MULTIPLY
#define OS_KEY_DIVIDE   VK_DIVIDE
#define OS_KEY_COMMA    VK_OEM_COMMA
#define OS_KEY_SLASH    VK_OEM_2
#define OS_KEY_F1       VK_F1
#define OS_KEY_F2       VK_F2
#define OS_KEY_F3       VK_F3
#define OS_KEY_F4       VK_F4
#define OS_KEY_F5       VK_F5
#define OS_KEY_F6       VK_F6
#define OS_KEY_F7       VK_F7
#define OS_KEY_F8       VK_F8
#define OS_KEY_F9       VK_F9
#define OS_KEY_F10      VK_F10
#define OS_KEY_F11      VK_F11
#define OS_KEY_F12      VK_F12
