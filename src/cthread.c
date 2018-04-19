#include "../include/support.h"
#include "../include/scheduler.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define STACKMEM 8192 // fica no header e aqui tambem?
#define SIZEIDENTIFY 67

/******************************************************************************
Par�metros:
	name:	ponteiro para uma �rea de mem�ria onde deve ser escrito um string que cont�m os nomes dos componentes do grupo e seus n�meros de cart�o.
		Deve ser uma linha por componente.
	size:	quantidade m�xima de caracteres que podem ser copiados para o string de identifica��o dos componentes do grupo.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cidentify (char *name, int size){
    int index = 0;

    if(name == NULL || size > SIZEIDENTIFY)
        return -1;

    for (; index < size; index++){
        putchar(name[index]);
    }
    return 0;
};

/******************************************************************************
Par�metros:
	start:	ponteiro para a fun��o que a thread executar�.
	arg:	um par�metro que pode ser passado para a thread na sua cria��o.
	prio:	N�O utilizado neste semestre, deve ser sempre zero.
Retorno:
	Se correto => Valor positivo, que representa o identificador da thread criada
	Se erro	   => Valor negativo.
******************************************************************************/
int ccreate (void* (*start)(void*), void *arg, int prio){
     schedulerInitialize();

    TCB_t *new_thread = (TCB_t *) malloc(sizeof(TCB_t));

    new_thread->tid = getNewTid(); //vamos usar a main como 0, a� vai facilitar no cjoin
    new_thread->state = PROCST_APTO;
    new_thread->prio = 0;
    new_thread->joinedWaitingToFinish = -1;
    new_thread->joinedBeingWaitBy = -1;
    new_thread->isSuspended = false;

    getcontext(&(new_thread->context));
    new_thread->context.uc_link = getContextToFinishProcess(); //como gerenciar os contextos sem ter certeza de que vai ter outro?
    new_thread->context.uc_stack.ss_sp = malloc(STACKMEM); //como liberar essa mem�ria depois? Colocar um processo s� pra garbage colection?
    new_thread->context.uc_stack.ss_size = STACKMEM;
    new_thread->context.uc_stack.ss_flags = 0;
    makecontext(&(new_thread->context), (void (*)(void)) start, 1, arg);

    int status = includeInReadyList(new_thread);

    if(status < 0)
        return status;
    else
        return new_thread->tid;
};


/******************************************************************************
Par�metros:
	Sem par�metros
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cyield(void){
    //TODO implementa��o
    return 0;
};


/******************************************************************************
Par�metros:
	tid:	identificador da thread cujo t�rmino est� sendo aguardado.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cjoin(int tid){
    TCB_t* joined_thread = getTidFromReadyList(tid);
    int status = 0;//refactor
    //pra evitar que mais de uma fun��o se joine
    if(joined_thread != NULL && joined_thread->joinedBeingWaitBy < 0){
        TCB_t *executing_thread = getExecutingThread();
        executing_thread->joinedWaitingToFinish = tid;
        joined_thread->joinedBeingWaitBy = executing_thread->tid;

        getcontext(&(executing_thread->context));
        //quando a main voltar pra c� ela n�o vai ser bloqueada novamente, com esse caso
        if(executing_thread->joinedBeingWaitBy < 0){
            status =  blockExecutingThread();
            dispatch();
            if(status < 0)
                return -1;
        }

        return 0;
    }


    return -1;
};

/******************************************************************************
Par�metros:
	tid:	identificador da thread a ser suspensa.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int csuspend(int tid){
    //TODO implementa��o
    return 0;
};

/******************************************************************************
Par�metros:
	tid:	identificador da thread que ter� sua execu��o retomada.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cresume(int tid){
    //TODO implementa��o
    return 0;
};

/******************************************************************************
Par�metros:
	sem:	ponteiro para uma vari�vel do tipo csem_t. Aponta para uma estrutura de dados que representa a vari�vel sem�foro.
	count: valor a ser usado na inicializa��o do sem�foro. Representa a quantidade de recursos controlados pelo sem�foro.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int csem_init(csem_t *sem, int count){
    //TODO implementa��o
    return 0;
};


/******************************************************************************
Par�metros:
	sem:	ponteiro para uma vari�vel do tipo sem�foro.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cwait(csem_t *sem){
    //TODO implementa��o
    return 0;
};


/******************************************************************************
Par�metros:
	sem:	ponteiro para uma vari�vel do tipo sem�foro.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int csignal(csem_t *sem){
    //TODO implementa��o
    return 0;
};

int test_me(void){
    printf("OLAR PESSOA, VOCE DEVE MEXER NO PROJETO DA LIB TRAB1SISOP2018-1 // QUANDO QUISER MEXER ALGO PRECISA DAR REBUILD!");
    return 0;
}
