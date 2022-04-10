#include <ps/ps.h>
#include <rtl/rtl.h>
#include <kd/com.h>

#include <io/io.h>


static INT iTaskCount = 0;

extern VOID
pop_regs(
	struct InterruptRegisters *regs
);

VOID
PsScheduleThreads(
	struct InterruptRegisters *regs
)
{
	if ( iTaskCount == 0 )
		return;

	INT iNext = 0;
	if ( iTaskCount > 1 )
	{
		for ( INT i = 0; i < MAX_TASKS; i++ )
		{
			if ( tasks[ i ].status == TASK_NO_INIT && i == MAX_TASKS - 1 )
				DbgPrintFmt( "[SCHED] Cannot make new threads, halting!" );
			else if ( tasks[ i ].status == TASK_NO_INIT )
				continue;
			else if ( tasks[ i ].status == TASK_RUNNING )
			{
				tasks[ i ].status = TASK_STOPPED;
				RtlCopyMemory( ( VOID * )&tasks[ i ].regs, regs, sizeof( struct InterruptRegisters ) ); 

				for ( INT j = i + 1; j < MAX_TASKS; j++ )
				{
					if ( tasks[ j ].status == TASK_STOPPED )
					{
						iNext = j;
						goto end;
					}
					else if ( j == MAX_TASKS - 1 )
					{
						for ( INT k = 0; k < i + 1; k++ )
						{
							if ( tasks[ k ].status == TASK_STOPPED )
							{
								iNext = k;
								goto end;
							}
							else if ( k == i )
								 DbgPrintFmt( "No available tasks to switch to [k]\n" );
                                
						}
					}
				}
			}
		}
	}
end:
	if ( tasks[ iNext ].status != TASK_RUNNING )
	{
		tasks[ iNext ].status = TASK_RUNNING;
        IoOutputByte( PIC1, PIC_EOI );
		pop_regs( &tasks[ iNext ].regs );
	}
}

VOID PsLoadTask(
	KSYSTEM_TASK *task
)
{
	for ( INT i = 0; i < MAX_TASKS; i++ )
	{
		if ( tasks[ i ].status == TASK_NO_INIT )
		{
			RtlCopyMemory( &tasks[ i ], task, sizeof( KSYSTEM_TASK ) );
			tasks[ i ].status = TASK_STOPPED;
			iTaskCount++;
			DbgPrintFmt( "[SCHD] task loaded into the queue\n" );
			break;
		}
		else if ( i == MAX_TASKS - 1 )
			DbgPrintFmt( "Cannot load a new task\n" );
	}
}

VOID
PsThreadsInit(

)
{
	tasks = ( KSYSTEM_TASK * )malloc( sizeof( KSYSTEM_TASK ) * MAX_TASKS );
	for ( INT i = 0; i < MAX_TASKS; i++ )
		tasks[ i ].status = TASK_NO_INIT;
	DbgPrintFmt( "[SCHD] tasks queue initialized\n" );
}