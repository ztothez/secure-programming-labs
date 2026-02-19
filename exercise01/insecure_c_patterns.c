#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Intentionally vulnerable examples for secure programming practice.

  These are not "bugs to fix" inside this file. They exist to demonstrate:
  - stack/heap overflows
  - off-by-one errors
  - function pointer clobbering
  - format string issues

  Run in a controlled environment.
*/

/* ---------------- T1: Unlimited buffer overflow ---------------- */
/*
  CWE-120 / CWE-242 (gets)
  Reads user input into a fixed-size stack buffer with no bounds checking.
*/
void T1_unlimited_buffer_overflow(void) {
    char buffer[16];

    printf("\n[T1] Unlimited buffer overflow\n");
    printf("Enter input: ");
    gets(buffer);  // UNSAFE: no bounds checking, deprecated
    printf("You entered: %s\n", buffer);
}

/* ---------------- T2: Off-by-one overflow ---------------- */
/*
  CWE-193
  Writes one byte past the end of the buffer due to a bad loop condition.
*/
void T2_off_by_one(void) {
    char buffer[8];

    printf("\n[T2] Off-by-one overflow\n");

    // Valid indices are 0..7, but this loop writes index 8 as well.
    for (int i = 0; i <= 8; i++) {
        buffer[i] = 'A';
    }

    buffer[7] = '\0';  // make printing predictable
    printf("Buffer: %s\n", buffer);
}

/* ---------------- T3: Heap overflow ---------------- */
/*
  CWE-122
  Allocates a small heap buffer and then copies a longer string into it.
*/
void T3_heap_overflow(void) {
    char *buffer = malloc(8);
    if (!buffer) {
        printf("malloc failed\n");
        return;
    }

    printf("\n[T3] Heap overflow\n");

    // Writing beyond allocated heap memory (8 bytes).
    strcpy(buffer, "AAAAAAAAAAAA");
    printf("Buffer: %s\n", buffer);

    free(buffer);
}

/* ---------------- T4: Function pointer clobbering ---------------- */
/*
  CWE-121 (stack overflow style)
  Demonstrates how overflowing a buffer inside a struct can corrupt an adjacent field.
*/
static void safe_function(void) {
    printf("Safe function executed\n");
}

static void hacked_function(void) {
    printf("Hacked function executed\n");
}

struct data {
    char buffer[8];
    void (*func)(void);
};

void T4_function_pointer_overflow(void) {
    struct data d;
    d.func = safe_function;

    printf("\n[T4] Function pointer overflow\n");

    // Overwrites beyond buffer and may corrupt d.func depending on layout.
    strcpy(d.buffer, "AAAAAAAA");
    d.func();
}

/* ---------------- T5: printf format string vulnerability ---------------- */
/*
  CWE-134
  Uses user input directly as the format string.
*/
void T5_printf_vulnerability(void) {
    char input[100];

    printf("\n[T5] printf format string vulnerability\n");
    printf("Enter format string: ");
    if (!fgets(input, sizeof(input), stdin)) {
        printf("Input error\n");
        return;
    }

    printf(input);  // VULNERABLE: user controls the format string
}

/* ---------------- Main menu ---------------- */
int main(void) {
    int choice;

    printf("Select task to run:\n");
    printf("1 - T1 Unlimited buffer overflow\n");
    printf("2 - T2 Off-by-one overflow\n");
    printf("3 - T3 Heap overflow\n");
    printf("4 - T4 Function pointer overflow\n");
    printf("5 - T5 printf vulnerability\n");
    printf("Choice: ");

    if (scanf("%d", &choice) != 1) {
        printf("Invalid input\n");
        return 1;
    }
    getchar();  // consume newline

    switch (choice) {
        case 1: T1_unlimited_buffer_overflow(); break;
        case 2: T2_off_by_one(); break;
        case 3: T3_heap_overflow(); break;
        case 4: T4_function_pointer_overflow(); break;
        case 5: T5_printf_vulnerability(); break;
        default: printf("Invalid choice\n"); break;
    }

    return 0;
}
