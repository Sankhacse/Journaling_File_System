#include <stdio.h>
#include "fs_structures.h"

int main() {
    FILE *fp = fopen("disk.img", "rb");
    Superblock sb;
    fread(&sb, sizeof(Superblock), 1, fp);
    printf("Disk Magic: 0x%X\n", sb.magic);
    
    fseek(fp, INODE_TABLE_BLOCK * BLOCK_SIZE, SEEK_SET);
    Inode node;
    for (int i = 0; i < 10; i++) {
        fread(&node, sizeof(Inode), 1, fp);
        if (node.is_used) {
            printf("[%d] Name: %s | Block: %d | Size: %u\n", i, node.filename, node.start_block, node.size);
        }
    }
    fclose(fp);
    return 0;
}
