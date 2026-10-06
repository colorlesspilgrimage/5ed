/*
 * Mr. 4th Dimention - Allen Webster
 *
 * 02.10.2019
 *
 * System API definition program.
 *
 */

// TOP

#include "base/5ed_base_types.h"
#include "base/5ed_stringf.h"
#include "base/5ed_hash_functions.h"
#include "base/5ed_log_helpers.h"
#include "base/5ed_malloc_allocator.h"
#include "base/5ed_mem.h"
#include "core/5ed_api_definition.h"

#include "core/5ed_api_definition.cpp"

#include <stdio.h>

////////////////////////////////

function API_Definition*
define_api(Arena *arena);

function Generated_Group
get_api_group(void);

int
main(void){
    Arena arena = make_arena_malloc();
    API_Definition *api = define_api(&arena);
    if (!api_definition_generate_api_includes(&arena, api, get_api_group(), 0)){
        return(1);
    }
    return(0);
}

// BOTTOM

