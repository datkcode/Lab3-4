---
name: stm32-baremetal-fsm-coder-v2
description: Generate bare-metal STM32 C code for Finite State Machines (FSM) without FreeRTOS or RTOS using a strict 1-to-1 enum-to-switch-case architecture where each case contains state hardware control, 1-to-1 outgoing-arrow transition if blocks, and next-state environment preparation. Use whenever the user asks to write, implement, refactor, or design an STM32 or embedded C finite state machine (may trang thai huu han, FSM) without RTOS.
---

# STM32 Bare-Metal FSM Coder

Write deterministic, non-blocking C code for Finite State Machines (FSM) on STM32 microcontrollers without an RTOS. Every state diagram translates mechanically 1-to-1 into a single `switch-case` structure where each `case` executes the current state's hardware control logic followed by one `if` statement per outgoing transition arrow.

## When to Use

- The user requests STM32 or embedded C firmware modeled as a Finite State Machine (FSM) without FreeRTOS or any RTOS.
- The user wants code generated directly from a state transition diagram with deterministic 1-to-1 state-to-case and outgoing-arrow-to-if mapping.
- The user asks to refactor messy flag-based `if-else` superloop logic into a clean switch-case state machine.

## Core Architectural Rules

1. **Include an explicit `STATE_INIT` entry state:** Always define a `STATE_INIT` (or `<PREFIX>_INIT`) state as the initial value of the state variable. Use `STATE_INIT` to prepare the environment (such as starting timers or resetting counters) for the first operational state and transition immediately, eliminating external initialization `if` checks.
2. **Strict 1-to-1 `enum` to `case` mapping:** Every state node in the diagram must correspond to exactly one `enum` member and one `case` inside a single `switch (current_state)` block. Always include a `default` branch that resets `current_state` to `STATE_INIT` for fault recovery.
3. **Zero `if` statements outside `switch-case`:** Never place `if` checks outside the `switch` statement inside the FSM runner function. Guarding transitions inside their owning `case` ensures events are evaluated only in the active state context.
4. **Two-part `case` structure (Hardware Control + Outgoing Transitions):** Inside each operational `case`, structure the body in two clear sections:
   - **State Hardware Control:** Execute the hardware outputs and control actions belonging to the current state (GPIO outputs, 7-segment display updates, PWM/peripheral control).
   - **Outgoing Transition `if` Blocks (1 `if` per outgoing arrow):** Write one `if (condition)` block for every outgoing transition arrow leaving that state on the diagram. If a state has N outgoing arrows, its `case` must have N corresponding `if` blocks.
5. **Next-state environment preparation on transition:** Inside each transition `if` block, prepare the environment and timing resources required by the destination state (`setTimer(id, duration_ms)`, counter resets, flag clears) immediately *before* reassigning `current_state = NEXT_STATE;`.
6. **Non-blocking execution only:** Never use `HAL_Delay()` or busy-wait loops inside an FSM. Use non-blocking software timers (`setTimer(id, duration_ms)` and `isTimerExpired(id)`) or interrupt-updated event flags.

## Implementation Workflow

1. **Map states and outgoing arrows:** Count all state nodes (plus `INIT`) and list every outgoing transition arrow for each state from the user's specification or diagram.
2. **Declare state `enum` and static state variable:** Create `typedef enum` starting with `INIT` and initialize `static <StateType> current_state = <PREFIX>_INIT;`.
3. **Implement modular `.h` and `.c` files:** Expose `<fsm_name>_run(void)` in the header file so `while (1)` in `main.c` simply calls the FSM runner alongside any decoupled FSMs.
4. **Populate each `case` systematically:**
   - In `STATE_INIT`, prepare the environment for the first active state and assign `current_state`.
   - In each operational `case`, write the hardware control logic for that state first, followed by one `if` block per outgoing arrow containing next-state environment setup and state reassignment.

## Reference Template

```c
#include "fsm_example.h"
#include "software_timer.h"

typedef enum {
    FSM_INIT,
    FSM_STATE_A,
    FSM_STATE_B
} FsmState_t;

static FsmState_t current_state = FSM_INIT;

void fsm_example_run(void) {
    switch (current_state) {
        case FSM_INIT:
            /* Prepare environment for first operational state (FSM_STATE_A) */
            setTimer(0, 1000);
            current_state = FSM_STATE_A;
            break;

        case FSM_STATE_A:
            /* 1. Hardware control program for FSM_STATE_A */
            HAL_GPIO_WritePin(LED_A_GPIO_Port, LED_A_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET);

            /* 2. Outgoing transitions (number of if blocks = number of outgoing arrows) */
            if (isTimerExpired(0)) {
                /* Prepare environment for FSM_STATE_B */
                setTimer(0, 2000);
                current_state = FSM_STATE_B;
            }
            if (is_reset_pressed()) {
                /* Prepare environment for FSM_INIT */
                current_state = FSM_INIT;
            }
            break;

        case FSM_STATE_B:
            /* 1. Hardware control program for FSM_STATE_B */
            HAL_GPIO_WritePin(LED_A_GPIO_Port, LED_A_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET);

            /* 2. Outgoing transitions (1 outgoing arrow -> 1 if block) */
            if (isTimerExpired(0)) {
                /* Prepare environment for FSM_STATE_A */
                setTimer(0, 1000);
                current_state = FSM_STATE_A;
            }
            break;

        default:
            current_state = FSM_INIT;
            break;
    }
}
```
