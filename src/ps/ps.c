#include <ps/ps.h>
#include <mm/mm.h>

INT iPidCount = 0;

VOID
PspCreateThread(
	LPTHREAD_START_ROUTINE lpThread
)
{
	KSYSTEM_TASK *Task = ( KSYSTEM_TASK * )malloc( sizeof( KSYSTEM_TASK ) );
	Task->flags = 0b1000000010; //krnl

	Task->pid = iPidCount++;
	Task->regs.rip = lpThread;
	Task->regs.rflags = ( 1 << 9 ) | ( 1 << 1 );

	Task->regs.cs = 0x28;
	Task->regs.ss = 0x30;

	Task->regs.rsp = ( UINT )malloc( STACK_SIZE ) + STACK_SIZE;

	PsLoadTask( Task );
}

VOID
PspTerminateThread(
	LPTHREAD_START_ROUTINE routine
)
{
	for ( INT i = 0; i < MAX_TASKS; i++ )
		if(tasks[i].regs.rip == routine)
		{
			tasks[i].status = TASK_NO_INIT;
			RtlZeroMemory( ( VOID * )&tasks[ i ].regs, sizeof( struct InterruptRegisters ) ); 
		}
			
}