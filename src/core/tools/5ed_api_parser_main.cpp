/*
 * Mr. 4th Dimention - Allen Webster
 *
 * 06.10.2019
 *
 * Parser that extracts an API from C++ source code.
 *
 */

// TOP

#include "base/5ed_base_types.h"
#include "base/5ed_stringf.h"
#include "base/5ed_hash_functions.h"
#include "base/5ed_log_helpers.h"
#include "base/5ed_malloc_allocator.h"
#include "base/5ed_stdio_file.h"
#include "base/5ed_mem.h"
#include "base/5ed_token.h"
#include "base/generated/lexer_cpp.h"
#include "core/5ed_api_definition.h"
#include "base/5ed_lexer_cpp_api.h"

#include "core/5ed_api_definition.cpp"
#include "core/5ed_api_parser.cpp"

#include <stdio.h>

////////////////////////////////

int
main(int argc, char **argv){
    Arena arena = make_arena_malloc();
    
    if (argc < 2){
        printf("usage: <script> <source> {<source>}\n"
               " source : file to load and parse into the output list\n");
        exit(1);
    }
    
    API_Definition_List list = {};
    for (i32 i = 1; i < argc; i += 1){
        char *file_name = argv[i];
        FILE *file = fopen(file_name, "rb");
        if (file == 0){
            printf("error: could not open input file: '%s'\n", argv[i]);
            continue;
        }
        
        String_Const_u8 text = data_from_file(&arena, file);
        fclose(file);
        
        if (text.size > 0){
            api_parse_source_add_to_list(&arena, SCu8(file_name), text, &list);
        }
    }
    
    for (API_Definition *node = list.first;
         node != 0;
         node = node->next){
        api_definition_generate_api_includes(&arena, node, GeneratedGroup_Custom, APIGeneration_NoAPINameOnCallables);
    }
}

// BOTTOM
