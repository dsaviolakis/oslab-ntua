#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main() {
	int pipe1[2], pipe2[2];

	pipe(pipe1);
	pipe(pipe2);

	dup2(pipe1[0], 33);
	dup2(pipe1[1], 34);

	dup2(pipe2[0], 53);
	dup2(pipe2[1], 54);

	char* args[] = {"./riddle", NULL};
	execv(args[0], args);	
}
