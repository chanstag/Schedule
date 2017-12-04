/*
 * CS425-002 (Fa17) OPERATING SYSTEMS 1
 *Project 3
 *By Antonio Chavez and Chandler Staggs
 *
 *
 *
 *
 * student.c
 * Multithreaded OS Simulation for CS 425, Project 3
 * Acknowledgement: The code is authored by Kishore Ramachandran at Gatech. 
 *
 * This file contains the CPU scheduler for the simulation.  
 
 */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "os-sim.h"


/*
 * current[] is an array of pointers to the currently running processes.
 * There is one array element corresponding to each CPU in the simulation.
 *
 * current[] should be updated by schedule() each time a process is scheduled
 * on a CPU.  Since the current[] array is accessed by multiple threads, you
 * will need to use a mutex to protect it.  current_mutex has been provided
 * for your use.
 */
static pcb_t **current;
static pthread_mutex_t current_mutex;
static pthread_mutex_t readyQueue_mutex;
static pthread_cond_t idle_condition;



//The ready queue will be built from a link of pcb's called head and tail.
//The head is the first element in the queue, and the tail is the final element
//in the queue. More pcb's can be linked into this list later on


pcb_t *head;
pcb_t *tail;




//This is our first attempt at building a linked list

/*
typedef struct Node
{
	pcb_t *info;
//	int info;
	struct Node *next;
}Node;


typedef struct readyQueue
{
//	struct Node *head, *tail;
	pcb_t *head, *tail;
}readyQueue;


readyQueue* newQueue = NULL;


struct Node* newNode( pcb_t *data)
{
	printf("node \n");
	struct Node *temp = (struct Node*)malloc(sizeof(struct Node));
	temp->info = data;
	temp->next = NULL;
	return temp;
}



struct readyQueue* createReadyQueue()
{
	printf("create \n");
	struct readyQueue* newQueue = (struct readyQueue*)malloc(sizeof (struct readyQueue));
	newQueue->head = newQueue->tail = NULL;
	return newQueue;
}
*/


//Push a new process into the ready queue
static void enqueue(pcb_t *data)
{
	if(head == NULL)
	{
		head = tail = data;
		return;
	}
	else
	{
		tail->next = data;
		tail = data;
	}
}


//Remove a process from the ready queue
	pcb_t *dequeue()
{
	if(head == NULL)
	{
		pthread_mutex_unlock(&readyQueue_mutex);
		return NULL;
	
	}
	
	pcb_t* removedProcess = head;	
	head = head->next;
	
	if(head == NULL)
	{
		tail = NULL;
	}
	return removedProcess;		
}



/*
 * schedule() is your CPU scheduler.  It should perform the following tasks:
 *
 *   1. Select and remove a runnable process from your ready queue which 
 *	you will have to implement with a linked list or something of the sort.
 *
 *   2. Set the process state to RUNNING
 *
 *   3. Call context_switch(), to tell the simulator which process to execute
 *      next on the CPU.  If no process is runnable, call context_switch()
 *      with a pointer to NULL to select the idle process.
 *	The current array (see above) is how you access the currently running process indexed by the cpu id. 
 *	See above for full description.
 *	context_switch() is prototyped in os-sim.h. Look there for more information 
 *	about it and its parameters.
 */


static void schedule(unsigned int cpu_id)
{
    /* FIX ME */
	if(head == NULL)
	{
		context_switch(cpu_id, NULL, -1);
	}
	else
	{
		
		pthread_mutex_lock(&readyQueue_mutex);
		pcb_t* newProcess = dequeue();	
		newProcess->state = PROCESS_RUNNING;
		pthread_mutex_lock(&current_mutex);
		current[cpu_id] = newProcess;
		pthread_mutex_unlock(&current_mutex);
		pthread_mutex_unlock(&readyQueue_mutex);
		context_switch(cpu_id, newProcess, -1);
	}

}


/*
 * idle() is your idle process.  It is called by the simulator when the idle
 * process is scheduled.
 *
 * This function should block until a process is added to your ready queue.
 i* It should then call schedule() to select the process to run on the CPU.
 */


extern void idle(unsigned int cpu_id)
{
    /* FIX ME */

	pthread_mutex_lock(&readyQueue_mutex);

	while(head == NULL)
	{
		pthread_cond_wait(&idle_condition, &readyQueue_mutex);
	}
	pthread_mutex_unlock(&readyQueue_mutex);
	schedule(cpu_id);

    /*
     * REMOVE THE LINE BELOW AFTER IMPLEMENTING IDLE()
     *
     * idle() must block when the ready queue is empty, or else the CPU threads
     * will spin in a loop.  Until a ready queue is implemented, we'll put the
     * thread to sleep to keep it from consuming 100% of the CPU time.  Once
so     * you implement a proper idle() function using a condition variable,
     * remove the call to mt_safe_usleep() below.
     */


//    mt_safe_usleep(1000000);
}


/*
 * preempt() is the handler called by the simulator when a process is
 * preempted due to its timeslice expiring.
 *
 * This function should place the currently running process back in the
 * ready queue, and call schedule() to select a new runnable process.
 */
extern void preempt(unsigned int cpu_id)
{
    /* FIX ME */
}


/*
 * yield() is the handler called by the simulator when a process yields the
 * CPU to perform an I/O request.
 *
 * It should mark the process as WAITING, then call schedule() to select
 * a new process for the CPU.
 */
extern void yield(unsigned int cpu_id)
{
    /* FIX ME */
	pthread_mutex_lock(&current_mutex);
	printf("yield\n"); 
   	current[cpu_id]->state = PROCESS_WAITING;
     	pthread_mutex_unlock(&current_mutex);
	schedule(cpu_id);
}


/*
 * terminate() is the handler called by the simulator when a process completes.
 * It should mark the process as terminated, then call schedule() to select
 * a new process for the CPU.
 */
extern void terminate(unsigned int cpu_id)
{
    /* FIX ME */


	printf("terminate\n"); 
   	current[cpu_id]->state = PROCESS_TERMINATED;
    	schedule(cpu_id);
}


/*
 * wake_up() is the handler called by the simulator when a process's I/O
 * request completes.  It should perform the following tasks:
 *
 *   1. Mark the process as READY, and insert it into the ready queue.
 *
 *   2. If the scheduling algorithm is static priority, wake_up() may need
 *      to preempt the CPU with the lowest priority process to allow it to
 *      execute the process which just woke up.  However, if any CPU is
 *      currently running idle, or all of the CPUs are running processes
 *      with a higher priority than the one which just woke up, wake_up()
 *      should not preempt any CPUs.
 *	To preempt a process, use force_preempt(). Look in os-sim.h for 
 * 	its prototype and the parameters it takes in.
 */
extern void wake_up(pcb_t *process)
{
    /* FIX ME */
   	 process->state = PROCESS_READY;
	 enqueue(process);	
 	 pthread_cond_signal(&idle_condition);
}


/*
 * main() simply parses command line arguments, then calls start_simulator().
 * You will need to modify it to support the -r and -p command-line parameters.
 */
int main(int argc, char *argv[])
{
    int cpu_count;

    /* Parse command-line arguments */
    if (argc != 2)
    {
        fprintf(stderr, "CS 425 Project 3 -- Multithreaded OS Simulator\n"
            "Usage: ./os-sim <# CPUs> [ -r <time slice> ]\n"
            "    Default : FIFO Scheduler\n"
            "         -r : Round-Robin Scheduler\n");

        return -1;
    }
    cpu_count = atoi(argv[1]);

    /* FIX ME - Add support for -r and -p parameters*/

    /* Allocate the current[] array and its mutex */
    current = malloc(sizeof(pcb_t*) * cpu_count);
    assert(current != NULL);
    pthread_mutex_init(&current_mutex, NULL);
    pthread_cond_init(&idle_condition,NULL);
    pthread_mutex_init(&readyQueue_mutex, NULL);
    /* Start the simulator in the library */
    
	
   start_simulator(cpu_count);

    return 0;
}


