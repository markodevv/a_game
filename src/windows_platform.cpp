#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <windows.h>
#include <dsound.h>
#include <math.h>

#include "game.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "math.h"

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


internal void
glfw_framebuffer_size_callback(GLFWwindow* Window, i32 w, i32 h)
{
    glViewport(0, 0, w, h);
}

internal void
opengl_init(GLFWwindow* window)
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        ASSERT(false);
    }

    glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);  
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
glfw_error_callback(i32 code, const char* description) {

    ALERT_MSG("GLFW Error Callback %i: \n%s \n", code, description);
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

i32 WinMain(HINSTANCE hinstance,
            HINSTANCE prev_hinstance,
            LPSTR cmd_line_args,
            i32 show_cmd)
{

    Win32GameCode game = win32_load_game_code();


    TIMECAPS time_caps = {};
    u32 tcs = sizeof(time_caps);
    MMRESULT mresult = timeGetDevCaps(&time_caps, tcs);
    if (mresult == MMSYSERR_ERROR) 
    {
        WARN_MSG("Failed to get dev caps. \n");
    }
    u32 time_period_min = time_caps.wPeriodMin;
    mresult = timeBeginPeriod(time_period_min);
    if (mresult != TIMERR_NOERROR)
    {
        WARN_MSG("Failed to set time begin period. \n");
    }

    QueryPerformanceFrequency(&global_pref_count_freq);
    f32 target_fps = 60.0f;
    f32 target_sec_per_frame = 1.0f / target_fps;

    glfwSetErrorCallback(glfw_error_callback);

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

    GLFWwindow* window;
    if (glfwInit())
    {
        window = glfwCreateWindow(1024, 768, "A GEMM", NULL, NULL);
        if (window)
        {
            glfwMakeContextCurrent(window);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        }
        else
        {
            // TODO: logging
            ALERT_MSG("Failed to create glfw window. ");
            ASSERT(false);
            glfwTerminate();
        }
    }
    else
    {
        // TODO: logging
        ALERT_MSG("Failed to initialize GLFW.");
        ASSERT(false);
    }


    opengl_init(window);

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

    HWND win32 = glfwGetWin32Window(window);
    win32_init_dsound(win32, sound_output.samples_per_sec, sound_output.buffer_size);

    
    i16* samples = (i16*)malloc(sound_output.buffer_size);
    GameInput game_input;
    LARGE_INTEGER last_counter = win32_get_preformance_counter();
    b8 sound_is_valid = false;
    b8 sound_is_playing = false;
    f32 acumilated_delta_time = 0.0f;

    i32 last_game_dll_write_time = win32_get_last_write_time("game.dll");

    while(!glfwWindowShouldClose(window))
    {
        DWORD bytes_to_write = 0;
        game_input = {};


        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            game_input.move_left.pressed = true;
        } 
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            game_input.move_right.pressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            game_input.move_down.pressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            game_input.move_up.pressed = true;
        }

        LARGE_INTEGER end_count = win32_get_preformance_counter();
        f32 delta_time = win32_get_elapsed_seconds(last_counter, end_count);
        last_counter = win32_get_preformance_counter();
        acumilated_delta_time += delta_time;

#ifdef GAME_DEBUG
        /*
        f32 frame_ms = delta_time * 1000.0f;
        f32 frames_per_second = 1.0f / delta_time;
        NORMAL_MSG("ms: %f \n", frame_ms);
        NORMAL_MSG("fps: %f \n", frames_per_second);
        */
#endif
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

        game.render();

        glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
}
        
