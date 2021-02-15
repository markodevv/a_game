#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"

global_variable GLFWwindow* Window;

global_variable const char* vertex_shader_source = "#version 330 core\n"
    "layout (location = 0) in vec3 att_pos;\n"
    "layout (location = 1) in vec4 att_color;\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(att_pos.x, att_pos.y, att_pos.z, 1.0);\n"
    "   color = att_color;\n"
    "}\0";

global_variable const char* fragment_shader_source = "#version 330 core\n"
    "out vec4 frag_color;\n"
    "in vec4 color;\n"
    "void main()\n"
    "{\n"
    "   frag_color = color;\n"
    "}\0";



internal void
create_shaders(i32 shader_program)
{
    i32 success;
    char info_log[512];


    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {

        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        printf("[ERROR] Failed to compile vertex shader: \n%s \n", info_log);
    }

    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {

        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        printf("[ERROR] Failed to compile fragment shader: \n%s \n", info_log);
    }

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        printf("[ERROR] Failed to link shader \n%s \n", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void
renderer_clear()
{
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


void
renderer_draw()
{
    glDrawArrays(GL_TRIANGLES, 0, 3);
}


void
renderer_initialize()
{
    u32 VBO, VAO, shader_program;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
    }

    glViewport(0, 0, 800, 600);

    shader_program = glCreateProgram();

    create_shaders(shader_program);
    glUseProgram(shader_program);

    f32 vertex_data[] = {
                          -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                           0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
                           0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f
    }; 


    

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0); 

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(f32), (void*)(3 * sizeof(f32)));
    glEnableVertexAttribArray(1); 
    glfwSwapInterval(0);

}
