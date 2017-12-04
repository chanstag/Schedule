/*
 * student.h
 * Multithreaded OS Simulation for CS 425, Project 3
 * Acknowledgement: The code is authored by Kishore Ramachandran at Gatech. 

 *
 * YOU WILL NOT NEED TO MODIFY THIS FILE
 *
 */

#ifndef __STUDENT_H__
#define __STUDENT_H__

#include "os-sim.h"

/* Function declarations */
extern void idle(unsigned int cpu_id);
extern void preempt(unsigned int cpu_id);
extern void yield(unsigned int cpu_id);
extern void terminate(unsigned int cpu_id);
extern void wake_up(pcb_t *process);

#endif /* __STUDENT_H__ */
