#include "../include/support.h"
#include "../include/scheduler.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define SIZEIDENTIFY 67
#define GROUPNAMESTRING "Guilherme Cattani 243589\nAugusto Timm 113887\nGabriel Warken 179787\n"

/******************************************************************************
Par�metros:
	name:	ponteiro para uma �rea de mem�ria onde deve ser escrito um string que cont�m os nomes dos componentes do grupo e seus n�meros de cart�o.
		Deve ser uma linha por componente.
	size:	quantidade m�xima de caracteres que podem ser copiados para o string de identifica��o dos componentes do grupo.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cidentify (char *name, int size) {
    if(size < SIZEIDENTIFY) {
        return ERROR;
    }

    int index = 0;
    while ((name[index] = GROUPNAMESTRING[index]) != '\0') {
        index++;
    }

    return SUCCESS;
}

/******************************************************************************
Par�metros:
	start:	ponteiro para a fun��o que a thread executar�.
	arg:	um par�metro que pode ser passado para a thread na sua cria��o.
	prio:	N�O utilizado neste semestre, deve ser sempre zero.
Retorno:
	Se correto => Valor positivo, que representa o identificador da thread criada
	Se erro	   => Valor negativo.
******************************************************************************/
int ccreate (void* (*start)(void*), void *arg, int prio) {
     schedulerInitialize();

    TCB_t *newThread = (TCB_t *) malloc(sizeof(TCB_t));

    newThread->tid = getNewTid(); //vamos usar a main como 0, a� vai facilitar no cjoin
    newThread->state = PROCST_APTO;
    newThread->prio = 0;
    newThread->joinedWaitingToFinish = -1;
    newThread->joinedBeingWaitBy = -1;
    newThread->wasJustScheduled = false;

    getcontext(&(newThread->context));
    newThread->context.uc_link = getContextToFinishProcess();
    newThread->context.uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
    newThread->context.uc_stack.ss_size = SIGSTKSZ;
    newThread->context.uc_stack.ss_flags = 0;
    makecontext(&(newThread->context), (void (*)(void)) start, 1, arg);

    int status = includeInReadyList(newThread);

    if(status < 0) {
        return ERROR;
    }
    else {
        return newThread->tid;
    }
}


/******************************************************************************
Par�metros:
	Sem par�metros
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cyield(void) {
    TCB_t *executingThread = getExecutingThread();
    if(executingThread == NULL) {
        return ERROR;
    }

    executingThread->wasJustScheduled = false;
    getcontext(&(executingThread->context));

    if(executingThread->wasJustScheduled == false) {
        int status = yieldExecutingThread();
        if(status < 0) {
            return ERROR;
        }

        dispatch();
    }

    return SUCCESS;
}


/******************************************************************************
Par�metros:
	tid:	identificador da thread cujo t�rmino est� sendo aguardado.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cjoin(int tid) {
    TCB_t* joinedThread = getTidFromReadyList(tid);
    int status = 0;
    //pra evitar que mais de uma fun��o se joine
    if(joinedThread != NULL && joinedThread->joinedBeingWaitBy < 0) {
        TCB_t *executingThread = getExecutingThread();
        executingThread->joinedWaitingToFinish = tid;
        joinedThread->joinedBeingWaitBy = executingThread->tid;

        getcontext(&(executingThread->context));
        //quando a thread que chamar voltar pra c� ela n�o vai ser bloqueada novamente, com esse caso
        //podemos usar o wasJustScheduled tamb�m
        if(executingThread->joinedWaitingToFinish >= 0) {
            status =  blockExecutingThread();
            if(status < 0) {
                return ERROR;
            }

            dispatch();
        }

        return SUCCESS;
    }

    return ERROR;
}

/******************************************************************************
Par�metros:
	tid:	identificador da thread a ser suspensa.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int csuspend(int tid) {
    TCB_t *executingThread = getExecutingThread();
    if(executingThread->tid != tid) {
        return suspendThread(tid);
    }

    return ERROR;
}

/******************************************************************************
Par�metros:
	tid:	identificador da thread que ter� sua execu��o retomada.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cresume(int tid) {
    return resumeThread(tid);
}

/******************************************************************************
Par�metros:
	sem:	ponteiro para uma vari�vel do tipo
_t. Aponta para uma estrutura de dados que representa a vari�vel sem�foro.
	count: valor a ser usado na inicializa��o do sem�foro. Representa a quantidade de recursos controlados pelo sem�foro.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int csem_init(csem_t *sem, int count) {
    sem->count = count;
    sem->fila = (FILA2*) malloc(sizeof(FILA2));
    int status = CreateFila2(sem->fila);

    if(status < 0) {
        return ERROR;
    }
    else {
        return SUCCESS;
    }
}


/******************************************************************************
Par�metros:
	sem:	ponteiro para uma vari�vel do tipo sem�foro.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int cwait(csem_t *sem) {
    int status  = 0;
    if(sem == NULL) {
        return -1;
    }

    TCB_t *executingThread = getExecutingThread();
    getcontext(&(executingThread->context));

    if(sem->count <= 0) { //recurso est� ocupado
        AppendFila2(sem->fila, executingThread);
        status =  blockExecutingThread();
        if(status < 0) {
            return ERROR;
        }

        dispatch();
    }
    sem->count -=1;

    if(status < 0) {
        return ERROR;
    }
    else {
        return SUCCESS;
    }
}


/******************************************************************************
Par�metros:
	sem:	ponteiro para uma vari�vel do tipo sem�foro.
Retorno:
	Se correto => 0 (zero)
	Se erro	   => Valor negativo.
******************************************************************************/
int csignal(csem_t *sem) {
    if(sem == NULL) {
        return ERROR;
    }

    sem->count +=1;
    FirstFila2(sem->fila);
    TCB_t *thread = GetAtIteratorFila2(sem->fila);
    if (thread == NULL) {
        return SUCCESS;
    }

    DeleteAtIteratorFila2(sem->fila);
    int status = unblockThread(thread->tid);

    if(status < 0) {
        return ERROR;
    }
    else {
        return SUCCESS;
    }
}
