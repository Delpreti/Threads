#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "rw.h"

// ----------- utils ----------
// ----------------------------

// macro para verificar resultado da alocacao de memoria
#define check_alloc(pointer, message){ \
    if (pointer == NULL) { \
        printf(message); \
        exit(-1); \
    } \
}


float get_temperature_rand(){
    float x = rand() % 150;
    return (x / 10.0) + 25;
}

// ----------------------------

int main(int argc, char** argv){

    // ---- Leitura da entrada ----
    // ----------------------------
    if(argc < 2){
        printf("Digite: %s <numero de sensores / escritores>\n", argv[0]);
        return 1;
    }

    // ---- Inicialização ----
    // -----------------------
    srand(time(NULL));

    printf("%.1f\n", get_temperature_rand());

    return 0;
}
