#include <windows.h>
#include <glad/glad.h>
#include <wingdi.h>
#include <math.h>
#include <stdio.h>

#define DLL_FILE

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


#include "log.h"
#include "platform.h"
#include "generated/memory.h"
#include "memory.cpp"
#include "string.cpp"
#include "generated/math.h"
#include "math.cpp"
#include "file_parse.cpp"
#include "generated/renderer.h"
#include "asset.cpp"
#include "renderer.cpp"
#include "opengl_renderer.h"
#include "opengl_renderer.cpp"

#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001

#define GLDECL
#define OpenGLProc(ret, name, ...) typedef ret GLDECL name##proc(__VA_ARGS__); name##proc*  name
OpenGLProc(HGLRC, wglCreateContextAttribsARB, HDC hdc, HGLRC hShareContext, const int* attribs);

#define Win32LoadOpenGLFunction(name) \
name = (name##proc *)wglGetProcAddress(#name); \
if (!name) \
{ \
HMODULE module = LoadLibraryA("opengl32.dll"); \
name = (name##proc *)GetProcAddress(module, #name); \
if (!name) \
{ \
LogM("OpenGL function " #name " couldn't be loaded.\n"); \
} \
} 

extern "C" PLATFORM_API Renderer2D*
Win32InitOpenGL(HWND window_handle)
{
    
    HDC window_dc = GetDC(window_handle);
    
    PIXELFORMATDESCRIPTOR pixel_format = {};
    pixel_format.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pixel_format.nVersion = 1;
    pixel_format.iPixelType = PFD_TYPE_RGBA;
    pixel_format.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
    pixel_format.cColorBits = 32;
    pixel_format.cAlphaBits = 8;
    pixel_format.iLayerType = PFD_MAIN_PLANE;
    
    i32 suggest_pf_index = ChoosePixelFormat(window_dc, &pixel_format);
    
    PIXELFORMATDESCRIPTOR suggested_pixel_format = {};
    DescribePixelFormat(window_dc, suggest_pf_index, sizeof(suggested_pixel_format), &pixel_format);
    SetPixelFormat(window_dc, suggest_pf_index, &suggested_pixel_format);
    
    i32 attrib_list[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };
    HGLRC opengl_rc = wglCreateContext(window_dc);
    if (opengl_rc)
    {
        if (!wglMakeCurrent(window_dc, opengl_rc))
        {
            Assert(false);
        }
    }
    else
    {
        Assert(false);
    }
    
    Win32LoadOpenGLFunction(wglCreateContextAttribsARB);
    
    opengl_rc = wglCreateContextAttribsARB(window_dc, 0, attrib_list);
    
    if (opengl_rc)
    {
        if (wglMakeCurrent(window_dc, opengl_rc))
        {
        }
        else
        {
            LogM("Failed to set current 3.0+ OpenGL context!\n");
            Assert(false);
        }
    }
    else
    {
        LogM("Failed to create 3.0+ OpenGL context!\n");
        Assert(false);
    }
    
    
    // NOTE: Vsync
    ((BOOL(WINAPI*)(int))wglGetProcAddress("wglSwapIntervalEXT"))(1);
    
    
    OpenGL* opengl = (OpenGL*)calloc(1, sizeof(OpenGL));
    
#define SetOpenGLFunction(name) \
opengl->name = gl##name
    
    SetOpenGLFunction(AttachShader);
    SetOpenGLFunction(BindBuffer);
    SetOpenGLFunction(BindFramebuffer);
    SetOpenGLFunction(BufferData);
    SetOpenGLFunction(BufferSubData);
    SetOpenGLFunction(CheckFramebufferStatus);
    SetOpenGLFunction(ClearBufferfv);
    SetOpenGLFunction(CompileShader);
    SetOpenGLFunction(CreateProgram);
    SetOpenGLFunction(CreateShader);
    SetOpenGLFunction(DeleteBuffers);
    SetOpenGLFunction(DeleteFramebuffers);
    SetOpenGLFunction(EnableVertexAttribArray);
    SetOpenGLFunction(DrawBuffers);
    SetOpenGLFunction(FramebufferTexture2D);
    SetOpenGLFunction(GenBuffers);
    SetOpenGLFunction(GenFramebuffers);
    SetOpenGLFunction(GetAttribLocation);
    SetOpenGLFunction(GetShaderInfoLog);
    SetOpenGLFunction(GetShaderiv);
    SetOpenGLFunction(GetUniformLocation);
    SetOpenGLFunction(LinkProgram);
    SetOpenGLFunction(ShaderSource);
    SetOpenGLFunction(Uniform1i);
    SetOpenGLFunction(Uniform1f);
    SetOpenGLFunction(Uniform2f);
    SetOpenGLFunction(Uniform4f);
    SetOpenGLFunction(Uniform1iv);
    SetOpenGLFunction(Uniform2iv);
    SetOpenGLFunction(Uniform3iv);
    SetOpenGLFunction(Uniform4iv);
    SetOpenGLFunction(Uniform1fv);
    SetOpenGLFunction(Uniform2fv);
    SetOpenGLFunction(Uniform3fv);
    SetOpenGLFunction(Uniform4fv);
    SetOpenGLFunction(UniformMatrix4fv);
    SetOpenGLFunction(UseProgram);
    SetOpenGLFunction(VertexAttribPointer);
    SetOpenGLFunction(GenVertexArrays);
    SetOpenGLFunction(BindVertexArray);
    SetOpenGLFunction(DeleteShader);
    SetOpenGLFunction(GetProgramInfoLog);
    SetOpenGLFunction(GetProgramiv);
    SetOpenGLFunction(ActiveTexture);
    SetOpenGLFunction(GetActiveUniform);
    //SetOpenGLFunction(GetActiveUniformBlockiv);
    //SetOpenGLFunction(GetActiveUniformName);
    SetOpenGLFunction(GenTextures);
    SetOpenGLFunction(BindTexture);
    SetOpenGLFunction(PixelStorei);
    SetOpenGLFunction(TexParameteri);
    SetOpenGLFunction(TexImage2D);
    SetOpenGLFunction(Enable);
    SetOpenGLFunction(Disable);
    SetOpenGLFunction(BlendFunc);
    SetOpenGLFunction(DepthFunc);
    SetOpenGLFunction(ClearColor);
    SetOpenGLFunction(Clear);
    SetOpenGLFunction(Viewport);
    SetOpenGLFunction(DrawArrays);
    SetOpenGLFunction(DrawElements);
    
    
    ReleaseDC(window_handle, window_dc);
    if (!gladLoadGL()) 
    {
        LogM("Failed to initialize OpenGL context");
    }
    /*

if (!gladLoadGLLoader((GLADloadproc)wglGetProcAddress)) {
    LogM("Failed to initialize OpenGL context");
}
// Alternative use the builtin loader, e.g. if no other loader function is available
if (!gladLoadGL()) {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
    return -1;
}
*/
    
    // glad populates global constants after loading to indicate,
    // if a certain extension/version is available.
    LogM("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
    
    if(GLAD_GL_VERSION_3_0) 
    {
        /* We support at least OpenGL version 3 */
        LogM("OpenGL 3.0 is supported.\n");
    }
    
    return (Renderer2D*)opengl;
}
