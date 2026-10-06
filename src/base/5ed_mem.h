#if !defined(FCODER_5ED_MEM_H)
#define FCODER_5ED_MEM_H

#if 0
#define block_zero_struct(s) block_zero((s), sizeof(*(s)))
#endif

#if 0
void
block_zero(void *a, u64 size);
void
block_fill_ones(void *a, u64 size);
void
block_copy(void *dst, void *src, u64 size);
i32
block_compare(void *a, void *b, u64 size);
void
block_fill_u8(void *a, u64 size, u8 val);
void
block_fill_u16(void *a, u64 size, u16 val);
void
block_fill_u32(void *a, u64 size, u32 val);
void
block_fill_u64(void *a, u64 size, u64 val);
#endif

#endif
