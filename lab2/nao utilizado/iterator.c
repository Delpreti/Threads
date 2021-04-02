
/* Antigo
void indexed_mult(int* a_mat, int a_line, int* b_mat, int b_col, int* out_mat, int size_line){
    for(int i = 0; i < size_line; i++){
        out_mat[a_line * size_line + b_col] += a_mat[a_line * size_line + i] * b_mat[i * size_line + b_col];
    }
}

void indexed_mult(thread_args* args){
    // Captura os argumentos e "libera a proxima thread". Depois, efetua a multiplicacao.
    int a_line = args->m_iter->line;
    int b_col = args->m_iter->col;
    iter_step(args->m_iter);

    for(int i = 0; i < args->m_iter->size; i++){
        args->m_out[a_line * args->m_iter->size + b_col] += args->m_a[a_line * args->m_iter->size + i] * args->m_b[i * args->m_iter->size + b_col];
    }
}
*/

// --------- Iterator --------- 
// ----------------------------

// O "objeto" mult_iterator organiza os indices que cada thread
typedef struct {
    int line;
    int col;
    int size;
} mult_iterator;

mult_iterator* new_mult_iter(int m_size){
    mult_iterator* mi = malloc( sizeof(mult_iterator) );
    check_malloc(mi);

    mi->size = m_size;
    mi->line = 0;
    mi->col = 0;

    return mi;
}

void iter_step(mult_iterator* mi){
    int next = mi->line * mi->size + mi->col + 1;
    mi->line = next / mi->size;
    mi->col = next % mi->size;
}

int not_finished(mult_iterator* mi){
    if(mi->line < mi->size){
        return 1;
    }
    return 0;
}
