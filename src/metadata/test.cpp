#include <md.h>
#include <md_c_helpers.h>

#include <md.c>
#include <md_c_helpers.c>


int 
main()
{
    MD_String8 file = MD_LoadEntireFile(MD_S8Lit("./test.mdesk"));
    
    MD_Node *code = MD_ParseWholeString(MD_S8Lit("Generated Test Code"), file).node;
    printf("Source Metadesk Code:\n");
    printf("%.*s\n\n", MD_StringExpand(file));
    
    printf("Generated C Code:\n");

    FILE* print_file = fopen("generated_print.c", "w+");

    for(MD_EachNode(node, code->first_child))
    {
        if(MD_NodeHasTag(node, MD_S8Lit("struct")))
        {
            MD_C_Generate_Struct(stdout, node);

            if (MD_NodeHasTag(node, MD_S8Lit("printable")))
            {
                fprintf(print_file, "void print_%.*s (%.*s var)\n{\n", MD_StringExpand(node->string),
                                                                       MD_StringExpand(node->string));
                for(MD_EachNode(child, node->first_child))
                {
                    if (MD_StringMatch(child->first_child->string, MD_S8Lit("f32"), 0) ||
                        MD_StringMatch(child->first_child->string, MD_S8Lit("f64"), 0))
                    {
                        fprintf(print_file, "printf(\"%%.2f\", ");
                        fprintf(print_file, "var.%.*s);\n", MD_StringExpand(child->string));
                    }
                    else if (MD_StringMatch(child->first_child->string, MD_S8Lit("i32"), 0) ||
                             MD_StringMatch(child->first_child->string, MD_S8Lit("i8"), 0)  ||
                             MD_StringMatch(child->first_child->string, MD_S8Lit("i16"), 0) ||
                             MD_StringMatch(child->first_child->string, MD_S8Lit("i64"), 0))
                    {
                        fprintf(print_file, "printf(\"%%d\", ");
                        fprintf(print_file, "var.%.*s);\n", MD_StringExpand(child->string));
                    }
                    else if (MD_StringMatch(child->first_child->string, MD_S8Lit("u32"), 0) ||
                             MD_StringMatch(child->first_child->string, MD_S8Lit("u8"), 0)  ||
                             MD_StringMatch(child->first_child->string, MD_S8Lit("u16"), 0) ||
                             MD_StringMatch(child->first_child->string, MD_S8Lit("u64"), 0))
                    {
                        fprintf(print_file, "printf(\"%%lu\", ");
                        fprintf(print_file, "var.%.*s);\n", MD_StringExpand(child->string));
                    }
                    else
                    {
                        fprintf(print_file, "print_%.*s(var.%.*s);\n", MD_StringExpand(child->first_child->string),
                                                                       MD_StringExpand(child->string));
                    }
                }
                fprintf(print_file, "\n}\n\n");
            }
        }
    }
    fclose(print_file);
    printf("\n\n");

    return 0;
}
