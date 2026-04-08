#include <stdio.h>
#include "fs_structures.h"

int main() {
    FILE *fp = fopen("disk.img", "rb+");
    JournalEntry entry;
    fseek(fp, JOURNAL_BLOCK * BLOCK_SIZE, SEEK_SET);
    fread(&entry, sizeof(JournalEntry), 1, fp);

    if (entry.is_committed == 1) {
        printf("Uncommitted transaction found for: %s. Replaying.\n", entry.metadata_copy.filename);
        fseek(fp, (INODE_TABLE_BLOCK * BLOCK_SIZE) + (entry.inode_index * sizeof(Inode)), SEEK_SET);
        fwrite(&entry.metadata_copy, sizeof(Inode), 1, fp);
        
        entry.is_committed = 0;
        fseek(fp, JOURNAL_BLOCK * BLOCK_SIZE, SEEK_SET);
        fwrite(&entry, sizeof(JournalEntry), 1, fp);
        printf("Recovery complete.\n");
    } else {
        printf("Disk is consistent.\n");
    }
    fclose(fp);
    return 0;
}
