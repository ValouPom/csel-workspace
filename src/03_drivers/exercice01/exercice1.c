#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	unsigned int chipid[4]={[0]=0,};

    int fd = open("/dev/mem", O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "error cannot read fd");
        return -1;
    }
    
    size_t page_len = getpagesize();
    off_t addr = 0x01c14200;
    off_t addr_align = addr % page_len; // -> align to a page size
    off_t offset = addr - addr_align;

    uint32_t* reg = mmap(NULL, page_len, PROT_READ, MAP_PRIVATE, fd, offset);
    if (reg == MAP_FAILED) {
        fprintf(stderr, "mmap failed");
        return -1;
    }

	chipid[0] = *(reg + (addr_align / sizeof(uint32_t)));
	chipid[1] = *(reg + (addr_align + 0x4) / sizeof(uint32_t));
	chipid[2] = *(reg + (addr_align + 0x8) / sizeof(uint32_t));
	chipid[3] = *(reg + (addr_align + 0xc) / sizeof(uint32_t));

	printf("Chip 1 %08x\n", chipid[0]);
	printf("Chip 2 %08x\n", chipid[1]);
	printf("Chip 3 %08x\n", chipid[2]);
	printf("Chip 4 %08x\n", chipid[3]);

    munmap(reg, page_len);
    close(fd);
}
