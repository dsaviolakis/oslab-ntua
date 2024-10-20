#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <dirent.h>

int main() {

	char filename[18];
	char chr;
	printf("Provide file name uwu ^_^\n");
	scanf("%s", filename);
	printf("Provide character:\n");
	getchar();
	chr = getchar();
	
	int fd = open(filename, O_RDWR);

	char *addr = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	int offset = 111;

	*(addr + offset) = chr;	
	
	return 0;
}

