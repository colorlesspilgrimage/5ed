/*
 * 5ed tables
 */

// TOP

#if !defined(FCODER_TABLES_H)
#define FCODER_TABLES_H

struct Table_Lookup{
    u64 hash;
    u32 index;
    b8 found_match;
    b8 found_empty_slot;
    b8 found_erased_slot;
};

struct Table_u64_u64{
    Base_Allocator *allocator;
    void *memory;
    u64 *keys;
    u64 *vals;
    u32 slot_count;
    u32 used_count;
    u32 dirty_count;
};

struct Table_u32_u16{
    Base_Allocator *allocator;
    void *memory;
    u32 *keys;
    u16 *vals;
    u32 slot_count;
    u32 used_count;
    u32 dirty_count;
};

struct Table_Data_u64{
    Base_Allocator *allocator;
    void *memory;
    u64 *hashes;
    String_Const_u8 *keys;
    u64 *vals;
    u32 slot_count;
    u32 used_count;
    u32 dirty_count;
};

struct Table_u64_Data{
    Base_Allocator *allocator;
    void *memory;
    u64 *keys;
    String_Const_u8 *vals;
    u32 slot_count;
    u32 used_count;
    u32 dirty_count;
};

struct Table_Data_Data{
    Base_Allocator *allocator;
    void *memory;
    u64 *hashes;
    String_Const_u8 *keys;
    String_Const_u8 *vals;
    u32 slot_count;
    u32 used_count;
    u32 dirty_count;
};


#define make_table_u64_u64(a,s) make_table_u64_u64__inner((a),(s),file_name_line_number_lit_u8)
#define make_table_u32_u16(a,s) make_table_u32_u16__inner((a),(s),file_name_line_number_lit_u8)
#define make_table_Data_u64(a,s) make_table_Data_u64__inner((a),(s),file_name_line_number_lit_u8)
#define make_table_u64_Data(a,s) make_table_u64_Data__inner((a),(s),file_name_line_number_lit_u8)
#define make_table_Data_Data(a,s) make_table_Data_Data__inner((a),(s),file_name_line_number_lit_u8)

u64
table_hash(String_Const_u8 key);
extern const u64 table_empty_slot;
extern const u64 table_erased_slot;
extern const u64 table_empty_key;
extern const u64 table_erased_key;
extern const u32 table_empty_u32_key;
extern const u32 table_erased_u32_key;
Table_u64_u64
make_table_u64_u64__inner(Base_Allocator *allocator, u32 slot_count, String_Const_u8 location);
void
table_free(Table_u64_u64 *table);
Table_Lookup
table_lookup(Table_u64_u64 *table, u64 key);
b32
table_read(Table_u64_u64 *table, Table_Lookup lookup, u64 *val_out);
b32
table_read(Table_u64_u64 *table, u64 key, u64 *val_out);
void
table_insert__inner(Table_u64_u64 *table, Table_Lookup lookup, u64 val);
b32
table_rehash(Table_u64_u64 *dst, Table_u64_u64 *src);
b32
table_insert(Table_u64_u64 *table, u64 key, u64 val);
b32
table_erase(Table_u64_u64 *table, Table_Lookup lookup);
b32
table_erase(Table_u64_u64 *table, u64 key);
void
table_clear(Table_u64_u64 *table);
Table_u32_u16
make_table_u32_u16__inner(Base_Allocator *allocator, u32 slot_count, String_Const_u8 location);
void
table_free(Table_u32_u16 *table);
Table_Lookup
table_lookup(Table_u32_u16 *table, u32 key);
b32
table_read(Table_u32_u16 *table, u32 key, u16 *val_out);
void
table_insert__inner(Table_u32_u16 *table, Table_Lookup lookup, u32 key, u16 val);
b32
table_rehash(Table_u32_u16 *dst, Table_u32_u16 *src);
b32
table_insert(Table_u32_u16 *table, u32 key, u16 val);
b32
table_erase(Table_u32_u16 *table, Table_Lookup lookup);
b32
table_erase(Table_u32_u16 *table, u32 key);
void
table_clear(Table_u32_u16 *table);
Table_Data_u64
make_table_Data_u64__inner(Base_Allocator *allocator, u32 slot_count, String_Const_u8 location);
void
table_free(Table_Data_u64 *table);
Table_Lookup
table_lookup(Table_Data_u64 *table, String_Const_u8 key);
b32
table_read(Table_Data_u64 *table, Table_Lookup lookup, u64 *val_out);
b32
table_read_key(Table_Data_u64 *table, Table_Lookup lookup, String_Const_u8 *key_out);
b32
table_read(Table_Data_u64 *table, String_Const_u8 key, u64 *val_out);
void
table_insert__inner(Table_Data_u64 *table, Table_Lookup lookup, String_Const_u8 key, u64 val);
b32
table_rehash(Table_Data_u64 *dst, Table_Data_u64 *src);
b32
table_insert(Table_Data_u64 *table, String_Const_u8 key, u64 val);
b32
table_erase(Table_Data_u64 *table, String_Const_u8 key);
void
table_clear(Table_Data_u64 *table);
Table_u64_Data
make_table_u64_Data__inner(Base_Allocator *allocator, u32 slot_count, String_Const_u8 location);
void
table_free(Table_u64_Data *table);
Table_Lookup
table_lookup(Table_u64_Data *table, u64 key);
b32
table_read(Table_u64_Data *table, Table_Lookup lookup, String_Const_u8 *val_out);
b32
table_read(Table_u64_Data *table, u64 key, String_Const_u8 *val_out);
void
table_insert__inner(Table_u64_Data *table, Table_Lookup lookup, String_Const_u8 val);
b32
table_rehash(Table_u64_Data *dst, Table_u64_Data *src);
b32
table_insert(Table_u64_Data *table, u64 key, String_Const_u8 val);
b32
table_erase(Table_u64_Data *table, Table_Lookup lookup);
b32
table_erase(Table_u64_Data *table, u64 key);
void
table_clear(Table_u64_Data *table);
Table_Data_Data
make_table_Data_Data__inner(Base_Allocator *allocator, u32 slot_count, String_Const_u8 location);
void
table_free(Table_Data_Data *table);
Table_Lookup
table_lookup(Table_Data_Data *table, String_Const_u8 key);
b32
table_read(Table_Data_Data *table, Table_Lookup lookup, String_Const_u8 *val_out);
b32
table_read_key(Table_Data_Data *table, Table_Lookup lookup, String_Const_u8 *key_out);
b32
table_read(Table_Data_Data *table, String_Const_u8 key, String_Const_u8 *val_out);
void
table_insert__inner(Table_Data_Data *table, Table_Lookup lookup, String_Const_u8 key, String_Const_u8 val);
b32
table_rehash(Table_Data_Data *dst, Table_Data_Data *src);
b32
table_insert(Table_Data_Data *table, String_Const_u8 key, String_Const_u8 val);
b32
table_erase(Table_Data_Data *table, String_Const_u8 key);
void
table_clear(Table_Data_Data *table);
#endif

// BOTTOM
