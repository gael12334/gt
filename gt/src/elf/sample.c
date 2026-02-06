#include <stdio.h>

void print_args(int argc, char** argv)
{
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
}

int main(int argc, char** argv)
{
    printf("argc : %i", argc);
    print_args(argc, argv);
    return 0;
}
