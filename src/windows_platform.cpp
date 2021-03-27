#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <wingdi.h>
#include <dsound.h>
#include <math.h>
#include <gl/gl.h>

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


#include "math.h"
#include "game.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "opengl_renderer.cpp"

#define LOAD_GL_FUNCTION(ret, name, ...) name##proc * gl##name;
GL_FUNCTION_LIST
#undef LOAD_GL_FUNCTION


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

internal void
DEBUG_print(char* text)
{
    OutputDebugString(text);
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
                DEBUG_PRINT("File malloc failed for file %s. \n", file_name);
            }
        }
        else
        {
        }

        CloseHandle(file_handle);
    }
    else
    {
        DEBUG_PRINT("Trying to open invalid file %s. \n", file_name);
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
DEBUG_write_entire_file(char* file_name, i32 size, void* memory)
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
            DEBUG_PRINT("Failed to write entire file: %s.\n", file_name);
        }

        CloseHandle(file_handle);
    }
    else
    {
        DEBUG_PRINT("Invalid file handle \n");
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
                DEBUG_PRINT("Created sound buffer. \n");
            }
        }
    }
    else
    {
        DEBUG_PRINT("Failed to create sound buffer. \n");
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
typedef void (*GameRender)(GameMemory* memory);

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

LRESULT CALLBACK 
win32_window_callback(
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
            i32 width = LOWORD(l_param);
            i32 height = HIWORD(l_param);
            // TODO: this should be in opengl file.
            glViewport(0, 0, width, height);
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
        DEBUG_PRINT("Function gl" #name " couldn't be loaded.\n"); \
    }
    GL_FUNCTION_LIST
#undef LOAD_GL_FUNCTION

    ReleaseDC(window_handle, window_dc);

    // TODO: pass memory here for opengl to allocate?
}

internal void
win32_process_input_messages(GameInput* game_input)
{
    MSG message;
    while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) 
    {
        switch(message.message)
        {
            case WM_DESTROY:
            {
            } break;
            case WM_KEYUP:
            case WM_KEYDOWN:
            {
                b8 is_down = ((message.lParam & (1 << 31)) == 0);
                b8 was_down = ((message.lParam & (1 << 30)) != 0);
                u32 vk_code = (u32)message.wParam;

                if (is_down != was_down)
                {
                    switch(vk_code)
                    {
                        case 'A':
                        {
                            game_input->move_left.is_down = is_down;
                        } break;
                        case 'D':
                        {
                            game_input->move_right.is_down = is_down;
                        } break;
                        case 'W':
                        {
                            game_input->move_up.is_down = is_down;
                        } break;
                        case 'S':
                        {
                            game_input->move_down.is_down = is_down;
                        } break;
                    }
                }
            } break;
            case WM_MOUSEWHEEL:
            {
                i32 wheel_delta = GET_WHEEL_DELTA_WPARAM(message.wParam);

                game_input->mouse_scroll.wheel_delta = wheel_delta;
                game_input->mouse_scroll.scrolled = true;;
            } break;
            default:
            {
                TranslateMessage(&message);
                DispatchMessageA(&message);
            } break;
        }
    }

}

i32
WinMain(HINSTANCE hinstance,
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
    if (window_handle)
    {
        win32_init_opengl(window_handle);
    }
    else
    {
        ASSERT(false);
    }

    Win32GameCode game = win32_load_game_code();

    QueryPerformanceFrequency(&global_pref_count_freq);
    f32 target_fps = 60.0f;
    f32 target_sec_per_frame = 1.0f / target_fps;


    GameMemory game_memory = {};
    game_memory.permanent_storage_size = MEGABYTES(64);
    game_memory.temporary_storage_size = MEGABYTES(10);
    game_memory.permanent_storage_index = 0;

    game_memory.is_initialized = false;
    sizet total_size = game_memory.permanent_storage_size + game_memory.temporary_storage_size;
    game_memory.permanent_storage = calloc(total_size, sizeof(u8));
    game_memory.temporary_storage =
        ((u8*)game_memory.permanent_storage +
         game_memory.permanent_storage_size);

    ASSERT(game_memory.permanent_storage);

    // NOTE(marko): this needs to be first memory push
    PUSH_STRUCT(game_memory, GameState);

    game_memory.renderer_init = &opengl_init;
    game_memory.frame_end = &opengl_frame_end;
    game_memory.draw_rectangle = &opengl_draw_rectangle;
    game_memory.draw_cube = &opengl_draw_cube;

#if defined(GAME_DEBUG)
    game_memory.DEBUG_print = DEBUG_print;
    game_memory.DEBUG_read_entire_file = DEBUG_read_entire_file;
    game_memory.DEBUG_write_entire_file = DEBUG_write_entire_file;
    game_memory.DEBUG_free_file_memory = DEBUG_free_file_memory;
#endif

    Win32SoundState sound_output = {};
    sound_output.bytes_per_sample = sizeof(i16) * 2;
    sound_output.samples_per_sec = AUDIO_SAMPLE_RATE;
    sound_output.buffer_size = sound_output.samples_per_sec * sound_output.bytes_per_sample;
    sound_output.running_sample_count = 0;
    sound_output.latency_sample_count = (i32)(sound_output.samples_per_sec / target_fps) * 3;

    GameInput game_input = {};
    win32_init_dsound(window_handle, sound_output.samples_per_sec, sound_output.buffer_size);

    
    LARGE_INTEGER last_counter = win32_get_preformance_counter();
    f32 acumilated_delta_time = 0.0f;

    i16* samples = (i16*)malloc(sound_output.buffer_size);
    b8 sound_is_valid = false;
    b8 sound_is_playing = false;

    i32 last_game_dll_write_time = win32_get_last_write_time("game.dll");

    b8 running = true;
    while(running)
    {
        DWORD bytes_to_write = 0;

        win32_process_input_messages(&game_input);


        LARGE_INTEGER end_count = win32_get_preformance_counter();
        f32 delta_time = win32_get_elapsed_seconds(last_counter, end_count);
        last_counter = win32_get_preformance_counter();
        acumilated_delta_time += delta_time;


        while (acumilated_delta_time >= target_sec_per_frame)
        {
            i32 game_dll_write_time = win32_get_last_write_time("game.dll");
            if (last_game_dll_write_time != game_dll_write_time)
            {
                win32_unload_game_code(&game);
                game = win32_load_game_code();
                last_game_dll_write_time = game_dll_write_time;
            }

#if 0
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
                DEBUG_PRINT("Could not get sound buffer cursor position! \n");
            }

            GameSoundBuffer sound_buffer = {};
            sound_buffer.samples_per_sec = sound_output.samples_per_sec;
            sound_buffer.sample_count = bytes_to_write / sound_output.bytes_per_sample;
            sound_buffer.samples = samples;




            if (sound_is_valid)
            {
                win32_fill_sound_buffer(&sound_output, &sound_buffer, byte_offset, bytes_to_write);
                if (!sound_is_playing)
                {
                    HRESULT hresult = global_sound_buffer->Play(0, 0, DSBPLAY_LOOPING);

                    if (FAILED(hresult))
                    {
                        DEBUG_PRINT("Failed to play sound. \n");
                    }
                    sound_is_playing = true;
                }
            }
#endif

            game.update(delta_time, &game_memory, NULL, &game_input);
            acumilated_delta_time -= target_sec_per_frame;
            game_input.mouse_scroll.scrolled = false;
        }
        glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        game.render(&game_memory);

        HDC window_dc = GetDC(window_handle);
        SwapBuffers(window_dc);
     }

    return 0;
}
        
