/**
 * @file    sys_scheduler.h
 * @author  marcin.swistun@gmail.com
 * @version V1.0.0
 * @date    26/09/2020
 * @brief   This file contains scheduler interface and types.
 *
*/

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdint.h>

/**
 * Scheduler task pointer.
 * Task returns ticks to next run (0 if finished).
 * Argument is a pointer to task context, interpretted individually in each task.
 */
typedef uint32_t(*SchedulerTask_t)(void*);

/**
 * Task execution modes
 */
typedef enum
{
	SCHEDULE_MODE_ONCE,                 //!< Single execution
	SCHEDULE_MODE_FIXED_RATE,           //!< Periodically repeated execution
	SCHEDULE_MODE_MULTI_SCHEDULE,       //!< Allows multiply schedule of the same task in single execution mode
} TaskExecutionModeEnum_t;

/**
 * Scheduler task order.
 */
typedef struct
{
	SchedulerTask_t execute; 			//!< Task pointer
	uint32_t rate;					//!< Period (in fixed mode) or delay (in once or multi mode)
	TaskExecutionModeEnum_t mode;	//!< Execution mode.
	void* context;					//!< Task context data pointer
} TaskOrder_t;

/**
 * Scheduler internal structure with task order and some state data
 */
typedef struct
{
	TaskOrder_t order;
	uint32_t config;						//!< Task config
	int64_t current_execution_beginning;	//!< Current execution begin sys_timestamp
	int64_t next_execution_time;			//!< Next execution begin sys_timestamp
} SchedulerEntry_t;

#define SCHEDULER_SIZE (16)

extern void scheduler_changed_notify(void);

void scheduler_init(void);
int32_t schedule(SchedulerTask_t task, TaskExecutionModeEnum_t execution_mode, uint32_t rate, void* context);
int32_t cancel_schedule(SchedulerTask_t task);

#endif // #ifndef _SCHEDULER_H_
