#include "schedular.h"
#include <string.h>

static sTask SCH_tasks_G[SCH_MAX_TASKS];
static uint32_t newTaskID = 0;

static inline uint32_t SCH_Lock(void) {
  uint32_t primask = __get_PRIMASK();
  __disable_irq();
  return primask;
}

static inline void SCH_Unlock(uint32_t primask) { __set_PRIMASK(primask); }

static uint32_t Get_New_Task_ID(void) {
  newTaskID++;
  if (newTaskID == NO_TASK_ID) {
    newTaskID++;
  }
  return newTaskID;
}

void SCH_Init(void) {
  uint32_t m = SCH_Lock();
  memset(SCH_tasks_G, 0, sizeof(SCH_tasks_G));
  newTaskID = 0;
  SCH_Unlock(m);
}

void SCH_Update(void) {
  for (uint8_t i = 0; i < SCH_MAX_TASKS; i++) {
    sTask *t = &SCH_tasks_G[i];
    if (t->pTask == 0) {
      continue;
    }
    if (t->Delay > 0) {
      t->Delay--;
      if (t->Delay == 0) {
        t->RunMe++;
        t->Delay = t->Period;
      }
    }
  }
}

void SCH_Dispatch_Tasks(void) {
  for (uint8_t i = 0; i < SCH_MAX_TASKS; i++) {
    if (SCH_tasks_G[i].RunMe == 0) {
      continue;
    }
    uint32_t m = SCH_Lock();
    SCH_tasks_G[i].RunMe--;
    void (*f)(void) = SCH_tasks_G[i].pTask;
    uint32_t id = SCH_tasks_G[i].TaskID;
    uint32_t period = SCH_tasks_G[i].Period;
    SCH_Unlock(m);

    f();

    if (period == 0) {
      SCH_Delete_Task(id);
    }
  }
}

uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY,
                      uint32_t PERIOD) {
  if (pFunction == 0) {
    return NO_TASK_ID;
  }
  uint8_t i = 0;
  uint32_t m = SCH_Lock();
  while (i < SCH_MAX_TASKS && SCH_tasks_G[i].pTask != 0) {
    i++;
  }
  if (i == SCH_MAX_TASKS) {
    SCH_Unlock(m);
    return NO_TASK_ID;
  }
  sTask *t = &SCH_tasks_G[i];
  t->pTask = pFunction;
  t->Delay = DELAY;
  t->Period = PERIOD;
  t->RunMe = 0;
  t->TaskID = Get_New_Task_ID();
  if (DELAY == 0) {
    t->RunMe = 1;
    t->Delay = PERIOD;
  }
  SCH_Unlock(m);
  return t->TaskID;
}

uint8_t SCH_Delete_Task(uint32_t taskID) {
  if (taskID == NO_TASK_ID) {
    return 0;
  }
  uint32_t m = SCH_Lock();
  for (uint8_t i = 0; i < SCH_MAX_TASKS; i++) {
    if (SCH_tasks_G[i].TaskID == taskID && SCH_tasks_G[i].pTask != 0) {
      memset(&SCH_tasks_G[i], 0, sizeof(sTask));
      SCH_Unlock(m);
      return 1;
    }
  }
  SCH_Unlock(m);
  return 0;
}
