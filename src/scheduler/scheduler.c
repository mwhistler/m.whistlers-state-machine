/**
 * @file    scheduler.c
 * @author  marcin.swistun@gmail.com
 * @version V1.0.0
 * @date    23/09/2020
 * @brief   This file contains _simplified_ system scheduler, implemented for state machine research purposes.
 * It is not a real time scheduler so can be used only for real time non relevant features.
 * Scheduler relies on 64 bit system timer which must still increment. Scheduler does not support timer overflow.
 * Main scheduler routine scheduler_core_process may be called every timer tick (it is not optimal but will work)
 * or when the timer reaches its last returned value.
*/

#include <string.h>
#include "../sys_time/sys_time.h"
#include "scheduler.h"

static SchedulerEntry_t scheduled_tasks[SCHEDULER_SIZE];


void scheduler_init(void)
{
	memset(scheduled_tasks, 0, sizeof(scheduled_tasks));
}

static int bind_task(SchedulerEntry_t* sch_entry, TaskOrder_t* task_order)
{
	memcpy(&(sch_entry->order), task_order, sizeof(TaskOrder_t));
	sch_entry->next_execution_time = systime_get() + MS_TO_SYSTICKS(task_order->rate);
	sch_entry->current_execution_beginning = sch_entry->next_execution_time;
	sch_entry->config = 0;
	scheduler_changed_notify();
	return (0);
}

static void free_task(SchedulerEntry_t* sch_entry)
{
	memset(sch_entry, 0, sizeof(SchedulerEntry_t));
}

/**
 * Funkcja sluzy do zlecania zadan schedulerowi.
 * @param task	task pointer
 * @param execution_mode execution mode
 * @param rate task delay (if SCHEDULE_MODE_ONCE or MULTI_SCHEDULE) or period (if FIXED_RATE)
 * @return slot to which task was bind. If there is no free slot returns -1.
 */
int32_t schedule(SchedulerTask_t task, TaskExecutionModeEnum_t execution_mode, uint32_t rate, void* context)
{
	TaskOrder_t new_task = {.execute = task,
							.rate = rate,
							.mode = execution_mode,
							.context = context
	};


	if (new_task.mode != SCHEDULE_MODE_MULTI_SCHEDULE)
	{	// check if task is allready in scheduler and replace with new order
		for (int i = 0; i < SCHEDULER_SIZE; i++)
		{
			if (scheduled_tasks[i].order.execute == task)
			{
				bind_task(&scheduled_tasks[i], &new_task);
				return (i);
			}
		}
	}
	// look for free slot and bind task:
	for (int i = 0; i < SCHEDULER_SIZE; i++)
	{
		if (scheduled_tasks[i].order.execute == 0)
		{
			bind_task(&scheduled_tasks[i], &new_task);
			return (i);
		}
	}
	return (-1);
}

/**
 * Schedule cancelation.
 * @param task poiner to task to be canceled
 * @return slot number if found and canceled, -1 if not
 */
int32_t cancel_schedule(SchedulerTask_t task)
{
	// remove all scheduled instances of this task:
	for (int i = 0; i < SCHEDULER_SIZE; i++)
	{
		if (scheduled_tasks[i].order.execute == task)
		{
			free_task(&scheduled_tasks[i]);
			return (i);
		}
	}
	return (-1);
}

/**
 * Scheduler main task.
 * @return sys timestamp on which at least this routine must be called again to correnctly proceed tasks, -1 if there is no further tasks to proceed.
 */
int64_t scheduler_core_process(void)
{
	int64_t next_wake_sys_time = INT64_MAX;

	int64_t now = (int64_t)systime_get();
	for (int i = 0; i < SCHEDULER_SIZE; i++)
	{
		SchedulerEntry_t* task = &scheduled_tasks[i];
		if (task->order.execute == 0)
		{
			continue;
		}

		if (task->next_execution_time <= now)
		{
			int64_t ticks_to_next_run = MS_TO_SYSTICKS((int64_t)task->order.execute(task->order.context));

			now = (int64_t)systime_get();
			if (ticks_to_next_run == 0)	// task says it is finished
			{
				if (task->order.mode == SCHEDULE_MODE_FIXED_RATE)
				{
					task->next_execution_time = task->current_execution_beginning +  MS_TO_SYSTICKS(task->order.rate);
					task->current_execution_beginning = task->next_execution_time;
					while (task->next_execution_time <= now)
					{	// skipping executions from the past, TODO - what to do with messed executions?
						task->next_execution_time += MS_TO_SYSTICKS(task->order.rate);
						task->current_execution_beginning = task->next_execution_time;
					}
				}
				else
				{
					free_task(task);
					continue;
				}
			}
			else	// task says it is not finished
			{
				task->next_execution_time = ticks_to_next_run + now;
			}
		}

		if (task->next_execution_time < next_wake_sys_time)
		{
			next_wake_sys_time = task->next_execution_time;
		}
	}
	if (next_wake_sys_time == INT64_MAX)
		return (-1); // no further orders
	return ((int64_t)next_wake_sys_time);
}
