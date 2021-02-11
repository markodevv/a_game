#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdint.h>

#define internal static 
#define global_variable static 
#define local_persist static

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

internal void
framebuffer_size_callback(GLFWwindow* window, i32 width, i32 height) {
	
	glViewport(0, 0, width, height);
}

global_variable const char* vertex_shader_source = "#version 330 core\n"
    "layout (location = 0) in vec3 att_pos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(att_pos.x, att_pos.y, att_pos.z, 1.0);\n"
    "}\0";

global_variable const char* fragment_shader_source = "#version 330 core\n"
    "out vec4 frag_color;\n"
	"void main()\n"
    "{\n"
    "   frag_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";


internal void
create_shaders(i32 shader_program) {
	
	i32 success;
	char info_log[512];


	u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

	if (!success)  {

		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		printf("[ERROR] Failed to compile vertex shader: \n%s \n", info_log);
	}

	u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

	if (!success)  {

		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		printf("[ERROR] Failed to compile fragment shader: \n%s \n", info_log);
	}

	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		printf("[ERROR] Failed to link shader \n%s \n", info_log);
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}


i32 main() {

	GLFWwindow* window;

    if (!glfwInit())
        return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    window = glfwCreateWindow(800, 600, "A GEMM", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD");
		return -1;
	} 
	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

	u32 shader_program = glCreateProgram();

	create_shaders(shader_program);
	glUseProgram(shader_program);

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	}; 

	u32 VBO, VAO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); 

    while (!glfwWindowShouldClose(window)) {


		glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
