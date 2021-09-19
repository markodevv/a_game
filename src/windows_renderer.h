/* date = September 19th 2021 10:33 am */

#ifndef WINDOWS_RENDERER_H
#define WINDOWS_RENDERER_H

typedef Renderer2D* (*Win32InitOpenGLProc)(HWND window_handle);
typedef void (*RendererProc)(Renderer2D* ren);


struct Win32RendererFunctions
{
    Win32InitOpenGLProc Win32InitOpenGL;
    RendererProc RendererInit;
    RendererProc RendererDraw;
};

internal char* RendererFunctionNameTable[] =
{
    "Win32InitOpenGL",
    "RendererInit",
    "RendererDraw",
};

#endif //WINDOWS_RENDERER_H
