#pragma once
#include <ntdef.h>
#include <hal/idt.h>

#define MAX_TASKS 64

typedef struct TASK_STRUCT
{
	int pid;
	int status;

	struct InterruptRegisters regs;
	UCHAR flags;
} KSYSTEM_TASK;


//scheduler.c
//////

VOID
PsThreadsInit(

);

VOID PsLoadTask(
	KSYSTEM_TASK *task
);

VOID
PsScheduleThreads(
	struct InterruptRegisters *regs
);



//task.c
////

#define TASK_STOPPED 1
#define TASK_RUNNING 2
#define TASK_NO_INIT 3

#define STACK_SIZE 0x10000

VOID
PspCreateThread(
	LPTHREAD_START_ROUTINE lpThread
);

VOID
PspTerminateThread(
	LPTHREAD_START_ROUTINE routine
);

KSYSTEM_TASK *tasks;