#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid = getpid();
    printf("From saller :Current Process ID: %d\n", pid);
    return 0;
}

