#if !defined(FCODER_5ED_MALLOC_ALLOCATOR_H)
#define FCODER_5ED_MALLOC_ALLOCATOR_H

void*
base_reserve__malloc(void *user_data, u64 size, u64 *size_out, String_Const_u8 location);
void
base_free__malloc(void *user_data, void *ptr);
Base_Allocator
make_malloc_base_allocator(void);
extern Base_Allocator malloc_base_allocator;
Base_Allocator*
get_allocator_malloc(void);
Arena
make_arena_malloc(u64 chunk_size, u64 align);
Arena
make_arena_malloc(u64 chunk_size);
Arena
make_arena_malloc(void);

#endif
