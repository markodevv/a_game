/* ========================================================================
   $File: $
   $Date: $
   $Creator: Marko Bisevac $
   $Notice: (C) Copyright 2021. All Rights Reserved. $
   ======================================================================== */
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
#include <stdio.h>

#include <windows.h>
#include <dsound.h>
#include <math.h>

#include "renderer.h"
#include "opengl_renderer.cpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "game.h"
#include "game.cpp"


#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter);
typedef DIRECT_SOUND_CREATE(DSC);

global_variable LPDIRECTSOUNDBUFFER global_sound_buffer;

inline internal u32
safe_truncate_u64(u64 value)
{
    ASSERT(value <= 0xFFFFFFFF, 0);
    u32 result = (u32)value;
    return result;
}

internal DebugFileResult
DEBUG_read_entire_file(char* file_name)
{
     DebugFileResult result = {};
    
    HANDLE file_handle = CreateFileA(file_name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
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
            }
        }
        else
        {
        }

        CloseHandle(file_handle);
    }
    else
    {
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
            // TODO: logging
        }

        CloseHandle(file_handle);
    }
    else
    {
        // TODO: logging
    }

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
                printf("Created sound buffer \n");
            }
        }
    }
    else
    {
        // TODO: logging
        printf("Failed to create sound buffer\n");
    }
}


struct Win32SoundState
{
    i32 buffer_size;
    u32 running_index;
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
            (sound_output->running_index)++;
        }


        DWORD region_2_sample_count = region_2_size / sound_output->bytes_per_sample;
        sample_out = (i16*)region_2;

        for (sizet sample_index = 0;
            sample_index < region_2_sample_count;
            ++sample_index)
        {
            *sample_out = *buffer_samples; sample_out++; buffer_samples++;
            *sample_out = *buffer_samples; sample_out++; buffer_samples++;
            (sound_output->running_index)++;
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


#define FRAMES_OF_AUDIO_LATENCY 3
#define AUDIO_SAMPLE_RATE 48000

i32 main()
{
    TIMECAPS time_caps = {};
    u32 tcs = sizeof(time_caps);
    MMRESULT mresult = timeGetDevCaps(&time_caps, tcs);
    if (mresult == MMSYSERR_ERROR) 
    {
        printf("Failed to get dev caps. \n");
    }
    u32 time_period_min = time_caps.wPeriodMin;
    mresult = timeBeginPeriod(time_period_min);
    if (mresult != TIMERR_NOERROR)
    {
        printf("Failed to set time begin period. \n");
    }

    QueryPerformanceFrequency(&global_pref_count_freq);
    f32 target_sec_per_frame = 1.0f / 30.0f;

    GameMemory game_memory = {};
    game_memory.permanent_storage_size = MEGABYTES(64);
    game_memory.temporary_storage_size = MEGABYTES(10);
    game_memory.is_initialized = false;
    sizet total_size = game_memory.permanent_storage_size + game_memory.temporary_storage_size;

    game_memory.permanent_storage = malloc(sizeof(u8) * total_size);

    game_memory.temporary_storage =
        ((u8*)game_memory.permanent_storage +
         game_memory.permanent_storage_size);

    ASSERT(game_memory.permanent_storage, "Game memory allocation failed");
    
    GLFWwindow* window = glfw_create_window();
    ASSERT(window != 0, "Failed to create window");

    renderer_initialize();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

    Win32SoundState sound_output = {};
    sound_output.bytes_per_sample = sizeof(i16) * 2;
    sound_output.samples_per_sec = AUDIO_SAMPLE_RATE;
    sound_output.buffer_size = sound_output.samples_per_sec * sound_output.bytes_per_sample;
    sound_output.running_index = 0;
    sound_output.latency_sample_count = (i32)(sound_output.samples_per_sec * target_sec_per_frame * FRAMES_OF_AUDIO_LATENCY);

    HWND win32 = glfwGetWin32Window(window);
    win32_init_dsound(win32, sound_output.samples_per_sec, sound_output.buffer_size);

    if (!win32_clear_sound_buffer(&sound_output))
    {
        printf("Could not clear sound buffer \n");
    }
    
    HRESULT hresult = global_sound_buffer->Play(0, 0, DSBPLAY_LOOPING);

    if (FAILED(hresult))
    {
        printf("Failed to play sound \n");
    }

    i16* samples = (i16*)malloc(sound_output.buffer_size);
    GameInput game_input;

    while(!glfwWindowShouldClose(window))
    {
        b8 sound_is_valid = false;
        DWORD play_cursor = 0;
        DWORD write_cursor = 0;
        DWORD bytes_to_write = 0;
        DWORD target_cursor = 0;
        DWORD byte_offset = 0;
        game_input = {};

        LARGE_INTEGER last_counter = {};
        last_counter = win32_get_preformance_counter();


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

        if (SUCCEEDED(global_sound_buffer->GetCurrentPosition(&play_cursor, &write_cursor)))
        {
            byte_offset = ((sound_output.running_index * sound_output.bytes_per_sample) % sound_output.buffer_size);
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
            sound_is_valid = false;
        }

        GameSoundBuffer sound_buffer = {};
        sound_buffer.samples_per_sec = sound_output.samples_per_sec;
        sound_buffer.sample_count = bytes_to_write / sound_output.bytes_per_sample;
        sound_buffer.samples = samples;

        game_update_and_render(&game_memory, &sound_buffer, &game_input);

        if (sound_is_valid) 
        {
            win32_fill_sound_buffer(&sound_output, &sound_buffer, byte_offset, bytes_to_write);
        }

        LARGE_INTEGER end_count = win32_get_preformance_counter();

        f32 seconds_elapsed = win32_get_elapsed_seconds(last_counter, end_count);
        if (seconds_elapsed < target_sec_per_frame)
        {
            while (seconds_elapsed < target_sec_per_frame)
            {
                DWORD sleepMS = (DWORD)((target_sec_per_frame - seconds_elapsed) * 1000.0f);
                if (sleepMS > 0)
                {
                    Sleep(sleepMS);
                }
                end_count = win32_get_preformance_counter();
                seconds_elapsed = win32_get_elapsed_seconds(last_counter, end_count);
            }
        }
        else
        {
            // TODO: Low framerate
        }

        f32 ms_per_frame = seconds_elapsed * 1000.0f;
        f32 frames_per_second = 1.0f / (ms_per_frame / 1000.0f);
        printf("ms: %f \n", ms_per_frame);
        printf("fps: %f \n", frames_per_second);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
        
