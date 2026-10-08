/*
 * Binds the custom API table.
 * scripts/test-check-structure.sh edits this file.
 */

// TOP

void
custom_layer_bind_api(API_VTable_custom *vtable){
    custom_api_read_vtable(vtable);
}

// BOTTOM

