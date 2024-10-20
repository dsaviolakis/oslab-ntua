#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {

	while(1) {	
		int p = fork();
		if (p == 0) {
			int p1 = getpid();
			printf("%d ", p1);
			if (p1 == 32767) {
				char* args[] = {"./riddle", NULL};
				execv(args[0], args);
			} else {
				//sleep(60);
				//while(1);
				break;
			}	
		} else if (p == 32767) {
			printf("found!!\n");
			while(1);
		} else {
			int status;
			wait(&status);
		}
	}
}
