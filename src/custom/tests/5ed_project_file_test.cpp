/*
5ed_project_file_test.cpp - Checks that the config parser accepts project and config files.
*/

// TOP

// Usage: project_file_test <empty-dir> [file.5ed ...]
// The test writes project.5ed and build.sh into <empty-dir> with the
// setup_new_project generators. Then it parses project.5ed and each other
// file with the real config parser. Each parse error makes the test fail.
// The test calls no host API, so it does not need the editor.

#include "custom/5ed_default_include.cpp"
#include "generated/managed_id_metadata.cpp"

// 5ed_default_bindings.cpp defines this for the real custom layer.
void
custom_layer_init(Application_Links *app){}

function b32
test_parse_file(Arena *arena, String8 file_name){
    b32 result = false;
    FILE *file = fopen((char*)file_name.str, "rb");
    if (file == 0){
        printf("FAIL: cannot open %.*s\n", string_expand(file_name));
    }
    else{
        String8 data = data_from_file(arena, file);
        fclose(file);
        Token_List list = lex_full_input_cpp(arena, data);
        Token_Array array = token_array_from_list(arena, &list);
        Config_Parser ctx = def_config_parser_init(arena, file_name, data, array);
        Config *config = def_config_parser_top(&ctx);
        if (config == 0){
            printf("FAIL: no parse result for %.*s\n", string_expand(file_name));
        }
        else if (config->errors.count > 0){
            printf("FAIL: %d parse errors in %.*s\n", config->errors.count, string_expand(file_name));
            for (Config_Error *error = config->errors.first; error != 0; error = error->next){
                printf("  %.*s\n", string_expand(error->text));
            }
        }
        else{
            printf("PASS: parse %.*s\n", string_expand(file_name));
            result = true;
        }
    }
    return(result);
}

int
main(int argc, char **argv){
    if (argc < 2){
        printf("FAIL: usage project_file_test <empty-dir> [file.5ed ...]\n");
        return(1);
    }
    Arena arena = make_arena_malloc();
    b32 ok = true;
    
    String8 dir = SCu8(argv[1]);
    if (!prj_generate_project(&arena, dir, str8_lit("build"), str8_lit("."), str8_lit("demo")) ||
        !prj_generate_sh(&arena, str8_lit("-g"), str8_lit("g++"), dir, str8_lit("build"),
                         str8_lit("main.cpp"), str8_lit("."), str8_lit("demo"))){
        printf("FAIL: cannot write the project files into %.*s\n", string_expand(dir));
        return(1);
    }
    
    String8 project = push_u8_stringf(&arena, "%.*s/project.5ed", string_expand(dir));
    ok = test_parse_file(&arena, project) && ok;
    for (int i = 2; i < argc; i += 1){
        ok = test_parse_file(&arena, SCu8(argv[i])) && ok;
    }
    return(ok?0:1);
}

// BOTTOM
