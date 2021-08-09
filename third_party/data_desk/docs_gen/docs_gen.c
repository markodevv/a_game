// NOTE(rjf): Utility program to generate docs pages for Data Desk

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define ENABLE_LOG_BY_DEFAULT 1

#include "data_desk.h"
#include "data_desk_debug.c"
#include "data_desk_utilities.c"
#include "data_desk_tokenizer.c"
#include "data_desk_parse.c"

#define DataDeskNodeType_Docs (0xffffffff)

static int
GetNextDocs(Tokenizer *tokenizer, Token *docs_string)
{
    int result = 0;
    if(RequireToken(tokenizer, "DataDeskDoc", 0) &&
       RequireToken(tokenizer, "(", 0) &&
       RequireTokenType(tokenizer, Token_StringConstant, docs_string) &&
       RequireToken(tokenizer, ")", 0))
    {
        result = 1;
    }
    return result;
}

static DataDeskNode *
ParseDocs(ParseContext *context, Tokenizer *tokenizer, DataDeskNode **result_ptr)
{
    DataDeskNode *node = 0;
    
    Token docs_str = {0};
    if(GetNextDocs(tokenizer, &docs_str))
    {
        node = ParseContextAllocateNode(context, tokenizer, docs_str, DataDeskNodeType_Docs);
    }
    
    if(result_ptr)
    {
        *result_ptr = node;
    }
    return node;
}

static DataDeskNode *
ParseCDeclarationList(ParseContext *context, Tokenizer *tokenizer);

static DataDeskNode *
ParseCTypeThatComesBeforeNameBecauseCIsATerribleLanguage(ParseContext *context, Tokenizer *tokenizer)
{
    DataDeskNode *type = 0;
    
    Token base_type_name = {0};
    
    if(RequireToken(tokenizer, "struct", 0))
    {
        if(!RequireToken(tokenizer, "{", 0))
        {
            goto end_parse;
        }
        
        type = ParseContextAllocateNode(context, tokenizer, (Token){0}, DataDeskNodeType_StructDeclaration);
        DataDeskNode *decl_list = ParseCDeclarationList(context, tokenizer);
        InsertChild(decl_list, type);
        
        if(!RequireToken(tokenizer, "}", 0))
        {
            goto end_parse;
        }
    }
    else if(RequireToken(tokenizer, "union", 0))
    {
        if(!RequireToken(tokenizer, "{", 0))
        {
            goto end_parse;
        }
        
        type = ParseContextAllocateNode(context, tokenizer, (Token){0}, DataDeskNodeType_UnionDeclaration);
        DataDeskNode *decl_list = ParseCDeclarationList(context, tokenizer);
        InsertChild(decl_list, type);
        
        if(!RequireToken(tokenizer, "}", 0))
        {
            goto end_parse;
        }
    }
    else if(RequireTokenType(tokenizer, Token_AlphanumericBlock, &base_type_name))
    {
        DataDeskNode *base_type = ParseContextAllocateNode(context, tokenizer, base_type_name, DataDeskNodeType_Identifier);
        
        type = base_type;
        
        for(;RequireToken(tokenizer, "*", 0);)
        {
            DataDeskNode *decorator = ParseContextAllocateNode(context, tokenizer, base_type_name, DataDeskNodeType_TypeDecorator);
            decorator->sub_type = DataDeskTypeDecoratorType_Pointer;
            InsertChild(type, decorator);
            type = decorator;
        }
    }
    
    end_parse:;
    return type;
}

static DataDeskNode *
ParseCDeclaration(ParseContext *context, Tokenizer *tokenizer)
{
    DataDeskNode *decl = 0;
    DataDeskNode *type = 0;
    DataDeskNode *docs = ParseDocs(context, tokenizer, 0);
    
    type = ParseCTypeThatComesBeforeNameBecauseCIsATerribleLanguage(context, tokenizer);
    
    if(type)
    {
        Token decl_name = {0};
        RequireTokenType(tokenizer, Token_AlphanumericBlock, &decl_name);
        decl = ParseContextAllocateNode(context, tokenizer, decl_name, DataDeskNodeType_Declaration);
        decl->declaration.type = InsertChild(type, decl);
        decl->tag_list_head = docs;
        
        
        for(;;)
        {
            if(RequireToken(tokenizer, "[", 0))
            {
                DataDeskNode *array = ParseContextAllocateNode(context, tokenizer, (Token){0}, DataDeskNodeType_TypeDecorator);
                array->sub_type = DataDeskTypeDecoratorType_Array;
                DataDeskNode *array_size = ParseExpression(context, tokenizer);
                InsertChild(type, array);
                InsertChild(array_size, array);
                decl->children_list_head = decl->children_list_tail = 0;
                decl->declaration.type = InsertChild(array, decl);
                type = array;
                if(!RequireToken(tokenizer, "]", 0))
                {
                    goto end_parse;
                }
            }
            else
            {
                break;
            }
        }
    }
    
    end_parse:;
    return decl;
}

static DataDeskNode *
ParseCDeclarationList(ParseContext *context, Tokenizer *tokenizer)
{
    DataDeskNode *head = 0;
    DataDeskNode *tail = 0;
    
    for(;;)
    {
        DataDeskNode *decl = ParseCDeclaration(context, tokenizer);
        RequireToken(tokenizer, ";", 0);
        RequireToken(tokenizer, ",", 0);
        
        if(decl)
        {
            if(tail == 0)
            {
                head = tail = decl;
            }
            else
            {
                tail->next = decl;
                decl->prev = tail;
                tail = tail->next;
            }
        }
        else
        {
            break;
        }
        
        if(TokenMatch(PeekToken(tokenizer), "}") ||
           TokenMatch(PeekToken(tokenizer), ")") ||
           PeekToken(tokenizer).type == Token_Invalid ||
           PeekToken(tokenizer).type != Token_AlphanumericBlock)
        {
            break;
        }
    }
    
    return head;
}

static DataDeskNode *
ParseCConstantList(ParseContext *context, Tokenizer *tokenizer)
{
    DataDeskNode *head = 0;
    DataDeskNode *tail = 0;
    
    for(;;)
    {
        if(RequireToken(tokenizer, "#", 0) &&
           RequireToken(tokenizer, "if", 0))
        {
            for(;;)
            {
                if(RequireToken(tokenizer, "#", 0) &&
                   RequireToken(tokenizer, "endif", 0))
                {
                    break;
                }
                else if(PeekToken(tokenizer).type == Token_Invalid)
                {
                    break;
                }
                NextToken(tokenizer);
            }
        }
        
        DataDeskNode *constant = 0;
        DataDeskNode *docs = ParseDocs(context, tokenizer, 0);
        
        Token name = {0};
        if(RequireTokenType(tokenizer, Token_AlphanumericBlock, &name))
        {
            constant = ParseContextAllocateNode(context, tokenizer, name, DataDeskNodeType_ConstantDefinition);
            constant->tag_list_head = docs;
            
            if(RequireToken(tokenizer, "=", 0))
            {
                InsertChild(ParseExpression(context, tokenizer), constant);
            }
            
            RequireToken(tokenizer, ",", 0);
            
            if(tail == 0)
            {
                head = tail = constant;
            }
            else
            {
                tail->next = constant;
                constant->prev = tail;
                tail = tail->next;
            }
        }
        else
        {
            break;
        }
    }
    
    return head;
}

static DataDeskNode *
ParseCCode(ParseContext *context, Tokenizer *tokenizer)
{
    DataDeskNode *root = 0;
    
    DataDeskNode *docs = ParseDocs(context, tokenizer, 0);
    
    RequireToken(tokenizer, "typedef", 0);
    
    // NOTE(rjf): Struct.
    if(RequireToken(tokenizer, "struct", 0))
    {
        Token name = {0};
        if(RequireTokenType(tokenizer, Token_AlphanumericBlock, &name))
        {
            if(!RequireToken(tokenizer, "{", 0))
            {
                goto end_parse;
            }
            
            root = ParseContextAllocateNode(context, tokenizer, name, DataDeskNodeType_StructDeclaration);
            DataDeskNode *declaration_list = ParseCDeclarationList(context, tokenizer);
            InsertChild(declaration_list, root);
            
            if(!RequireToken(tokenizer, "}", 0))
            {
                goto end_parse;
            }
        }
    }
    
    // NOTE(rjf): Enum.
    else if(RequireToken(tokenizer, "enum", 0))
    {
        Token name = {0};
        if(RequireTokenType(tokenizer, Token_AlphanumericBlock, &name))
        {
            if(!RequireToken(tokenizer, "{", 0))
            {
                goto end_parse;
            }
            
            root = ParseContextAllocateNode(context, tokenizer, name, DataDeskNodeType_EnumDeclaration);
            DataDeskNode *constant_list = ParseCConstantList(context, tokenizer);
            InsertChild(constant_list, root);
            
            if(!RequireToken(tokenizer, "}", 0))
            {
                goto end_parse;
            }
        }
    }
    
    // NOTE(rjf): Procedure.
    else if(RequireToken(tokenizer, "DATA_DESK_HEADER_PROC", 0))
    {
        DataDeskNode *return_type = ParseCTypeThatComesBeforeNameBecauseCIsATerribleLanguage(context, tokenizer);
        
        if(return_type)
        {
            Token proc_name = {0};
            if(!RequireTokenType(tokenizer, Token_AlphanumericBlock, &proc_name))
            {
                goto end_parse;
            }
            
            if(!RequireToken(tokenizer, "(", 0))
            {
                goto end_parse;
            }
            DataDeskNode *declaration_list = ParseCDeclarationList(context, tokenizer);
            if(!RequireToken(tokenizer, ")", 0))
            {
                goto end_parse;
            }
            
            DataDeskNode *proc = ParseContextAllocateNode(context, tokenizer, proc_name, DataDeskNodeType_ProcedureHeader);
            proc->procedure_header.return_type = InsertChild(return_type, proc);
            proc->procedure_header.first_parameter = InsertChild(declaration_list, proc);
            root = proc;
        }
    }
    
    // NOTE(rjf): Other... skip.
    else
    {
        NextToken(tokenizer);
    }
    
    if(root)
    {
        root->tag_list_head = docs;
    }
    end_parse:;
    return root;
}

static DataDeskNode *
ParseFile(ParseContext *context, char *file, char *filename)
{
    Tokenizer tokenizer_ = {0};
    Tokenizer *tokenizer = &tokenizer_;
    {
        tokenizer->at = file;
        tokenizer->filename = filename;
        tokenizer->line = 1;
    }
    
    DataDeskNode *head = 0;
    DataDeskNode *tail = 0;
    
    for(;;)
    {
        // NOTE(rjf): Break if there are no tokens left.
        {
            Token token = PeekToken(tokenizer);
            if(!token.type || !token.string)
            {
                break;
            }
        }
        
        Tokenizer reset_tokenizer = *tokenizer;
        Token docs_string = {0};
        if(GetNextDocs(tokenizer, &docs_string))
        {
            *tokenizer = reset_tokenizer;
            DataDeskNode *root = ParseCCode(context, tokenizer);
            
            if(root)
            {
                if(tail == 0)
                {
                    tail = head = root;
                }
                else
                {
                    tail->next = root;
                    root->prev = tail;
                    tail = tail->next;
                }
            }
        }
        
        NextToken(tokenizer);
    }
    
    return head;
}

static void
GenerateDocsString(FILE *file, DataDeskNode *docs)
{
    if(docs)
    {
        fprintf(file, "<p>");
        for(int i = 1; i < docs->string_length-1; ++i)
        {
            fprintf(file, "%c", docs->string[i]);
        }
        fprintf(file, "</p>\n");
    }
}

static void
GenerateCodeHeaderHTML(FILE *file, DataDeskNode *node)
{
    fprintf(file, "<h1>%.*s</h1>\n", node->string_length, node->string);
    if(node->tag_list_head)
    {
        DataDeskNode *docs = node->tag_list_head;
        GenerateDocsString(file, docs);
    }
    if(node->parent == 0)
    {
        fprintf(file, "<h2>C Specification</h2>\n");
        fprintf(file, "<div class=\"code\">\n");
        fprintf(file, "<pre>\n");
        DataDeskFWriteGraphAsC(file, node);
        fprintf(file, "</pre>\n");
        fprintf(file, "</div>\n");
    }
}

static void
GenerateDocsHTML(FILE *file, DataDeskNode *node)
{
    switch(node->type)
    {
        
        case DataDeskNodeType_Declaration:
        {
            fprintf(file, "<div class=\"minicode\"><pre>");
            DataDeskFWriteGraphAsC(file, node);
            fprintf(file, "</pre></div>\n");
            GenerateDocsString(file, node->tag_list_head);
            break;
        }
        
        case DataDeskNodeType_ConstantDefinition:
        {
            fprintf(file, "<div class=\"minicode\"><pre>%.*s</pre></div>\n", node->string_length, node->string);
            GenerateDocsString(file, node->tag_list_head);
            break;
        }
        
        case DataDeskNodeType_EnumDeclaration:
        {
            GenerateCodeHeaderHTML(file, node);
            fprintf(file, "<h2>Values</h2>\n");
            fprintf(file, "<ul>\n");
            for(DataDeskNode *child = node->children_list_head; child; child = child->next)
            {
                fprintf(file, "<li>\n");
                GenerateDocsHTML(file, child);
                fprintf(file, "</li>\n");
            }
            fprintf(file, "</ul>\n");
            break;
        }
        
        case DataDeskNodeType_StructDeclaration:
        {
            GenerateCodeHeaderHTML(file, node);
            fprintf(file, "<h2>Members</h2>\n");
            fprintf(file, "<ul>\n");
            for(DataDeskNode *child = node->children_list_head; child; child = child->next)
            {
                fprintf(file, "<li>\n");
                GenerateDocsHTML(file, child);
                fprintf(file, "</li>\n");
            }
            fprintf(file, "</ul>\n");
            break;
        }
        
        case DataDeskNodeType_ProcedureHeader:
        {
            GenerateCodeHeaderHTML(file, node);
            fprintf(file, "<h2>Parameters</h2>\n");
            fprintf(file, "<ul>\n");
            for(DataDeskNode *child = node->procedure_header.first_parameter; child; child = child->next)
            {
                fprintf(file, "<li>\n");
                GenerateDocsHTML(file, child);
                fprintf(file, "</li>\n");
            }
            fprintf(file, "</ul>\n");
            break;
        }
        
        default:
        {
            fprintf(file, "<ul>\n");
            for(DataDeskNode *child = node->children_list_head; child; child = child->next)
            {
                fprintf(file, "<li>\n");
                GenerateDocsHTML(file, child);
                fprintf(file, "</li>\n");
            }
            fprintf(file, "</ul>\n");
            break;
        }
    }
    
}

int main(int argument_count, char **arguments)
{
    if(argument_count > 1)
    {
        ParseContext context = {0};
        
        DataDeskNode *head = 0;
        DataDeskNode *tail = 0;
        
        for(int argument_index = 1; argument_index < argument_count; ++argument_index)
        {
            Log("Loading file \"%s\".", arguments[argument_index]);
            char *file = LoadEntireFileAndNullTerminate(arguments[argument_index]);
            DataDeskNode *root = ParseFile(&context, file, arguments[argument_index]);
            if(tail == 0)
            {
                head = tail = root;
                for(DataDeskNode *node = root; node; node = node->next)
                {
                    tail = node;
                }
            }
            else
            {
                tail->next = root;
                root->prev = tail;
                root->next = 0;
                tail = root;
            }
        }
        
        FILE *output_file = fopen("custom_layer_api.html", "w");
        if(output_file)
        {
            fprintf(output_file, "<script type=\"text/javascript\"/ src=\"search.js\"></script>\n");
            fprintf(output_file, "<div id=\"index_menu\">\n");
            fprintf(output_file, "<h1>Custom Layer API</h1>");
            fprintf(output_file, "<input class=\"docs_searcher\" id=\"search_input\" oninput=\"SearchInput(event)\" onkeydown=\"SearchKeyDown(event)\" placeholder=\"Filter\"></input>\n");
            fprintf(output_file, "<ul id=\"docs_menu\" class=\"doc_menu_list\">\n");
            for(DataDeskNode *node = head; node; node = node->next)
            {
                char *node_type_string = 0;
                
                switch(node->type)
                {
                    case DataDeskNodeType_ProcedureHeader: { node_type_string = "Procedure"; break; }
                    case DataDeskNodeType_StructDeclaration: { node_type_string = "Struct"; break; }
                    case DataDeskNodeType_UnionDeclaration: { node_type_string = "Union"; break; }
                    case DataDeskNodeType_EnumDeclaration: { node_type_string = "Enum"; break; }
                    
                    default: break;
                }
                
                fprintf(output_file, "<a href=\"#");
                DataDeskFWriteStringAsLowercaseWithUnderscoresN(output_file, node->string, node->string_length);
                fprintf(output_file, "\">\n");
                fprintf(output_file, "<li class=\"doc_menu_link\">");
                fprintf(output_file, "%s%s%s%.*s</li>\n",
                        node_type_string ? "(" : "",
                        node_type_string ? node_type_string : "",
                        node_type_string ? ") " : "",
                        node->string_length, node->string);
                fprintf(output_file, "</a>\n");
            }
            fprintf(output_file, "</ul>\n");
            fprintf(output_file, "</div>\n");
            
            for(DataDeskNode *node = head; node; node = node->next)
            {
                fprintf(output_file, "<div class=\"hidden\" id=\"");
                DataDeskFWriteStringAsLowercaseWithUnderscoresN(output_file, node->string, node->string_length);
                fprintf(output_file, "\">\n");
                fprintf(output_file, "<a class=\"link\" href=\"#\">← Back to Index</a><br><br><br>\n");
                GenerateDocsHTML(output_file, node);
                fprintf(output_file, "</div>\n");
            }
            fclose(output_file);
        }
    }
    else
    {
        LogError("USAGE: %s <files to process>", arguments[0]);
    }
    
    return 0;
}