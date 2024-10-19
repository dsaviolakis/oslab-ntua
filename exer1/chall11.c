#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>

int main() {
	int fd = open("secret_number", O_RDWR);
	void *addr = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
		
	uint8_t *ptr;
	ptr = addr;
		
	sleep(10);

	for(int i=0; i<4096; i++) {
		printf("%c", *(ptr + i));
	}

	printf("\n");
	return 0;

}

