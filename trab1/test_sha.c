#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "sha256.h"

void print_sha(uint32_t *h_arr){
    printf("%x%x%x%x%x%x%x%x\n", h_arr[0], h_arr[1], h_arr[2],
        h_arr[3], h_arr[4], h_arr[5], h_arr[6], h_arr[7]);
    free(h_arr);
}

int main(void) {
    char *ptr = "hello world";
    print_sha( sha256((unsigned char *)ptr, strlen(ptr)) );
    return 0;
}
