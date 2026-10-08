#if !defined(FCODER_COMMAND_TABLE_H)
#define FCODER_COMMAND_TABLE_H

// Forward declarations of every command.
#define COMMAND(name, is_ui, description) CUSTOM_COMMAND_SIG(name);
#include "custom/5ed_command_list.h"
#undef COMMAND

struct Command_Metadata{
    Custom_Command_Function *proc;
    b32 is_ui;
    char *name;
    i32 name_len;
    char *description;
    i32 description_len;
};

#define COMMAND(name, is_ui, description) { name, is_ui, #name, (i32)(sizeof(#name) - 1), description, (i32)(sizeof(description) - 1) },
static Command_Metadata fcoder_metacmd_table[] = {
#include "custom/5ed_command_list.h"
};
#undef COMMAND

#define command_table_count ArrayCount(fcoder_metacmd_table)

#endif
