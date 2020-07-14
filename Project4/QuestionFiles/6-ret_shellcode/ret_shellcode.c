#include <unistd.h>
#include<stdio.h>

char name[0x100];

int main() {
    char buff[0x10];
    write(1, "What's your name:", 17);
    read(0, name, 0x100);
    write(1, "What's your nickname:", 21);
    read(0, buff, 0x20);
    return 0;
}
