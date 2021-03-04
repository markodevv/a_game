#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <windows.h>
#include <wingdi.h>
#include <dsound.h>
#include <math.h>
#include <gl/gl.h>

#include "game.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "math.h"

#define GLDECL WINAPI
typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

#define GL_ARRAY_BUFFER                   0x8892 // Acquired from:
#define GL_ARRAY_BUFFER_BINDING           0x8894 // https://www.opengl.org/registry/api/GL/glext.h
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COMPILE_STATUS                 0x8B81
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
#define GL_LINK_STATUS                    0x8B82
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_FRAMEBUFFER                    0x8D40
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FUNC_ADD                       0x8006
#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_STATIC_DRAW                    0x88E4
#define GL_STREAM_DRAW                    0x88E0
#define GL_TEXTURE0                       0x84C0
#define GL_VERTEX_SHADER                  0x8B31

#define GL_FUNCTION_LIST \
    LOAD_GL_FUNCTION(void,      AttachShader,            GLuint program, GLuint shader) \
    LOAD_GL_FUNCTION(void,      BindBuffer,              GLenum target, GLuint buffer) \
    LOAD_GL_FUNCTION(void,      BindFramebuffer,         GLenum target, GLuint framebuffer) \
    LOAD_GL_FUNCTION(void,      BufferData,              GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage) \
    LOAD_GL_FUNCTION(void,      BufferSubData,           GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data) \
    LOAD_GL_FUNCTION(GLenum,    CheckFramebufferStatus,  GLenum target) \
    LOAD_GL_FUNCTION(void,      ClearBufferfv,           GLenum buffer, GLint drawbuffer, const GLfloat * value) \
    LOAD_GL_FUNCTION(void,      CompileShader,           GLuint shader) \
    LOAD_GL_FUNCTION(GLuint,    CreateProgram,           void) \
    LOAD_GL_FUNCTION(GLuint,    CreateShader,            GLenum type) \
    LOAD_GL_FUNCTION(void,      DeleteBuffers,           GLsizei n, const GLuint *buffers) \
    LOAD_GL_FUNCTION(void,      DeleteFramebuffers,      GLsizei n, const GLuint *framebuffers) \
    LOAD_GL_FUNCTION(void,      EnableVertexAttribArray, GLuint index) \
    LOAD_GL_FUNCTION(void,      DrawBuffers,             GLsizei n, const GLenum *bufs) \
    LOAD_GL_FUNCTION(void,      FramebufferTexture2D,    GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) \
    LOAD_GL_FUNCTION(void,      GenBuffers,              GLsizei n, GLuint *buffers) \
    LOAD_GL_FUNCTION(void,      GenFramebuffers,         GLsizei n, GLuint * framebuffers) \
    LOAD_GL_FUNCTION(GLint,     GetAttribLocation,       GLuint program, const GLchar *name) \
    LOAD_GL_FUNCTION(void,      GetShaderInfoLog,        GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) \
    LOAD_GL_FUNCTION(void,      GetShaderiv,             GLuint shader, GLenum pname, GLint *params) \
    LOAD_GL_FUNCTION(GLint,     GetUniformLocation,      GLuint program, const GLchar *name) \
    LOAD_GL_FUNCTION(void,      LinkProgram,             GLuint program) \
    LOAD_GL_FUNCTION(void,      ShaderSource,            GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length) \
    LOAD_GL_FUNCTION(void,      Uniform1i,               GLint location, GLint v0) \
    LOAD_GL_FUNCTION(void,      Uniform1f,               GLint location, GLfloat v0) \
    LOAD_GL_FUNCTION(void,      Uniform2f,               GLint location, GLfloat v0, GLfloat v1) \
    LOAD_GL_FUNCTION(void,      Uniform4f,               GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) \
    LOAD_GL_FUNCTION(void,      UniformMatrix4fv,        GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) \
    LOAD_GL_FUNCTION(void,      UseProgram,              GLuint program) \
    LOAD_GL_FUNCTION(void,      VertexAttribPointer,     GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer) \
    LOAD_GL_FUNCTION(void,      GenVertexArrays,         GLsizei n, GLuint *arrays) \
    LOAD_GL_FUNCTION(void,      BindVertexArray,         GLuint array) \
    LOAD_GL_FUNCTION(void,      DeleteShader,            GLuint shader) \
    LOAD_GL_FUNCTION(void,      GetProgramInfoLog,      GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) \
    LOAD_GL_FUNCTION(void,      GetProgramiv,            GLuint program, GLenum pname, GLint *params) \


#define LOAD_GL_FUNCTION(ret, name, ...) typedef ret GLDECL name##proc(__VA_ARGS__); extern name##proc * gl##name;
GL_FUNCTION_LIST
#undef LOAD_GL_FUNCTION


global_variable const char* vertex_shader_source =
R"(
#version 330 core
layout (location = 0) in vec3 att_pos;
layout (location = 1) in vec4 att_color;
layout (location = 2) in vec2 att_texture;

out vec4 color;
out vec2 tex;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
   gl_Position = u_projection * u_model * vec4(att_pos, 1.0f);
   color = att_color;
   tex = att_texture;
}
)";

global_variable const char* fragment_shader_source =
R"(
#version 330 core

out vec4 frag_color;

in vec4 color;
in vec2 tex;

uniform sampler2D u_texture;

void main()
{
    frag_color = texture(u_texture, tex);
}
)";

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
        ALERT_MSG("Failed to compile vertex shader: \n%s \n", info_log);
    }

    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        ALERT_MSG("Failed to compile fragment shader: \n%s \n", info_log);
    }

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        ALERT_MSG("Failed to link shader \n%s \n", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter);
typedef DIRECT_SOUND_CREATE(DSC);

global_variable LPDIRECTSOUNDBUFFER global_sound_buffer;

inline internal u32
safe_truncate_u64(u64 value)
{
    ASSERT(value <= 0xFFFFFFFF);
    u32 result = (u32)value;
    return result;
}

internal DebugFileResult
DEBUG_read_entire_file(char* file_name)
{
     DebugFileResult result = {};
    
    HANDLE file_handle = (file_name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(file_handle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER file_size;
        if(GetFileSizeEx(file_handle, &file_size))
        {
            u32 file_size_32 = safe_truncate_u64(file_size.QuadPart);
            result.data = malloc(sizeof(i8) * file_size_32);
            if(result.data)
            {
                DWORD bytes_read;
                if(ReadFile(file_handle, result.data, file_size_32, &bytes_read, 0) &&
                   (file_size_32 == bytes_read))
                {
                    // File read successfully
                    result.size = file_size_32;
                }
                else
                {                    
                    DEBUG_free_file_memory(result.data);
                    result.data = 0;
                }
            }
            else
            {
                ALERT_MSG("File malloc failed for file %s. \n", file_name);
            }
        }
        else
        {
        }

        CloseHandle(file_handle);
    }
    else
    {
        ALERT_MSG("Trying to open invalid file %s. \n", file_name);
    }

    return result;
}

internal void
DEBUG_free_file_memory(void* memory)
{
    if (memory)
    {
        free(memory);
    }
}

internal b8
DEBUG_write_entire_file(char* file_name, DWORD size, void* memory)
{
    b8 result = false;
    
    HANDLE file_handle = CreateFileA(file_name, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if(file_handle != INVALID_HANDLE_VALUE)
    {
        DWORD bytes_written;
        if(WriteFile(file_handle, memory, size, &bytes_written, 0))
        {
            result = (bytes_written == size);
        }
        else
        {
            WARN_MSG("Failed to write entire file: %s.\n", file_name);
        }

        CloseHandle(file_handle);
    }
    else
    {
        WARN_MSG("Invalid file handle \n");
    }

    return result;
}





internal void
win32_init_dsound(HWND window, i32 samples_per_sec, i32 buffer_size)
{

   LPDIRECTSOUND8 direct_sound;
   HRESULT status = DirectSoundCreate8(0, &direct_sound, 0);

   if (!FAILED(status))
   {
        if (!FAILED(direct_sound->SetCooperativeLevel(window, DSSCL_PRIORITY)))
        {

            WAVEFORMATEX wave_format = {};
            wave_format.wFormatTag = WAVE_FORMAT_PCM;
            wave_format.nChannels = 2;
            wave_format.nSamplesPerSec = samples_per_sec;
            wave_format.wBitsPerSample = 16;
            wave_format.nBlockAlign = (wave_format.nChannels * wave_format.wBitsPerSample) / 8;
            wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;
            wave_format.cbSize = 0;

            DSBUFFERDESC buffer_description = {};
            buffer_description.dwSize = sizeof(buffer_description);
            buffer_description.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
            buffer_description.dwBufferBytes = buffer_size;
            buffer_description.lpwfxFormat = &wave_format;

            if (!FAILED(direct_sound->CreateSoundBuffer(&buffer_description, &global_sound_buffer, 0)))
            {
                NORMAL_MSG("Created sound buffer. \n");
            }
        }
    }
    else
    {
        ALERT_MSG("Failed to create sound buffer. \n");
    }
}


struct Win32SoundState
{
    i32 buffer_size;
    u32 running_sample_count;
    i32 latency_sample_count;
    i32 samples_per_sec;
    i32 bytes_per_sample;
};

internal b8
win32_clear_sound_buffer(Win32SoundState* sound_output)
{
    VOID* region_1;
    DWORD region_1_size;
    VOID* region_2;
    DWORD region_2_size;
    if (SUCCEEDED(global_sound_buffer->Lock(0,
                                            sound_output->buffer_size,
                                            &region_1, 
                                            &region_1_size, 
                                            &region_2, 
                                            &region_2_size, 
                                            0)))
    {
        u8* dest_sample = (u8*)region_1;

        for (sizet sample_index = 0;
            sample_index < region_1_size;
            ++sample_index)
        {
            *dest_sample = 0;
            dest_sample++;
        }

        dest_sample = (u8*)region_2;
        for (sizet sample_index = 0;
            sample_index < region_2_size;
            ++sample_index)
        {
            *dest_sample = 0;
            dest_sample++;
        }

        global_sound_buffer->Unlock(region_1, region_1_size, region_2, region_2_size);
        return true;
    }
    return false;
}

internal void
win32_fill_sound_buffer(Win32SoundState* sound_output, GameSoundBuffer* sound_buffer, DWORD byte_offset, DWORD bytes_to_write)
{
    VOID* region_1;
    DWORD region_1_size;
    VOID* region_2;
    DWORD region_2_size;

    if (SUCCEEDED(global_sound_buffer->Lock(byte_offset, 
                                            bytes_to_write,
                                            &region_1, 
                                            &region_1_size, 
                                            &region_2, 
                                            &region_2_size, 
                                            0)))
    {
        DWORD region_1_sample_count = region_1_size / sound_output->bytes_per_sample;
        i16* sample_out = (i16*)region_1;
        i16* buffer_samples = sound_buffer->samples;

        for (sizet sample_index = 0;
            sample_index < region_1_sample_count;
            ++sample_index)
        {
            *sample_out = *buffer_samples; sample_out++; buffer_samples++;
            *sample_out = *buffer_samples; sample_out++; buffer_samples++;
            (sound_output->running_sample_count)++;
        }


        DWORD region_2_sample_count = region_2_size / sound_output->bytes_per_sample;
        sample_out = (i16*)region_2;

        for (sizet sample_index = 0;
            sample_index < region_2_sample_count;
            ++sample_index)
        {
            *sample_out = *buffer_samples; sample_out++; buffer_samples++;
            *sample_out = *buffer_samples; sample_out++; buffer_samples++;
            (sound_output->running_sample_count)++;
        }

        global_sound_buffer->Unlock(region_1, region_1_size, region_2, region_2_size);

    }
}


global_variable LARGE_INTEGER global_pref_count_freq;

internal f32 
win32_get_elapsed_seconds(LARGE_INTEGER start, LARGE_INTEGER end)
{
    i64 elapsed_sec = end.QuadPart - start.QuadPart;
    f32 out = (f32)elapsed_sec / (f32)global_pref_count_freq.QuadPart;

    return out;
}

internal LARGE_INTEGER
win32_get_preformance_counter()
{
    LARGE_INTEGER out = {};
    QueryPerformanceCounter(&out);

    return out;
}


#define AUDIO_SAMPLE_RATE 48000

typedef void (*GameUpdate)(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input);
typedef void (*GameRender)();

struct Win32GameCode
{
    HINSTANCE game_code_dll;
    GameUpdate update;
    GameRender render;
};

internal Win32GameCode
win32_load_game_code()
{
    Win32GameCode game_code;
    CopyFile("game.dll", "game_temp.dll", FALSE);
    game_code.game_code_dll = LoadLibrary("game_temp.dll");
    if (game_code.game_code_dll != NULL)
    {
        game_code.update = (GameUpdate)GetProcAddress(game_code.game_code_dll, "game_update");
        game_code.render = (GameRender)GetProcAddress(game_code.game_code_dll, "game_render");
        ASSERT(game_code.update && game_code.render);
    }
    else
    {
        ASSERT(false);
    }
    return game_code;
}

internal void
win32_unload_game_code(Win32GameCode* game_code)
{
    ASSERT(FreeLibrary(game_code->game_code_dll));
    game_code->update = 0;
    game_code->render = 0;
    game_code->game_code_dll = 0;
}

internal i32
win32_get_last_write_time(char* file_name) 
{
    FILETIME last_write_time = {};
    WIN32_FIND_DATA find_data = {};

    HANDLE find_handle = FindFirstFileA("game.dll", &find_data);
    if (find_handle != INVALID_HANDLE_VALUE)
    {
        last_write_time = find_data.ftLastWriteTime;
        FindClose(find_handle);
    }

    return (i32)last_write_time.dwLowDateTime;
}

LRESULT CALLBACK win32_window_callback(
  _In_ HWND   window,
  _In_ UINT   message,
  _In_ WPARAM w_param,
  _In_ LPARAM l_param
) 
{
    LRESULT result = 0;

    switch(message)
    {
        case WM_SIZE:
        {
        } break;
        case WM_DESTROY:
        {
        } break;
        case WM_CLOSE:
        {
        } break;
        case WM_ACTIVATEAPP:
        {
        } break;

        default:
        {
            result = DefWindowProc(window, message, w_param, l_param);
        } break;
    }
    return result;
}

internal void*
win32_get_opengl_function(char *name)
{
}

internal void
win32_init_opengl(HWND window_handle)
{
    PIXELFORMATDESCRIPTOR pixel_format = {};

    HDC window_dc = GetDC(window_handle);

    pixel_format.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pixel_format.nVersion = 1;
    pixel_format.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
    pixel_format.cColorBits = 32;
    pixel_format.cAlphaBits = 8;
    pixel_format.iLayerType = PFD_MAIN_PLANE;

    i32 suggest_pf_index = ChoosePixelFormat(window_dc, &pixel_format);

    PIXELFORMATDESCRIPTOR suggested_pixel_format = {};
    DescribePixelFormat(window_dc, suggest_pf_index, sizeof(suggested_pixel_format), &pixel_format);
    SetPixelFormat(window_dc, suggest_pf_index, &suggested_pixel_format);

    HGLRC opengl_rc = wglCreateContext(window_dc);
    if (opengl_rc)
    {
        if (wglMakeCurrent(window_dc, opengl_rc))
        {
        }
        else
        {
            ASSERT(false);
        }
    }
    else
    {
        ASSERT(false);
    }
#define LOAD_GL_FUNCTION(ret, name, ...)                                                                    \
    gl##name = (name##proc *)wglGetProcAddress("gl" #name);                                \
    if (!gl##name) {                                                                       \
        NORMAL_MSG("Function gl" #name " couldn't be loaded.\n"); \
    }
    GL_FUNCTION_LIST
#undef LOAD_GL_FUNCTION
    ReleaseDC(window_handle, window_dc);
}

#define LOAD_GL_FUNCTION(ret, name, ...) name##proc * gl##name;
GL_FUNCTION_LIST
#undef LOAD_GL_FUNCTION

i32 WinMain(HINSTANCE hinstance,
            HINSTANCE prev_hinstance,
            LPSTR cmd_line,
            i32 show_code)
{
    WNDCLASS window_class = {};
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = win32_window_callback;
    window_class.hInstance = hinstance;
    window_class.lpszClassName = "Win32WindowClass";

    RegisterClass(&window_class);
    HWND window_handle = CreateWindowEx(
            0,
            window_class.lpszClassName,
            "Game",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            hinstance, 
            0); 

    ASSERT(window_handle);

    win32_init_opengl(window_handle);
    Win32GameCode game = win32_load_game_code();



    QueryPerformanceFrequency(&global_pref_count_freq);
    f32 target_fps = 60.0f;
    f32 target_sec_per_frame = 1.0f / target_fps;


    GameMemory game_memory = {};
    game_memory.permanent_storage_size = MEGABYTES(64);
    game_memory.temporary_storage_size = MEGABYTES(10);
    game_memory.is_initialized = false;
    sizet total_size = game_memory.permanent_storage_size + game_memory.temporary_storage_size;
    game_memory.permanent_storage = malloc(sizeof(u8) * total_size);
    game_memory.temporary_storage =
        ((u8*)game_memory.permanent_storage +
         game_memory.permanent_storage_size);
    ASSERT(game_memory.permanent_storage);

    u32 VBO, VAO, EBO, shader_program, texture;

    
    shader_program = glCreateProgram();
    create_shaders(shader_program);
    glUseProgram(shader_program);

    f32 vertex_data[] = {
                          -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
                           0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
                           0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f,
                          -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f
    }; 

    u32 indices[] = {
        0, 1, 2,
        0, 2, 3
    };


    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex layout
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(f32), (void*)0);
    // Enable the vertex atribute;
    glEnableVertexAttribArray(0); 

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(f32), (void*)(3 * sizeof(f32)));
    // Enable the vertex atribute;
    glEnableVertexAttribArray(1); 

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(f32), (void*)(7 * sizeof(f32)));
    // Enable the vertex atribute;
    glEnableVertexAttribArray(2); 

    i32 image_w, image_h, channels;
    stbi_set_flip_vertically_on_load(1);
    u8* image = stbi_load("../test_image.jpg", &image_w, &image_h, &channels, 0);
    ASSERT(image);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_w, image_h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);
    i32 texture_location = glGetUniformLocation(shader_program, "u_texture");
    if (texture_location == -1)
    {
        WARN_MSG("Invalid texture location. \n");
    }


    Win32SoundState sound_output = {};
    sound_output.bytes_per_sample = sizeof(i16) * 2;
    sound_output.samples_per_sec = AUDIO_SAMPLE_RATE;
    sound_output.buffer_size = sound_output.samples_per_sec * sound_output.bytes_per_sample;
    sound_output.running_sample_count = 0;
    sound_output.latency_sample_count = (i32)(sound_output.samples_per_sec / target_fps) * 3;

    win32_init_dsound(window_handle, sound_output.samples_per_sec, sound_output.buffer_size);

    
    i16* samples = (i16*)malloc(sound_output.buffer_size);
    GameInput game_input;
    LARGE_INTEGER last_counter = win32_get_preformance_counter();
    b8 sound_is_valid = false;
    b8 sound_is_playing = false;
    f32 acumilated_delta_time = 0.0f;

    i32 last_game_dll_write_time = win32_get_last_write_time("game.dll");

    b8 running = true;
    while(running)
    {
        DWORD bytes_to_write = 0;
        game_input = {};


        LARGE_INTEGER end_count = win32_get_preformance_counter();
        f32 delta_time = win32_get_elapsed_seconds(last_counter, end_count);
        last_counter = win32_get_preformance_counter();
        acumilated_delta_time += delta_time;

#if 0
        f32 frame_ms = delta_time * 1000.0f;
        f32 frames_per_second = 1.0f / delta_time;
        NORMAL_MSG("ms: %f \n", frame_ms);
        NORMAL_MSG("fps: %f \n", frames_per_second);
#endif
        MSG message;
        while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) 
        {
            switch(message.message)
            {
                case WM_DESTROY:
                {
                    running = false;
                } break;
                case WM_KEYUP:
                {
                    u32 vk_code = (u32)message.wParam;

                } break;
                default:
                {
                    TranslateMessage(&message);
                    DispatchMessageA(&message);
                } break;
            }
        }
        while (acumilated_delta_time >= target_sec_per_frame)
        {
            i32 game_dll_write_time = win32_get_last_write_time("game.dll");
            if (last_game_dll_write_time != game_dll_write_time)
            {
                win32_unload_game_code(&game);
                game = win32_load_game_code();
                last_game_dll_write_time = game_dll_write_time;
            }

            DWORD play_cursor;
            DWORD write_cursor;
            DWORD byte_offset;
            DWORD target_cursor;
            if (global_sound_buffer->GetCurrentPosition(&play_cursor, &write_cursor) == DS_OK)
            {
                byte_offset = (sound_output.running_sample_count * sound_output.bytes_per_sample) %
                              sound_output.buffer_size;
                target_cursor = ((play_cursor + 
                                 (sound_output.latency_sample_count * sound_output.bytes_per_sample)) %
                                 sound_output.buffer_size);


                if (byte_offset > target_cursor)
                {
                    bytes_to_write = (sound_output.buffer_size - byte_offset);
                    bytes_to_write += target_cursor;
                }
                else 
                {
                    bytes_to_write = target_cursor - byte_offset;
                }

                sound_is_valid = true;
            }
            else
            {
                // TODO: logging
                sound_is_valid = false;
                WARN_MSG("Could not get sound buffer cursor position! \n");
            }

            GameSoundBuffer sound_buffer = {};
            sound_buffer.samples_per_sec = sound_output.samples_per_sec;
            sound_buffer.sample_count = bytes_to_write / sound_output.bytes_per_sample;
            sound_buffer.samples = samples;


            game.update(delta_time, &game_memory, &sound_buffer, &game_input);

#if 0
            if (sound_is_valid)
            {
                win32_fill_sound_buffer(&sound_output, &sound_buffer, byte_offset, bytes_to_write);
                if (!sound_is_playing)
                {
                    HRESULT hresult = global_sound_buffer->Play(0, 0, DSBPLAY_LOOPING);

                    if (FAILED(hresult))
                    {
                        WARN_MSG("Failed to play sound. \n");
                    }
                    sound_is_playing = true;
                }
            }
#endif

            NORMAL_MSG("delta time : %f \n", acumilated_delta_time);
            acumilated_delta_time -= target_sec_per_frame;
            local_persist vec3 rotate = {0.0f, 0.0f, 1.0f};
            local_persist vec3 translate = {500.0f, 300.0f, 0.0f};
            local_persist vec3 scale = {500.0f, 500.0f, 0.0f};

            mat4 model = identity_mat4();
            mat4 projection = identity_mat4();

            model = translate_mat4(translate) * scale_mat4(scale);
            projection = orthographic_mat4(1024.0f, 768.0f);

            model = transpose_mat4(model);
            projection = transpose_mat4(projection);

            i32 mod_loc = glGetUniformLocation(shader_program, "u_model");
            i32 proj_loc = glGetUniformLocation(shader_program, "u_projection");

            glUniformMatrix4fv(mod_loc, 1, GL_FALSE, (f32*)model.rows);
            glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (f32*)projection.rows);
        }
        glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        game.render();
        HDC window_dc = GetDC(window_handle);
        SwapBuffers(window_dc);

    }

    return 0;
}
        
