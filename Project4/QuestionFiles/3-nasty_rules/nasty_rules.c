#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#define PASSWORD_LENGTH 15
#define XORKEY 0xA

void xor(char *s, int len) {
	for (int i = len - 1; i > -1; --i) {
		s[i] ^= XORKEY;
	}
}

int main(int argc, char *argv[]) {
	int fd;
	int len;
	char format[30];
	char password_buf1[PASSWORD_LENGTH + 1];
	char password_buf2[PASSWORD_LENGTH + 1];

	setvbuf(stdout, NULL, _IONBF, 0);

	if (fd=open("password", O_RDONLY, 0400) < 0) {
		perror("Error: ");
		return -1;
	}

	printf("Use your wise, DO NOT bruteforce...\n");
	sleep(time(0) % 12);

	// Read the password
	if(!((len = read(fd, password_buf1, PASSWORD_LENGTH)) > 0)){
		perror("Error: ");
		close(fd);
		return -1;
	}

	// Get your input password
	printf("Input password: ");
	snprintf(format, sizeof(format), "%%%ds", PASSWORD_LENGTH);
	scanf(format, password_buf2);

	// Encode input
	xor(password_buf2, PASSWORD_LENGTH);

	// Boom
	if(!strncmp(password_buf2, password_buf1, PASSWORD_LENGTH)){
		printf("You really pass it\n");
		setreuid(0, 0);
		system("/bin/cat flag\n");
	}
	else{
		printf("Although password length is only 15, you still not get the correct one\n");
	}

	close(fd);
	return 0;
}
