#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

static char buff[0x10];
static int ans;
static char flag[0x40];

void init(void) {
	setvbuf(stdout, NULL, _IONBF, 0);
	srand(time(NULL));
	ans = rand();
	puts("---------------------");
	puts("Wanna defeat the agents? I leave an backdoor for you, Hack it if you can ▼ω▼");
	puts("---------------------");
	puts("This is a secret teller machine proto., follow steps below to get the secret");
}
void print_flag(void) {
	execlp("/bin/cat", "/bin/cat", "flag", NULL);
}

int readint() {
	char buff2[0x11];
	int nb = read(0, buff2, 0x10);
	buff2[nb] = 0;
	return atoi(buff2);
}

int main() {
	init();
	printf("What is your name, agent: ");
	read(0, buff, 0x14);
	printf("OK, now give me your secret token: ");
	int guess = readint();
	if (guess == ans) {
		print_flag();
	} else {
		printf("work hard bro\n");
	}
	return 0;
}
