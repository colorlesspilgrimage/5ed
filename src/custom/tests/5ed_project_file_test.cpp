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

#include <sys/stat.h>

function Config*
test_open_config(Arena *arena, String8 file_name){
    FILE *file = fopen((char*)file_name.str, "rb");
    if (file == 0){
        printf("FAIL: cannot open %.*s\n", string_expand(file_name));
        return(0);
    }
    String8 data = data_from_file(arena, file);
    fclose(file);
    Token_List list = lex_full_input_cpp(arena, data);
    Token_Array array = token_array_from_list(arena, &list);
    Config_Parser ctx = def_config_parser_init(arena, file_name, data, array);
    Config *config = def_config_parser_top(&ctx);
    if (config == 0){
        printf("FAIL: no parse result for %.*s\n", string_expand(file_name));
    }
    return(config);
}

function b32
test_parse_file(Arena *arena, String8 file_name){
    b32 result = false;
    Config *config = test_open_config(arena, file_name);
    if (config == 0){
        result = false;
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
    return(result);
}

function b32
test_make_dir(String8 path){
    if (mkdir((char*)path.str, 0755) != 0){
        printf("FAIL: cannot make %.*s\n", string_expand(path));
        return(false);
    }
    return(true);
}

function b32
test_hostile_project(Arena *arena, String8 dir){
    b32 ok = true;
    String8 hostile = push_u8_stringf(arena, "%.*s/hostile", string_expand(dir));
    if (!test_make_dir(hostile)){
        return(false);
    }
    String8 name = string_u8_litexpr("a\"b\\c $(x)");
    String8 od = string_u8_litexpr("out dir");
    String8 script = string_u8_litexpr("build");
    if (!prj_generate_project(arena, hostile, script, od, name)){
        printf("FAIL: hostile project was not written\n");
        return(false);
    }
    String8 project = push_u8_stringf(arena, "%.*s/project.5ed", string_expand(hostile));
    Config *config = test_open_config(arena, project);
    if (config == 0 || config->errors.count > 0){
        printf("FAIL: hostile project parse\n");
        ok = false;
    }
    else{
        String8 got_name = {};
        String8 expect_run = string_u8_litexpr("'out dir/a\"b\\c $(x)'");
        String8 got_run = {};
        Config_Compound *commands = 0;
        if (!config_string_var(config, "project_name", 0, &got_name) ||
            !string_match(got_name, name)){
            printf("FAIL: project_name round trip\n");
            ok = false;
        }
        if (!config_compound_var(config, "commands", 0, &commands)){
            printf("FAIL: no commands compound\n");
            ok = false;
        }
        else{
            Config_Get_Result run_get = config_compound_member(config, commands, string_u8_litexpr("run"), 0);
            Config_Compound *run = 0;
            if (run_get.success && run_get.type == ConfigRValueType_Compound){
                run = run_get.compound;
            }
            if (run == 0 ||
                !config_compound_string_member(config, run, "linux", 0, &got_run) ||
                !string_match(got_run, expect_run)){
                printf("FAIL: commands.run.linux round trip\n");
                ok = false;
            }
        }
        if (ok){
            printf("PASS: hostile project round trip\n");
        }
    }
    
    String8 bad_nl = push_u8_stringf(arena, "%.*s/badnl", string_expand(dir));
    if (!test_make_dir(bad_nl)){
        ok = false;
    }
    else if (prj_generate_project(arena, bad_nl, script, od, string_u8_litexpr("a\nb"))){
        printf("FAIL: newline name was accepted\n");
        ok = false;
    }
    else{
        String8 bad_file = push_u8_stringf(arena, "%.*s/project.5ed", string_expand(bad_nl));
        FILE *bad = fopen((char*)bad_file.str, "rb");
        if (bad != 0){
            fclose(bad);
            printf("FAIL: newline name wrote a file\n");
            ok = false;
        }
        else{
            printf("PASS: control character refused\n");
        }
    }
    
    String8 bad_slash = push_u8_stringf(arena, "%.*s/badslash", string_expand(dir));
    if (!test_make_dir(bad_slash)){
        ok = false;
    }
    else if (prj_generate_project(arena, bad_slash, string_u8_litexpr("../x"), od, name)){
        printf("FAIL: slash script name was accepted\n");
        ok = false;
    }
    else{
        printf("PASS: slash script name refused\n");
    }
    
    String8 ver_path = push_u8_stringf(arena, "%.*s/version1.5ed", string_expand(dir));
    FILE *vf = fopen((char*)ver_path.str, "wb");
    if (vf == 0){
        printf("FAIL: cannot write version file\n");
        ok = false;
    }
    else{
        fputs("version(1);\nproject_name = \"x\";\n", vf);
        fclose(vf);
        Config *ver = test_open_config(arena, ver_path);
        if (ver == 0 || ver->version == 0 || *ver->version != 1){
            printf("FAIL: version(1) parse\n");
            ok = false;
        }
        else{
            printf("PASS: version(1) parses\n");
        }
    }
    return(ok);
}

function Config*
test_parse_text(Arena *arena, String8 data){
    Token_List list = lex_full_input_cpp(arena, data);
    Token_Array array = token_array_from_list(arena, &list);
    Config_Parser ctx = def_config_parser_init(arena, str8_lit("limits"), data, array);
    return(def_config_parser_top(&ctx));
}

// A hostile project.5ed must not use all the stack or memory.
function b32
test_config_limits(Arena *arena){
    b32 ok = true;
    
    // Deep compound nesting is a parse error, not a stack overflow.
    i32 depth = 200000;
    u8 *deep = push_array(arena, u8, depth*2 + 16);
    u64 n = 0;
    block_copy(deep, "x = ", 4);
    n = 4;
    for (i32 i = 0; i < depth; i += 1){
        deep[n] = '{';
        n += 1;
    }
    for (i32 i = 0; i < depth; i += 1){
        deep[n] = '}';
        n += 1;
    }
    deep[n] = ';';
    n += 1;
    Config *config = test_parse_text(arena, SCu8(deep, n));
    if (config == 0 || config->errors.count == 0){
        printf("FAIL: deep nesting gave no parse error\n");
        ok = false;
    }
    else{
        printf("PASS: deep nesting is a parse error\n");
    }
    
    // Nesting at the limit still parses.
    n = 0;
    block_copy(deep, "x = ", 4);
    n = 4;
    for (i32 i = 0; i < config_parser_max_depth; i += 1){
        deep[n] = '{';
        n += 1;
    }
    deep[n] = '1';
    n += 1;
    for (i32 i = 0; i < config_parser_max_depth; i += 1){
        deep[n] = '}';
        n += 1;
    }
    deep[n] = ';';
    n += 1;
    config = test_parse_text(arena, SCu8(deep, n));
    if (config == 0 || config->errors.count != 0){
        printf("FAIL: nesting at the limit gave a parse error\n");
        ok = false;
    }
    else{
        printf("PASS: nesting at the limit parses\n");
    }
    
    // A compound that refers to itself must not recurse without end.
    char *cycles[] = {
        "version(2);\na = { a };\n",
        "version(2);\na = { b, b };\nb = { a, a };\n",
    };
    for (i32 i = 0; i < ArrayCount(cycles); i += 1){
        config = test_parse_text(arena, SCu8(cycles[i]));
        b32 complete = true;
        if (config == 0 || config->errors.count != 0){
            printf("FAIL: cycle %d parse\n", i);
            ok = false;
            continue;
        }
        def_fill_var_from_config(arena, vars_get_root(), vars_save_string_lit("limits_test"), config, &complete);
        if (complete){
            printf("FAIL: cycle %d dump was not stopped\n", i);
            ok = false;
        }
        else{
            printf("PASS: cycle %d dump is stopped\n", i);
        }
    }
    
    // A normal file dumps completely.
    config = test_parse_text(arena, str8_lit("version(2);\nb = 1;\na = { b, { .c = \"x\" } };\n"));
    b32 complete = false;
    if (config != 0){
        def_fill_var_from_config(arena, vars_get_root(), vars_save_string_lit("limits_test"), config, &complete);
    }
    if (!complete){
        printf("FAIL: normal dump was stopped\n");
        ok = false;
    }
    else{
        printf("PASS: normal dump is complete\n");
    }
    return(ok);
}

function b32
test_command_table(void){
    b32 ok = true;
    if (command_table_count <= 200){
        printf("FAIL: command table count %d\n", (i32)command_table_count);
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
    ok = test_hostile_project(&arena, dir) && ok;
    ok = test_config_limits(&arena) && ok;
    for (int i = 2; i < argc; i += 1){
        ok = test_parse_file(&arena, SCu8(argv[i])) && ok;
    }
    return(ok?0:1);
}

// BOTTOM
