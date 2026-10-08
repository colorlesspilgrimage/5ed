/*
 * Profile list and block types.
 */

// TOP

#if !defined(FCODER_PROFILE_H)
#define FCODER_PROFILE_H

struct Profile_Global_List{
    System_Mutex mutex;
    Arena node_arena;
    Arena_Node *first_arena;
    Arena_Node *last_arena;
    Profile_Thread *first_thread;
    Profile_Thread *last_thread;
    i32 thread_count;
    Profile_Enable_Flag disable_bits;
};

struct Profile_Block{
    Thread_Context *tctx;
    Profile_Global_List *list;
    b32 is_closed;
    Profile_ID id;
    
    Profile_Block(Thread_Context *tctx, Profile_Global_List *list,
                  String_Const_u8 name, String_Const_u8 location);
    Profile_Block(Application_Links *app, String_Const_u8 name, String_Const_u8 location);
    ~Profile_Block();
    void close_now();
};

struct Profile_Scope_Block{
    Thread_Context *tctx;
    Profile_Global_List *list;
    b32 is_closed;
    Profile_ID id;
    
    Profile_Scope_Block(Thread_Context *tctx, Profile_Global_List *list,
                        String_Const_u8 name, String_Const_u8 location);
    Profile_Scope_Block(Application_Links *app, String_Const_u8 name, String_Const_u8 location);
    ~Profile_Scope_Block();
    void close_now();
};

void
profile_clear(Profile_Global_List *list);
void
profile_thread_set_name(Thread_Context *tctx, Profile_Global_List *list, String_Const_u8 name);
void
profile_set_enabled(Profile_Global_List *list, b32 value, Profile_Enable_Flag flag);
#define ProfileThreadName(tctx,list,name) profile_thread_set_name((tctx), (list), (name))

#endif

// BOTTOM

