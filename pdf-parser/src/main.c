#include <stdio.h>

void func(char* str) {
    printf("%s", str);
}

int main(void) {
    FILE* file = fopen("animal farm.pdf", "r");
    if(!file) {
        printf("fopen fail\n");
        fclose(file);
        return 0;
    }
    for(int i = 0; i < 8; i++) {
        printf("%c", getc(file));
    }
    fclose(file);
    return 0;
}
