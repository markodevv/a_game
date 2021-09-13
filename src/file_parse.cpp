internal bool 
IsNewLineChar(char c)
{
    return c == '\n' || c == '\n\r';
}

internal bool 
IsWhiteSpace(char c)
{
    return c == ' ';
}

internal bool
IsDigit(char c)
{
    return c >= 48 && c <= 57 || c == '.' || c == '-';
}

internal char
NextChar(FileResult* file)
{
    Assert(file->cursor <= file->size);
    
    return ((char*)file->data)[++file->cursor];
}

internal char
CurrentChar(FileResult* file)
{
    return ((char*)file->data)[file->cursor];
}

internal b8
IsEOF(FileResult* file)
{
    return (file->size == file->cursor);
}

internal void
SkipWhiteSpace(FileResult* file)
{
    while (IsWhiteSpace(CurrentChar(file)))
    {
        NextChar(file);
    }
}

internal u32
NextInt(FileResult* file)
{
    u32 result = 0;
    char string[32];
    u8 index = 0;
    
    char c = CurrentChar(file);
    Assert(IsDigit(c));
    
    while(IsDigit(c))
    {
        string[index++] = c;
        c = NextChar(file);
    }
    
    string[index] = '\0';
    result = ToInt(string);
    
    if (c == '/')
    {
        NextChar(file);
    }
    
    return result;
    
}

internal f32
NextFloat(FileResult* file)
{
    f32 result = 0;
    char string[32];
    u8 index = 0;
    
    char c = CurrentChar(file);
    Assert(IsDigit(c));
    
    while(IsDigit(c))
    {
        string[index++] = c;
        c = NextChar(file);
    }
    
    string[index] = '\0';
    result = ToFloat(string);
    
    if (c == '/')
    {
        NextChar(file);
    }
    
    return result;
    
}



internal vec3
NextVec3(FileResult* file)
{
    vec3 out = {};
    u8 vertex_axis = 0;
    
    char c = CurrentChar(file);
    while(1)
    {
        if (IsDigit(c))
        {
            Vec3At(out, vertex_axis++) = NextFloat(file);
        }
        
        c = CurrentChar(file);
        
        if (IsNewLineChar(c))
        {
            break;
        }
        
        c = NextChar(file);
    }
    
    Assert(vertex_axis == 3);
    
    return out;
}

internal vec2
NextVec2(FileResult* file)
{
    vec2 out = {};
    u8 vertex_axis = 0;
    
    char c = CurrentChar(file);
    
    while(1)
    {
        if (IsDigit(c))
        {
            Vec2At(out, vertex_axis++) = NextFloat(file);
        }
        
        c = CurrentChar(file);
        
        if (IsNewLineChar(c))
        {
            break;
        }
        
        c = NextChar(file);
    }
    Assert(vertex_axis == 2);
    
    return out;
}

internal b8
NextLine(FileResult* file)
{
    b8 result = true;
    char c = CurrentChar(file);
    
    while(1)
    {
        if (IsNewLineChar(c))
        {
            NextChar(file);
            break;
        }
        c = NextChar(file);
    }
    
    return result;
}

internal void
NextWord(FileResult* file, char* buffer)
{
    char c = CurrentChar(file);
    while(1)
    {
        if (IsEOF(file) || IsWhiteSpace(c))
        {
            *buffer = '\0';
            break;
        }
        
        *buffer++ = c;
        c = NextChar(file);
    }
}

internal b8
IsPathSlash(char c)
{
    return (c == '\\' || c == '/');
}

internal u32
GetPathFromFilePath(char* buffer, u32 buffer_size, char* file_path)
{
    u32 path_len = StringLength(file_path);
    
    Assert(!IsPathSlash(file_path[path_len-1]) &&
           buffer_size >= path_len);
    
    u32 cursor = path_len;
    
    for (cursor; cursor >= 0; --cursor)
    {
        if (IsPathSlash(file_path[cursor]))
        {
            // This is for null char
            cursor++;
            buffer[cursor] = '\0';
            break;
        }
    }
    
    
    for (u32 i = 0; i < cursor; ++i)
    {
        buffer[i] = file_path[i];
    }
    
    return cursor;
}