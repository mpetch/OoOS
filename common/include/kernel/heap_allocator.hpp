#ifndef __HEAP_ALLOC
#define __HEAP_ALLOC
#include "kernel/libk_decls.h"
#ifndef MAX_BLOCK_EXP
#define MAX_BLOCK_EXP 32u
#endif
#ifndef MIN_BLOCK_EXP
#define MIN_BLOCK_EXP 8u // This times 2^(index) is the full size of a block at that index in a frame 
#endif
#ifndef MAX_COMPLETE_PAGES
#define MAX_COMPLETE_PAGES 5u
#endif
// 64 bits means new levels of l33tpuns! Now featuring Pokemon frustrations using literal suffixes.
constexpr uint64_t BLOCK_MAGIC = 0xB1600FBA615FULL;
// Of course, we wouldn't want to offend anyone, so... (the 7s are T's...don't judge me >.<)
constexpr uint64_t FRAME_MAGIC = 0xD0BE7AC7FUL;
constexpr inline uint64_t REGION_SIZE = PAGESIZE * PT_LEN;
struct block_tag
{
    uint64_t magic { BLOCK_MAGIC };
    size_t block_size;
    size_t held_size;
    int32_t index;
    block_tag* left_split { nullptr };
    block_tag* right_split { nullptr };
    block_tag* previous { nullptr };
    block_tag* next { nullptr };
    size_t align_bytes { 0 };
    constexpr block_tag() = default;
    constexpr block_tag(size_t size, size_t held, int32_t idx, block_tag* left, block_tag* right, block_tag* prev = nullptr, block_tag* nxt = nullptr, size_t align = 0) noexcept :
        block_size      { size },
        held_size       { held },
        index           { idx },
        left_split      { left },
        right_split     { right },
        previous        { prev },
        next            { nxt },
        align_bytes     { align }
                        {}
    constexpr block_tag(size_t size, size_t held, int32_t idx = -1, size_t align = 0) noexcept : block_size{ size }, held_size{ held }, index { idx }, align_bytes { align } {}
    constexpr size_t allocated_size() const noexcept { return block_size - sizeof(block_tag); }
    constexpr size_t available_size() const noexcept { return allocated_size() - (held_size + align_bytes); }
    constexpr vaddr_t actual_start() const noexcept { return vaddr_t { const_cast<block_tag*>(this) } + ptrdiff_t(sizeof(block_tag) + align_bytes); }
    block_tag* split();
} __pack;
struct frame_tag
{
    uint64_t magic { FRAME_MAGIC };
    page_frame* the_frame;
    vaddr_t next_vaddr {};
    uint16_t complete_pages[MAX_BLOCK_EXP - MIN_BLOCK_EXP] {};
    block_tag* available_blocks[MAX_BLOCK_EXP - MIN_BLOCK_EXP] {};
    frame_tag* previous { nullptr };
    frame_tag* next { nullptr };
private:
    spinlock_t __my_mutex{};
public:
    constexpr frame_tag() = default;
    constexpr frame_tag(page_frame* frame, uintptr_t vaddr_next = 0, frame_tag* prev = nullptr, frame_tag* nxt = nullptr) noexcept :
        the_frame   { frame }, 
        next_vaddr  { vaddr_next },
        previous    { prev }, 
        next        { nxt } 
                    {}
    void insert_block(block_tag* blk, int idx);
    void remove_block(block_tag* blk);
    vaddr_t allocate(size_t size, size_t align = 0);
    void deallocate(vaddr_t ptr, size_t align = 0);
    vaddr_t reallocate(vaddr_t ptr, size_t size, size_t align = 0);
    vaddr_t array_allocate(size_t num, size_t size);
private:
    block_tag* __create_tag(size_t size, size_t align);
    block_tag* __melt_left(block_tag* tag) noexcept;
    block_tag* __melt_right(block_tag* tag) noexcept;
    void __lock();
    void __unlock();
} __pack;
enum block_idx : uint8_t
{
    I0  = 0x01,
    I1  = 0x02,
    I2  = 0x04,
    I3  = 0x08,
    I4  = 0x10,
    I5  = 0x20,
    I6  = 0x40,
    I7  = 0x80,
    ALL = 0xFF
};
enum block_size : uint32_t
{
    S512 = 512*PAGESIZE,
    S256 = 256*PAGESIZE,
    S128 = 128*PAGESIZE,
    S64  = 64*PAGESIZE,
    S32  = 32*PAGESIZE,
    S16  = 16*PAGESIZE,
    S08  = 8*PAGESIZE,
    S04  = 4*PAGESIZE
};
#define BS2BI(i) i == S04 ? (I6 | I7) : (i == S08 ? I5 : (i ==  S16 ? I4 : (i ==  S32 ? I3 : (i ==  S64 ? I2 : (i == S128 ? I1 : (i == S256 ? I0 : ALL))))))
/*
 *  Each 512-page region of physical memory is divided into the following blocks:
 *  [256P] B0: |< 000 - 255 >|
 *  [128P] B1: |< 256 - 383 >|
 *  [064P] B2: |< 383 - 447 >|
 *  [032P] B3: |< 448 - 479 >|
 *  [016P] B4: |< 480 - 495 >|
 *  [008P] B5: |< 496 - 503 >|
 *  [004P] B6: |< 504 - 507 >|
 *  [004P] B7: |< 508 - 511 >|
 *  In addition, a region can be allocated in its entirety as a 512-page block.
 *  Blocks are assigned as follows:
 *      1. When memory is requested (i.e. a frame needs additional blocks), the amount of memory requested is used to determine the block size to allocate.
 *          - If the requested block is larger than 256 pages but smaller than 512 pages, a 512-page block is allocated to be divided later.
 *          - If the requested block is larger than 512 pages, a set of physically-contiguous 512-page blocks is allocated as a single block to be divided later.
 *      2. Physical addresses are assigned in order low-to-high, globally, from regions of conventional memory.
 *      3. An amount of identity-mapped space directly after the kernel is reserved at startup for use with these structures. It is never released.
 *          - This amount depends on how much memory is available; 512 bytes track 1 GB of RAM. The kernel's frame-tag will also go here.
 *          - The pagefile entry for the kernel has been allocated by the bootloader and should be below the kernel in memory. 
 *            Entries generated after startup will come from the kernel heap.
 *      4. Virtual addresses are assigned in order low-to-high per page frame. Each page frame tracks its own address-mapping watermark.
 *          - Because of the extreme size of the address space, it should not be necessary to track released virtual addresses.
 *            Instead, blocks that are released from the frame are unmapped, and if they are reallocated later their virtual address will likely change.
 *      5. The kernel heap is initially allocated and mapped by the bootloader and consists of all addresses between the status byte array
 *         and the address corresponding to the end of the identity-mapped region (1GB by default) at startup.
 */
typedef struct mem_status_byte
{
    uint8_t the_byte : 8;
private:
    constexpr bool __has(uint8_t i) const noexcept { return (the_byte & i) == 0; }
 public:
    constexpr bool all_free() const noexcept { return the_byte == 0; }
    constexpr bool has_free(block_idx i) const noexcept { return __has(i); }
    constexpr bool all_used() const noexcept { return the_byte == 0xFF; }
    constexpr void set_used(block_idx i) noexcept { the_byte |= i; }
    constexpr void set_free(block_idx i) noexcept { the_byte &= ~i; }
    constexpr bool operator[](block_idx i) const noexcept { return has_free(i); }
    constexpr bool operator[](block_size i) const noexcept { if(i == S04) return __has(I7) || __has(I6); return __has(BS2BI(i)); }
    constexpr operator bool() const noexcept { return !all_used(); }
    constexpr bool operator!() const noexcept { return all_used(); }
    constexpr static unsigned int gb_of(uintptr_t addr) { return addr / GIGABYTE; }
    constexpr static unsigned int sb_of(uintptr_t addr) { return (addr / (PAGESIZE * PT_LEN)) % 512; }
} __align(1) __pack status_byte, gb_status[512];
class heap_allocator
{
    spinlock_t __heap_mutex{};            // Calls to block allocations lock this mutex to prevent comodification
    pagefile* const __my_pagefile;        // Contains pointers to the various page frames.
    uint16_t __active_frame_index;        // 0 is kernel; 1+ is anything else
    gb_status* const __status_bytes;      // Array of 512-byte arrays
    size_t const __num_status_bytes;      // Length of said array
    uintptr_t const __kernel_heap_begin;  // Convenience pointer to the end of above array
    uintptr_t __physical_open_watermark;  // The lowest physical address known to be open.
    static heap_allocator* __instance;
    constexpr heap_allocator(pagefile* pagefile, gb_status* status_bytes, size_t num_status_bytes, uintptr_t kernel_heap_addr) noexcept :
        __my_pagefile               { pagefile },
        __active_frame_index        { 0 },
        __status_bytes              { status_bytes },
        __num_status_bytes          { num_status_bytes },
        __kernel_heap_begin         { kernel_heap_addr },
        __physical_open_watermark   { 0 }
                                    {}
    constexpr status_byte* __get_sb(uintptr_t addr) { return &(__status_bytes[status_byte::gb_of(addr)][status_byte::sb_of(addr)]); }
    constexpr status_byte& __status(uintptr_t addr) { return *__get_sb(addr); }
    void __mark_used(uintptr_t addr_start, size_t num_regions);
    uintptr_t __find_claim_avail_region(size_t sz);
    void __release_claimed_region(size_t sz, uintptr_t start);
    uintptr_t __new_page_table_block();
    void __lock();
    void __unlock();
public:
    static void init_instance(pagefile* pagefile, mmap_t* mmap);
    static heap_allocator& get();
    inline bool avail() const { return !test_lock(&__heap_mutex); }
    heap_allocator(heap_allocator const&) = delete;
    heap_allocator(heap_allocator&&) = delete;
    heap_allocator& operator=(heap_allocator const&) = delete;
    heap_allocator& operator=(heap_allocator&&) = delete;
    paging_table allocate_pt();
    vaddr_t allocate_block(vaddr_t const& base, size_t sz, uint64_t align);
    vaddr_t allocate_mmio_block(size_t sz, uint64_t align);
    void deallocate_block(vaddr_t const& base, size_t sz);
};
#endif