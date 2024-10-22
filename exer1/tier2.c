#include <stdio.h>
#include <sys/mman.h>

int setup_tier2 () {
	long int *addr = mmap(0x6042000, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	*addr = getpid();
	return 0;
}
