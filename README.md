# Implementing EDF Scheduler on FreeRTOS

This repository contains the implementation of the Earliest Deadline First (EDF) scheduling algorithm on FreeRTOS. EDF is a dynamic priority-based preemptive scheduling policy where a task's priority can change during its execution. This implementation is based on the detailed research provided in the thesis titled "Implementation and Test of EDF and LLREFSchedulers in FreeRTOS".

## Table of Contents

- [Introduction](#introduction)
- [Project Requirements](#project-requirements)
- [File Structure](#file-structure)
- [Usage](#usage)
- [Verification and Results](#verification-and-results)
- [Suggestions for Enhancement](#suggestions-for-enhancement)
- [Acknowledgment](#acknowledgment)
- [Contributions](#contributions)
---

![Screenshot 2023-08-12 093244](https://github.com/t0ti20/Implementing_EDF_Scheduler/assets/61616031/0185c2ae-bae6-466b-8179-f8a37876917b)

## Introduction

Earliest Deadline First (EDF) adopts a dynamic priority-based preemptive scheduling policy. This means that the priority of a task can change during its execution. If there is a request for a higher-priority task, the current task's processing will be interrupted.

## Project Requirements

1. Design and implement the EDF scheduler based on FreeRTOS.
2. Read and implement changes mentioned in the thesis: "Implementation and Test of EDF and LLREFSchedulers in FreeRTOS".
3. Implement additional changes not mentioned in the thesis for the EDF scheduler.
4. Create four tasks and two additional tasks to test the EDF scheduler.
5. Verify the system implementation.

Detailed requirements and instructions are provided in the `instructions.txt` file.

## File Structure

- `main.c` - Main source file containing the tasks to test the EDF scheduler.
- `tasks.c` - Modified FreeRTOS tasks source file implementing the EDF scheduler and additional changes.
- `freertosconfig.h` - Configuration file for FreeRTOS.

## Usage

1. Read the relevant chapters from the thesis to understand the modifications made in `tasks.c`.
2. Run the Keil simulator to view the execution of the tasks with the EDF scheduler.
3. Monitor tasks using GPIOs and a logic analyzer for real-time verification.

## Verification and Results

The system implementation with the EDF scheduler has been verified using:
- Analytical methods to calculate system hyperperiod, CPU load, and system schedulability.
- The Simso offline simulator.
- Keil simulator in runtime.

Results of the verification methods, along with screenshots and comments, can be found in the - [Report](https://github.com/t0ti20/Implementing_EDF_Scheduler/blob/master/4-Verifying%20The%20System%20Implementation/Schedulability%20Calculation.pdf).

![Screenshot 2023-08-12 093244](https://github.com/t0ti20/Implementing_EDF_Scheduler/assets/61616031/03195b42-b7d8-4ce6-be3e-f3e505428e86)


## Suggestions for Enhancement

- Modify the `uxTaskGetSystemState` function to read the task status from the EDF ready list.
- Use FreeRTOS run-time stats function `uxTaskGetSystemState` to print the stats summary of all tasks during system verification.

## Acknowledgment

We thank the authors of the thesis "Implementation and Test of EDF In FWD Team and LLREFSchedulers in FreeRTOS" for providing the foundational research for this project.

## Contributions

Contributions and feedback are always welcome. Kindly open an issue or submit a pull request.

