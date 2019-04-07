#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#pragma pack(push,1) 
struct Superblock { 
    unsigned inode_count; 
    unsigned block_count; 
    unsigned r_block_count;
    unsigned free_block_count;
    unsigned free_inode_count;
    unsigned first_data_block;
    unsigned logical_block_size;
    unsigned logical_fragment_size;
    unsigned blocks_per_group;
    unsigned fragments_per_group;
    unsigned inodes_per_group;
    unsigned mounttime;
    unsigned writetime;
    unsigned short mountcount;
    unsigned short maxmountcount;
    unsigned short magic;
    unsigned short state;
    unsigned short errors;
    unsigned short minorrev;
    unsigned lastcheck;
    unsigned checktime;
    unsigned creator;
    unsigned revision;
    unsigned short resuid;
    unsigned short resgid;
    unsigned first_inode;
    unsigned short inode_size;
    unsigned short block_group_number;
    unsigned feature_compat;
    unsigned feature_incompat;
    unsigned feature_ro_compat;
    unsigned char uuid[16];
    char volname[16];
    char lastmount[64];
    char reserved[824];
};
#pragma pack(pop)

#pragma pack(push)
struct BlockGroupDescriptor {
    unsigned block_bitmap;
    unsigned inode_bitmap;
    unsigned inode_table;
    unsigned short free_blocks;
    unsigned short free_inodes;
    unsigned short used_dirs;
    unsigned short pad;
    char reserved[12];
};
#pragma pack(pop)

#pragma pack(push,1)
struct Inode { 
    unsigned short mode;
    unsigned short uid; 
    unsigned size; 
    unsigned atime; 
    unsigned ctime; 
    unsigned mtime; 
    unsigned dtime;
    unsigned short gid;
    unsigned short links;
    unsigned blocks;
    unsigned flags;
    unsigned osd1;
    unsigned direct[12];
    unsigned indirect;
    unsigned doubleindirect;
    unsigned tripleindirect;
    unsigned generation;
    unsigned fileacl;
    unsigned diracl;
    unsigned osd2;
    char reserved[12];
};
#pragma pack(pop)

#endif