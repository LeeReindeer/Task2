#include "task2.h"
#include <avr/sleep.h>

static Task2 tasks[MAX_TASK];

/**
 * @brief Configure Timer2 to generate interrupt every 1 ms.
 *
 * @note The internal clock (normally 16 MHz on a Uno),
 * and "pre-scale" it by dividing it by 128. The pre-scaled clock will then
 * "tick" every 8 microseconds (since the clock itself runs with a period of
 * 1/16000000 or 62.5 ns).
 *
 * So we configure Timer 2 to count up to 125 and then generate an interrupt.
 * This interrupt gives us a chance to see if our counting period is up. Since 8
 * µs times 125 gives 1000 µs, that means we get interrupted exactly every 1 ms.
 *
 * And Timer2 has a higher priority than Timer0 and Timer1, so neither the
 * millis() timer, nor the Timer1's counter will take precedene over this
 * interrupt.
 */
static void setup_timer2() {
  // 125 * 62.5ns * 128 = 1ms
  TCNT2 = 0;                       // reset counter
  TCCR2A = bit(WGM21);             // CTC mode
  TCCR2B = bit(CS20) | bit(CS22);  // 128 pre-scale
  OCR2A = 124;                     // count up from 0 to 124
  TIMSK2 = bit(OCIE2A);            // enable Timer2 interrupt
}

void init_scheduler() {
  for (int i = 0; i < MAX_TASK; i++) {
    tasks[i].task = NULL;
  }
  cli();
  setup_timer2();
  sei();
}

void idle() {
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  sleep_mode();
}

uint8_t create_task(uint16_t period, uint16_t delay, TaskFunction_t task_func) {
  uint8_t cur = 0;
  while (tasks[cur].task != NULL && cur < MAX_TASK) cur++;
  if (cur >= MAX_TASK) return -1;
  tasks[cur].period = period;
  tasks[cur].delay = delay;
  tasks[cur].task = task_func;
  tasks[cur].status = true;
}

void suspend_task(uint8_t tid) { tasks[tid].status = false; }

void resume_task(uint8_t tid) { tasks[tid].status = true; }

bool delete_task(uint8_t tid) {
  cli();
  tasks[tid].task = NULL;
  tasks[tid].period = 0;
  tasks[tid].delay = 0;
  tasks[tid].status = 0;
  sei();
}

// inturrpt every 1ms
ISR(TIMER2_COMPA_vect) {
  for (int i = 0; i < MAX_TASK; i++) {
    if (!tasks[i].task) continue;    // no task
    if (!tasks[i].status) continue;  // task suspended
    if (tasks[i].delay == 0) {
      tasks[i].task();  // run the task
      if (tasks[i].period == 0)
        delete_task(i);  // set period to 0 to create oneshot task
      else
        tasks[i].delay = tasks[i].period;  // next time to run
    } else {
      tasks[i].delay--;
    }
  }
}
