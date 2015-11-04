/*
The MIT License (MIT)

Copyright (c) 2015 Killian Koenig

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "Windows.h"

static BOOL CALLBACK enum_windows_proc(
        _In_ HWND hwnd,
        _In_ LPARAM lparam)
{
    DWORD process;
    if (IsWindowVisible(hwnd) &&
        GetWindowThreadProcessId(hwnd, &process) &&
        process == GetCurrentProcessId())
    {
        *((HWND*)lparam) = hwnd;
        return FALSE;
    }

    return TRUE;
}

static BOOL CALLBACK enum_child_windows_proc(
        _In_ HWND hwnd,
        _In_ LPARAM lparam)
{
    char class_name[MAX_PATH];
    if (!GetClassNameA(hwnd, class_name, sizeof(class_name) / sizeof(class_name[0])) ||
        strcmp(class_name, "VimTextArea"))
    {
        return TRUE;
    }

    *((HWND*)lparam) = hwnd;
    return FALSE;
}

static HWND get_hwnd(void)
{
    HWND hwnd = NULL;
    EnumWindows(&enum_windows_proc, (LPARAM)&hwnd);
    return hwnd;
}

static HWND get_textarea_hwnd(void)
{
    HWND hwnd = get_hwnd();
    if (hwnd == NULL)
    {
        return NULL;
    }

    HWND child = NULL;
    EnumChildWindows(hwnd, &enum_child_windows_proc, (LPARAM)&child);
    return child;
}

static void force_redraw(HWND hwnd)
{
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOSIZE);
}

#define ASSERT_TRUE(condition) do { int value = !(condition);  if (value) { goto error; } } while(0)

static int adjust_exstyle_flags(HWND hwnd, long flags, int predicate)
{
    DWORD style = GetWindowLong(hwnd, GWL_EXSTYLE);
    ASSERT_TRUE(style || !GetLastError());

    if (predicate)
    {
        style |= flags;
    }
    else
    {
        style &= ~flags;
    }

    /* Error code for SetWindowLong is ambiguous see:
     * https://msdn.microsoft.com/en-us/library/windows/desktop/ms633591(v=vs.85).aspx */
    SetLastError(0);
    ASSERT_TRUE(
        SetWindowLong(hwnd, GWL_EXSTYLE, style) || !GetLastError());

    return 1;

error:
    return 0;
}

static int adjust_style_flags(HWND hwnd, long flags, int predicate)
{
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    ASSERT_TRUE(style || !GetLastError());

    if (predicate)
    {
        style |= flags;
    }
    else
    {
        style &= ~flags;
    }

    /* Error code for SetWindowLong is ambiguous see:
     * https://msdn.microsoft.com/en-us/library/windows/desktop/ms633591(v=vs.85).aspx */
    SetLastError(0);
    ASSERT_TRUE(
        SetWindowLong(hwnd, GWL_STYLE, style) || !GetLastError());

    return 1;

error:
    return 0;
}

__declspec(dllexport) int set_alpha(long arg)
{
    arg = min(arg, 0xFF);
    arg = max(arg, 0x00);

    HWND hwnd;
    ASSERT_TRUE(hwnd = get_hwnd());

    /* WS_EX_LAYERED must be set if there is any transparency */
    ASSERT_TRUE(adjust_exstyle_flags(hwnd, WS_EX_LAYERED, arg != 0xFF));
    ASSERT_TRUE(SetLayeredWindowAttributes(hwnd, 0, (BYTE)(arg), LWA_ALPHA));

    return 1;

error:
    return 0;
}

static int set_window_style(int is_clean_enabled, int arg)
{
    /* TODO : Don't leak brush */
    HBRUSH brush;
    ASSERT_TRUE(brush = CreateSolidBrush(RGB((arg >> 16) & 0xFF, (arg >> 8) & 0xFF, arg & 0xFF)));

    HWND child;
    ASSERT_TRUE(child = get_textarea_hwnd());
    ASSERT_TRUE(SetClassLongPtr(child, GCLP_HBRBACKGROUND, (LONG)brush));
    HWND parent;
    ASSERT_TRUE(parent = get_hwnd());
    ASSERT_TRUE(SetClassLongPtr(parent, GCLP_HBRBACKGROUND, (LONG)brush));

    /* TODO : Check return value */
    adjust_exstyle_flags(child, WS_EX_CLIENTEDGE, is_clean_enabled);

    force_redraw(child);

    return 1;

error:
    return 0;

}

static int set_fullscreen(int should_be_fullscreen, int color)
{
    HWND parent;
    ASSERT_TRUE(parent = get_hwnd());

    set_window_style(should_be_fullscreen, color);
    adjust_style_flags(parent, WS_CAPTION |  WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX, should_be_fullscreen);

    force_redraw(parent);

    return 1;

error:
    return 0;
}

__declspec(dllexport) int set_fullscreen_on(long arg)
{
    return set_fullscreen(1, arg);
}

__declspec(dllexport) int set_fullscreen_off(long arg)
{
    return set_fullscreen(0, arg);
}

__declspec(dllexport) int set_window_style_clean(long arg)
{
    return set_window_style(1, arg);
}

__declspec(dllexport) int set_window_style_default(long arg)
{
    return set_window_style(0, arg);
}

