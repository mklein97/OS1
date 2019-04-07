#include "file.h"
#include "console.h"

int file_open(const char* fname, int flags) {
	int file_index = 0;
	while (ftable[file_index].in_use == 1) {
		++file_index;
		if (file_index >= 16)
			return -EMFILE;
	}

	int inode_number = get_file_inode(2, fname);
	if (inode_number == 0)
		return -ENOENT;
	disk_read_inode(inode_number, &ftable[file_index].inode);
	ftable[file_index].in_use = 1;
	ftable[file_index].flags = flags;
	ftable[file_index].offset = 0;

	return file_index;
}

int file_close(int fd) {
	if (fd >= 16 || fd < 0)
		return -EINVAL;
	if (ftable[fd].in_use == 0)
		return -4;

	ftable[fd].in_use = 0;
	return SUCCESS;
}

int file_read(int fd, void* buf, int count) {
	if (fd >= 16 || fd < 0)
		return -EINVAL;
	struct File* fp = &ftable[fd];
	if (fp->in_use == 0)
		return -EMFILE;
	if (count <= 0 || fp->offset >= fp->inode.size)
		return SUCCESS;

	int bi = fp->offset / 4096;
	static char buffy[4096];
	static unsigned int U[1024];

	if (bi < 12)
		disk_read_block(fp->inode.direct[bi], buffy);
	else {
		bi -= 12;
		if (bi < 1024) {
			disk_read_block(fp->inode.indirect, (void*)U);
			disk_read_block(U[bi], buffy);
		}
		else {
			bi -= 1024;
			if (bi < 1048576) {
				disk_read_block(fp->inode.doubleindirect, (void*)U);
				disk_read_block(U[bi >> 10], (void*)U);
				disk_read_block(U[bi % 1024], buffy);
			}
		}
	}
	int bo = fp->offset % 4096;
	int final_count = count;
	int buffer_size = 4096 - bo;
	int remaining_file_size = fp->inode.size - fp->offset;
	if (buffer_size < final_count)
		final_count = buffer_size;
	if (remaining_file_size < final_count)
		final_count = remaining_file_size;
	kmemcpy(buf, buffy + bo, final_count);
	fp->offset += final_count;
	return final_count;
}

int file_write(int fd, const void* buf, int count) {
	return -ENOSYS;
}

int file_seek(int fd, int offset, int whence) {
	if (fd >= 16 || fd < 0)
		return EINVAL;
	struct File* fp = &ftable[fd];
	if (fp->in_use == 0)
		return -EMFILE;
	
	if (whence == SEEK_SET) {
		if (offset < 0)
			return -EINVAL;
		
		fp->offset = offset;
		return fp->offset;
	}
	else if (whence == SEEK_CUR) {
		if (fp->offset + offset < 0)
			return -EINVAL;
		
		fp->offset += offset;
		return fp->offset;
	}
	else if (whence == SEEK_END) {
		if (offset + (int)fp->inode.size < 0)
			return -EINVAL;
		fp->offset = fp->inode.size + offset;
		return fp->offset;
	}
	else
		return -EINVAL;
	
	return SUCCESS;
}

unsigned get_file_inode(unsigned dir_inode, const char * filename) {
	struct Inode inode;
	disk_read_inode(dir_inode, &inode);
	static char buffer[4096];
	disk_read_block(inode.direct[0], buffer);
	struct DirEntry * dirEntry = (struct DirEntry*)buffer;

	while (dirEntry->rec_len) {
		disk_read_inode(dirEntry->inode, &inode);
		int mode = inode.mode >> 12;

		if (mode == 8) {
			int equal = 1;
			for (int i = 0; i < dirEntry->name_len; ++i) {
				if (dirEntry->name[i] != filename[i]) {
					equal = 0;
					break;
				}
			}
			if (equal == 1)
				return dirEntry->inode;
		}
		dirEntry = (struct DirEntry *)(((char *)dirEntry) + dirEntry->rec_len);	
	}
	return 0;
}