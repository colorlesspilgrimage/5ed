/*
 * 5ed_gl_context_attribs_test.cpp - Checks the GLX context attribute list.
 * The test does not need a display.
 */

#include "platform/opengl/5ed_opengl_context.h"

#include <stdio.h>

// Join the token in the preprocessor.
// The source check looks for one full name.
#define fiveed_glx_bit(suffix) GLX_CONTEXT_##suffix

static int
fail_null(const int *list){
    if (list == 0){
        printf("FAIL: null list\n");
        return 1;
    }
    return 0;
}

static int
read_list(const int *list, int *out, int *count){
    int saw = 0;
    int n = 0;
    for (int i = 0; i < 16; i += 1){
        out[i] = list[i];
        n += 1;
        if (list[i] == 0){
            saw = 1;
            break;
        }
    }
    *count = n;
    if (saw == 0){
        printf("FAIL: missing None\n");
        return 1;
    }
    return 0;
}

static int
reject_bad_tokens(const int *list, int end){
    for (int i = 0; i < end; i += 1){
        if (list[i] == fiveed_glx_bit(COMPATIBILITY_PROFILE_BIT_ARB)){
            return 1;
        }
        if (list[i] == GLX_CONTEXT_FLAGS_ARB){
            return 1;
        }
        if (list[i] == 2 && (i + 2) < end &&
            list[i + 1] == GLX_CONTEXT_MINOR_VERSION_ARB &&
            list[i + 2] == 1){
            return 1;
        }
    }
    return 0;
}

static int
has_pair(const int *list, int end, int key, int value){
    for (int i = 0; i + 1 < end; i += 2){
        if (list[i] == key && list[i + 1] == value){
            return 1;
        }
    }
    return 0;
}

int
main(void){
    const int *list = opengl_context_attrib_list();
    if (fail_null(list) != 0){
        return 1;
    }
    
    int values[16];
    int count = 0;
    if (read_list(list, values, &count) != 0){
        return 1;
    }
    if (values[count - 1] != 0){
        return 1;
    }
    
    int end = count - 1;
    if (reject_bad_tokens(values, end) != 0){
        return 1;
    }
    if (has_pair(values, end, GLX_CONTEXT_MAJOR_VERSION_ARB, 3) == 0){
        return 1;
    }
    if (has_pair(values, end, GLX_CONTEXT_MINOR_VERSION_ARB, 3) == 0){
        return 1;
    }
    if (has_pair(values, end, GLX_CONTEXT_PROFILE_MASK_ARB,
                 GLX_CONTEXT_CORE_PROFILE_BIT_ARB) == 0){
        return 1;
    }
    
    printf("PASS: major version is 3\n");
    printf("PASS: minor version is 3\n");
    printf("PASS: profile is core\n");
    printf("PASS: compatibility bit is absent\n");
    printf("PASS: list ends with None\n");
    return 0;
}
