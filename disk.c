#include "disk.h"
#include "console.h"
#include "kprintf.h"
#include "Filesystem.h"

struct Superblock ySB;
char ybuff[4096];

union U{
    char block[4096];
    struct BlockGroupDescriptor bgd[16];
};

int disk_read_block(unsigned block, void* datablock){
    unsigned blockStart = block * 8;
    unsigned i = 0;
    for(i = blockStart; i < blockStart + 8; i++)
        disk_read_sector(i, datablock + ((i - blockStart) * 512));
    return 0;
}

void print_blocks(){
    int k;
    int gs;
    int grs = 0;
    for(gs = ySB.block_count; gs > 0; gs -= ySB.blocks_per_group)
        grs++;
    for(k = 0; k<grs; k++)
    {
        kprintf("Reading BGDT from group %d\n", k);
        static union U tmp;
        disk_read_block(k * ySB.blocks_per_group + 1, tmp.block);
        for(int i = 0; i < grs; i++)
        {
            kprintf("Group %d: ", i);
            kprintf("Free blocks = %.*d\n", sizeof(tmp.bgd[i].free_blocks), tmp.bgd[i].free_blocks);
        }
    }
}

void disk_init(){
    static char buff[4096];
    disk_read_block(0, buff);
    kmemcpy((void*)&ySB, (void*)(buff + 1024), sizeof(struct Superblock));
    //list_directory(2, 0);
    /*kprintf("Volume label: ");
    kprintf("%.*s \t",sizeof(ySB.volname),ySB.volname);
    kprintf("Free: %.*d \n", sizeof(ySB.free_block_count), ySB.free_block_count);
    kprintf("Blocks per group: %.*d\t", sizeof(ySB.blocks_per_group), ySB.blocks_per_group);
    kprintf("Total blocks: %.*d\n", sizeof(ySB.block_count), ySB.block_count);*/
    //print_blocks();
}

void disk_write_sector(unsigned sector, const void* datablock) {
    while(isBusy());
    outb(0x1f6, 0xe0 | (sector >> 24));
    outb(0x3f6, 2);
    outb(0x1f2, 1);
    outb(0x1f3, sector);
    outb(0x1f4, (sector >> 8));
    outb(0x1f5, (sector >> 16));
    outb(0x1f7, 0x30);
    while(isBusy());
    unsigned int i = 0;
    unsigned short w;
    while (i < 1)
    {
        w = inw(0x1f7);
        if (w & 8)
            i = 1;
        if (w & 1) {
            i = 1;
            kprintf("Error");
        }
        if (w & 32) {
            i = 1;
            kprintf("Error");
        }
    }
    short* p = (short*)datablock;
    for(i = 0; i < 256; ++i) {
        outw(0x1f0, *p);
        p++;
    }
    outw(0x1f7, 0xe7);
}

void disk_read_sector(unsigned sector, void* datablock) {
    while(isBusy());
    outb(0x1f6, 0xe0 | (sector >> 24));
    outb(0x3f6, 2);
    outb(0x1f2, 1);
    outb(0x1f3, sector);
    outb(0x1f4, (sector >> 8));
    outb(0x1f5, (sector >> 16));
    outb(0x1f7, 0x20);
    while(isBusy());
    unsigned int i = 0;
    unsigned short w;
    while (i != 1)
    {
        w = inw(0x1f7);
        if (w & 8)
            i = 1;
        if (w & 1) {
            i = 1;
            kprintf("Error");
        }
        if (w & 32) {
            i = 1;
            kprintf("Error");
        }
    }
    short* p = (short*)datablock;
    for(i = 0; i < 256; ++i) {
        unsigned short d = inw(0x1f0);
        p[i] = d;
    }
}

int disk_read_block_partial(unsigned block, void* vp, unsigned start, unsigned count) {
    static char pbb[4096];
    int rv;
    if( (rv = disk_read_block(block, pbb)) < 0 )
        return rv;
    kmemcpy(vp, pbb + start, count);
    return 0;
}


int disk_read_inode(unsigned inode_number, struct Inode* ino) {
    --inode_number;
	int inode_group_number = inode_number / ySB.inodes_per_group;
	int inode_block_number = ySB.blocks_per_group * inode_group_number + 4;
	int byte_offset = (inode_number % ySB.inodes_per_group) * sizeof(ino);
	int block_offset = byte_offset / 4096;
	int inode_offset = inode_number % 32;

	disk_read_block_partial(inode_block_number + block_offset, ino, inode_offset * sizeof(struct Inode), sizeof(struct Inode));
    return 0;
}

void print_file_name(int indent, int name_len, char * fname) {
	int i;
	for (i = 0; i < indent; ++i)
		kprintf("  ");
	kprintf(">%.*s\n", name_len, fname);
}

int list_directory(int inode_number, int indent) {
    static struct Inode inode;
	disk_read_inode(inode_number, &inode);
	
	static char buffer[4096];
	disk_read_block(inode.direct[0], buffer);
	struct DirEntry* dirEntry = (struct DirEntry*)buffer;

	while (dirEntry->rec_len && dirEntry->inode != 0) {
		disk_read_inode(dirEntry->inode, &inode);
		if (dirEntry->inode != 0) {
            kprintf("<%d", dirEntry->inode);
			print_file_name(indent, dirEntry->name_len, dirEntry->name);	
		}
		dirEntry = (struct DirEntry*)(((char*)dirEntry) + dirEntry->rec_len);	
	}
    return 0;
}
