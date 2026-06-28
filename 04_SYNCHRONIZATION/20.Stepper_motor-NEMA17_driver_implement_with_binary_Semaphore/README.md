# Stepper Motor Control using Binary Semaphore (STM32F446RE + FreeRTOS)

## Overview

This project demonstrates the practical implementation of a **FreeRTOS Binary Semaphore** to synchronize a task controlling a **NEMA17 stepper motor** through a **DRV8825 stepper motor driver** using the **STM32F446RE** (ARM Cortex-M4).

Unlike simple LED synchronization examples, this project applies RTOS synchronization to actual hardware control.

---

## Hardware Used

- STM32F446RE Development Board
- NEMA17 Stepper Motor
- DRV8825 Stepper Motor Driver
- 12V Power Supply
- ST-Link V2

---

## Software Used

- STM32CubeIDE
- FreeRTOS
- C Programming

---

## FreeRTOS Concepts Demonstrated

- Task Creation
- Binary Semaphore
- Task Synchronization
- Blocking and Unblocking Tasks
- Hardware Control using RTOS

---

## Firmware Features

- Configures STEP and DIR pins using STM32 GPIO.
- Generates STEP pulses for the DRV8825 driver.
- Uses a Binary Semaphore to synchronize motor execution.
- Motor rotates only after the semaphore is released.
- Demonstrates deterministic hardware control using FreeRTOS.

---

## Motor Specifications

| Parameter | Value |
|-----------|-------|
| Motor | NEMA17 |
| Driver | DRV8825 |
| Step Angle | 1.8° |
| Steps / Revolution | 200 |
| Controller | STM32F446RE |

---

## Learning Outcome

This project demonstrates how FreeRTOS Binary Semaphores can be used for real-world embedded firmware applications by synchronizing hardware operations instead of simple LED tasks.

It also serves as the foundation for more advanced motor-control applications using RTOS.

---

## Repository Structure

```
Core/
Drivers/
Middlewares/
README.md
```

---

## Future Improvements

- Interrupt-based motor triggering
- Queue-based motion commands
- Multiple motor synchronization
- Closed-loop position control using encoders
- RTOS-based motion planner