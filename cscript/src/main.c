#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "map.h"

#define MAX_LINE_BUFFER_SIZE 1024
#define MAX_WORD_BUFFER_SIZE 256
#define SPACE (' ')
#define TERMINATION ('\0')
#define NEXTLINE ('\n')

#define TO_STRING(x) #x,
#define TO_SIZE(x) sizeof(#x),

#define DEFINE_TYPES(...)\
    enum Type {\
        __VA_ARGS__,\
        Count\
    };\
    const char* type_name[Count] = {\
        MAP(TO_STRING, __VA_ARGS__)\
    };\
    const int type_size[Count] = {\
        MAP(TO_SIZE, __VA_ARGS__)\
    };

DEFINE_TYPES(Int, Float, A);

void parser_word(char word[MAX_WORD_BUFFER_SIZE], int size) {
    int index = -1;
    for (int i = 0; i < Count; i++) {
        if (size == type_size[i] - 1) {
            if (strcmp(type_name[i], word) == 0) {
                index = i;
                break;
            }
        }
    }

    if (index >= 0) {
        printf("[Type:%s]", type_name[index]);
    }
}

void parser_line(FILE* file, char* buffer, int size) {
    char word_buffer[MAX_WORD_BUFFER_SIZE];
    memset(word_buffer, 0, MAX_WORD_BUFFER_SIZE);

    int len = size < MAX_WORD_BUFFER_SIZE ? MAX_WORD_BUFFER_SIZE : size;
    int count = 0;

    for (int i = 0; i < len; ++i, ++count) { 
        word_buffer[count] = buffer[i];
        if (word_buffer[count] == SPACE || word_buffer[count] == NEXTLINE) {
            word_buffer[count] = 0;
            parser_word(word_buffer, count);
            printf("%s ", word_buffer);
            count = -1;
            if (word_buffer[count] == NEXTLINE)
                break;
            memset(word_buffer, 0, MAX_WORD_BUFFER_SIZE);
        }
    }

    putchar(NEXTLINE);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("error: no file input\n");
        return 0;
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("no such file: %s\n", argv[1]);
        exit(1);
    }

    int line_count = 0;
    char line_buffer[MAX_LINE_BUFFER_SIZE];
    memset(line_buffer, 0, MAX_LINE_BUFFER_SIZE);

    while (!feof(file)) {
        char c = fgetc(file);
        line_buffer[line_count++] = c;

        if (c == '\n') {
            line_buffer[line_count] = TERMINATION;
            parser_line(file, line_buffer, line_count);
            memset(line_buffer, 0, line_count);
            line_count = 0;
        }
    }

    fclose(file);

    printf("%d\n", line_count);
}
