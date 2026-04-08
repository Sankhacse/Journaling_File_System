#ifndef FS_STRUCTS_H
#define FS_STRUCTS_H

#include <stdint.h>

#define BLOCK_SIZE 4096
#define MAX_FILES 100
#define DISK_SIZE (10 * 1024 * 1024) 
#define MAX_BLOCKS (DISK_SIZE / BLOCK_SIZE)

#define SB_BLOCK 0
#define INODE_BITMAP_BLOCK 1
#define DATA_BITMAP_BLOCK 2
#define JOURNAL_BLOCK 3
#define INODE_TABLE_BLOCK 4

typedef struct {
    uint32_t magic;
    uint32_t total_blocks;
    uint32_t free_blocks;
} Superblock;

typedef struct {
    char filename[32];
    uint32_t size;
    uint32_t start_block;
    uint8_t is_used;
    uint8_t is_dir; 
} Inode;

typedef struct {
    uint32_t inode_index;
    Inode metadata_copy;
    uint8_t is_committed; 
} JournalEntry;

#endif
