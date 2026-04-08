#define FUSE_USE_VERSION 31
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "fs_structures.h"

FILE *disk;

int allocate_block() {
    uint8_t bitmap[BLOCK_SIZE];
    fseek(disk, DATA_BITMAP_BLOCK * BLOCK_SIZE, SEEK_SET);
    fread(bitmap, 1, BLOCK_SIZE, disk);

    for (int i = 0; i < MAX_BLOCKS / 8; i++) {
        for (int bit = 0; bit < 8; bit++) {
            if (!(bitmap[i] & (1 << bit))) {
                bitmap[i] |= (1 << bit);
                fseek(disk, DATA_BITMAP_BLOCK * BLOCK_SIZE, SEEK_SET);
                fwrite(bitmap, 1, BLOCK_SIZE, disk);
                fflush(disk);
                return (i * 8) + bit;
            }
        }
    }
    return -1;
}

void journal_metadata(int idx, Inode *node) {
    JournalEntry entry;
    entry.inode_index = idx;
    memcpy(&entry.metadata_copy, node, sizeof(Inode));
    entry.is_committed = 1;
    fseek(disk, JOURNAL_BLOCK * BLOCK_SIZE, SEEK_SET);
    fwrite(&entry, sizeof(JournalEntry), 1, disk);
    fflush(disk);
}

static int fs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    (void)fi;
    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }
    fseek(disk, INODE_TABLE_BLOCK * BLOCK_SIZE, SEEK_SET);
    Inode node;
    for (int i = 0; i < MAX_FILES; i++) {
        fread(&node, sizeof(Inode), 1, disk);
        if (node.is_used && strcmp(path + 1, node.filename) == 0) {
            stbuf->st_mode = (node.is_dir ? S_IFDIR : S_IFREG) | 0644;
            stbuf->st_size = node.size;
            return 0;
        }
    }
    return -ENOENT;
}

static int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    (void)offset; (void)fi; (void)flags;
    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    fseek(disk, INODE_TABLE_BLOCK * BLOCK_SIZE, SEEK_SET);
    Inode node;
    for (int i = 0; i < MAX_FILES; i++) {
        fread(&node, sizeof(Inode), 1, disk);
        if (node.is_used) filler(buf, node.filename, NULL, 0, 0);
    }
    return 0;
}

static int fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    fseek(disk, INODE_TABLE_BLOCK * BLOCK_SIZE, SEEK_SET);
    Inode node;
    for (int i = 0; i < MAX_FILES; i++) {
        fread(&node, sizeof(Inode), 1, disk);
        if (node.is_used && strcmp(path + 1, node.filename) == 0) {
            fseek(disk, node.start_block * BLOCK_SIZE + offset, SEEK_SET);
            return fread(buf, 1, size, disk);
        }
    }
    return -ENOENT;
}

static int create_entry(const char *path, int is_dir) {
    Inode node;
    for (int i = 0; i < MAX_FILES; i++) {
        long pos = (INODE_TABLE_BLOCK * BLOCK_SIZE) + (i * sizeof(Inode));
        fseek(disk, pos, SEEK_SET);
        fread(&node, sizeof(Inode), 1, disk);
        if (!node.is_used) {
            memset(&node, 0, sizeof(Inode));
            strncpy(node.filename, path + 1, 31);
            node.is_used = 1;
            node.is_dir = is_dir;
            node.size = 0;
            node.start_block = allocate_block();

            journal_metadata(i, &node);
            fseek(disk, pos, SEEK_SET);
            fwrite(&node, sizeof(Inode), 1, disk);

            JournalEntry clear = {0};
            fseek(disk, JOURNAL_BLOCK * BLOCK_SIZE, SEEK_SET);
            fwrite(&clear, sizeof(JournalEntry), 1, disk);
            fflush(disk);
            return 0;
        }
    }
    return -ENOSPC;
}

static int fs_mknod(const char *path, mode_t mode, dev_t rdev) { return create_entry(path, 0); }
static int fs_mkdir(const char *path, mode_t mode) { return create_entry(path, 1); }

static int fs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    if (strncmp(buf, "CRASH", 5) == 0) { 
        printf("\nPOWER FAILURE SIMULATED\n");
        exit(1); 
    }
    fseek(disk, INODE_TABLE_BLOCK * BLOCK_SIZE, SEEK_SET);
    Inode node;
    for (int i = 0; i < MAX_FILES; i++) {
        long pos = ftell(disk);
        fread(&node, sizeof(Inode), 1, disk);
        if (node.is_used && strcmp(path + 1, node.filename) == 0) {
            node.size = size;
            journal_metadata(i, &node);
            fseek(disk, node.start_block * BLOCK_SIZE + offset, SEEK_SET);
            fwrite(buf, 1, size, disk);
            fseek(disk, pos, SEEK_SET);
            fwrite(&node, sizeof(Inode), 1, disk);
            fflush(disk);
            return size;
        }
    }
    return -ENOENT;
}

static int fs_utimens(const char *path, const struct timespec tv[2], struct fuse_file_info *fi) { return 0; }

static struct fuse_operations my_ops = {
    .getattr = fs_getattr,
    .readdir = fs_readdir,
    .mknod = fs_mknod,
    .mkdir = fs_mkdir,
    .write = fs_write,
    .read = fs_read,
    .utimens = fs_utimens,
};

int main(int argc, char *argv[]) {
    disk = fopen("disk.img", "rb+");
    return fuse_main(argc, argv, &my_ops, NULL);
}
