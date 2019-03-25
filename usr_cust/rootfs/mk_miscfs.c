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

// misc_fs disk data structure start
#define MISC_FS_MAX_CHILD 32
#define MISC_FS_NAME_MAX_LEN 16
#define MISC_FS_MAX_DATA_BLOCK 32

#define cust_len32 unsigned int
#define cust_len16 unsigned short
#define cust_len8 char

typedef struct misc_fs_child_id
{
	cust_len32 groud_id; // group index
	cust_len32 inode_id; // inode index in the group
};

typedef struct misc_fs_data_id
{
	cust_len32 groud_id; // group index
	cust_len32 block_id; // data block index in the group
};

typedef struct misc_fs_super_block
{
	cust_len32 block_size;
	cust_len32 group_count; // total groups
	cust_len32 blocks_per_group;
	cust_len32 inodes_per_group;
	cust_len32 inode_table_blocks; // the blocks that inode table occupy
	
};

typedef struct misc_fs_group_desc
{
	cust_len32 free_data_blocks;
	cust_len32 free_inodes;
};

typedef struct misc_fs_inode
{
	cust_len16 mode;
	struct misc_fs_data_id data[MISC_FS_MAX_DATA_BLOCK]; // data block table
	cust_len32 data_blocks;
	cust_len32 data_len;
	cust_len32 child_num;
	struct misc_fs_child_id child[MISC_FS_MAX_CHILD];
	cust_len8 name[MISC_FS_NAME_MAX_LEN];
};
// misc_fs disk data structure end

int misc_fs_reset_disk(int disk_fd, unsigned long disk_size, unsigned int boot_size,
				unsigned int block_size)
{
	unsigned long group_size, group_size_in_blocks;
	unsigned long group_num, inode_table_blocks, i;
	int ret;
	struct misc_fs_super_block super_block;
	struct misc_fs_group_desc group_desc;
	char *block_buf;

	inode_table_blocks = (sizeof(struct misc_fs_inode) + block_size - 1)/block_size;
	group_size = 4 * block_size + block_size * 8 * (block_size + inode_table_blocks);
	group_size_in_blocks = 4 + block_size + inode_table_blocks;
	group_num = (disk_size - boot_size)/group_size;

	block_buf = malloc(block_size);
	if(!block_buf) {
		perror("malloc block_buf filed\n");
		return 1;
	}
	memset(block_buf, 0, block_size);

	// fill super_block
	super_block.block_size = block_size;
	super_block.group_count = group_num;
	super_block.blocks_per_group = group_size_in_blocks;
	super_block.inodes_per_group = block_size;
	super_block.inode_table_blocks = inode_table_blocks;

	// fill group_desc
	group_desc.free_data_blocks = block_size;
	group_desc.free_inodes = block_size;

	for(i = 0; i < group_num; i ++ ) {
		// write super_block
		ret = lseek(disk_fd, boot_size + i * group_size, SEEK_SET);
		if(ret != boot_size + i * group_size) {
			printf("lseek the first %lu super_block failed, error = %s\n", i, strerror(errno));
			return 1;
		}
		ret = write(disk_fd, &super_block, sizeof(struct misc_fs_super_block));
		if(ret != sizeof(struct misc_fs_super_block)) {
			printf("write the forst %lu super_block failed, error = %s\n", i, strerror(errno));
			return 1;
		}

		// write group_desc
		ret = lseek(disk_fd, boot_size + i * group_size + block_size, SEEK_SET);
		if(ret != boot_size + i * group_size + block_size) {
			printf("lseek the first %lu group_desc failed, error = %s\n", i, strerror(errno));
			return 1;
		}
		ret = write(disk_fd, &group_desc, sizeof(struct misc_fs_group_desc));
		if(ret != sizeof(struct misc_fs_group_desc)) {
			printf("write the first %lu group_desc failed, error = %s\n", i, strerror(errno));
			return 1;
		}

		// reset data block bitmap
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

		// reset innode block bitmap
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

int misc_fs_create_lost_found_dir(int disk_fd, unsigned long  disk_size, unsigned int boot_size,
				unsigned int block_size)
{
	char *name = "lost+found";
	int len;
	unsigned long group_size, group_size_in_blocks;
	unsigned long group_num, inode_table_blocks, i;
	int ret;
	struct misc_fs_super_block super_block;
	struct misc_fs_group_desc group_desc;
	struct misc_fs_inode inode;
	unsigned long pos;

	len = strlen(name);
	if(len > MISC_FS_NAME_MAX_LEN - 1) {
		printf("directory name exceed max length\n");
		return 1;
	}

	inode_table_blocks = (sizeof(struct misc_fs_inode) + block_size - 1)/block_size;
	group_size = 4 * block_size + block_size * 8 * (block_size + inode_table_blocks);
	group_size_in_blocks = 4 + block_size + inode_table_blocks;
	group_num = (disk_size - boot_size)/group_size;

	// update misc_fs_group_desc 0
	group_desc.free_data_blocks = block_size;
	group_desc.free_inodes = block_size - 1;
	pos = boot_size + block_size;
	ret = lseek(disk_fd, pos, SEEK_SET);
	if(ret != pos) {
		printf("lseek the fist 0 group_desc failed, error = %s\n", strerror(errno));
		return 1;
	}
	ret = write(disk_fd, &group_desc, sizeof(struct misc_fs_group_desc));
	if(ret != sizeof(struct misc_fs_group_desc)) {
		printf("write the first 0 group_desc failed, error = %s\n",  strerror(errno));
		return 1;
	}

	// update inode bitmap
	pos = boot_size + 3 * block_size;
	// occupy 2 inodes, one is for root and another is for "lost+found"
	cust_len32 bitmap = 0x00000003;
	ret = lseek(disk_fd, pos, SEEK_SET);
	if(ret != pos) {
		printf("lseek the inode bitmap failed in group0, error = %s\n", strerror(errno));
		return 1;
	}
	ret = write(disk_fd, &bitmap, sizeof(bitmap));
	if(ret != sizeof(bitmap)) {
		printf("write the inode bitmap failed in group0, error = %s\n", strerror(errno));
		return 1;
	}
	// update root misc_fs_inode 0
	inode.mode = 040777; // set as a directory
	inode.data_blocks = 0;
	inode.data_len = 0;
	inode.child_num = 1;
	inode.child[0].groud_id = 0;
	inode.child[0].inode_id = 1;

	pos = boot_size + 4 * block_size;
	ret = lseek(disk_fd, pos, SEEK_SET);
	if(ret != pos) {
		printf("lseek the inode 0 failed in group0, error = %s\n", strerror(errno));
		return 1;
	}
	ret = write(disk_fd, &inode, sizeof(struct misc_fs_inode));
	if(ret != sizeof(struct misc_fs_inode)) {
		printf("write the inode 0 failed in group0, error = %s\n", strerror(errno));
		return 1;
	}

	// fill misc_fs_inode 1
	inode.mode = 040777; // set as a directory
	inode.data_blocks = 0;
	inode.data_len = 0;
	inode.child_num = 0;
	memcpy(inode.name, name, len);
	inode.name[len] = '\0';

	pos = boot_size + 4 * block_size + sizeof(struct misc_fs_inode);
	ret = lseek(disk_fd, pos, SEEK_SET);
	if(ret != pos) {
		printf("lseek the inode 1 failed in group0, error = %s\n", strerror(errno));
		return 1;
	}
	ret = write(disk_fd, &inode, sizeof(struct misc_fs_inode));
	if(ret != sizeof(struct misc_fs_inode)) {
		printf("write the inode 1 failed in group0, error = %s\n", strerror(errno));
		return 1;
	}

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

	ret = misc_fs_reset_disk(fd, disk_size, BOOT_BLOCK_SIZE, misc_fs_block_size);
	if(0 == ret) {
		printf("misc_fs_reset_disk success\n");
	}
	ret = misc_fs_create_lost_found_dir(fd, disk_size, BOOT_BLOCK_SIZE, misc_fs_block_size);
	if(0 == ret) {
		printf("misc_fs_create_lost_found_dir success\n");
	}
	close(fd);
	return 0;
}
