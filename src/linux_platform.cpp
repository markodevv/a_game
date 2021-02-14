#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

#include "renderer.h"
#include "opengl_renderer.cpp"

#include "main.h"
#include "main.cpp"


internal void*
DEBUG_read_entire_file(const char* file_name)
{
    struct stat s;
    void* buffer = NULL;
    i32 fd;

    if (stat(file_name, &s) == -1)
    {
        printf("Invalid file %s \n", file_name);
        return NULL;
    }
    
    
    fd = open(file_name, O_RDONLY);
    if (fd != -1)
    {
        buffer = malloc(s.st_size * sizeof(char));
        if (buffer)
        {
            if ((read(fd, buffer, s.st_size)) != -1)
            {
                // NOTE: file read successfully
            }
            else
            {
                // TODO: logging
                DEBUG_free_file_memory(buffer);
                buffer = NULL;
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
    
    
    return buffer;
}

internal void
DEBUG_free_file_memory(void* memory)
{
    if (memory)
       free(memory);
}

internal b8
DEBUG_write_entire_file(const char* file_name, sizet size, void* memory)
{
    i32 fd = open(file_name, O_WRONLY);
    b8 result = true;

    if (fd != -1)
    {
        if (write(fd, memory, size) != -1)
        {
            // NOTE: file writen successfully
        }
        else
        {
            perror("write");
            result = false;
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
            ASSERT(false, "failed to create windiw");
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

    void* file_data = DEBUG_read_entire_file("penguin.bmp");
    
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
