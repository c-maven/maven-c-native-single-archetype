#include <stdio.h>
#include <string.h>

#include "hello.h"

int main(void) {
    const char *actual = hello_message();
    const char *expected = "Hello from Maven native-maven-plugin and C!";

    if (strcmp(actual, expected) != 0) {
        fprintf(stderr, "expected: %s\nactual: %s\n", expected, actual);
        return 1;
    }

    puts("hello_message test passed");
    return 0;
}
