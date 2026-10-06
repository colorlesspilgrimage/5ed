#if !defined(FCODER_5ED_HASH_FUNCTIONS_H)
#define FCODER_5ED_HASH_FUNCTIONS_H

u64
table_hash_u8(u8 *v, u64 size);
u64
table_hash_u16(u16 *v, u64 size);
u64
table_hash_u32(u32 *v, u64 size);
u64
table_hash_u64(u64 *v, u64 size);
u64
table_hash(void *v, i32 it_size, u64 size);

#endif
