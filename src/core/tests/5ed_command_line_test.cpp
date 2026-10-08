/*
5ed_command_line_test.cpp - Checks the command line parser of the core layer.
*/

// TOP

// Usage: test_command_line
// The test calls app_read_command_line through app_get_functions.
// It uses the stubs of the project-file test. It does not need a display.

#include "base/5ed_base.h"
#include "core/5ed_font_interface.h"
#include "base/generated/graphics_api.h"
#include "base/generated/font_api.h"
#include "core/5ed_font_set.h"
#include "core/5ed_render_target.h"
#include "core/5ed.h"

#include <stdio.h>
#include <string.h>

// The core layer calls these functions of the custom layer.
// The test does not start the core, so they do nothing.
void
custom_layer_init(Application_Links *app){}
void
custom_layer_bind_api(API_VTable_custom *vtable){}

struct Parse_Result{
    Plat_Settings plat;
    char **files;
    i32 file_count;
};

function Parse_Result
parse(i32 argc, char **argv){
    Parse_Result result = {};
    App_Functions app = app_get_functions();
    char **files = 0;
    i32 *file_count = 0;
    app.read_command_line(0, string_u8_litexpr(""), &result.plat, &files, &file_count, argc, argv);
    result.files = files;
    result.file_count = *file_count;
    return(result);
}

global i32 fail_count = 0;

function void
expect_files(char *name, Parse_Result result, i32 count, char **expected){
    b32 ok = (result.file_count == count);
    for (i32 i = 0; ok && i < count; i += 1){
        ok = (strcmp(result.files[i], expected[i]) == 0);
    }
    if (ok){
        printf("PASS: %s\n", name);
    }
    else{
        printf("FAIL: %s: expected %d files, got %d:", name, count, result.file_count);
        for (i32 i = 0; i < result.file_count; i += 1){
            printf(" '%s'", result.files[i]);
        }
        printf("\n");
        fail_count += 1;
    }
}

int
main(void){
    {
        char *argv[] = {"5ed", "a.txt"};
        char *expected[] = {"a.txt"};
        expect_files("one file", parse(ArrayCount(argv), argv), 1, expected);
    }
    {
        char *argv[] = {"5ed", "-d", "x.so", "a.txt"};
        char *expected[] = {"a.txt"};
        expect_files("-d skips its path and opens the next file", parse(ArrayCount(argv), argv), 1, expected);
    }
    {
        char *argv[] = {"5ed", "-D", "x.so", "a.txt", "b.txt"};
        char *expected[] = {"a.txt", "b.txt"};
        expect_files("-D skips its path and opens the next files", parse(ArrayCount(argv), argv), 2, expected);
    }
    {
        char *argv[] = {"5ed", "-d"};
        expect_files("-d as last argument", parse(ArrayCount(argv), argv), 0, 0);
    }
    {
        char *argv[] = {"5ed", "a.txt", "-d", "x.so", "-w", "800", "600", "b.txt"};
        char *expected[] = {"a.txt", "b.txt"};
        Parse_Result result = parse(ArrayCount(argv), argv);
        expect_files("-d before other options", result, 2, expected);
        if (result.plat.set_window_size && result.plat.window_w == 800 && result.plat.window_h == 600){
            printf("PASS: -w after -d\n");
        }
        else{
            printf("FAIL: -w after -d: size %d x %d\n", result.plat.window_w, result.plat.window_h);
            fail_count += 1;
        }
    }
    return(fail_count == 0 ? 0 : 1);
}

// BOTTOM
