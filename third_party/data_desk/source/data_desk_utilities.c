/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Data Desk

Author  : Ryan Fleury
Updated : 5 December 2019
License : MIT, at end of file.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))
#define MemorySet memset
#define MemoryCopy memcpy
#define CalculateCStringLength strlen

#define MEMORY_ARENA_BLOCK_SIZE_DEFAULT 4096
typedef struct MemoryArenaBlock MemoryArenaBlock;
struct MemoryArenaBlock
{
    char *memory;
    int memory_size;
    int memory_alloc_position;
    MemoryArenaBlock *next;
};

typedef struct MemoryArena MemoryArena;
struct MemoryArena
{
    MemoryArenaBlock *first_block;
    MemoryArenaBlock *active_block;
};

static void *
MemoryArenaAllocate(MemoryArena *arena, unsigned int size)
{
    if(!arena->active_block ||
       arena->active_block->memory_alloc_position + size > arena->active_block->memory_size)
    {
        unsigned int needed_bytes = MEMORY_ARENA_BLOCK_SIZE_DEFAULT;
        if(size > needed_bytes)
        {
            needed_bytes = size;
        }
        
        MemoryArenaBlock *new_block = 0;
        new_block = calloc(1, sizeof(MemoryArenaBlock) + needed_bytes);
        Assert(new_block != 0);
        new_block->memory = (char *)new_block + sizeof(MemoryArenaBlock);
        new_block->memory_size = needed_bytes;
        new_block->next = 0;
        
        if(arena->active_block)
        {
            arena->active_block->next = new_block;
            arena->active_block = new_block;
        }
        else
        {
            arena->first_block = new_block;
            arena->active_block = new_block;
        }
    }
    
    Assert(arena->active_block &&
           arena->active_block->memory_alloc_position + size <=
           arena->active_block->memory_size);
    
    void *memory = arena->active_block->memory + arena->active_block->memory_alloc_position;
    arena->active_block->memory_alloc_position += size;
    return memory;
}

static void
MemoryArenaClear(MemoryArena *arena)
{
    for(MemoryArenaBlock *block = arena->first_block; block;)
    {
        MemoryArenaBlock *next = block->next;
        free(block);
        block = next;
    }
}

static int
StringMatchCaseSensitiveN(char *a, char *b, int n)
{
    int matches = 0;
    if(a && b && n)
    {
        matches = 1;
        for(int i = 0; i < n; ++i)
        {
            if(a[i] != b[i])
            {
                matches = 0;
                break;
            }
        }
    }
    return matches;
}

static int
StringMatchCaseSensitive(char *a, char *b)
{
    int matches = 0;
    if(a && b)
    {
        matches = 1;
        for(int i = 0;; ++i)
        {
            if(a[i] != b[i])
            {
                matches = 0;
                break;
            }
            else if(!a[i])
            {
                break;
            }
        }
    }
    return matches;
}

static int
StringMatchCaseInsensitive(char *a, char *b)
{
    int matches = 0;
    if(a && b)
    {
        matches = 1;
        for(int i = 0; a[i] && b[i]; ++i)
        {
            if(a[i] != b[i])
            {
                matches = 0;
                break;
            }
        }
    }
    return matches;
}

static int
CharIsAlpha(int c)
{
    return ((c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z'));
}

static int
CharIsDigit(int c)
{
    return (c >= '0' && c <= '9');
}

static int
CharIsSymbol(int c)
{
    return (c == '~' ||
            c == '`' ||
            c == '!' ||
            c == '#' ||
            c == '$' ||
            c == '%' ||
            c == '^' ||
            c == '&' ||
            c == '*' ||
            c == '(' ||
            c == ')' ||
            c == '-' ||
            c == '+' ||
            c == '=' ||
            c == '{' ||
            c == '}' ||
            c == '[' ||
            c == ']' ||
            c == ':' ||
            c == ';' ||
            c == '<' ||
            c == '>' ||
            c == ',' ||
            c == '.' ||
            c == '?' ||
            c == '/');
}

static int
CharToLower(int c)
{
    if(c >= 'A' && c <= 'Z')
    {
        return c + 32;
    }
    return c;
}

static int
CharToUpper(int c)
{
    if(c >= 'a' && c <= 'z')
    {
        return c - 32;
    }
    return c;
}

static char *
LoadEntireFileAndNullTerminate(char *filename)
{
    char *result = 0;
    
    FILE *file = fopen(filename, "rb");
    if(file)
    {
        fseek(file, 0, SEEK_END);
        unsigned int file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        result = malloc(file_size+1);
        if(result)
        {
            fread(result, 1, file_size, file);
            result[file_size] = 0;
        }
        fclose(file);
    }
    
    return result;
}

/*
Copyright 2019 Ryan Fleury

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
