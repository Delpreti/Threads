/* Verifica retorno de malloc */
#define check_alloc(pointer, message){ \
    if (pointer == NULL) { \
        printf(message); \
        exit(-1); \
    } \
}

/* Soma elementos de um array */
#define sum_array(arr, size, soma_var){ \
    for(int i = 0; i < size; i++){ \
        soma_var += arr[i]; \
    } \
}
