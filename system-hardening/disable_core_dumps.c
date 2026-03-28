#include <stdio.h>
#include <sys/resource.h>

/*
  Disables core dump generation for the current process.
  Useful as a basic hardening measure to prevent sensitive memory
  from being written to disk on crash.
*/

int main(void) {
    struct rlimit rl = {0, 0};

    if (setrlimit(RLIMIT_CORE, &rl) != 0) {
        perror("setrlimit failed");
        return 1;
    }

    return 0;
}
