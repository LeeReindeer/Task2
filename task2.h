#ifndef __TASK2_H__
#define __TASK2_H__

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// task funcation pointer
typedef void (*TaskFunction_t)();

typedef struct _task {
  TaskFunction_t task;  // the Task
  bool status;          // set false to suspend task
  uint16_t period;      // task run every <period> millseconds
  uint16_t delay;       // delay <delay> millseconds before task start
} Task2;

#include <stddef.h>

#define MAX_TASK 15  // 2^ 4 - 1

#ifdef __cplusplus
extern "C" {
#endif

// task funcation pointer
typedef void (*TaskFunction_t)();

void init_scheduler();

void idle();

/**
 * @brief  Create a task with params, set the task to
 * READY, and add it to the task queue.
 * @param  period: when task in period time
 * @param  delay: delay time before task start
 * @param  priority: when tasks are in same step, run the highest priority task
 * first
 * @param  task_func: the task
 * @retval return the task tid
 */
uint8_t create_task(uint16_t period, uint16_t delay, TaskFunction_t task_func);
/**
 * @brief  Suspend task means task will not be scheduled in following interrupts
 * set the task status to SUSPENDED
 */
void suspend_task(uint8_t tid);
/**
 * @brief  Set task status to READY
 */
void resume_task(uint8_t tid);

/**
 * @brief Delete the task by tid
 * @return return true if task exists and deleted succeed.
 */
bool delete_task(uint8_t tid);

#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // __TASK2_H__