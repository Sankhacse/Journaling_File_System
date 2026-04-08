#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fs_structures.h"

int main() {
    FILE *fp = fopen("disk.img", "wb+");
    char *zero_buf = calloc(1, DISK_SIZE);
    fwrite(zero_buf, 1, DISK_SIZE, fp);
    free(zero_buf);

    rewind(fp);
    Superblock sb = {0xDEADBEEF, MAX_BLOCKS, MAX_BLOCKS - 20};
    fwrite(&sb, sizeof(Superblock), 1, fp);

    fseek(fp, DATA_BITMAP_BLOCK * BLOCK_SIZE, SEEK_SET);
    uint8_t bitmap[MAX_BLOCKS / 8] = {0};
    for(int i = 0; i < 20; i++) bitmap[i/8] |= (1 << (i % 8));
    fwrite(bitmap, 1, sizeof(bitmap), fp);

    printf("Disk Formatted. Bitmaps initialized.\n");
    fclose(fp);
    return 0;
}
