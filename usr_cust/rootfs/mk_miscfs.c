#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/***************************************************************************************

			misc_fs filesystem disk layout
	+----+-----+-----+-----------+------------+-----------+-----------+---+-----+
	|boot|super|group|data bitmap|inode bitmap|inode table|data blocks|...|super|
	|    |     |     |           |            |           |           |   |     |
	+----+-----+-----+-----------+------------+-----------+-----------+---+-----+
	boot: 512 Bytes
	super: 1 block
	group: 1 block
	data bitmap: 1 block
	inode bitmap: 1 block
	inode table: m blocks
	data blocks: n blocks

 ***************************************************************************************/
#define BOOT_BLOCK_SIZE 512

char *disk_node;
int misc_fs_block_size = 1024;

typedef struct misc_fs_super_block
{
	unsigned int block_size;
	unsigned int inodes_count;
	unsigned int blocks_per_group;
	unsigned int inodes_per_group;
	
} misc_fs_super_block;

typedef struct misc_fs_group_desc
{
	unsigned int free_blocks;
	unsigned int free_inodes;
	unsigned int inode_table;
} misc_fs_group_desc;

typedef struct misc_fs_inode
{
	unsigned int block;
	unsigned int blocks;
} misc_fs_innode;

int misc_fs_fill_disk(int disk_fd, unsigned long  disk_size,  unsigned int boot_size,
				unsigned int block_size)
{
	unsigned long group_size;
	unsigned long group_num, inode_table_blocks, i;
	int ret;
	misc_fs_super_block super_block;
	misc_fs_group_desc group_desc;
	char *block_buf;


	inode_table_blocks = (sizeof(misc_fs_innode) + block_size - 1)/block_size;
	group_size = 4 * block_size + block_size * 8 * (block_size + inode_table_blocks);
	group_num = (disk_size - boot_size)/group_size;

	block_buf = malloc(block_size);
	if(!block_buf) {
		perror("malloc block_buf filed\n");
		return 1;
	}
	memset(block_buf, 0, block_size);

	// fill super_block
	super_block.block_size = block_size;

	for(i = 0; i < group_num; i ++ ) {
		// write super_block
		ret = lseek(disk_fd, boot_size + i * group_size, SEEK_SET);
		if(ret != boot_size + i * group_size) {
			printf("lseek the first %lu super_block failed, error = %s\n", i, strerror(errno));
			return 1;
		}
		ret = write(disk_fd, &super_block, sizeof(misc_fs_super_block));
		if(ret != sizeof(misc_fs_super_block)) {
			printf("write the forst %lu super_block failed, error = %s\n", i, strerror(errno));
			return 1;
		}
		// write group_desc
		// fill group_desc

		ret = lseek(disk_fd, boot_size + i * group_size + block_size, SEEK_SET);
		if(ret != boot_size + i * group_size + block_size) {
			printf("lseek the first %lu group_desc failed, error = %s\n", i, strerror(errno));
			return 1;
		}
		ret = write(disk_fd, &group_desc, sizeof(misc_fs_group_desc));
		if(ret != sizeof(misc_fs_group_desc)) {
			printf("write the first %lu group_desc failed, error = %s\n", i, strerror(errno));
			return 1;
		}
		// write data block bitmap
		ret = lseek(disk_fd, boot_size + i * group_size + 2 * block_size, SEEK_SET);
		if(ret != boot_size + i * group_size + 2 *  block_size) {
			printf("lseek the first %lu data block bitmap failed, error = %s\n", i, strerror(errno));
			return 1;
		}
		ret = write(disk_fd, block_buf, block_size);
		if(ret != block_size) {
			printf("write the first %lu data block bitmap failed, error = %s\n", i, strerror(errno));
			return 1;
		}
		// write innode block bitmap
		ret = lseek(disk_fd, boot_size + i * group_size + 3 * block_size, SEEK_SET);
		if(ret != boot_size + i * group_size + 3 *  block_size) {
			printf("lseek the first %lu innode block bitmap failed, error = %s\n", i, strerror(errno));
 			return 1;
 		}
 		ret = write(disk_fd, block_buf, block_size);
 		if(ret != block_size) {
			printf("write the first %lu innode block bitmap failed, error = %s\n", i, strerror(errno));
 			return 1;
		}
	}
	free(block_buf);
	return 0;
}

int main(int argc, char *argv[])
{
	int fd, ret;
	off_t disk_size, pos;
	unsigned int inodes;

	if(argc < 2) {
		printf("error: disk node is not specified\n");
		return 1;
	}
	disk_node = argv[1];
	printf("disk_node = %s\n", disk_node);
	fd = open(disk_node, O_WRONLY, 0666);
	if(fd < 0) {
		printf("open %s failed\n", disk_node);
		return 1;
	}

	disk_size = lseek(fd, 0, SEEK_END);
	if(disk_size < 0) {
		printf("lseek %s failed\n", disk_node);
		close(fd);
		return 1;
	}
	printf("disk size = %lu\n", disk_size);

	ret = misc_fs_fill_disk(fd, disk_size, BOOT_BLOCK_SIZE, misc_fs_block_size);
	printf("misc_fs_fill_disk success\n");
	close(fd);
	return 0;
}
