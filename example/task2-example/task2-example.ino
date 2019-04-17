#include <task2.h>

bool state = true;
uint8_t t2;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  bitSet(PORTB, 5);  // set LED_BUILTIN to HIGH
  init_scheduler();
  create_task(2000, 0, task1);
  t2 = create_task(1000, 500, task2);
  create_task(0, 0, oneshot_task);
}

void task1() {
  static volatile int cnt1 = 0;
  Serial.println("task1");
  if (cnt1 == 10) resume_task(t2);  // resume task2 after 20 seconds
  cnt1++;
}

void task2() {
  static volatile int cnt2 = 0;
  Serial.println("task2");
  bitSet(PINB, 5);                   // toogle the LED_BUILTIN
  if (cnt2 == 10) suspend_task(t2);  // suspend task2 after 10 second
  cnt2++;
}

void oneshot_task() { Serial.println("task3"); }

// call idle() to enter power save mode
void loop() { idle(); }
