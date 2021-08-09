#include <stdio.h>
#include "data_desk.h"

static FILE *global_header_file = 0;
static FILE *global_implementation_file = 0;



///////////////////////////////////////////////////////////////////////////////



// NOTE(rjf): "access_string" is basically a prefix to a member of a struct, so something like
// "object." or "object->". This can be changed in recursive calls, so you can descend a struct
// recursively.
static void GeneratePrintCode(FILE *file, DataDeskNode *root, char *access_string);



///////////////////////////////////////////////////////////////////////////////



DATA_DESK_FUNC void
DataDeskCustomInitCallback(void)
{
	global_header_file = fopen("generated_print.h", "w");
	global_implementation_file = fopen("generated_print.c", "w");
}

DATA_DESK_FUNC void
DataDeskCustomParseCallback(DataDeskNode *root, char *filename)
{
	DataDeskFWriteGraphAsC(global_header_file, root);
    
	if(root->type == DataDeskNodeType_StructDeclaration)
	{
		fprintf(global_header_file, "void Print%s(%s *object);\n", root->string, root->string);
		fprintf(global_implementation_file, "void Print%s(%s *object)\n{\n", root->string, root->string);
		GeneratePrintCode(global_implementation_file, root->children_list_head, "object->");
		fprintf(global_implementation_file, "}\n\n");
	}
}

DATA_DESK_FUNC void
DataDeskCustomCleanUpCallback(void)
{
	fclose(global_header_file);
	fclose(global_implementation_file);
}



///////////////////////////////////////////////////////////////////////////////



static void
GeneratePrintCode(FILE *file, DataDeskNode *root, char *access_string)
{
	fprintf(file, "printf(\"{ \");\n");
	for(DataDeskNode *node = root; node; node = node->next)
	{
		if(!DataDeskNodeHasTag(node, "NoPrint") && node->type == DataDeskNodeType_Declaration)
		{
			if(DataDeskMatchType(node, "int"     ) || DataDeskMatchType(node, "uint") ||
			   DataDeskMatchType(node, "int32_t" ) || DataDeskMatchType(node, "i32" ) ||
			   DataDeskMatchType(node, "int16_t" ) || DataDeskMatchType(node, "i16" ) ||
			   DataDeskMatchType(node, "int8_t"  ) || DataDeskMatchType(node, "i8"  ) ||
			   DataDeskMatchType(node, "uint32_t") || DataDeskMatchType(node, "u32" ) ||
			   DataDeskMatchType(node, "uint16_t") || DataDeskMatchType(node, "u16" ) ||
			   DataDeskMatchType(node, "uint8_t" ) || DataDeskMatchType(node, "u8"  ))
			{
				fprintf(file, "printf(\"%%i\", %s%s);\n", access_string, node->string);
			}
            
			else if(DataDeskMatchType(node, "float" ) || DataDeskMatchType(node, "double") ||
			        DataDeskMatchType(node, "f32"   ) || DataDeskMatchType(node, "f64"   ))
			{
				fprintf(file, "printf(\"%%f\", %s%s);\n", access_string, node->string);
			}
            
			else if(DataDeskMatchType(node, "[]char"))
			{
				fprintf(file, "printf(\"%%s\", %s%s);\n", access_string, node->string);
			}
            
			else if(DataDeskMatchType(node, "char"))
			{
				fprintf(file, "printf(\"%%c\", %s%s);\n", access_string, node->string);
			}
            
			else if(DataDeskMatchType(node, "*char"))
			{
				fprintf(file, "printf(\"%%s\", %s%s);\n", access_string, node->string);
			}
            
			else if(DataDeskMatchType(node, "*void"))
			{
				fprintf(file, "printf(\"%%p\", %s%s);\n", access_string, node->string);
			}
            
            // NOTE(rjf): Recursively descending for inline struct definition.
            else if(node->declaration.type->type == DataDeskNodeType_StructDeclaration)
            {
                char next_access_string[128] = {0};
                snprintf(next_access_string, sizeof(next_access_string), "%s%s%s", access_string, node->string,
                         DataDeskGetAccessStringForDeclaration(node));
                GeneratePrintCode(file, node->declaration.type->children_list_head,
                                  next_access_string);
            }
            
            // NOTE(rjf): Recursively descending for other type definition that we know about.
            else if((node->declaration.type->type == DataDeskNodeType_Identifier &&
                     node->declaration.type->reference))
            {
                char next_access_string[128] = {0};
                snprintf(next_access_string, sizeof(next_access_string), "%s%s%s", access_string, node->string,
                         DataDeskGetAccessStringForDeclaration(node));
                GeneratePrintCode(file, node->declaration.type->reference->children_list_head,
                                  next_access_string);
            }
            
            else
            {
                DataDeskError(node, "Unhandled type for printing code generation.");
            }
            
			fprintf(file, "printf(\", \");\n");
		}
	}
	fprintf(file, "printf(\"}\");\n");
}
