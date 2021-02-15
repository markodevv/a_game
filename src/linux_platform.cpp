#define internal static 
#define global_variable static 
#define local_persist static

#include <stdint.h>
#include <stddef.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef i8 b8;
typedef size_t sizet;


#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>


#include "game.h"
#include "game.cpp"

#include "renderer.h"
#include "opengl_renderer.cpp"


internal DebugFileResult
DEBUG_read_entire_file(char* file_name)
{
    struct stat s;
    DebugFileResult result = {};
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
                DEBUG_free_file_memory(result.data);
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

internal void
DEBUG_free_file_memory(void* memory)
{
    if (memory)
       free(memory);
}

internal b8
DEBUG_write_entire_file(char* file_name, sizet size, void* memory)
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

internal GLFWwindow*
glfw_create_window()
{
    GLFWwindow* window = NULL;
    
    if (glfwInit())
    {
        window = glfwCreateWindow(800, 600, "A GEMM", NULL, NULL);
        if (window)
        {
            glfwMakeContextCurrent(window);
        }
        else
        {
            // TODO: logging
            ASSERT(false, "failed to create window");
            glfwTerminate();
        }
    }
    else
    {
        // TODO: logging
    }

    return window;
}

internal void
framebuffer_size_callback(GLFWwindow* Window, i32 w, i32 h)
{
    glViewport(0, 0, w, h);
}


i32 main()
{

    GameMemory game_memory = {};
    game_memory.permanent_storage_size = MEGABYTES(64);
    game_memory.temporary_storage_size = MEGABYTES(10);
    i32 total_size = game_memory.permanent_storage_size + game_memory.temporary_storage_size;

    game_memory.permanent_storage = malloc(sizeof(u8) * total_size);

    game_memory.temporary_storage =
        ((u8*)game_memory.permanent_storage +
         game_memory.permanent_storage_size);

    ASSERT(game_memory.permanent_storage, "Game memory allocation failed");
    
    GLFWwindow* window = glfw_create_window();
    ASSERT(window != 0, "Failed to create window");

    renderer_initialize();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  


    while(!glfwWindowShouldClose(window))
    {
        game_update_and_render(&game_memory);
            
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
        
}
