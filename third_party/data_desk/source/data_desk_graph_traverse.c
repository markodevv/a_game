/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Data Desk

Author  : Ryan Fleury
Updated : 11 May 2020
License : MIT, at end of file.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static void
GenerateGraphNullTerminatedStrings(ParseContext *context, DataDeskNode *root)
{
    for(DataDeskNode *node = root; node; node = node->next)
    {
        if(node->string)
        {
            char *new_string = ParseContextAllocateMemory(context, node->string_length+1);
            MemoryCopy(new_string, node->string, node->string_length);
            new_string[node->string_length] = 0;
            node->string = new_string;
            
            typedef struct Word Word;
            struct Word
            {
                char *string;
                int length;
            };
            
            typedef struct WordChunk WordChunk;
            struct WordChunk
            {
                Word words[16];
                int word_count;
                WordChunk *next;
            };
            
            unsigned int total_bytes_from_all_words = 0;
            unsigned int total_word_count = 0;
            WordChunk *first_word_chunk = 0;
            WordChunk *last_word_chunk = 0;
            
            // NOTE(rjf): Find all words in node's name.
            {
                for(int i = 0; i < node->string_length; ++i)
                {
                    char *word_start = 0;
                    int word_length = 0;
                    
                    if(i == 0 || node->string[i - 1] == '_' ||
                       (node->string[i-1] >= 'a' && node->string[i-1] <= 'z' &&
                        node->string[i+0] >= 'A' && node->string[i+0] <= 'Z'))
                    {
                        word_start = node->string + i;
                        int found_first_lowercase = 0;
                        for(int j = i+1; j <= node->string_length; ++j)
                        {
                            if(node->string[j] >= 'a' && node->string[j] <= 'z')
                            {
                                found_first_lowercase = 1;
                            }
                            
                            if(node->string[j] == '_' ||
                               (node->string[j] >= 'A' && node->string[j] <= 'Z' &&
                                found_first_lowercase) ||
                               j == node->string_length)
                            {
                                word_length = j-i;
                                break;
                            }
                        }
                    }
                    
                    if(word_start)
                    {
                        WordChunk *chunk = last_word_chunk;
                        if(chunk == 0 || chunk->word_count >= ArrayCount(chunk->words))
                        {
                            WordChunk *new_chunk = ParseContextAllocateMemory(context, sizeof(*chunk));
                            if(chunk == 0)
                            {
                                first_word_chunk = last_word_chunk = new_chunk;
                            }
                            else
                            {
                                last_word_chunk->next = new_chunk;
                                last_word_chunk = last_word_chunk->next;
                            }
                            chunk = new_chunk;
                        }
                        
                        Word word =
                        {
                            word_start,
                            word_length,
                        };
                        chunk->words[chunk->word_count++] = word;
                        total_bytes_from_all_words += word.length;
                        total_word_count += 1;
                    }
                }
            }
            
            // NOTE(rjf): Generate name transforms from words.
            {
                for(DataDeskWordStyle word_style = (DataDeskWordStyle)0;
                    word_style < DataDeskWordStyle_Max;
                    word_style += 1)
                {
                    for(DataDeskWordStyle first_word_style = (DataDeskWordStyle)0;
                        first_word_style < DataDeskWordStyle_Max;
                        first_word_style += 1)
                    {
                        for(DataDeskWordSeparator separator = (DataDeskWordSeparator)0;
                            separator < DataDeskWordSeparator_Max;
                            separator += 1)
                        {
                            unsigned int needed_bytes = 0;
                            
                            // NOTE(rjf): Bytes for all characters in name
                            needed_bytes += total_bytes_from_all_words;
                            
                            // NOTE(rjf): Bytes for all separators
                            if(separator != DataDeskWordSeparator_None)
                            {
                                needed_bytes += total_word_count - 1;
                            }
                            
                            // NOTE(rjf): Null-terminator byte
                            needed_bytes += 1;
                            
                            char *transformed = ParseContextAllocateMemory(context, needed_bytes);
                            int name_write_pos = 0;
                            for(WordChunk *chunk = first_word_chunk; chunk; chunk = chunk->next)
                            {
                                for(int i = 0; i < chunk->word_count; ++i)
                                {
                                    MemoryCopy(transformed + name_write_pos, chunk->words[i].string, chunk->words[i].length);
                                    
                                    DataDeskWordStyle style = word_style;
                                    
                                    // NOTE(rjf): Style first word differently.
                                    if(chunk == first_word_chunk && i == 0)
                                    {
                                        style = first_word_style;
                                    }
                                    
                                    if(style == DataDeskWordStyle_Word)
                                    {
                                        for(int j = name_write_pos; j < name_write_pos + chunk->words[i].length; ++j)
                                        {
                                            transformed[j] = CharToLower(transformed[j]);
                                        }
                                        transformed[name_write_pos] = CharToUpper(transformed[name_write_pos]);
                                    }
                                    else if(style == DataDeskWordStyle_word)
                                    {
                                        for(int j = name_write_pos; j < name_write_pos + chunk->words[i].length; ++j)
                                        {
                                            transformed[j] = CharToLower(transformed[j]);
                                        }
                                    }
                                    else if(style == DataDeskWordStyle_WORD)
                                    {
                                        for(int j = name_write_pos; j < name_write_pos + chunk->words[i].length; ++j)
                                        {
                                            transformed[j] = CharToUpper(transformed[j]);
                                        }
                                    }
                                    
                                    name_write_pos += chunk->words[i].length;
                                    
                                    // NOTE(rjf): Add separator if this isn't the last word.
                                    if(separator)
                                    {
                                        if(i < chunk->word_count || chunk->next)
                                        {
                                            transformed[name_write_pos] =
                                                separator == DataDeskWordSeparator_Space ? ' ' : '_';
                                            name_write_pos += 1;
                                        }
                                    }
                                }
                            }
                            transformed[needed_bytes-1] = 0;
                            node->name_transforms[word_style][first_word_style][separator] = transformed;
                        }
                        
                    }
                    
                }
                
            }
            
        }
        
        if(node->type != DataDeskNodeType_Identifier)
        {
            GenerateGraphNullTerminatedStrings(context, node->children_list_head);
        }
        GenerateGraphNullTerminatedStrings(context, node->tag_list_head);
    }
}

static void
LinkNamespaceAliases(ParseContext *context, DataDeskNode *root)
{
    for(DataDeskNode *node = root; node; node = node->next)
    {
        if(node->string && !node->namespace_alias_prev && !node->namespace_alias_next)
        {
            ParseContextSymbolTableKey key = {0};
            key.key = node->string;
            key.key_length = node->string_length;

            DataDeskNode *prev = 0;
            for(int i_namespace = 0; i_namespace < context->namespace_count; ++i_namespace){
                key.namespace_index = i_namespace;
                DataDeskNode *cur = ParseContextLookUpSymbol(context, key);
                if(cur)
                {
                    if(prev)
                    {
                        cur->namespace_alias_prev = prev;
                        prev->namespace_alias_next = cur;
                    }
                    prev = cur;
                }
            }
        }
    }
}

static void
CallCustomParseCallbacks(ParseContext *context, DataDeskNode *root, DataDeskCustom custom, char *filename)
{
    if(custom.ParseCallback)
    {
        for(DataDeskNode *node = root; node; node = node->next)
        {
            custom.ParseCallback(node, filename);
        }
    }
}

static void
PatchGraphSymbols(ParseContext *context, DataDeskNode *root)
{
    for(DataDeskNode *node = root; node; node = node->next)
    {
        if(node->type == DataDeskNodeType_Identifier)
        {
            ParseContextSymbolTableKey key = {0};
            key.key = node->string;
            key.key_length = node->string_length;
            key.namespace_index = node->namespace_index;

            node->reference = ParseContextLookUpSymbol(context, key);

            if(!node->reference && node->parent->type != DataDeskNodeType_EnumDeclaration)
            {
                node->namespace_string = 0;
            }
        }
        else
        {
            PatchGraphSymbols(context, node->children_list_head);
            PatchGraphSymbols(context, node->tag_list_head);
        }
    }
}

/*
Copyright 2020 Ryan Fleury

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