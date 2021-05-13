#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <math.h>
#include <string.h>
#include <dlfcn.h> // dlopen..

#include <stb_image.h>


#include "common.h"
#include "memory.h"
#include "string.cpp"
#include "log.h"
#include "math.h"
#include "debug.h"
#include "renderer.h"
#include "renderer.cpp"
#include "game.h"
#include "asset_loading.cpp"
#include "opengl_renderer.h"
#include "opengl_renderer.cpp"

// X11 windowing lib
#define Font FontTest
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#undef Font

// typedef XID GLXContextID;
// typedef XID GLXPixmap;
// typedef XID GLXDrawable;
// typedef XID GLXPbuffer;
// typedef XID GLXWindow;
// typedef XID GLXFBConfigID;
// typedef struct __GLXcontextRec *GLXContext;
// typedef struct __GLXFBConfigRec *GLXFBConfig;
// typedef double GLdouble;


// #include "opengl_renderer.h"
// #include "opengl_renderer.cpp"

internal void
free_file_memory(void* memory)
{
    if (memory)
       free(memory);
}


internal FileResult
read_entire_file(char* file_name)
{
    struct stat s;
    FileResult result = {};
    i32 fd;

    if (stat(file_name, &s) == -1)
    {
        printf("Invalid file %s \n", file_name);
        return result;
    }
    
    result.size = s.st_size;
    fd = open(file_name, O_RDONLY);
    if (fd != -1)
    {
        result.data = malloc(s.st_size * sizeof(char));
        if (result.data)
        {
            if ((read(fd, result.data, s.st_size)) != -1)
            {
                // NOTE: file read successfully
            }
            else
            {
                // TODO: logging
                free_file_memory(result.data);
                result.data = NULL;
                perror("read");
                close(fd);

            }
        } 
        else
        {
            // TODO: logging
        }
    }
    else
    {
        // TODO: logging
        perror("open");
    }
    
    
    return result;
}

internal b8
write_entire_file(char* file_name, u32 size, void* memory)
{
    i32 fd = open(file_name, O_CREAT | O_RDWR);
    b8 result = 0;

    if (fd != -1)
    {
        if (write(fd, memory, size) != -1)
        {
            // NOTE: file writen successfully
            printf("written file successfuly \n");
            result = true;
        }
        else
        {
            perror("write");
        }
    }
    else
    {
        perror("open");
    }
    
    close(fd);
    return result;
}

internal void
linux_init_opengl()
{

    void* lib_GL = dlopen("libGL.so", RTLD_LAZY);
    if (!lib_GL) {
        PRINT("libGL.so couldn't be loaded");
    }

    #define LinuxLoadOpenGLFunction(name) \
            name = (name##proc *) dlsym(lib_GL, #name); \
            if (!name) { \
                PRINT("OpenGL function " #name " couldn't be loaded.\n"); \
            }

    LinuxLoadOpenGLFunction(glAttachShader);
    LinuxLoadOpenGLFunction(glBindBuffer);
    LinuxLoadOpenGLFunction(glBindFramebuffer);
    LinuxLoadOpenGLFunction(glBufferData);
    LinuxLoadOpenGLFunction(glBufferSubData);
    LinuxLoadOpenGLFunction(glCheckFramebufferStatus);
    LinuxLoadOpenGLFunction(glClearBufferfv);
    LinuxLoadOpenGLFunction(glCompileShader);
    LinuxLoadOpenGLFunction(glCreateProgram);
    LinuxLoadOpenGLFunction(glCreateShader);
    LinuxLoadOpenGLFunction(glDeleteBuffers);
    LinuxLoadOpenGLFunction(glDeleteFramebuffers);
    LinuxLoadOpenGLFunction(glEnableVertexAttribArray);
    LinuxLoadOpenGLFunction(glDrawBuffers);
    LinuxLoadOpenGLFunction(glFramebufferTexture2D);
    LinuxLoadOpenGLFunction(glGenBuffers);
    LinuxLoadOpenGLFunction(glGenFramebuffers);
    LinuxLoadOpenGLFunction(glGetAttribLocation);
    LinuxLoadOpenGLFunction(glGetShaderInfoLog);
    LinuxLoadOpenGLFunction(glGetShaderiv);
    LinuxLoadOpenGLFunction(glGetUniformLocation);
    LinuxLoadOpenGLFunction(glLinkProgram);
    LinuxLoadOpenGLFunction(glShaderSource);
    LinuxLoadOpenGLFunction(glUniform1i);
    LinuxLoadOpenGLFunction(glUniform1f);
    LinuxLoadOpenGLFunction(glUniform2f);
    LinuxLoadOpenGLFunction(glUniform4f);
    LinuxLoadOpenGLFunction(glUniform1iv);
    LinuxLoadOpenGLFunction(glUniform2iv);
    LinuxLoadOpenGLFunction(glUniform3iv);
    LinuxLoadOpenGLFunction(glUniform4iv);
    LinuxLoadOpenGLFunction(glUniform1fv);
    LinuxLoadOpenGLFunction(glUniform2fv);
    LinuxLoadOpenGLFunction(glUniform3fv);
    LinuxLoadOpenGLFunction(glUniform4fv);
    LinuxLoadOpenGLFunction(glUniformMatrix4fv);
    LinuxLoadOpenGLFunction(glUseProgram);
    LinuxLoadOpenGLFunction(glVertexAttribPointer);
    LinuxLoadOpenGLFunction(glGenVertexArrays);
    LinuxLoadOpenGLFunction(glBindVertexArray);
    LinuxLoadOpenGLFunction(glDeleteShader);
    LinuxLoadOpenGLFunction(glGetProgramInfoLog);
    LinuxLoadOpenGLFunction(glGetProgramiv);
    LinuxLoadOpenGLFunction(glActiveTexture);
    LinuxLoadOpenGLFunction(glGenTextures);
    LinuxLoadOpenGLFunction(glBindTexture);
    LinuxLoadOpenGLFunction(glPixelStorei);
    LinuxLoadOpenGLFunction(glTexParameteri);
    LinuxLoadOpenGLFunction(glTexImage2D);
    LinuxLoadOpenGLFunction(glEnable);
    LinuxLoadOpenGLFunction(glDisable);
    LinuxLoadOpenGLFunction(glBlendFunc);
    LinuxLoadOpenGLFunction(glClearColor);
    LinuxLoadOpenGLFunction(glClear);
    LinuxLoadOpenGLFunction(glViewport);
    LinuxLoadOpenGLFunction(glDrawArrays);
    LinuxLoadOpenGLFunction(glDrawElements);
}


i32 
main()
{
    Window window;
    XEvent event;
    Display *display = 0;

    char* default_display = getenv("DISPLAY");

    if (default_display)
    {
        display = XOpenDisplay(default_display);
    }
    else
    {
        PRINT("Couldn't open X11 window.");
    }
    
    if (!display)
    {
        display = XOpenDisplay(NULL);
    }

    window = XCreateSimpleWindow(display, DefaultRootWindow(display), 10, 10, 100, 100, 1,
                                 BlackPixel(display, DefaultScreen(display)), 
                                 WhitePixel(display, DefaultScreen(display)));
    XSelectInput(display, window, ExposureMask | StructureNotifyMask |
                 KeyReleaseMask | PointerMotionMask | ButtonPressMask |
                 ButtonReleaseMask);
    XMapWindow(display, window);
    
    GameMemory game_memory = {};
    game_memory.permanent_storage_size = Megabytes(64);
    game_memory.temporary_storage_size = Megabytes(128);

    game_memory.is_initialized = false;
    game_memory.permanent_storage = calloc(game_memory.permanent_storage_size, sizeof(u8));
    game_memory.temporary_storage = calloc(game_memory.temporary_storage_size, sizeof(u8));

    ASSERT(game_memory.permanent_storage);
    ASSERT(game_memory.temporary_storage);

    game_memory.platform.read_entire_file = read_entire_file;
    game_memory.platform.write_entire_file = write_entire_file;
    game_memory.platform.free_file_memory = free_file_memory;

    // GameInput game_input = {};
    char msg[] = "Hello world, Linux!";

    while (1)
    {

        XNextEvent(display, &event);

        if (event.type == Expose) {
            XFillRectangle(display, window, DefaultGC(display, DefaultScreen(display)), 20, 20, 10, 10);
            XDrawString(display, window, DefaultGC(display, DefaultScreen(display)), 10, 50, msg, strlen(msg));
        }
        if (event.type == KeyPress)
            break;
    }

    XCloseDisplay(display);
    return 0;
}
