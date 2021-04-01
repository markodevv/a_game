#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <wingdi.h>
#include <dsound.h>
#include <math.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>

#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext, const int *attribList);
wglCreateContextAttribsARB_type *wglCreateContextAttribsARB;

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


#include "log.h"
#include "memory.h"
#include "math.h"
#include "debug.h"
#include "renderer.h"
#include "game.h"
#include "opengl_renderer.h"
#include "opengl_renderer.cpp"



#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter);
typedef DIRECT_SOUND_CREATE(DSC);

global_variable LPDIRECTSOUNDBUFFER global_sound_buffer;
global_variable b8 global_running = true;

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
    
    HANDLE file_handle = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
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
                DEBUG_PRINT("File malloc failed for file %s.\n", file_name);
            }
        }
        else
        {
            DEBUG_PRINT("Trying to open invalid file %s.\n", file_name);
        }

        CloseHandle(file_handle);
    }
    else
    {
        DEBUG_PRINT("Trying to open invalid file %s.\n", file_name);
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
        } break;
        case WM_CLOSE:
        {
            PostQuitMessage(0);
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

    i32 attrib_list[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        0,
    };
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

    wglCreateContextAttribsARB = 
       (wglCreateContextAttribsARB_type*)wglGetProcAddress( "wglCreateContextAttribsARB");
    opengl_rc = wglCreateContextAttribsARB(window_dc, 0, attrib_list);

    if (opengl_rc)
    {
        if (wglMakeCurrent(window_dc, opengl_rc))
        {
        }
        else
        {
            DEBUG_PRINT("Failed to set current 3.0+ OpenGL context!\n");
        }
    }
    else
    {
        DEBUG_PRINT("Failed to create 3.0+ OpenGL context!\n");
    }

    
#define Win32LoadOpenGLFunction(name) \
    name = (name##proc *)wglGetProcAddress(#name); \
    if (!name) \
    { \
        HMODULE module = LoadLibraryA("opengl32.dll"); \
        name = (name##proc *)GetProcAddress(module, #name); \
        if (!name) \
        { \
            DEBUG_PRINT("Function " #name " couldn't be loaded.\n"); \
        } \
    } 
    // NOTE: Vsync
    ((BOOL(WINAPI*)(int))wglGetProcAddress("wglSwapIntervalEXT"))(1);

    Win32LoadOpenGLFunction(glAttachShader);
    Win32LoadOpenGLFunction(glBindBuffer);
    Win32LoadOpenGLFunction(glBindFramebuffer);
    Win32LoadOpenGLFunction(glBufferData);
    Win32LoadOpenGLFunction(glBufferSubData);
    Win32LoadOpenGLFunction(glCheckFramebufferStatus);
    Win32LoadOpenGLFunction(glClearBufferfv);
    Win32LoadOpenGLFunction(glCompileShader);
    Win32LoadOpenGLFunction(glCreateProgram);
    Win32LoadOpenGLFunction(glCreateShader);
    Win32LoadOpenGLFunction(glDeleteBuffers);
    Win32LoadOpenGLFunction(glDeleteFramebuffers);
    Win32LoadOpenGLFunction(glEnableVertexAttribArray);
    Win32LoadOpenGLFunction(glDrawBuffers);
    Win32LoadOpenGLFunction(glFramebufferTexture2D);
    Win32LoadOpenGLFunction(glGenBuffers);
    Win32LoadOpenGLFunction(glGenFramebuffers);
    Win32LoadOpenGLFunction(glGetAttribLocation);
    Win32LoadOpenGLFunction(glGetShaderInfoLog);
    Win32LoadOpenGLFunction(glGetShaderiv);
    Win32LoadOpenGLFunction(glGetUniformLocation);
    Win32LoadOpenGLFunction(glLinkProgram);
    Win32LoadOpenGLFunction(glShaderSource);
    Win32LoadOpenGLFunction(glUniform1i);
    Win32LoadOpenGLFunction(glUniform1f);
    Win32LoadOpenGLFunction(glUniform2f);
    Win32LoadOpenGLFunction(glUniform4f);
    Win32LoadOpenGLFunction(glUniform1fv);
    Win32LoadOpenGLFunction(glUniform2fv);
    Win32LoadOpenGLFunction(glUniform3fv);
    Win32LoadOpenGLFunction(glUniformMatrix4fv);
    Win32LoadOpenGLFunction(glUseProgram);
    Win32LoadOpenGLFunction(glVertexAttribPointer);
    Win32LoadOpenGLFunction(glGenVertexArrays);
    Win32LoadOpenGLFunction(glBindVertexArray);
    Win32LoadOpenGLFunction(glDeleteShader);
    Win32LoadOpenGLFunction(glGetProgramInfoLog);
    Win32LoadOpenGLFunction(glGetProgramiv);
    Win32LoadOpenGLFunction(glActiveTexture);
    Win32LoadOpenGLFunction(glGenTextures);
    Win32LoadOpenGLFunction(glBindTexture);
    Win32LoadOpenGLFunction(glPixelStorei);
    Win32LoadOpenGLFunction(glTexParameteri);
    Win32LoadOpenGLFunction(glTexImage2D);
    Win32LoadOpenGLFunction(glEnable);
    Win32LoadOpenGLFunction(glBlendFunc);
    Win32LoadOpenGLFunction(glClearColor);
    Win32LoadOpenGLFunction(glClear);
    Win32LoadOpenGLFunction(glViewport);
    Win32LoadOpenGLFunction(glDrawArrays);
    

    ReleaseDC(window_handle, window_dc);
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
            case WM_QUIT:
                global_running= false;
                DEBUG_PRINT("Closing game...\n");
            case WM_KEYUP:
            case WM_KEYDOWN:
            {
                b8 is_down = ((message.lParam & (1 << 31)) == 0);
                b8 was_down = ((message.lParam & (1 << 30)) != 0);
                u32 vk_code = (u32)message.wParam;

                if (is_down == was_down)
                    break;
                switch(vk_code)
                {
                    case 'A':
                    {
                        game_input->move_left.is_down = is_down;
                        game_input->move_left.pressed = (was_down == 0) && (is_down == 1);
                        game_input->move_left.released = (was_down == 1) && (is_down == 0);
                    } break;
                    case 'D':
                    {
                        game_input->move_right.is_down = is_down;
                        game_input->move_right.pressed = (was_down == 0) && (is_down == 1);
                        game_input->move_right.released = (was_down == 1) && (is_down == 0);
                    } break;
                    case 'W':
                    {
                        game_input->move_up.is_down = is_down;
                        game_input->move_up.pressed = (was_down == 0) && (is_down == 1);
                        game_input->move_up.released = (was_down == 1) && (is_down == 0);
                    } break;
                    case 'S':
                    {
                        game_input->move_down.is_down = is_down;
                        game_input->move_down.pressed = (was_down == 0) && (is_down == 1);
                        game_input->move_down.released = (was_down == 1) && (is_down == 0);
                    } break;
                    case 'P':
                    {
                        game_input->pause_button.is_down = is_down;
                        game_input->pause_button.pressed = (was_down == 0) && (is_down == 1);
                        game_input->pause_button.released = (was_down == 1) && (is_down == 0);
                    } break;
                }
            } break;
            case WM_MOUSEWHEEL:
            {
                i32 wheel_delta = GET_WHEEL_DELTA_WPARAM(message.wParam);
                game_input->mouse.wheel_delta = wheel_delta;
            } break;
            case WM_MOUSEMOVE:
            {
                /*
                game_input->mouse.position.x = (f32)GET_X_LPARAM(message.lParam); 
                game_input->mouse.position.y = (f32)GET_Y_LPARAM(message.lParam); 
                */

                game_input->mouse.moved = true;
                if ((message.wParam & MK_LBUTTON) == MK_LBUTTON)
                {
                    game_input->right_mouse_button.is_down = true;
                }
            } break;
            case WM_LBUTTONDOWN:
            {
                game_input->left_mouse_button.is_down = 1;
                game_input->left_mouse_button.pressed = 1;
            } break;
            case WM_RBUTTONDOWN:
            {
                game_input->right_mouse_button.is_down = 1;
                game_input->right_mouse_button.pressed = 1;
            } break;
            case WM_LBUTTONUP:
            {
                game_input->left_mouse_button.is_down = 0;
                game_input->left_mouse_button.released = 1;
            } break;
            case WM_RBUTTONUP:
            {
                game_input->right_mouse_button.is_down = 0;
                game_input->right_mouse_button.released = 1;
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
    AllocConsole();

    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;
    freopen("CONOUT$", "w+", stdout);


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

    printf("HEllo to console :)\n");

    Win32GameCode game = win32_load_game_code();

    QueryPerformanceFrequency(&global_pref_count_freq);
    f32 target_fps = 60.0f;
    f32 target_sec_per_frame = 1.0f / target_fps;


    GameMemory game_memory = {};
    game_memory.permanent_storage_size = MEGABYTES(64);
    game_memory.temporary_storage_size = MEGABYTES(64);

    game_memory.is_initialized = false;
    game_memory.permanent_storage = calloc(game_memory.permanent_storage_size, sizeof(u8));
    game_memory.temporary_storage = calloc(game_memory.temporary_storage_size, sizeof(u8));


    ASSERT(game_memory.permanent_storage);
    ASSERT(game_memory.temporary_storage);

    game_memory.renderer_init = &opengl_init;
    game_memory.renderer_begin = &opengl_begin_frame;
    game_memory.renderer_end = &opengl_end_frame;


#if defined(GAME_DEBUG)
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

    i16* samples = (i16*)calloc(sound_output.buffer_size, sizeof(i16));
    b8 sound_is_valid = false;
    b8 sound_is_playing = false;

    i32 last_game_dll_write_time = win32_get_last_write_time("game.dll");

    b8 paused = false;
    while(global_running)
    {

        for (sizet i = 0; i < ARRAY_COUNT(game_input.buttons); ++i)
        {
            game_input.buttons[i].released = 0;
            game_input.buttons[i].pressed = 0;
        }
        game_input.mouse.wheel_delta = 0;
        game_input.mouse.moved = false;

        win32_process_input_messages(&game_input);

        if (button_pressed(game_input.pause_button))
        {
            paused = !paused;
        }

        POINT p; 
        GetCursorPos(&p);
        ScreenToClient(window_handle, &p);
        game_input.mouse.position.x = (f32)p.x;
        game_input.mouse.position.y = (f32)p.y;


        LARGE_INTEGER end_count = win32_get_preformance_counter();
        f32 delta_time = win32_get_elapsed_seconds(last_counter, end_count);
        last_counter = win32_get_preformance_counter();

        if (game_memory.is_initialized)
        {
            game_memory.debug->game_fps = 1.0f/delta_time;
        }

        i32 game_dll_write_time = win32_get_last_write_time("game.dll");
        if (last_game_dll_write_time != game_dll_write_time)
        {
            win32_unload_game_code(&game);
            game = win32_load_game_code();
            last_game_dll_write_time = game_dll_write_time;
        }

#if 0
        DWORD bytes_to_write = 0;
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

        if (!paused)
        {
            game.update(delta_time, &game_memory, NULL, &game_input);
            game.render(&game_memory);
        }


        HDC window_dc = GetDC(window_handle);
        SwapBuffers(window_dc);

        RECT screen_rect;
        GetClientRect(window_handle, &screen_rect);
        i32 w = screen_rect.right - screen_rect.left;
        i32 h = screen_rect.bottom - screen_rect.top;
        game_memory.screen_width = w;
        game_memory.screen_height = h;

     }

     system("pause");

     return 0;
}
        
