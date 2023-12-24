#include "csv.h"
#include "matrix.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

struct matrix* load_csv(const char* path)
{
    FILE* file = fopen(path, "r");
    assert(file != NULL);

    struct matrix* result = NULL;
    u64 acol = 0;
    char c;

    {
        u64 col = 1;
        u64 row = 0;
        char line[100];
        fgets(line, 100, file);
        acol = strlen(line);
        for(u64 i = 0; i < acol; i++)
            if(line[i] == ',')
                col++;
        fseek(file, 0, SEEK_SET);

        do
        {
            c = fgetc(file);
            if(c == '\n')
                row++;
        }while(c != EOF);

        fseek(file, 0, SEEK_SET);

        result = matrix_create(col, row);
    }

    i64 ccol;
    i64 s;

    char data[10];
    for(i32 k = 0; k < 10; k++)
        data[k] = ' ';
    for(u64 i = 0; i < result->row; i++)
    {
        s = 0;
        ccol = 0;
        for(u64 j = 0; j <= acol; j++)
        {
            c = fgetc(file);
            if(c == ',' || c == '\n')
            {
                result->data[i][ccol] = atof(data);
                for(i32 k = 0; k < 10; k++)
                    data[k] = ' ';
                ccol++;
                s = j;
                if(c == '\n') break;
                continue;
            }
            data[j-s] = c;
        }
    }

    fclose(file);

    return result;
}
