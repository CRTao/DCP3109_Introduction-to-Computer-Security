#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	int random;
	int value;

	setvbuf(stdout, NULL, _IONBF, 0);

	// We need a random value to prevent someone break us!
	random = rand();

	// Get value from stdin
	scanf("%d", &value);

	// BIG DEAL! DON'T LET OTHER BREAK IT!
	if((value ^ random) == 0xDEADBEAF){
		printf("Good!\n");

		system("/bin/cat flag");
		return 0;
	}

	printf("You really need to read the manual, otherwise you you should try 2^32 cases.\n");
	return 0;
}
