#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#if _MSC_VER
#include <malloc.h>
#endif
#include "data_desk.h"

static FILE *global_header_file = 0;

DATA_DESK_FUNC void
DataDeskCustomInitCallback(void)
{
    global_header_file = fopen("generated.h", "w");
}

static DataDeskNode * get_child_by_name(DataDeskNode *root, char *name)
{
    DataDeskNode *result = 0;

    for(DataDeskNode *node = root; node; node = node->next)
    {
        if(!strcmp(node->string, name)){
            result = node;
            break;
        }
    }

    return result;
}

static void append_conversion_code(FILE *f, DataDeskNode *new_node,
                                   DataDeskNode *old_node, char *member_path){
#define STACK_CONCATENATE_MEMBER(name, str, node) \
    char *name = alloca(strlen(str) + 1 + (node)->string_length + 1); \
    sprintf(name, "%s.%s", str, node->string);

    switch(old_node->type){
      case DataDeskNodeType_Declaration:
        {
            DataDeskNode *old_type = old_node->declaration.type;
            if(old_type->string){
                switch(old_type->type){
                  case DataDeskNodeType_Identifier:
                    {
                        DataDeskNode *new_type = new_node->declaration.type;

                        if(old_type->namespace_string || new_type->namespace_string){
                            old_type = old_type->reference; assert(old_type);
                            new_type = new_type->reference; assert(new_type);

                            char *old_name = 0;
                            {
                                int old_name_length = strlen(old_type->namespace_string) + 1 + old_type->string_length + 1;
                                old_name = alloca(old_name_length);
                                sprintf(old_name, "%s_%s", old_type->namespace_string, old_type->string);
                            }

                            char *new_name = new_type->string;

                            STACK_CONCATENATE_MEMBER(extended_member_path, member_path, old_node);
                            fprintf(f, "    result%s = convert_%s_to_%s(v%s);\n", 
                                    extended_member_path, old_name, new_name, extended_member_path);
                        }
                        else{
                            assert(!strcmp(old_type->string, new_type->string));
                            STACK_CONCATENATE_MEMBER(extended_member_path, member_path, old_node);
                            fprintf(f, "    result%s = v%s;\n", extended_member_path, extended_member_path);
                        }
                    } break;
                  case DataDeskNodeType_TypeDecorator:
                    {
                        switch(old_type->sub_type){
                          case DataDeskTypeDecoratorType_Array:
                            {
                                assert(!"Array conversion not yet implemented");
                            } break;
                          default: assert(!"Not implemented"); break;
                        }
                    } break;
                  default: assert(!"Not implemented"); break;
                }
            }
            else{   // NOTE: Anonymous composite
                STACK_CONCATENATE_MEMBER(extended_member_path, member_path, old_node);
                // TODO: Check that types have the same shape
                append_conversion_code(f, new_node->declaration.type, old_node->declaration.type,
                                       extended_member_path);
            }
        } break;
      case DataDeskNodeType_StructDeclaration:
        {
            if(old_node->string){
                assert(!"Not implemented");
            }
            else{
                for(DataDeskNode *child = new_node->children_list_head; child; child = child->next)
                {
                    DataDeskNode *old_child = get_child_by_name(old_node->children_list_head, child->string);
                    if(old_child){
                        append_conversion_code(f, child, old_child, member_path);
                    }
                }
            }
        } break;
      case DataDeskNodeType_Identifier:
        {
            assert(!"Not implemented");
        } break;
      default: assert(!"Not implemented"); break;
    }
#undef STACK_CONCATENATE_MEMBER
}

DATA_DESK_FUNC void
DataDeskCustomParseCallback(DataDeskNode *root, char *filename)
{
	DataDeskFWriteGraphAsC(global_header_file, root);

    if(!root->namespace_index && root->namespace_alias_next)
    {
        DataDeskNode *node = root;
        DataDeskNode *old_node = node->namespace_alias_next;

        assert(node->type == old_node->type);

        char *old_name = 0;
        {
            int old_name_length = strlen(old_node->namespace_string) + 1 + old_node->string_length + 1;
            old_name = alloca(old_name_length);
            sprintf(old_name, "%s_%s", old_node->namespace_string, old_node->string);
        }

        char *new_name = node->string;

        fprintf(global_header_file, "\n");

        switch(node->type){
          case DataDeskNodeType_EnumDeclaration:
            {
                fprintf(global_header_file, "static %s convert_%s_to_%s(%s v)\n{\n",
                        new_name, old_name, new_name, old_name);

                fprintf(global_header_file, "    %s result = 0;\n", new_name);
                fprintf(global_header_file, "    switch(v)\n    {\n");

                for(DataDeskNode *old_child = old_node->children_list_head; old_child; old_child = old_child->next)
                {
                    DataDeskNode *child = get_child_by_name(node->children_list_head, old_child->string);

                    fprintf(global_header_file, "        case %s_%s: ",  old_node->namespace_string, old_child->string);

                    if(child){
                        fprintf(global_header_file, "result = %s; break;\n",  child->string);
                    }
                    else{
                        fprintf(global_header_file, "assert(!\"Enumerand %s_%s is no longer allowed\\n\");\n", 
                                old_node->namespace_string, old_child->string);
                    }
                }

                fprintf(global_header_file, "        default: assert(!\"Illegal value for enum %s\\n\"); break;\n",
                        old_name);

                fprintf(global_header_file, "    }\n");

                fprintf(global_header_file, "    return result;\n");
                fprintf(global_header_file, "}\n");

            } break;
          case DataDeskNodeType_FlagsDeclaration:
            {
                fprintf(global_header_file, "static %s convert_%s_to_%s(%s v)\n{\n",
                        new_name, old_name, new_name, old_name);

                fprintf(global_header_file, "    %s result = 0;\n", new_name);

                for(DataDeskNode *old_child = old_node->children_list_head; old_child; old_child = old_child->next)
                {
                    DataDeskNode *child = get_child_by_name(node->children_list_head, old_child->string);
                    fprintf(global_header_file, "    if(v & %s_%s) ", old_node->namespace_string, old_child->string);
                    if(child){
                        fprintf(global_header_file, "result |= %s;\n", child->string);
                    }
                    else{
                        fprintf(global_header_file, "assert(!\"Flag %s_%s is no longer allowed\\n\");\n", 
                                old_node->namespace_string, old_child->string);
                    }
                }


                fprintf(global_header_file, "    return result;\n");
                fprintf(global_header_file, "}\n");
            } break;
          case DataDeskNodeType_StructDeclaration:
            {
                fprintf(global_header_file, "static %s convert_%s_to_%s(%s v)\n{\n",
                        new_name, old_name, new_name, old_name);
                fprintf(global_header_file, "    %s result = {0};\n", new_name);

                for(DataDeskNode *child = node->children_list_head; child; child = child->next)
                {
                    DataDeskNode *old_child = get_child_by_name(old_node->children_list_head, child->string);

                    if(old_child){
                        append_conversion_code(global_header_file, child, old_child, "");
                    }
                }

                fprintf(global_header_file, "    return result;\n");
                fprintf(global_header_file, "}\n");

            } break;
          case DataDeskNodeType_UnionDeclaration:
            {
                fprintf(global_header_file, "static %s convert_%s_to_%s(%s v)\n{\n",
                        new_name, old_name, new_name, old_name);
                fprintf(global_header_file, "    %s result = {0};\n", new_name);

                DataDeskNode *authoritative_child = 0;
                {
                    for(DataDeskNode *cur_child = node->children_list_head; cur_child; cur_child = cur_child->next)
                    {
                        if(DataDeskGetNodeTag(cur_child, "AuthoritativeUnionMember"))
                        {
                            authoritative_child = cur_child;
                        }
                    }
                    if(!authoritative_child){
                        authoritative_child = node->children_list_head;
                    }
                }

                DataDeskNode *old_child = get_child_by_name(old_node->children_list_head, authoritative_child->string);

                if(old_child){
                    append_conversion_code(global_header_file, authoritative_child, old_child, "");
                }


                fprintf(global_header_file, "    return result;\n");
                fprintf(global_header_file, "}\n");
            } break;
          default: 
            {
                fprintf(stderr, "Missing type mapping\n");
                exit(1); 
                break;
            }
        }

        fprintf(global_header_file, "\n");
    }
}

DATA_DESK_FUNC void
DataDeskCustomCleanUpCallback(void)
{
    fclose(global_header_file);
}
