#include <stdio.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

bool authorize_tier3() {
	return 0;
}

void unlock_tier3() {
	return;	
}

