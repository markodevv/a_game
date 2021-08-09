/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Data Desk

Author  : Ryan Fleury
Updated : 20 May 2020
License : MIT, at end of file.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// NOTE(rjf): Platform-Specific
#if BUILD_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif BUILD_LINUX
#include <dlfcn.h>
#endif

// NOTE(rjf): C Runtime Library
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

// NOTE(rjf): Data Desk Code
#define DATA_DESK_USE_OLD_NAMES 0
#include "data_desk.h"
#include "data_desk_debug.c"
#include "data_desk_utilities.c"
#include "data_desk_tokenizer.c"
#include "data_desk_custom.c"
#include "data_desk_parse.c"
#include "data_desk_graph_traverse.c"

static void
PrintAndResetParseContextErrors(ParseContext *context)
{
    for(int i = 0; i < context->error_stack_size; ++i)
    {
        fprintf(stderr, "ERROR (%s:%i): %s\n",
                context->error_stack[i].file,
                context->error_stack[i].line,
                context->error_stack[i].string);
    }
    context->error_stack_size = 0;
}

static DataDeskNode *
ParseFile(ParseContext *context, char *file, char *filename)
{
    Tokenizer tokenizer = {0};
    {
        tokenizer.at = file;
        tokenizer.filename = filename;
        tokenizer.line = 1;
    }
    
    DataDeskNode *root = ParseCode(context, &tokenizer);
    PrintAndResetParseContextErrors(context);
    
    // NOTE(rjf): ParseContextCleanUp shouldn't be called, because often time, code
    // will depend on ASTs persisting between files (which should totally work).
    // So, we won't clean up anything, and let the operating system do it ond
    // program exit. We can still let a ParseContext go out of scope, though,
    // because the memory will stay allocated. Usually, this is called a leak and
    // thus harmful, but because we want memory for all ASTs ever parsed within
    // a given runtime to persist (because we store pointers to them, etc.), we
    // actually don't care; this is /exactly/ what we want. The operating system
    // frees the memory on exit, and for this reason, there is literally no reason
    // to care about AST clean-up at all.
    // ParseContextCleanUp(&context);
    
    // NOTE(rjf): This is a reason why non-nuanced and non-context-specific programming
    // rules suck.
    
    return root;
}

static void
ProcessParsedGraph(char *filename, DataDeskNode *root, ParseContext *context, DataDeskCustom custom)
{
    PatchGraphSymbols(context, root);
    GenerateGraphNullTerminatedStrings(context, root);
    LinkNamespaceAliases(context, root);
    CallCustomParseCallbacks(context, root, custom, filename);
    PrintAndResetParseContextErrors(context);
}

int
main(int argument_count, char **arguments)
{
    if(argument_count > 1)
    {
        if(StringMatchCaseInsensitive(arguments[1], "help"  ) ||
           StringMatchCaseInsensitive(arguments[1], "-help" ) ||
           StringMatchCaseInsensitive(arguments[1], "--help") ||
           StringMatchCaseInsensitive(arguments[1], "-h"    ) ||
           StringMatchCaseInsensitive(arguments[1], "--h"   ) ||
           StringMatchCaseInsensitive(arguments[1], "/?"    ) ||
           StringMatchCaseInsensitive(arguments[1], "?"     ) ||
           StringMatchCaseInsensitive(arguments[1], "-?"    ))
        {
            printf("Data Desk Flags\n");
            printf("--custom    (-c)        Specify the path to a custom layer to which parsed information is to be sent.\n");
            printf("--log       (-l)        Enable logging.\n");
        }
        else
        {
            DataDeskCustom custom = {0};
            char *custom_layer_dll_path = 0;
            int expected_number_of_files = 0;
            
            // NOTE(rjf): Load command line arguments and set all non-file arguments
            // to zero, so that we know the arguments to process in the file-processing
            // loop.
            {
                
                // NOTE(rjf): Figure out if logging is enabled.
                {
                    for(int i = 1; i < argument_count; ++i)
                    {
                        if(StringMatchCaseInsensitive(arguments[i], "-l") ||
                           StringMatchCaseInsensitive(arguments[i], "--log"))
                        {
                            global_log_enabled = 1;
                        }
                    }
                }
                
                // NOTE(rjf): Print out command line.
                if(global_log_enabled)
                {
                    for(int i = 0; i < argument_count; ++i)
                    {
                        fprintf(stdout, "%s ", arguments[i]);
                    }
                }
                
                // NOTE(rjf): Find all files and custom DLL stuff.
                {
                    int argument_read_mode = 0;
                    enum
                    {
                        ArgumentReadMode_Files,
                        ArgumentReadMode_CustomLayerDLL,
                    };
                    
                    for(int i = 1; i < argument_count; ++i)
                    {
                        if(arguments[i])
                        {
                            if(argument_read_mode == ArgumentReadMode_Files)
                            {
                                if(StringMatchCaseInsensitive(arguments[i], "-c") ||
                                   StringMatchCaseInsensitive(arguments[i], "--custom"))
                                {
                                    argument_read_mode = ArgumentReadMode_CustomLayerDLL;
                                    arguments[i] = 0;
                                }
                                if(StringMatchCaseInsensitive(arguments[i], "-l") ||
                                   StringMatchCaseInsensitive(arguments[i], "--log"))
                                {
                                    arguments[i] = 0;
                                }
                                else
                                {
                                    ++expected_number_of_files;
                                }
                            }
                            else if(argument_read_mode == ArgumentReadMode_CustomLayerDLL)
                            {
                                custom_layer_dll_path = arguments[i];
                                arguments[i] = 0;
                                argument_read_mode = ArgumentReadMode_Files;
                            }
                        }
                    }
                }
            }
            
            Log("Data Desk v" DATA_DESK_VERSION_STRING);
            
            // NOTE(rjf): Load custom code DLL if needed.
            if(custom_layer_dll_path)
            {
                Log("Loading custom layer from \"%s\".", custom_layer_dll_path);
                custom = DataDeskCustomLoad(custom_layer_dll_path);
            }
            else
            {
                LogError("WARNING: No custom layer loaded.");
            }
            
            if(custom.InitCallback)
            {
                custom.InitCallback();
            }
            
            ParseContext parse_context = {0};
            
            int number_of_parsed_files = 0;
            struct
            {
                DataDeskNode *root;
                char *filename;
            }
            *parsed_files = ParseContextAllocateMemory(&parse_context, sizeof(*parsed_files) * expected_number_of_files);
            
            Assert(parsed_files != 0);
            
            for(int i = 1; i < argument_count; ++i)
            {
                if(arguments[i] != 0)
                {
                    char *filename = arguments[i];
                    Log("Processing file at \"%s\".", filename);
                    char *file = LoadEntireFileAndNullTerminate(filename);
                    if(file)
                    {
                        DataDeskNode *root = ParseFile(&parse_context, file, filename);
                        parsed_files[number_of_parsed_files].root = root;
                        parsed_files[number_of_parsed_files].filename = filename;
                        ++number_of_parsed_files;
                    }
                    else
                    {
                        LogError("ERROR: Could not load \"%s\".", filename);
                    }
                }
            }
            
            for(int i = 0; i < number_of_parsed_files; ++i)
            {
                ProcessParsedGraph(parsed_files[i].filename, parsed_files[i].root, &parse_context, custom);
            }
            
            if(custom.CleanUpCallback)
            {
                custom.CleanUpCallback();
            }
            
            DataDeskCustomUnload(&custom);
        }
    }
    else
    {
        LogError("USAGE: %s [-c|--custom <path to custom layer DLL>] [-l|--log] <files to process>",
                 arguments[0]);
    }
    
    return 0;
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