float execution_time(int (*function)(void*), void* function_arguments){
// As funcoes cujo tempo de execucao nos interessam irao todas passar por essa funcao
// Essas funcoes precisam ter uma assinatura padrao:
    // retorna um inteiro: 0 em caso de sucesso, 1 em caso de falha
    // recebe um unico ponteiro como parametro:
        // O ponteiro deve apontar para uma struct que contem os parametros da funcao
        // O type cast desse ponteiro para o tipo apropriado deve ser realizado dentro da funcao
// Retorna o tempo para executar a funcao passada como parametro, ou -1 em caso de falha
    int start, end;
    GET_TIME(start);
    if( function(function_arguments) ){
        return -1;
    }
    GET_TIME(end);
    return end - start;
}
