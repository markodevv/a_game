/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Data Desk

Author  : Ryan Fleury
Updated : 5 December 2019
License : MIT, at end of file.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct Tokenizer Tokenizer;
struct Tokenizer
{
    char *at;
    char *filename;
    int line;
};

enum
{
    Token_Invalid,
    Token_AlphanumericBlock,
    Token_NumericConstant,
    Token_StringConstant,
    Token_CharConstant,
    Token_SymbolicBlock,
    Token_Tag,
};

typedef struct Token Token;
struct Token
{
    int type;
    
    // NOTE(rjf): The actual string that anyone cares about.
    char *string;
    int string_length;
    
    // NOTE(rjf): The "entire string", for skipping the relevant memory of the token.
    char *base;
    int base_length;
    
    int lines_traversed;
};

static Token
GetNextTokenFromBuffer(Tokenizer *tokenizer)
{
    Token token = {0};
    
    int skip_mode = 0;
    enum
    {
        SkipMode_None,
        SkipMode_LineComment,
        SkipMode_BlockComment,
    };
    int block_comment_nest_level = 0;
    
    char *buffer = tokenizer->at;
    for(int i = 0; buffer[i]; ++i)
    {
        if(skip_mode == SkipMode_None)
        {
            if(buffer[i] == '/' && buffer[i+1] == '/')
            {
                skip_mode = SkipMode_LineComment;
                ++i;
            }
            else if(buffer[i] == '/' && buffer[i+1] == '*')
            {
                block_comment_nest_level = 1;
                skip_mode = SkipMode_BlockComment;
                ++i;
            }
            else
            {
                
                int j = 0;
                
                // NOTE(rjf): Alphanumeric block
                if(CharIsAlpha(buffer[i]) || buffer[i] == '_')
                {
                    for(j = i+1; buffer[j]; ++j)
                    {
                        if(!CharIsAlpha(buffer[j]) && buffer[j] != '_' &&
                           !CharIsDigit(buffer[j]))
                        {
                            break;
                        }
                    }
                    token.type = Token_AlphanumericBlock;
                }
                
                // NOTE(rjf): Arbitrary String Alphanumeric Block
                else if(buffer[i] == '`')
                {
                    for(j = i+1; buffer[j]; ++j)
                    {
                        if(buffer[j] == '`')
                        {
                            j += 1;
                            break;
                        }
                    }
                    token.type = Token_AlphanumericBlock;
                    token.string = buffer + i + 1;
                    token.string_length = j - i - 2;
                }
                
                // NOTE(rjf): Numeric block
                else if(CharIsDigit(buffer[i]) || (buffer[i] == '-' && CharIsDigit(buffer[i+1])))
                {
                    for(j = i+1; buffer[j]; ++j)
                    {
                        if(!CharIsAlpha(buffer[j]) && buffer[j] != '.' &&
                           !CharIsDigit(buffer[j]))
                        {
                            break;
                        }
                    }
                    token.type = Token_NumericConstant;
                }
                
                // NOTE(rjf): String constant
                else if(buffer[i] == '"')
                {
                    
                    // NOTE(rjf): Multiline string constant
                    if(buffer[i+1] == '"' && buffer[i+2] == '"')
                    {
                        for(j = i+3; buffer[j]; ++j)
                        {
                            if(buffer[j] == '"' && buffer[j+1] == '"' && buffer[j+2] == '"')
                            {
                                j += 3;
                                break;
                            }
                        }
                    }
                    
                    // NOTE(rjf): Single line string constant
                    else
                    {
                        int escaped = 0;
                        for(j = i+1; buffer[j]; ++j)
                        {
                            if(escaped)
                            {
                                escaped = 0;
                            }
                            else
                            {
                                if(buffer[j] == '"')
                                {
                                    ++j;
                                    break;
                                }
                                else if(buffer[j] == '\\')
                                {
                                    escaped = 1;
                                }
                            }
                        }
                    }
                    token.type = Token_StringConstant;
                }
                
                // NOTE(rjf): Char constant
                else if(buffer[i] == '\'')
                {
                    int escaped = 0;
                    for(j = i+1; buffer[j]; ++j)
                    {
                        if(escaped)
                        {
                            escaped = 0;
                        }
                        else
                        {
                            if(buffer[j] == '\'')
                            {
                                break;
                            }
                            else if(buffer[j] == '\\')
                            {
                                escaped = 1;
                            }
                        }
                    }
                    token.type = Token_CharConstant;
                }
                
                // NOTE(rjf): Tag block
                else if(buffer[i] == '@')
                {
                    for(j = i+1; buffer[j]; ++j)
                    {
                        if(!CharIsAlpha(buffer[j]) && buffer[j] != '_' &&
                           !CharIsDigit(buffer[j]))
                        {
                            break;
                        }
                    }
                    token.type = Token_Tag;
                }
                
                // NOTE(rjf): Symbolic block
                else if(CharIsSymbol(buffer[i]))
                {
                    static char *symbolic_blocks_to_break_out[] =
                    {
                        "(",
                        ")",
                        "[",
                        "]",
                        "{",
                        "}",
                        ";",
                        "::",
                        "->",
                        ":",
                        "*",
                        "+",
                        "-",
                        "/",
                        ";"
                    };
                    
                    for(j = i+1; buffer[j]; ++j)
                    {
                        if(!CharIsSymbol(buffer[j]))
                        {
                            break;
                        }
                    }
                    
                    for(int k = 0; k < ArrayCount(symbolic_blocks_to_break_out); ++k)
                    {
                        int string_length = 0;
                        while(symbolic_blocks_to_break_out[k][string_length++]);
                        --string_length;
                        
                        if(StringMatchCaseSensitiveN(symbolic_blocks_to_break_out[k],
                                                     buffer+i,
                                                     string_length))
                        {
                            j = i + string_length;
                            goto end_symbolic_block;
                        }
                    }
                    
                    end_symbolic_block:;
                    token.type = Token_SymbolicBlock;
                }
                
                if(j)
                {
                    token.base = buffer+i;
                    token.base_length = j-i;
                    
                    // NOTE(rjf): If one of the tokenizing rules didn't set the string/length
                    // manually, assume it is the same as the base.
                    if(token.string == 0)
                    {
                        token.string = token.base;
                        token.string_length = token.base_length;
                    }
                    break;
                }
            }
        }
        else if(skip_mode == SkipMode_LineComment)
        {
            if(buffer[i] == '\n')
            {
                skip_mode = 0;
            }
        }
        
        else if(skip_mode == SkipMode_BlockComment)
        {
            
            if(buffer[i] == '/' && buffer[i+1] == '*')
            {
                ++block_comment_nest_level;
                ++i;
            }
            else if(buffer[i] == '*' && buffer[i+1] == '/')
            {
                if(!--block_comment_nest_level)
                {
                    skip_mode = 0;
                }
                ++i;
            }
        }
        
    }
    
    // NOTE(rjf): This is probably a little slow, but will work
    // for now. Ideally this calculation would happen as a token
    // is being processed, but I'm in a crunch so I'm not going
    // to care about performance right now.
    if(token.base)
    {
        token.lines_traversed = 0;
        for(int i = (char *)tokenizer->at - (char *)token.base;
            i < token.base_length;
            ++i)
        {
            if(token.base[i] == '\n')
            {
                ++token.lines_traversed;
            }
        }
    }
    
    return token;
}

static Token
PeekToken(Tokenizer *tokenizer)
{
    Token token = GetNextTokenFromBuffer(tokenizer);
    return token;
}

static Token
NextToken(Tokenizer *tokenizer)
{
    Token token = GetNextTokenFromBuffer(tokenizer);
    tokenizer->at = token.base + token.base_length;
    tokenizer->line += token.lines_traversed;
    return token;
}

static int
TokenMatch(Token token, char *string)
{
    return (token.type != Token_Invalid &&
            StringMatchCaseSensitiveN(token.string, string, token.string_length) &&
            string[token.string_length] == 0);
}

static int
RequireToken(Tokenizer *tokenizer, char *string, Token *token_ptr)
{
    int match = 0;
    Token token = GetNextTokenFromBuffer(tokenizer);
    if(TokenMatch(token, string))
    {
        tokenizer->at = token.base + token.base_length;
        tokenizer->line += token.lines_traversed;
        if(token_ptr)
        {
            *token_ptr = token;
        }
        match = 1;
    }
    return match;
}

static int
RequireTokenType(Tokenizer *tokenizer, int type, Token *token_ptr)
{
    int match = 0;
    Token token = GetNextTokenFromBuffer(tokenizer);
    if(type == token.type)
    {
        tokenizer->at = token.base + token.base_length;
        tokenizer->line += token.lines_traversed;
        if(token_ptr)
        {
            *token_ptr = token;
        }
        match = 1;
    }
    return match;
}

static char *
GetBinaryOperatorStringFromType(int type)
{
    static char *binary_operator_strings[] =
    {
        "INVALIDBINARYOPERATOR",
        "+",
        "-",
        "*",
        "/",
        "%",
        "<<",
        ">>",
        "&",
        "|",
        "&&",
        "||",
    };
    
    return binary_operator_strings[type];
}

static int
GetBinaryOperatorTypeFromToken(Token token)
{
    int type = DataDeskBinaryOperatorType_Invalid;
    
    for(int i = 1; i < DataDeskBinaryOperatorType_Max; ++i)
    {
        if(TokenMatch(token, GetBinaryOperatorStringFromType(i)))
        {
            type = i;
            break;
        }
    }
    
    return type;
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
