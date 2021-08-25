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
#include <intrin.h>

#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001

#ifdef GAME_DEBUG
struct DebugState* global_debug_state;
#endif

#include <stb_image.h>
#include "log.h"
#include "platform.h"
#include "generated/memory.h"
#include "memory.cpp"
#include "array.cpp"
#include "hashmap.cpp"
#include "string.cpp"
#include "generated/math.h"
#include "math.cpp"
#include "generated/input.h"
#include "generated/renderer.h"
#include "generated/debug.h"
#include "debug_profiler.cpp"
#include "generated/game.h"
#include "generated_print.c"
#include "asset.cpp"
#include "opengl_renderer.h"
#include "opengl_renderer.cpp"

OpenGLFunction(HGLRC, wglCreateContextAttribsARB, HDC hdc, HGLRC hShareContext, const int* attribs);
typedef void (*GameMainLoopProc)(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input);

#define Win32LoadOpenGLFunction(name) \
name = (name##proc *)wglGetProcAddress(#name); \
if (!name) \
{ \
HMODULE module = LoadLibraryA("opengl32.dll"); \
name = (name##proc *)GetProcAddress(module, #name); \
if (!name) \
{ \
LogM("OpenGL function " #name " couldn't be loaded.\n"); \
} \
} 



#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter);
typedef DIRECT_SOUND_CREATE(DSC);

global_variable LPDIRECTSOUNDBUFFER global_sound_buffer;
global_variable b8 global_running = true;


inline internal u32
SafeTruncateU64(u64 value)
{
    Assert(value <= 0xFFFFFFFF);
    u32 result = (u32)value;
    return result;
}


internal void
FreeFileMemory(void* memory)
{
    if (memory)
    {
        free(memory);
    }
}

internal FileResult
ReadEntireFile(char* file_name)
{
    FileResult result = {};
    
    HANDLE file_handle = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(file_handle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER file_size;
        if(GetFileSizeEx(file_handle, &file_size))
        {
            u32 file_size_32 = SafeTruncateU64(file_size.QuadPart);
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
                    FreeFileMemory(result.data);
                    result.data = 0;
                }
            }
            else
            {
                LogM("File malloc failed for file %s.\n", file_name);
            }
        }
        else
        {
            LogM("Trying to open invalid file %s.\n", file_name);
        }
        
        CloseHandle(file_handle);
    }
    else
    {
        LogM("Trying to open invalid file %s.\n", file_name);
    }
    
    return result;
}


internal b8
WriteEntireFile(char* file_name, u32 size, void* memory)
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
            LogM("Failed to write entire file: %s.\n", file_name);
        }
        
        CloseHandle(file_handle);
    }
    else
    {
        LogM("Invalid file handle \n");
    }
    
    return result;
}





internal void
Win32InitDSound(HWND window, i32 samples_per_sec, i32 buffer_size)
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
                LogM("Created sound buffer. \n");
            }
        }
    }
    else
    {
        LogM("Failed to create sound buffer. \n");
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
Win32ClearSoundBuffer(Win32SoundState* sound_output)
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
Win32FillSoundBuffer(Win32SoundState* sound_output, GameSoundBuffer* sound_buffer, DWORD byte_offset, DWORD bytes_to_write)
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

internal u64
Win32GetPerformanceCounter()
{
    LARGE_INTEGER out = {};
    QueryPerformanceCounter(&out);
    
    return out.QuadPart;
}

internal f64 
Win32GetElapsedSeconds(u64 start)
{
    u64 end = Win32GetPerformanceCounter();
    u64 elapsed_sec = end - start;
    f64 out = (f64)elapsed_sec / (f64)global_pref_count_freq.QuadPart;
    
    return out;
}


#define AUDIO_SAMPLE_RATE 48000


struct Win32GameCode
{
    HINSTANCE game_code_dll;
    GameMainLoopProc MainLoop;
};

// Model load_3D_model(MemoryArena* arena, char* path);

internal Win32GameCode
Win32LoadGameCode()
{
    
    Win32GameCode game_code;
    CopyFile("game.dll", "game_temp.dll", FALSE);
    game_code.game_code_dll = LoadLibrary("game_temp.dll");
    if (game_code.game_code_dll != NULL)
    {
        game_code.MainLoop = (GameMainLoopProc)GetProcAddress(game_code.game_code_dll, "GameMainLoop");
        Assert(game_code.MainLoop);
    }
    else
    {
        Assert(false);
    }
    return game_code;
}

internal void
Win32UnloadGameCode(Win32GameCode* game_code)
{
    Assert(FreeLibrary(game_code->game_code_dll));
    game_code->MainLoop = 0;
    game_code->game_code_dll = 0;
}

internal i32
Win32GetLastWriteTime(char* file_name) 
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
Win32WindowCallback(
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

internal u64
Win32GetTimeInMs()
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    
    return st.wMilliseconds;
}


internal void
Win32InitOpenGL(HWND window_handle)
{
    
    HDC window_dc = GetDC(window_handle);
    
    PIXELFORMATDESCRIPTOR pixel_format = {};
    pixel_format.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pixel_format.nVersion = 1;
    pixel_format.iPixelType = PFD_TYPE_RGBA;
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
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };
    HGLRC opengl_rc = wglCreateContext(window_dc);
    if (opengl_rc)
    {
        if (!wglMakeCurrent(window_dc, opengl_rc))
        {
            Assert(false);
        }
    }
    else
    {
        Assert(false);
    }
    
    Win32LoadOpenGLFunction(wglCreateContextAttribsARB);
    
    opengl_rc = wglCreateContextAttribsARB(window_dc, 0, attrib_list);
    
    if (opengl_rc)
    {
        if (wglMakeCurrent(window_dc, opengl_rc))
        {
        }
        else
        {
            LogM("Failed to set current 3.0+ OpenGL context!\n");
            Assert(false);
        }
    }
    else
    {
        LogM("Failed to create 3.0+ OpenGL context!\n");
        Assert(false);
    }
    
    
    // NOTE: Vsync
    ((BOOL(WINAPI*)(int))wglGetProcAddress("wglSwapIntervalEXT"))(0);
    
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
    Win32LoadOpenGLFunction(glUniform1iv);
    Win32LoadOpenGLFunction(glUniform2iv);
    Win32LoadOpenGLFunction(glUniform3iv);
    Win32LoadOpenGLFunction(glUniform4iv);
    Win32LoadOpenGLFunction(glUniform1fv);
    Win32LoadOpenGLFunction(glUniform2fv);
    Win32LoadOpenGLFunction(glUniform3fv);
    Win32LoadOpenGLFunction(glUniform4fv);
    Win32LoadOpenGLFunction(glUniformMatrix4fv);
    Win32LoadOpenGLFunction(glUseProgram);
    Win32LoadOpenGLFunction(glVertexAttribPointer);
    Win32LoadOpenGLFunction(glGenVertexArrays);
    Win32LoadOpenGLFunction(glBindVertexArray);
    Win32LoadOpenGLFunction(glDeleteShader);
    Win32LoadOpenGLFunction(glGetProgramInfoLog);
    Win32LoadOpenGLFunction(glGetProgramiv);
    Win32LoadOpenGLFunction(glActiveTexture);
    Win32LoadOpenGLFunction(glGetActiveUniform);
    Win32LoadOpenGLFunction(glGetActiveUniformBlockiv);
    Win32LoadOpenGLFunction(glGetActiveUniformName);
    Win32LoadOpenGLFunction(glGenTextures);
    Win32LoadOpenGLFunction(glBindTexture);
    Win32LoadOpenGLFunction(glPixelStorei);
    Win32LoadOpenGLFunction(glTexParameteri);
    Win32LoadOpenGLFunction(glTexImage2D);
    Win32LoadOpenGLFunction(glEnable);
    Win32LoadOpenGLFunction(glDisable);
    Win32LoadOpenGLFunction(glBlendFunc);
    Win32LoadOpenGLFunction(glDepthFunc);
    Win32LoadOpenGLFunction(glClearColor);
    Win32LoadOpenGLFunction(glClear);
    Win32LoadOpenGLFunction(glViewport);
    Win32LoadOpenGLFunction(glDrawArrays);
    Win32LoadOpenGLFunction(glDrawElements);
    
    
    ReleaseDC(window_handle, window_dc);
}

internal void
SetButtonState(GameInput* input, ButtonCode button, b8 is_down, b8 was_down)
{
    input->buttons[button].is_down = is_down;
    input->buttons[button].pressed =  (!was_down) && (is_down);
    input->buttons[button].released = (was_down) &&  (!is_down);
}

internal void
Win32ProcessInputMessages(GameInput* game_input)
{
    
    MSG message;
    while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) 
    {
        switch(message.message)
        {
            case WM_CHAR:
            {
                game_input->character = (char)message.wParam;
            } break;
            case WM_QUIT:
            global_running= false;
            LogM("Closing game...\n");
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
                        SetButtonState(game_input, BUTTON_LEFT, is_down, was_down);
                    } break;
                    case 'D':
                    {
                        SetButtonState(game_input, BUTTON_RIGHT, is_down, was_down);
                    } break;
                    case 'W':
                    {
                        SetButtonState(game_input, BUTTON_UP, is_down, was_down);
                    } break;
                    case 'S':
                    {
                        SetButtonState(game_input, BUTTON_DOWN, is_down, was_down);
                    } break;
                    case 'P':
                    {
                        SetButtonState(game_input, BUTTON_PAUSE, is_down, was_down);
                    } break;
                    case VK_BACK:
                    {
                        SetButtonState(game_input, BUTTON_BACKSPACE, is_down, was_down);
                    } break;
                    case VK_ESCAPE:
                    {
                        SetButtonState(game_input, BUTTON_ESCAPE, is_down, was_down);
                    } break;
                    case VK_RETURN:
                    {
                        SetButtonState(game_input, BUTTON_ENTER, is_down, was_down);
                    } break;
                    case VK_F1:
                    {
                        SetButtonState(game_input, BUTTON_F1, is_down, was_down);
                    } break;
                }
                TranslateMessage(&message);
                DispatchMessageA(&message);
            } break;
            case WM_MOUSEWHEEL:
            {
                i32 wheel_delta = GET_WHEEL_DELTA_WPARAM(message.wParam);
                game_input->mouse.wheel_delta = wheel_delta;
            } break;
            case WM_MOUSEMOVE:
            {
                //if ((message.wParam & MK_LBUTTON) == MK_LBUTTON)
                //{
                //game_input->right_mouse_button.is_down = true;
                //}
            } break;
            case WM_LBUTTONDOWN:
            {
                game_input->buttons[BUTTON_MOUSE_LEFT].is_down = 1;
                game_input->buttons[BUTTON_MOUSE_LEFT].pressed = 1;
            } break;
            case WM_RBUTTONDOWN:
            {
                game_input->buttons[BUTTON_MOUSE_RIGHT].is_down = 1;
                game_input->buttons[BUTTON_MOUSE_RIGHT].pressed = 1;
            } break;
            case WM_LBUTTONUP:
            {
                game_input->buttons[BUTTON_MOUSE_LEFT].is_down = 0;
                game_input->buttons[BUTTON_MOUSE_LEFT].released = 1;
            } break;
            case WM_RBUTTONUP:
            {
                game_input->buttons[BUTTON_MOUSE_RIGHT].is_down = 1;
                game_input->buttons[BUTTON_MOUSE_RIGHT].released = 0;
            } break;
            default:
            {
                TranslateMessage(&message);
                DispatchMessageA(&message);
            } break;
            
        }
    }
    
}

internal void
Win32CreateConsole()
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
}

struct WorkEntry
{
    void* data;
    WorkCallback* work_callback;
};


struct WorkQueue
{
    u32 volatile completion_count = 0;
    u32 volatile completion_goal = 0;
    
    u32 volatile next_entry_to_write = 0;
    u32 volatile next_entry_to_read = 0;
    HANDLE semaphore;
    
    WorkEntry entries[256];
};


struct ThreadParam
{
    WorkQueue* queue;
};

void PushWorkEntry(WorkQueue* queue, WorkCallback* work_callback, void* data)
{
    u32 new_next_entry_to_write = (queue->next_entry_to_write + 1) % ArrayCount(queue->entries);
    Assert(new_next_entry_to_write != queue->next_entry_to_read);
    
    WorkEntry* entry = queue->entries + queue->next_entry_to_write;
    entry->data = data;
    entry->work_callback = work_callback;
    ++queue->completion_goal;
    
    _WriteBarrier();
    _mm_sfence();
    queue->next_entry_to_write = new_next_entry_to_write;
    ReleaseSemaphore(queue->semaphore, 1, 0);
}

internal b8
WorkerDoWork(WorkQueue* queue)
{
    b8 result = false;
    u32 original_next_entry_to_read = queue->next_entry_to_read;
    u32 new_next_entry_to_read = (original_next_entry_to_read + 1) % ArrayCount(queue->entries);
    
    if (original_next_entry_to_read != queue->next_entry_to_write)
    {
        u32 index = InterlockedCompareExchange(&queue->next_entry_to_read, 
                                               new_next_entry_to_read,
                                               original_next_entry_to_read);
        if (original_next_entry_to_read == index)
        {
            WorkEntry* entry = queue->entries + index;
            entry->work_callback(entry->data);
            InterlockedIncrement(&queue->completion_count);
        }
    }
    else
    {
        result = true;
    }
    return result;
}


DWORD WINAPI
ThreadLoop(LPVOID _param)
{
    WorkQueue* queue = (WorkQueue*)_param;
    for (;;)
    {
        if (!WorkerDoWork(queue))
        {
            WaitForSingleObjectEx(queue->semaphore, INFINITE, FALSE);
        }
    }
    
    return 0;
}

internal void
WaitForWorkers(WorkQueue* queue)
{
    while (queue->completion_goal != queue->completion_count)
    {
        WorkerDoWork(queue);
    }
    
    queue->completion_count = 0;
    queue->completion_goal = 0;
}

struct StringWork
{
    int i;
};

internal void 
PrintStringWork(void* data)
{
    StringWork* string = (StringWork*)data;
    LogM("Thread %i I is %i\n", GetCurrentThreadId(), string->i);
}


internal void
Win32InitThreads(Platform* platform)
{
    u32 thread_count = 3;
    platform->work_queue = (WorkQueue*)calloc(1, sizeof(WorkQueue));
    platform->work_queue->semaphore = CreateSemaphoreExA(0, 0, thread_count, 0, 0,SEMAPHORE_ALL_ACCESS);
    
    for (u32 i = 0; i < thread_count; ++i)
    {
        DWORD thread_id;
        HANDLE thread_handle = CreateThread(0, 0, ThreadLoop, platform->work_queue, 0, &thread_id);
    }
    
    /*
        StringWork string_work[32];
        
        for (u32 i = 0; i < 32; ++i)
        {
            string_work[i].i = i;
            PushWorkEntry(platform->work_queue, 
                          PrintStringWork, 
                          &string_work[i]);
        }
        
        WaitForWorkers(platform->work_queue);
    */
}

i32
WinMain(HINSTANCE hinstance,
        HINSTANCE prev_hinstance,
        LPSTR cmd_line,
        i32 show_code)
{
    Win32CreateConsole();
    
    WNDCLASS window_class = {};
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = Win32WindowCallback;
    window_class.hInstance = hinstance;
    window_class.lpszClassName = "Win32WindowClass";
    
    RegisterClass(&window_class);
    HWND window_handle = CreateWindowEx(0,
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
        Win32InitOpenGL(window_handle);
    }
    else
    {
        Assert(false);
    }
    BOOL result = SetWindowPos(window_handle,
                               HWND_TOP,
                               0,
                               0,
                               1920,
                               1080,
                               0);
    Assert(result);
    
    Win32GameCode game = Win32LoadGameCode();
    
    QueryPerformanceFrequency(&global_pref_count_freq);
    f32 target_fps = 60.0f;
    f32 target_sec_per_frame = 1.0f / target_fps;
    
    
    GameMemory game_memory = {};
    game_memory.permanent_storage_size = Megabytes(64);
    game_memory.temporary_storage_size = Megabytes(128);
    game_memory.debug_storage_size = Megabytes(48);
    
    game_memory.is_initialized = false;
    game_memory.permanent_storage = calloc(game_memory.permanent_storage_size, sizeof(u8));
    game_memory.temporary_storage = calloc(game_memory.temporary_storage_size, sizeof(u8));
    game_memory.debug_storage = calloc(game_memory.debug_storage_size, sizeof(u8));
    
    game_memory.debug = (DebugState*)game_memory.debug_storage;
    global_debug_state = game_memory.debug;
    
    InitArena(&game_memory.debug->arena, 
              (game_memory.debug_storage_size - sizeof(DebugState)),
              (u8*)game_memory.debug_storage + sizeof(DebugState));
    SubArena(&game_memory.debug->arena, &game_memory.debug->temp_arena, Megabytes(16));
    
    
    Assert(game_memory.debug_storage);
    Assert(game_memory.permanent_storage);
    Assert(game_memory.temporary_storage);
    
    
    game_memory.platform.ReadEntireFile = ReadEntireFile;
    game_memory.platform.WriteEntireFile = WriteEntireFile;
    game_memory.platform.FreeFileMemory = FreeFileMemory;
    
    game_memory.platform.Allocate = DefaultAllocate;
    game_memory.platform.Reallocate = DefaultReallocate;
    game_memory.platform.Free = DefaultFree;
    
    // TODO: should be able to change graphics API on runtime
    game_memory.platform.InitRenderer = OpenGLInit;
    game_memory.platform.RendererEndFrame = OpenGLEndFrame;
    
    game_memory.platform.GetPrefCounter = Win32GetPerformanceCounter;
    game_memory.platform.GetElapsedSeconds = Win32GetElapsedSeconds;
    
    game_memory.platform.PushWorkEntry = PushWorkEntry;
    game_memory.platform.WaitForWorkers = WaitForWorkers;
    
    Win32InitThreads(&game_memory.platform);
    
    game_memory.platform.LogFunction = printf;
    g_Platform = game_memory.platform;
    
    Win32SoundState sound_output = {};
    sound_output.bytes_per_sample = sizeof(i16) * 2;
    sound_output.samples_per_sec = AUDIO_SAMPLE_RATE;
    sound_output.buffer_size = sound_output.samples_per_sec * sound_output.bytes_per_sample;
    sound_output.running_sample_count = 0;
    sound_output.latency_sample_count = (i32)(sound_output.samples_per_sec / target_fps) * 3;
    
    GameInput game_input = {};
    Win32InitDSound(window_handle, sound_output.samples_per_sec, sound_output.buffer_size);
    
    
    u64 last_counter = Win32GetPerformanceCounter();
    
    i16* samples = (i16*)calloc(sound_output.buffer_size, sizeof(i16));
    b8 sound_is_valid = false;
    b8 sound_is_playing = false;
    
    i32 last_game_dll_write_time = Win32GetLastWriteTime("game.dll");
    
    b8 paused = false;
    // ASSIMP 
    
    
    
    while(global_running)
    {
        PROFILER_FRAME_START();
        
        for (sizet i = 0; i < NUM_BUTTONS; ++i)
        {
            game_input.buttons[i].released = 0;
            game_input.buttons[i].pressed = 0;
        }
        game_input.character = -1;
        game_input.mouse.wheel_delta = 0;
        
        Win32ProcessInputMessages(&game_input);
        
        POINT p; 
        GetCursorPos(&p);
        ScreenToClient(window_handle, &p);
        game_input.mouse.position.x = (f32)p.x;
        game_input.mouse.position.y = (f32)p.y;
        
        
        f64 delta_time = Win32GetElapsedSeconds(last_counter);
        last_counter = Win32GetPerformanceCounter();
        
        i32 game_dll_write_time = Win32GetLastWriteTime("game.dll");
        if (last_game_dll_write_time != game_dll_write_time)
        {
            Win32UnloadGameCode(&game);
            game = Win32LoadGameCode();
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
            Log("Could not get sound buffer cursor position! \n");
        }
        
        GameSoundBuffer sound_buffer = {};
        sound_buffer.samples_per_sec = sound_output.samples_per_sec;
        sound_buffer.sample_count = bytes_to_write / sound_output.bytes_per_sample;
        sound_buffer.samples = samples;
        
        
        
        
        if (sound_is_valid)
        {
            Win32FillSoundBuffer(&sound_output, &sound_buffer, byte_offset, bytes_to_write);
            if (!sound_is_playing)
            {
                HRESULT hresult = global_sound_buffer->Play(0, 0, DSBPLAY_LOOPING);
                
                if (FAILED(hresult))
                {
                    Log("Failed to play sound. \n");
                }
                sound_is_playing = true;
            }
        }
#endif
        
        RECT screen_rect;
        GetClientRect(window_handle, &screen_rect);
        i32 w = screen_rect.right - screen_rect.left;
        i32 h = screen_rect.bottom - screen_rect.top;
        game_memory.screen_width = w;
        game_memory.screen_height = h;
        
        if (!paused)
        {
            game.MainLoop(delta_time, &game_memory, NULL, &game_input);
        }
        
        HDC window_dc = GetDC(window_handle);
        SwapBuffers(window_dc);
        
        PROFILER_FRAME_END();
    }
    
    
#if 0
    system("pause");
#endif
    
    return 0;
}
