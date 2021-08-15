#include "data_desk.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>

static FILE *global_print_gen_file = 0;

#define INDENT_SPACES 2
int global_indent_level = 0;
char global_ident_string[32];
FILE* file_table[1024];

static FILE** 
GetFile(char* filename)
{
    long long hash = 5381;
    int c;
    
    while((c = *filename++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    
    int hash_index = hash % 1024;
    
    FILE** result = file_table + hash_index;
    
    return result;
}


static void GeneratePrintCode(FILE *file, DataDeskNode* parent, DataDeskNode *root, char *access_string);

DATA_DESK_FUNC void
DataDeskCustomInitCallback(void)
{
    for (int i = 0; i < 32; ++i)
    {
        global_ident_string[i] = ' ';
    }
    global_print_gen_file = fopen("../src/generated_print.c", "w");
    global_ident_string[global_indent_level * INDENT_SPACES] = '\0';
}

DATA_DESK_FUNC void
DataDeskCustomParseCallback(DataDeskNode *root, char* filename)
{
    char header_filename[128] = {"generated/"};
    for (int i = 0; i < strlen(filename); ++i)
    {
        header_filename[i + 10] = filename[i];
        if (filename[i] == '.')
        {
            header_filename[i + 11] = 'h';
            header_filename[i + 12] = '\0';
            break;
        }
    }
    FILE** generated_header_file = GetFile(header_filename);
    if (!(*generated_header_file))
    {
        *generated_header_file = fopen(header_filename, "a");
        fprintf(*generated_header_file, "#pragma once\n\n");
    }
    else
    {
        *generated_header_file = fopen(header_filename, "a");
    }
    
    DataDeskFWriteGraphAsC(*generated_header_file, root);
    
    fclose(*generated_header_file);
    
    if(root->type == DataDeskNodeType_StructDeclaration &&
       DataDeskNodeHasTag(root, "Print"))
    {
        global_indent_level = 0;
        
        fprintf(global_print_gen_file, "void Log(%s *object)\n{\n", root->string, root->string);
        GeneratePrintCode(global_print_gen_file, root, root->children_list_head, "object->");
        fprintf(global_print_gen_file, "LogM(\"\\n\\n\");\n");
        fprintf(global_print_gen_file, "}\n\n");
    }
}

DATA_DESK_FUNC void
DataDeskCustomCleanUpCallback(void)
{
	fclose(global_print_gen_file);
}

static void 
OffsetIndent(int offset)
{
    global_ident_string[global_indent_level * INDENT_SPACES] = ' ';
    global_indent_level += offset;
    global_ident_string[global_indent_level * INDENT_SPACES] = '\0';
}

static void
PrintIndentation(FILE* file)
{
    fprintf(file, "LogM(\"%s\");\n", global_ident_string);
}

static void
GeneratePrintCode(FILE *file, DataDeskNode* parent, DataDeskNode *root, char *access_string)
{
    fprintf(file, "LogM(\"%s\\n\");\n", parent->string);
    PrintIndentation(file);
	fprintf(file, "LogM(\"{\\n\");\n");
    OffsetIndent(+1);
    
	for(DataDeskNode *node = root; node; node = node->next)
	{
        PrintIndentation(file);
        
		if(!DataDeskNodeHasTag(node, "NoPrint") && node->type == DataDeskNodeType_Declaration)
		{
			if(DataDeskMatchType(node, "int"     ) || DataDeskMatchType(node, "u64") ||
			   DataDeskMatchType(node, "int32_t" ) || DataDeskMatchType(node, "i32" ) ||
			   DataDeskMatchType(node, "int16_t" ) || DataDeskMatchType(node, "i16" ) ||
			   DataDeskMatchType(node, "int8_t"  ) || DataDeskMatchType(node, "i8"  ) ||
			   DataDeskMatchType(node, "uint32_t") || DataDeskMatchType(node, "u32" ) ||
			   DataDeskMatchType(node, "uint16_t") || DataDeskMatchType(node, "u16" ) ||
			   DataDeskMatchType(node, "uint8_t" ) || DataDeskMatchType(node, "u8"  ) ||
               DataDeskMatchType(node, "i64"))
			{
				fprintf(file, "LogM(\"%s : %%i\", %s%s);\n", node->string, access_string, node->string);
			}
            
			else if(DataDeskMatchType(node, "float" ) || DataDeskMatchType(node, "double") ||
			        DataDeskMatchType(node, "f32"   ) || DataDeskMatchType(node, "f64"   ))
			{
				fprintf(file, "LogM(\"%s : %%f\", %s%s);\n", node->string, access_string, node->string);
			}
            
			else if(DataDeskMatchType(node, "[]char"))
			{
				fprintf(file, "LogM(\"%s : %%s\", %s%s);\n", node->string, access_string, node->string);
			}
            
			else if(DataDeskMatchType(node, "char"))
			{
				fprintf(file, "LogM(\"%s : %%c\", %s%s);\n", node->string, access_string, node->string);
			}
            
			else if(DataDeskMatchType(node, "*char"))
			{
				fprintf(file, "LogM(\"%s : %%s\", %s%s);\n", node->string, access_string, node->string);
			}
            
			else if(DataDeskMatchType(node, "*void"))
			{
				fprintf(file, "LogM(\"%s : %%p\", %s%s);\n", node->string, access_string, node->string);
			}
            
            // NOTE(rjf): Recursively descending for inline struct definition.
            else if(node->declaration.type->type == DataDeskNodeType_StructDeclaration)
            {
                char next_access_string[128] = {0};
                snprintf(next_access_string, sizeof(next_access_string), "%s%s%s", access_string, node->string,
                         DataDeskGetAccessStringForDeclaration(node));
                GeneratePrintCode(file, node, node->declaration.type->children_list_head,
                                  next_access_string);
            }
            
            // NOTE(rjf): Recursively descending for other type definition that we know about.
            else if((node->declaration.type->type == DataDeskNodeType_Identifier &&
                     node->declaration.type->reference))
            {
                char next_access_string[128] = {0};
                snprintf(next_access_string, sizeof(next_access_string), "%s%s%s", access_string, node->string,
                         DataDeskGetAccessStringForDeclaration(node));
                GeneratePrintCode(file, node, node->declaration.type->reference->children_list_head,
                                  next_access_string);
            }
            
            else
            {
                DataDeskError(node, "Unhandled type for printing code generation.");
            }
            
			fprintf(file, "LogM(\",\\n\");\n");
		}
	}
    OffsetIndent(-1);
    PrintIndentation(file);
	fprintf(file, "LogM(\"}\");\n");
}
