#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    int i;
    char **ptr;
    extern char **environ;

    printf("Printing arguments\n");
    for (i = 0; i < argc; i++){
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    printf("\nPrinting all env strings\n");
    for (ptr = environ; *ptr != 0; ptr++){ /* and all env strings */
        printf("%s\n", *ptr);
    }
    printf("\n");
    exit(0);
}