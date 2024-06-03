#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


#define MEBIBYTE (1024 * 1024) 

int main()
{
    char* block = NULL;

    block = (char*)malloc(MEBIBYTE * 50);

    if (block == NULL) {
        perror("Error cannot allocate more memory\n");
        return 1;
    }

    memset(block, 0, MEBIBYTE * 50);

    return 0;
}
