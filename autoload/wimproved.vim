" The MIT License (MIT)
"
" Copyright (c) 2015 Killian Koenig
"
" Permission is hereby granted, free of charge, to any person obtaining a copy
" of this software and associated documentation files (the \"Software\"), to deal
" in the Software without restriction, including without limitation the rights
" to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
" copies of the Software, and to permit persons to whom the Software is
" furnished to do so, subject to the following conditions:
"
" The above copyright notice and this permission notice shall be included in
" all copies or substantial portions of the Software.
"
" THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
" IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
" FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
" AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
" LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
" OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
" THE SOFTWARE.
if exists('g:loaded_wimproved') || &compatible
    finish
else
    let g:loaded_wimproved = 1
endif

let s:dll_path = fnamemodify(resolve(expand('<sfile>:p')), ':h') . '/../Build/wimproved.dll'

function! s:get_background_color()
    let l:s = synIDattr(hlID('Normal'), 'bg#')
    if !len(l:s)
        return 0
    endif

    return str2nr(strpart(l:s, 1), 16) " Skip over the #
endfunction

function! wimproved#set_alpha(alpha)
    call libcallnr(s:dll_path, 'set_alpha', str2nr(a:alpha))
endfunction

function! wimproved#set_monitor_center()
    call libcallnr(s:dll_path, 'set_monitor_center', 0)
endfunction

let s:clean_window_style_on = 0
function! wimproved#toggle_clean()
    if !s:clean_window_style_on
        call libcallnr(s:dll_path, 'set_window_style_clean', s:get_background_color())
    else
        call libcallnr(s:dll_path, 'set_window_style_default', s:get_background_color())
    endif
    let s:clean_window_style_on = !s:clean_window_style_on
endfunction

let s:fullscreen_on = 0
function! wimproved#toggle_fullscreen()
    if !s:fullscreen_on
        call libcallnr(s:dll_path, 'set_fullscreen_on', s:get_background_color())
    else
        call libcallnr(s:dll_path, 'set_fullscreen_off', s:get_background_color())
    endif
    let s:fullscreen_on = !s:fullscreen_on
endfunction


