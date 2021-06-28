#include <md.h>
#include <md_c_helpers.h>

#include <md.c>
#include <md_c_helpers.c>

FILE* print_file;
FILE* syntax_file;
bool generate_print_functions = false;
bool update_syntax_file = false;

MD_String8 syntax_file_last_line;

MD_String8
GetHeaderStringFromMdesk(MD_String8 string)
{
    MD_String8 result = MD_PushStringCopy(string);
    result = MD_StringChop(result, 4);
    result.str[result.size-1] = 'h';

    return result;
}

void
GeneratePrintFunction(MD_Node* node)
{
    fprintf(print_file, "void DEBUG_LOG(char* name, %.*s var)\n{\n", MD_StringExpand(node->string),
                                                       MD_StringExpand(node->string));
    fprintf(print_file, "printf(\"@%%s\\n\", name);\n");

    for(MD_EachNode(child, node->first_child))
    {
        if (MD_StringMatch(child->first_child->string, MD_S8Lit("f32"), 0) ||
            MD_StringMatch(child->first_child->string, MD_S8Lit("f64"), 0))
        {
            fprintf(print_file, "printf(\"%.*s : %%.2f\\n\", ", MD_StringExpand(child->string));
            fprintf(print_file, "var.%.*s);\n", MD_StringExpand(child->string));
        }
        else if (MD_StringMatch(child->first_child->string, MD_S8Lit("i32"), 0) ||
                 MD_StringMatch(child->first_child->string, MD_S8Lit("i8"), 0)  ||
                 MD_StringMatch(child->first_child->string, MD_S8Lit("i16"), 0) ||
                 MD_StringMatch(child->first_child->string, MD_S8Lit("u8"), 0)  ||
                 MD_StringMatch(child->first_child->string, MD_S8Lit("u16"), 0) ||
                 MD_StringMatch(child->first_child->string, MD_S8Lit("i64"), 0))
        {
            fprintf(print_file, "printf(\"%.*s : %%d\\n\", ", MD_StringExpand(child->string));
            fprintf(print_file, "var.%.*s);\n", MD_StringExpand(child->string));
        }
        else if (MD_StringMatch(child->first_child->string, MD_S8Lit("u32"), 0) ||
                 MD_StringMatch(child->first_child->string, MD_S8Lit("u64"), 0))
        {
            fprintf(print_file, "printf(\"%.*s : %%lu\\n\", ", MD_StringExpand(child->string));
            fprintf(print_file, "var.%.*s);\n", MD_StringExpand(child->string));
        }
        else
        {
            // printf("Type name |%.*s| \n", MD_StringExpand(child->string));
            if (!MD_StringMatch(child->string, MD_S8Lit(""), 0))
            {
                fprintf(print_file, "DEBUG_LOG(\"%.*s\", var.%.*s);\n", MD_StringExpand(child->string),
                                                                        MD_StringExpand(child->string));
            }
        }
    }
    fprintf(print_file, "printf(\"-----------\\n\");");
    fprintf(print_file, "\n}\n\n");
}

// bool
// SloppyStringContains(MD_String8 s1, MD_String8 s2)
// {
    // for (u32 i = 0; i < MD_CalculateCStringLength(s1); ++i)
    // {
        // if (s1[i] == ' ')
            // continue;
        // S2 is shorter string
        // for (u32 j = 0; j < s2.size; ++j)
        // {
            // if (s2.str[i + j] != s1[j])
            // {
                // i += j;
                // break;
            // }
            // else if (j == (s2.size - 1))
            // {
                // return true;
            // }
        // }
    // }
// 
    // return false;
// }

void 
GenerateHeaderFromMdesk(MD_String8 file_path)
{
    MD_String8 file = MD_LoadEntireFile(file_path);
    MD_Node *code = MD_ParseWholeString(MD_S8Lit("Generated Test Code"), file).node;

    MD_String8 header_file_name = GetHeaderStringFromMdesk(file_path);

    char str[256];
    sprintf(str, "headers/%.*s", MD_StringExpand(header_file_name));
    FILE* header_file = fopen(str, "w+");

    for(MD_EachNode(node, code->first_child))
    {
        if(MD_NodeHasTag(node, MD_S8Lit("struct")))
        {
            MD_C_Generate_Struct(header_file, node);

            // Check if struct isn't already added to syntax_file
            if (update_syntax_file)
            {
                if (MD_FindSubstring(syntax_file_last_line, node->string, 0, 0)
                    == syntax_file_last_line.size)
                {
                    static bool is_first = true;

                    // NOTE: this is a hack
                    if (is_first)
                    {
                        fprintf(syntax_file, "syn keyword marko_keyword");
                        is_first = false;
                    }
                    // Add it
                    fprintf(syntax_file, " %.*s", MD_StringExpand(node->string));
                }
            }


            for(MD_Node *child = node->first_child; !MD_NodeIsNil(child); child = child->next)
            {
                if (MD_NodeHasTag(child, MD_S8Lit("function")))
                {
                    fprintf(header_file, "%.*s;\n", MD_StringExpand(child->first_child->string));
                }
            }
            fprintf(header_file, "};\n\n");

            if (MD_NodeHasTag(node, MD_S8Lit("printable")))
            {
                if (generate_print_functions)
                {
                    GeneratePrintFunction(node);
                }
            }
        }
        else if(MD_NodeHasTag(node, MD_S8Lit("enum")))
        {
            fprintf(header_file, "enum %.*s \n{\n", MD_StringExpand(node->string));
            for(MD_EachNode(child, node->first_child))
            {
                fprintf(header_file, "%.*s,\n", MD_StringExpand(child->string));
            }
            fprintf(header_file, "};\n\n");
        }
        else if(MD_NodeHasTag(node, MD_S8Lit("define")))
        {
            fprintf(header_file, "#define %.*s\n", MD_StringExpand(node->first_child->string));
        }
        else if (MD_NodeHasTag(node, MD_S8Lit("typedef")))
        {
            fprintf(header_file, "typedef %.*s\n", MD_StringExpand(node->first_child->string));
        }
    }

    fclose(header_file);
}


int 
main(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        if (MD_StringMatch(MD_S8CString(argv[i]), MD_S8Lit("-generate_print"), 0))
        {
            generate_print_functions = true;
        }
        else if (MD_StringMatch(MD_S8CString(argv[i]), MD_S8Lit("-update_syntax"), 0))
        {
            update_syntax_file = true;
        }
        else if (MD_StringMatch(MD_S8CString(argv[i]), MD_S8Lit("-help"), 0))
        {
            printf("Available flags: -generate_print, -update_syntax\n");
            return 0;
        }
        else
        {
            printf("Usage: %s -flag1 -flag2...\n", argv[0]);
            return 0;
        }
    }

    char buffer[2048];

    if (generate_print_functions)
        print_file = fopen("generated_print.cpp", "w+");
    if (update_syntax_file)
    {

        syntax_file = fopen("/home/marko/.vim/syntax/c.vim", "a+");
        if (!syntax_file)
        {
            printf("Failed to open syntax file!\n");
        }
        // We get the last line of syntax_file
        while(!feof(syntax_file))
        {
            fgets(buffer, 2048, syntax_file);
        }

        syntax_file_last_line = MD_S8CString(buffer);
    }

    GenerateHeaderFromMdesk(MD_S8Lit("memory.mdesk"));
    GenerateHeaderFromMdesk(MD_S8Lit("math.mdesk"));
    GenerateHeaderFromMdesk(MD_S8Lit("input.mdesk"));
    GenerateHeaderFromMdesk(MD_S8Lit("renderer.mdesk"));
    GenerateHeaderFromMdesk(MD_S8Lit("debug.mdesk"));
    GenerateHeaderFromMdesk(MD_S8Lit("game.mdesk"));
    GenerateHeaderFromMdesk(MD_S8Lit("opengl_renderer.mdesk"));

    if (generate_print_functions)
        fclose(print_file);
    if (update_syntax_file)
        fclose(syntax_file);

    return 0;
}
