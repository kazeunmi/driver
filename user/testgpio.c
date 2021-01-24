#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/mman.h>

#define GPIO_BASE 0x3F200000
#define GPIO_LEN 0x100
#define GPFSEL 0x0000
#define GPSET 0x001C
#define GPCLR 0x0028
#define GPLEV 0x0034

uint32_t read32(void* base, int offset)
{
    char* addr = (char*)base + offset;
    return *(volatile uint32_t*)addr;
}

void write32(void* base, int offset, uint32_t data)
{
    char* addr = (char*)base + offset;
    *(volatile uint32_t*)addr = data;
}

int main()
{
    void* base;
    int fd;
    uint32_t val;

    fd = open("/dev/mem", (O_RDWR | O_SYNC));
    assert(fd > 0);
    printf("fd=%d\n", fd);

    base = mmap(NULL, GPIO_LEN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, GPIO_BASE);
    assert(base != MAP_FAILED);
    printf("base=%p\n", base);

    // Set GPIO4 to output (3 bit for each GPIO, 1 is for output)
    val = read32(base, GPFSEL);
    val &= ~(7 << (3*4));
    val |= 1 << (3*4);
    write32(base, GPFSEL, val);
    printf("GPFSEL=%08x\n", read32(base, GPFSEL));

    // Blink LED on GPIO4
    for (int i = 0; i < 5; i++) {
        printf("set\n");
        write32(base, GPSET, 1 << 4);  // 0 bits have no effect
        printf("GPLEV=%08x\n", read32(base, GPLEV));
        usleep(500 * 1000);

        printf("clr\n");
        write32(base, GPCLR, 1 << 4);  // 0 bits have no effect
        printf("GPLEV=%08x\n", read32(base, GPLEV));
        usleep(500 * 1000);
    }

    munmap(base, GPIO_LEN);
    close(fd);

    return 0;
}
