#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main() {
	int fd = open("bf00", O_RDWR);
	lseek(fd, 1073741824, SEEK_SET);
	write(fd, "XXXXXXXXXXXXXXXX", 16);
	close(fd);

	for (int i = 1; i <= 99; i++) {
        	char command[50];
        	snprintf(command, sizeof(command), "cp bf00 bf%02d", i); // Create the command string
        	system(command); // Execute the command
    	}

	char* args[] = {"./riddle", NULL};
	execv(args[0], args);	

}
