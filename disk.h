#pragma once

struct Inode;

#pragma pack(push,1)
struct DirEntry{
    unsigned inode;
    unsigned short rec_len;
    unsigned short name_len;
    char name[1]; //might be longer! Variable size
};
#pragma pack(pop)

void disk_write_sector(unsigned sector, const void* datablock);
void disk_read_sector(unsigned sector, void* datablock);
void disk_init();
void print_blocks();
int disk_read_block(unsigned block, void* datablock);
int disk_read_block_partial(unsigned block, void* vp, unsigned start, unsigned count);
int disk_read_inode(unsigned num, struct Inode* ino);
int list_directory(int dir_inode, int indent);