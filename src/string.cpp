internal inline u32
StringLength(char* string)
{
    u32 len = 0;
    while(string[len])
    {
        len++;
    }
    
    return len;
}

internal inline b32
StringMatch(char* s1, char* s2)
{
    Assert(s1 && s2);
    
    u32 len = StringLength(s1);
    
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
LastBackslashIndex(char* str)
{
    u32 len = StringLength(str);
    for(u32 i = len-1; i >= 0; --i)
    {
        if (str[i] == '/')
        {
            return i;
        }
    }
    Assert(false);
    return -1;
}


internal inline u32
StringCopy(char* dest, char* src)
{
    u32 len = StringLength(src);
    
    for (u32 i = 0; i < len; ++i)
    {
        dest[i] = src[i];
    }
    dest[len] = '\0';
    
    return len;
}

internal inline u32
StringCopy(char* dest, char* src, u32 num_chars)
{
    
    for (u32 i = 0; i < num_chars; ++i)
    {
        dest[i] = src[i];
    }
    dest[num_chars] = '\0';
    
    return num_chars;
}

internal inline char*
StringCopy(MemoryArena* arena, char* src)
{
    Assert(src);
    
    u32 len = StringLength(src);
    char* out = PushMemory(arena, char, len + 1);
    
    for (u32 i = 0; i < len; ++i)
    {
        out[i] = src[i];
    }
    out[len] = '\0';
    
    return out;
}

internal inline u32
StringAppend(char* dest, char* src, u32 num_chars)
{
    u32 len = StringLength(dest);
    
    for (u32 i = 0; i < num_chars; ++i)
    {
        dest[len+i] = src[i];
    }
    dest[len+num_chars] = '\0';
    
    return len+num_chars;
}
