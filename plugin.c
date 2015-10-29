/*
The MIT License (MIT)

Copyright (c) 2016 Killian Koenig

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

__declspec(dllexport) int remove_titlebar(long arg)
{
    HWND hwnd = get_hwnd();
    if (!hwnd)
    {
        return 0;
    }

    /* Set the clear color before resizing to avoid seeing any white borders */
    HBRUSH brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG)brush);

    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
    SetWindowLong(hwnd, GWL_STYLE, style);

    DWORD ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
    ex_style &= ~(WS_EX_WINDOWEDGE);
    SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
    force_redraw(hwnd);

    return 1;
}

__declspec(dllexport) int restore_titlebar(long arg)
{
    HWND hwnd = get_hwnd();
    if (!hwnd)
    {
        return 0;
    }

    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    style |= WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
    SetWindowLong(hwnd, GWL_STYLE, style);

    DWORD ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
    ex_style |= WS_EX_WINDOWEDGE;
    SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
    force_redraw(hwnd);

    return 1;
}

static void add_style_flags(HWND hwnd, long flags)
{
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLong(hwnd, GWL_STYLE, style |= flags);
}

static void remove_style_flags(HWND hwnd, long flags)
{
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLong(hwnd, GWL_STYLE, style &= ~flags);
}

static void add_exstyle_flags(HWND hwnd, long flags)
{
    DWORD style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style |= flags);
}

static void remove_exstyle_flags(HWND hwnd, long flags)
{
    DWORD style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style &= ~flags);
}

__declspec(dllexport) int remove_edge(long arg)
{
    HWND child = get_textarea_hwnd();
    HWND parent = get_hwnd();
    HBRUSH brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    SetClassLongPtr(child, GCLP_HBRBACKGROUND, (LONG)brush);
    SetClassLongPtr(parent, GCLP_HBRBACKGROUND, (LONG)brush);

    remove_exstyle_flags(child, WS_EX_CLIENTEDGE);

    force_redraw(child);
    return 0;
}

__declspec(dllexport) int restore_edge(long arg)
{
    HWND child = get_textarea_hwnd();
    HWND parent = get_hwnd();
    HBRUSH brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    SetClassLongPtr(child, GCLP_HBRBACKGROUND, (LONG)brush);
    SetClassLongPtr(parent, GCLP_HBRBACKGROUND, (LONG)brush);

    add_exstyle_flags(child, WS_EX_CLIENTEDGE);

    force_redraw(child);
    return 0;
}

