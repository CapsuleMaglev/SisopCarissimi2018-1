#include "../include/support.h"
#include "../include/cdata.h"
#include <ucontext.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define SUCCESS 0;
#define ERROR -1;

/**
getContextToFinishProcess
    devolve um contexto que vai estar atrelado a uma fun��o que termina a execu��o da thread
Par�metros:
    void
Retorno:
    contexto da fun��o de terminar thread
**/
ucontext_t* getContextToFinishProcess();


/**
getExecutingThread
    devolve a thread em execu��o
Par�metros:
    void
Retorno:
    contexto da fun��o de terminar thread
**/
TCB_t* getExecutingThread();


/**
finishThread
    termina a thread e libera a mem�ria que ela ocupava
Par�metros:
    TCB_t* thread, a thread a ser liberada da mem�ria
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
void finishThread();


/**
schedulerInitialize
    Inicializa todas filas
Par�metros:
    void
Retorno:
    void
**/
void schedulerInitialize(void);


/**
getNewTid
    Retorna um novo thread id
Par�metros:
    void
Retorno:
    Se correto:         n�mero da tid
**/
int getNewTid(void);


/**
(Precisamos de uma fun��o pra cada fila que inserirmos porque no escopo est�tico n�o � poss�vel chamar de outro arquivo)
putInReadyList
    Bota a thread na fila especificada
Par�metros:
    PFILA2 List, lista na qual thread ser� inserida
    TCB_t *thread, thread que ser� inserida na lista
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
//static int putInList (PFILA2 pointer_list, TCB_t* thread);

int includeInReadyList (TCB_t* thread);
int includeInBlockedList (TCB_t* thread);

/**
getTidFromReadyList
    Pega o tid da fila de aptos
Par�metros:
    int tid, tid a ser procurada
Retorno:
    Se correto:         devolve TCB_t* da thread
    Se erro ocorreu:    devolve NULL
**/
TCB_t* getTidFromReadyList(int tid);
/**


getTidFromBlockedList
    Pega o tid da fila de bloqueados
Par�metros:
    int tid, tid a ser procurada
Retorno:
    Se correto:         devolve TCB_t* da thread
    Se erro ocorreu:    devolve NULL
**/
TCB_t* getTidFromBlockedList(int tid);


/**
dispatch
    Usa o escalonador para pegar a pr�xima tarefa no estado apto
Par�metros:
    void
Retorno:
    void
**/
void dispatch(void);


/**
blockExecutingThread
    Bota no estado bloqueado a thread que est� rodando
Par�metros:
    void
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int blockExecutingThread(void);
/**


unblockThread
    Tira no estado bloqueado a thread especificada
Par�metros:
    void
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int unblockThread(int tid);


/**
yieldExecutingThread
    Tira voluntariamente de execu��o thread que est� rodando
Par�metros:
    void
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int yieldExecutingThread(void);


/**
suspendThread
    Suspende a thread especificada
Par�metros:
    int tid -> thread id que vai ser suspensa
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int suspendThread(int tid);


/**
resumeThread
    Resume a thread especificada para apto ou bloqueado
Par�metros:
    int tid -> thread id que vai ser resumida
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int resumeThread(int tid);
