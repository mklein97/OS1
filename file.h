#pragma once

#include "errno.h"
#include "filesystem.h"
#include "disk.h"

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

struct File {
	int offset;
	int in_use;
	struct Inode inode;
	int flags;
};

struct File ftable[16];
int file_open(const char * fname, int flags);
int file_close(int fd);
int file_read(int fd, void* buf, int count);
int file_write(int fd, const void* buf, int count);
int file_seek(int fd, int offset, int whence);
unsigned get_file_inode(unsigned dir_inode, const char * filename);