internal inline u32
string_length(char* string)
{
    u32 len = 0;
    while(string[len])
    {
        len++;
    }

    return len;
}

internal inline b8
string_equals(char* s1, char* s2)
{
    u32 len = string_length(s1);

    for (u32 i = 0; i < len; ++i)
    {
        if (s1[i] != s2[i])
        {
           return false;
        }
    }

    return true;
}


internal inline i32
last_backslash_index(char* str)
{
    u32 len = string_length(str);
    for(u32 i = len-1; i >= 0; --i)
    {
        if (str[i] == '/')
        {
            return i;
        }
    }
    ASSERT(false);
    return -1;
}


internal inline u32
string_copy(char* dest, char* src)
{
    u32 len = string_length(src);

    for (u32 i = 0; i < len; ++i)
    {
        dest[i] = src[i];
    }
    dest[len] = '\0';

    return len;
}

internal inline u32
string_copy(char* dest, char* src, u32 num_chars)
{

    for (u32 i = 0; i < num_chars; ++i)
    {
        dest[i] = src[i];
    }
    dest[num_chars] = '\0';

    return num_chars;
}

internal inline char*
string_copy(MemoryArena* arena, char* src)
{
    u32 len = string_length(src);
    char* out = PushMemory(arena, char, len + 1);

    for (u32 i = 0; i < len; ++i)
    {
        out[i] = src[i];
    }
    out[len] = '\0';

    return out;
}

internal inline u32
string_append(char* dest, char* src, u32 num_chars)
{
    u32 len = string_length(dest);

    for (u32 i = 0; i < num_chars; ++i)
    {
        dest[len+i] = src[i];
    }
    dest[len+num_chars] = '\0';

    return len+num_chars;
}
