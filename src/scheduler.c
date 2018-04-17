#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/scheduler.h"
#include <ucontext.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define STACKMEM 8192

/*O escalonador a ser implementado � do tipo FIFO n�o preemptivo sem prioridades.
Dessa forma, sempre que uma thread for posta no estado apto, ela ser� inserida no final da fila de threads aptas a executar.
Quando necess�rio, o escalonador selecionar� para execu��o a primeira thread da fila*/

//inicializa��o de vari�veis globais gerais
int tid_id = 1;
bool isInitialized = false;

//Filas para representar os estados
static FILA2 filaApto;
static FILA2 filaBloqueado;
static FILA2 filaAptoSuspenso;
static FILA2 filaBloqueadoSuspenso;

//thread que est� na CPU
static TCB_t* executing_thread = NULL;

//contexto para liberar mem�ria depois que thread executar
ucontext_t context_to_finish_thread;

/**
getContextToFinishProcess
    devolve um contexto qu9e vai estar atrelado a uma fun��o que termina a execu��o da thread
Par�metros:
    void
Retorno:
    contexto da fun��o de terminar thread
**/
ucontext_t* getContextToFinishProcess() {
    return &context_to_finish_thread;
}


/**
getExecutingThread
    devolve a thread em execu��o
Par�metros:
    void
Retorno:
    contexto da fun��o de terminar thread
**/
TCB_t* getExecutingThread() {
    return executing_thread;
}


/**
finishThread
    termina a thread e libera a mem�ria que ela ocupava
Par�metros:
    TCB_t* thread, a thread a ser liberada da mem�ria
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
void finishThread(){
    if(executing_thread != NULL){
        free(executing_thread);
        executing_thread = NULL;

        dispatch();
    }
}


/**
schedulerInitialize
    Inicializa todas filas
Par�metros:
    void
Retorno:
    void
**/
void schedulerInitialize(void){
    if(!isInitialized){
    //cria contexto para a fun��o que termina threads
    getcontext(&context_to_finish_thread);
    context_to_finish_thread.uc_link = NULL; //uc_link � pra onde a thread vai depois da execu��o
    context_to_finish_thread.uc_stack.ss_sp = malloc(STACKMEM);
    context_to_finish_thread.uc_stack.ss_size = STACKMEM;
    context_to_finish_thread.uc_stack.ss_flags = 0;
    makecontext(&context_to_finish_thread, (void *)&finishThread, 0);

    //declara o tcb da thread da main
    TCB_t* main_function = (TCB_t*) malloc(sizeof(TCB_t));
    main_function->tid = 0;
    main_function->state = PROCST_EXEC;
    main_function->prio = 0;
    main_function->context.uc_link = getContextToFinishProcess();

    executing_thread = main_function;

    CreateFila2(&filaApto);
    CreateFila2(&filaBloqueado);
    CreateFila2(&filaBloqueadoSuspenso);
    CreateFila2(&filaAptoSuspenso);
    isInitialized = true;
    }
}


/**
getNewTid
    Retorna um novo thread id
Par�metros:
    void
Retorno:
    novo n�mero tid
**/
int getNewTid(void){
    return tid_id++;
}


/**
(Precisamos de uma fun��o pra cada fila que inserirmos porque no escopo est�tico n�o � poss�vel chamar de outro arquivo)
putInReadyQueue
    Bota a thread na fila especificada
Par�metros:
    PFILA2 queue, lista na qual thread ser� inserida
    TCB_t *thread, thread que ser� inserida na lista
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int putInReadyQueue (TCB_t* thread){
    struct sFilaNode2* nodo = (struct sFilaNode2*) malloc(sizeof(struct sFilaNode2));
    nodo->node = thread;

    int retorno = AppendFila2(&filaApto, nodo);
    return retorno;
}


/**
(Precisamos de uma fun��o pra cada fila que inserirmos porque no escopo est�tico n�o � poss�vel chamar de outro arquivo)
putInBlockedQueue
    Bota a thread na fila especificada
Par�metros:
    PFILA2 queue, lista na qual thread ser� inserida
    TCB_t *thread, thread que ser� inserida na lista
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int putInBlockedQueue (TCB_t* thread){
    struct sFilaNode2* nodo = (struct sFilaNode2*) malloc(sizeof(struct sFilaNode2));
    nodo->node = thread;

    int retorno = AppendFila2(&filaBloqueado, nodo);
    return retorno;
}


/**
getTidFromReadyQueue
    Pega o tid da fila de aptos
Par�metros:
    int tid, tid a ser procurada
Retorno:
    Se correto:         devolve TCB_t* da thread
    Se erro ocorreu:    devolve NULL
**/

//TODO LIMPAR ESSA FUN��O ASAP!
TCB_t* getTidFromReadyQueue(int tid){
    FirstFila2(&filaApto);

    struct sFilaNode2* node = (struct sFilaNode2*) malloc(sizeof(struct sFilaNode2));
    TCB_t* threadBeingSearched = NULL;

    node = GetAtIteratorFila2(&filaApto);
    threadBeingSearched = node->node;

    while(threadBeingSearched != NULL && threadBeingSearched->tid != tid){
        if(NextFila2(&filaApto) != -NXTFILA_ENDQUEUE){
            node = GetAtIteratorFila2(&filaApto);
            threadBeingSearched = node->node;
        }
        else
            threadBeingSearched = NULL;
    }
    free(node);
    return threadBeingSearched;
}


/**
dispatch
    Usa o escalonador para pegar a pr�xima tarefa no estado apto
Par�metros:
    void
Retorno:
    void
**/
void dispatch(void){
    if(executing_thread == NULL){
        FirstFila2(&filaApto);
        TCB_t* up_next_thread = GetAtIteratorFila2(&filaApto);

        if(up_next_thread != NULL){
            up_next_thread->state = PROCST_EXEC;
            executing_thread = up_next_thread;

            DeleteAtIteratorFila2(&filaApto);

            //TODO ERRO TA AQUI
            setcontext(&(executing_thread->context));
        }
    }
}


/**
blockExecutingThread
    Bota no estado bloqueado a thread que est� rodando
Par�metros:
    void
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int blockExecutingThread(void){
    int status = -1;
    if(executing_thread != NULL){
        executing_thread->state = PROCST_BLOQ;
        status = putInBlockedQueue(executing_thread);

        executing_thread = NULL;
    }
    return status;
}



//TODO l�gica de sem�foros
