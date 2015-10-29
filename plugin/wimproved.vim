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

if exists('g:loaded_fullscreen_windows') || &compatible
"    finish
else
    let g:loaded_fullscreen_windows = 1
endif

let g:vfw_dllpath = expand('%:p') . "/../../Build/vim-fullscreen-windows.dll"
echom g:vfw_dllpath
function! RestoreEdge()
    echo libcallnr(g:vfw_dllpath, "restore_edge", 0)
endfunction

function! RemoveEdge()
    echo libcallnr(g:vfw_dllpath, "remove_edge", 0)
endfunction

function! RestoreTitlebar()
    echo libcallnr(g:vfw_dllpath, "restore_titlebar", 0)
endfunction

function! RemoveTitlebar()
    echo libcallnr(g:vfw_dllpath, "remove_titlebar", 0)
endfunction

nnoremap <Leader>b :call RemoveEdge()<ENTER>
nnoremap <Leader>n :call RestoreEdge()<ENTER>

nnoremap <Leader>g :call RemoveTitlebar()<ENTER>
nnoremap <Leader>h :call RestoreTitlebar()<ENTER>

echo "Reloaded!"

