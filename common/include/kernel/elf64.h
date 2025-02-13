#ifndef __ELF64
#define __ELF64
#ifndef _GCC_STDINT_H
#include "stdint.h"
#endif
#include "stddef.h"
#define HAVE_SIZE_T 1
#define HAVE_STDINT 1
#ifdef __cplusplus
#define __alias [[gnu::may_alias]]
enum elf_class : uint8_t
{
    EC_NONE = 0,
    EC_32 = 1,
    EC_64 = 2
};
enum elf_data_encoding : uint8_t
{
    ED_NONE = 0,
    ED_LSB = 1,
    ED_MSB = 2
};
enum elf_machine_type : uint16_t
{
    EM_NONE = 0,
    EM_M32 = 1,
    EM_SPARC = 2,
    EM_I386 = 3,
    EM_68K = 4,
    EM_88K = 5,
    EM_I860 = 7,
    EM_MIPS = 8,
    EM_PPC = 0x14,
    EM_ARM = 0x28,
    EM_SUPERH = 0x2A,
    EM_ITANIUM = 0x32,
    EM_AMD64 = 0x3E,
    EM_AARCH64 = 0xB7,
    EM_RISCV = 0xF3
};
enum elf_object_type : uint16_t
{
    ET_NONE = 0,
    ET_REL = 1,
    ET_EXEC = 2,
    ET_DYN = 3,
    ET_CORE = 4
    // PC-Specific = 0xFF00~0xFFFF
};
enum elf_segment_type : uint32_t
{
    PT_NULL = 0,
    PT_LOAD = 1,
    PT_DYNAMIC = 2,
    PT_INTERP = 3,
    PT_NOTE = 4,
    PT_SHLIB = 5,
    PT_PHDR = 6,
    // PC-Specific = 0x70000000~0x7FFFFFFF
};
enum elf_section_type : uint32_t
{
    ST_NULL = 0,
    ST_PROGBITS = 1,
    ST_SYMTAB = 2,
    ST_STRTAB = 3,
    ST_RELA = 4,
    ST_HASH = 5,
    ST_DYNAMIC = 6,
    ST_NOTE = 7,
    ST_NOBITS = 8,
    ST_REL = 9,
    ST_SHLIB = 10,
    ST_DYNSYM = 11,
    // OS-Specific = 0x60000000~0x6FFFFFFF
    // PC-Specific = 0x70000000~0x7FFFFFFF
    ST_AMD64_UNWIND = 0x70000001
};
enum elf_sym_type : uint8_t
{
    SYM_NOTYPE = 0,
    SYM_OBJECT = 1,
    SYM_FUNCTION = 2,
    SYM_SECTION = 3,
    SYM_FILE = 4
};
enum elf_sym_bind : uint8_t
{
    SB_LOCAL = 0,
    SB_GLOBAL = 1,
    SB_WEAK = 2
};
enum elf_dyn_tag : uint64_t
{
    // NAME             VALUE           d_un        executable  shared
    DT_NULL             = 0,    //      N/A         req         req
    DT_NEEDED           = 1,    //      d_val       opt         opt
    DT_PLTRELSZ         = 2,    //      d_val       opt         opt
    DT_PLTGOT           = 3,    //      d_ptr       opt         opt
    DT_HASH             = 4,    //      d_ptr       req         req
    DT_STRTAB           = 5,    //      d_ptr       req         req
    DT_SYMTAB           = 6,    //      d_ptr       req         req
    DT_RELA             = 7,    //      d_ptr       req         opt
    DT_RELASZ           = 8,    //      d_val       req         opt
    DT_RELAENT          = 9,    //      d_val       req         opt
    DT_STRSZ            = 10,   //      d_val       req         req
    DT_SYMENT           = 11,   //      d_val       req         req
    DT_INIT             = 12,   //      d_ptr       opt         opt
    DT_FINI             = 13,   //      d_ptr       opt         opt
    DT_SONAME           = 14,   //      d_val       N/A         opt
    DT_RPATH            = 15,   //      d_val       opt         N/A
    DT_SYMBOLIC         = 16,   //      N/A         N/A         opt
    DT_REL              = 17,   //      d_ptr       req         opt
    DT_RELSZ            = 18,   //      d_val       req         opt
    DT_RELENT           = 19,   //      d_val       req         opt
    DT_PLTREL           = 20,   //      d_val       opt         opt
    DT_DEBUG            = 21,   //      d_ptr       opt         N/A
    DT_TEXTREL          = 22,   //      N/A         opt         opt
    DT_JMPREL           = 23,   //      d_ptr       opt         opt
    DT_BIND_NOW         = 24,   //      N/A         opt         opt
    DT_INIT_ARRAY       = 25,   //      d_ptr       opt         opt
    DT_FINI_ARRAY       = 26,   //      d_ptr       opt         opt
    DT_INIT_ARRAYSZ     = 27,   //      d_val       opt         opt
    DT_FINI_ARRAYSZ     = 28,   //      d_val       opt         opt 
    DT_RUNPATH          = 29,   //      d_val       opt         opt
    DT_FLAGS            = 30,   //      d_val       opt         opt
    DT_ENCODING         = 31,   //          ?         ?           ?
    DT_PREINIT_ARRAY    = 32,   //      d_ptr       opt         N/A
    DT_PREINIT_ARRAYSZ  = 33    //      d_val       opt         N/A
    // OS-specific 0x6000000D~0x6ffff000    ?         ?           ?
    // PC-specific 0x7000000D~0x7ffff000    ?         ?           ?
};
enum elf_rel_type
{
    // A:		Represents the addend used to compute the value of the relocatable field.
    // B:		Represents the base address at which a shared object has been loaded into memory during execution. Generally, a shared object is built with a 0 base virtual address, but the execution address will be different.
    // G:		Represents the offset into the global offset table at which the relocation entry’s symbol will reside during execution.
    // GOT:	    Represents the address of the global offset table.
    // L:		Represents the place (section offset or address) of the Procedure Linkage Table entry for a symbol.
    // P:		Represents the place (section offset or address) of the storage unit being relocated (computed using r_offset).
    // S:		Represents the value of the symbol whose index resides in the relocation entry.
    // Name 				Value	Field 	Calculation
    R_X86_64_NONE		=	0, // 	none 	none
    R_X86_64_64			=	1, // 	word64 	S + A
    R_X86_64_PC32		=	2, // 	word32 	S + A - P
    R_X86_64_GOT32		=	3, // 	word32 	G + A
    R_X86_64_PLT32		=	4, // 	word32	L + A - P
    R_X86_64_COPY		=	5, // 	none	none
    R_X86_64_GLOB_DAT	=	6, // 	word64 	S
    R_X86_64_JUMP_SLOT	=	7, // 	word64 	S
    R_X86_64_RELATIVE	=	8, // 	word64 	B + A
    R_X86_64_GOTPCREL	=	9, // 	word32 	G + GOT + A - P
    R_X86_64_32			=	10, // 	word32 	S + A
    R_X86_64_32S		=	11, // 	word32 	S + A
    R_X86_64_16			=	12, // 	word16	S + A
    R_X86_64_PC16		=	13, // 	word16 	S + A - P
    R_X86_64_8			=	14, // 	word8 	S + A
    R_X86_64_PC8		=	15, // 	word8 	S + A - P
    R_X86_64_DPTMOD64	=	16, // 	word64
    R_X86_64_DTPOFF64	=	17, // 	word64
    R_X86_64_TPOFF64	=	18, // 	word64
    R_X86_64_TLSGD		=	19, // 	word32
    R_X86_64_TLSLD		=	20, // 	word32
    R_X86_64_DTPOFF32	=	21, // 	word32
    R_X86_64_GOTTPOFF	=	22, // 	word32
    R_X86_64_TPOFF32	=	23, // 	word32
    R_X86_64_PC64		=	24, // 	word64 	S + A - P
    R_X86_64_GOTOFF64	=	25, // 	word64 	S + A - GOT
    R_X86_64_GOTPC32	=	26, // 	word32 	GOT + A - P
    R_X86_64_GOT64 		= 	27, //  word64 	G + A
    R_X86_64_GOTPCREL64 = 	28, //  word64 	G + GOT - P + A
    R_X86_64_GOTPC64 	= 	29, //  word64 	GOT - P + A
    R_X86_64_GOTPLT64 	=	30, //  word64 	G + A
    R_X86_64_PLTOFF64 	= 	31  //  word64 	L - GOT + A
};
constexpr unsigned phdr_flag_execute = 0x1U;
constexpr unsigned phdr_flag_write = 0x2U;
constexpr unsigned phdr_flag_read = 0x04U;
constexpr unsigned shdr_flag_write = 0x1U;
constexpr unsigned shdr_flag_alloc = 0x2U;
constexpr unsigned shdr_flag_execute = 0x4U;
constexpr unsigned shdr_flag_amd64_large = 0x10000000U;
constexpr unsigned elf_ident_class_idx = 4U;
constexpr unsigned elf_ident_encoding_idx = 5U;
constexpr unsigned dyn_flag_origin = 0x1U;
constexpr unsigned dyn_flag_symbolic = 0x2U;
constexpr unsigned dyn_flag_textrel = 0x4U;
constexpr unsigned dyn_flag_bind_now = 0x8U;
constexpr unsigned dyn_flag_static_tls = 0x10U;
enum elf_segment_prot : uint8_t
{
    PV_NONE = 0b000U,
    PV_READ = 0b100U,
    PV_WRITE = 0b010U,
    PV_EXEC = 0b001U,
    PV_READ_EXEC = 0b101U,
    PV_READ_WRITE = 0b110U,
    PV_RWX = 0b111U
};
#else
typedef unsigned char elf_sym_bind;
typedef unsigned char elf_sym_type;
#define __alias
#endif
typedef struct __elf64_ehdr
{
    uint8_t             e_ident[16];           /* Magic number and other info */
    uint16_t            e_type;                /* Object file type */
    uint16_t            e_machine;             /* Architecture */
    uint32_t            e_version;             /* Object file version */
    uint64_t            e_entry;               /* Entry point virtual address */
    uint64_t            e_phoff;               /* Program header table file offset */
    uint64_t            e_shoff;               /* Section header table file offset */
    uint32_t            e_flags;               /* Processor-specific flags */
    uint16_t            e_ehsize;              /* ELF header size in bytes */
    uint16_t            e_phentsize;           /* Program header table entry size */
    uint16_t            e_phnum;               /* Program header table entry count */
    uint16_t            e_shentsize;           /* Section header table entry size */
    uint16_t            e_shnum;               /* Section header table entry count */
    uint16_t            e_shstrndx;            /* Section header string table index */
} elf64_ehdr;
typedef struct __elf64_phdr
{
    uint32_t            p_type;                /* Segment type */
    uint32_t            p_flags;               /* Segment flags */
    uint64_t            p_offset;              /* Segment file offset */
    uint64_t            p_vaddr;               /* Segment virtual address */
    uint64_t            p_paddr;               /* Segment physical address */
    uint64_t            p_filesz;              /* Segment size in file */
    uint64_t            p_memsz;               /* Segment size in memory */
    uint64_t            p_align;               /* Segment alignment */
} elf64_phdr;
typedef struct __elf64_shdr
{
    uint32_t    sh_name;        /* Section name (string tbl index) */
    uint32_t    sh_type;        /* Section type */
    uint64_t    sh_flags;       /* Section flags */
    uint64_t    sh_addr;        /* Section virtual addr at execution */
    uint64_t    sh_offset;      /* Section file offset */
    uint64_t    sh_size;        /* Section size in bytes */
    uint32_t    sh_link;        /* Link to another section */
    uint32_t    sh_info;        /* Additional section information */
    uint64_t    sh_addralign;   /* Section alignment */
    uint64_t    sh_entsize;     /* Entry size if section holds table */
} elf64_shdr;
typedef struct __elf64_sym
{
    uint32_t  st_name;                  /* Symbol name (string tbl index) */
    struct
    {
        elf_sym_type type : 4;          /* Symbol type (object, function, etc) */
        elf_sym_bind bind : 4;          /* Symbol binding (local, global, or weak) */
    } __pack __align(1) st_info;
    uint8_t   st_other;                 /* Symbol visibility */
    uint16_t  st_shndx;                 /* Section index */
    uint64_t  st_value;                 /* Symbol value */
    uint64_t  st_size;                  /* Symbol size */
} elf64_sym;
typedef struct __elf64_rel
{
	uintptr_t   r_offset;
	uint64_t    r_info;
} elf64_rel;
typedef struct __elf64_rela
{
	uintptr_t   r_offset;
	uint64_t    r_info;
	int64_t	    r_addend;
} elf64_rela;
typedef struct __elf64_dyn
{
    int64_t d_tag;
    union __alias
    {
        uint64_t d_val;
        uintptr_t d_ptr;
    };
} elf64_dyn;
// Contains the important details for loading an elf executable as a task.
typedef struct __elf64_program_desc
{
    void* frame_ptr;
    void* prg_stack;
    size_t stack_size;
    void* prg_tls;
    size_t tls_size;
    void* entry;
} elf64_program_descriptor;
#endif