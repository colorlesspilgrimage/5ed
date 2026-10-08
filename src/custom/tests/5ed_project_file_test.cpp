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

function b32
test_command_table(void){
    b32 ok = true;
    if (command_table_count <= 200){
        printf("FAIL: command table count %d\n", command_table_count);
        ok = false;
    }
    for (i32 i = 0; i < command_table_count; i += 1){
        Command_Metadata *entry = &fcoder_metacmd_table[i];
        if (entry->proc == 0){
            printf("FAIL: null proc %s\n", entry->name);
            ok = false;
        }
        if (entry->name_len != (i32)cstring_length(entry->name)){
            printf("FAIL: name length %s\n", entry->name);
            ok = false;
        }
        if (entry->description_len != (i32)cstring_length(entry->description) ||
            entry->description_len <= 0){
            printf("FAIL: description length %s\n", entry->name);
            ok = false;
        }
        for (i32 j = i + 1; j < command_table_count; j += 1){
            if (string_match(SCu8(entry->name), SCu8(fcoder_metacmd_table[j].name))){
                printf("FAIL: duplicate name %s\n", entry->name);
                ok = false;
            }
        }
        Command_Metadata *by_name = get_command_metadata_from_name(SCu8(entry->name));
        if (by_name != entry){
            printf("FAIL: name lookup %s\n", entry->name);
            ok = false;
        }
        Command_Metadata *by_proc = get_command_metadata(entry->proc);
        if (by_proc != entry){
            printf("FAIL: proc lookup %s\n", entry->name);
            ok = false;
        }
    }
    if (get_command_metadata_from_name(SCu8("no_such_command")) != 0){
        printf("FAIL: unknown command lookup\n");
        ok = false;
    }
    Command_Metadata *lister = get_command_metadata_from_name(SCu8("command_lister"));
    Command_Metadata *open = get_command_metadata_from_name(SCu8("interactive_open"));
    Command_Metadata *undo_cmd = get_command_metadata_from_name(SCu8("undo"));
    if (lister == 0 || !lister->is_ui){
        printf("FAIL: command_lister is_ui\n");
        ok = false;
    }
    if (open == 0 || !open->is_ui){
        printf("FAIL: interactive_open is_ui\n");
        ok = false;
    }
    if (undo_cmd == 0 || undo_cmd->is_ui){
        printf("FAIL: undo is_ui\n");
        ok = false;
    }
    if (get_command_metadata_from_name(SCu8("custom_api_documentation")) != 0 ||
        get_command_metadata_from_name(SCu8("command_documentation")) != 0){
        printf("FAIL: removed documentation command is in the table\n");
        ok = false;
    }
    if (ok){
        printf("PASS: command table\n");
    }
    return(ok);
}

int
main(int argc, char **argv){
    if (argc < 2){
        printf("FAIL: usage project_file_test <empty-dir> [file.5ed ...]\n");
        return(1);
    }
    Arena arena = make_arena_malloc();
    b32 ok = test_command_table();
    
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
