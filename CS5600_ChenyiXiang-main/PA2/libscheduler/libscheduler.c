/*
 * Programming Assignment 2
 * CS 5600, Spring 2024
 * Library for Scheduler Code
 * February 6, 2024
 */

#include "libscheduler.h"
#include <limits.h>

/*
 * You may need to define some global variables or a struct to
 * store your job queue elements. Feel free to do it here at the
 * top of the .c file, or in the .h file.
 */
extern int scheme; // FCFS, SJF, PSJF, PRI, PPRI, RR

typedef enum
{
    JOB_NEW,     // Job has been created
    JOB_WAITING, // Job is waiting to be scheduled
    JOB_RUNNING, // Job is currently running
    JOB_FINISHED // Job has finished
} job_state_t;
/* See Canvas function descriptions for details of each function */

typedef struct job_struct
{
    // This is a linked list
    int job_number;
    int arrival_time;
    int time_left;
    int priority;
    job_state_t state;   // current state of the job
    int response_time;   // time when the job was first scheduled
    int waiting_time;    // time spent in the waiting queue
    int start_time;      // time when the job was first scheduled
    int completion_time; // time when the job was completed
    int turnaround_time; // Time from job arrival to completion
    int running_time;
    struct job_struct *next; // pointer to the next job in the queue
} job_t;

// Assume we have a global pointer to the head of the job queue and a current job variable
job_t *job_queue_head = NULL;
job_t *current_job = NULL;
static int total_waiting_time = 0;
static int total_turnaround_time = 0;
static int total_response_time = 0;
static int total_jobs = 0;

// Function to insert a job into the queue (you'll need to implement this)
void insert_job_into_queue(job_t *new_job)
{
    if (job_queue_head == NULL)
    {
        job_queue_head = new_job;
    }
    else
    {
        // Insert to end of linked list
        job_t *temp = job_queue_head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_job;
    }
};

// Function to determine the next job to run (you'll need to implement this)
job_t *determine_next_job_to_run()
{
    if (job_queue_head == NULL)
    {
        return NULL;
    }
    job_t *selected_job = NULL;
    job_t *temp = job_queue_head;
    job_t *prev_selected = NULL;
    job_t *prev_temp = NULL;

    switch (scheme)
    {
    case FCFS:
    {
        selected_job = job_queue_head;
        break;
    }

    case SJF:
    {
        int shortest_time = INT_MAX;
        while (temp != NULL)
        {
            if (temp->time_left < shortest_time && temp->state == JOB_WAITING)
            {
                shortest_time = temp->time_left;
                selected_job = temp;
                prev_selected = prev_temp;
            }
            prev_temp = temp;
            temp = temp->next;
        }
    }
    break;

    case PRI:
    {
        int highest_priority = INT_MAX;
        while (temp != NULL)
        {
            if (temp->priority < highest_priority && temp->state == JOB_WAITING)
            {
                highest_priority = temp->priority;
                selected_job = temp;
                prev_selected = prev_temp;
            }
            prev_temp = temp;
            temp = temp->next;
        }
    }
    break;

    case PPRI:
    {
        int highest_priority = INT_MAX;
        while (temp != NULL)
        {
            if (temp->priority < highest_priority && temp->state == JOB_WAITING)
            {
                highest_priority = temp->priority;
                selected_job = temp;
                prev_selected = prev_temp;
            }
            prev_temp = temp;
            temp = temp->next;
        }
    }
    break;

    case PSJF:
    {
        int shortest_time = INT_MAX;
        while (temp != NULL)
        {
            if (temp->time_left < shortest_time && temp->state == JOB_WAITING)
            {
                shortest_time = temp->time_left;
                selected_job = temp;
                prev_selected = prev_temp;
            }
            prev_temp = temp;
            temp = temp->next;
        }
    }
    break;

    case RR:
        // For Round Robin, you would typically move to the next job in the queue.
        // This example does not handle the quantum expiration directly.
        if (current_job != NULL && current_job->next != NULL)
        {
            selected_job = current_job->next; // Move to the next job
        }
        else
        {
            selected_job = job_queue_head; // Or circle back to the start
        }
        break;

        // Add additional cases for RR1, RR2, RR4 as needed, potentially adjusting 'quantum'
    }

    // Optionally remove the selected job from the queue if your logic requires
    // This might be necessary for PSJF or if you're implementing job removal upon selection
    if (prev_selected != NULL)
    {
        prev_selected->next = selected_job->next; // Remove selected from its current position
        selected_job->next = job_queue_head;      // Optionally re-add it at the start or based on your scheme
    }

    return selected_job;
}

// Function to update job stats like waiting time (you'll need to implement this)
void update_job_stats(job_t *job, int current_time)
{
    // Example for RR and PSJF, considering preemptive nature
    if (scheme == PSJF || scheme == RR || scheme == PPRI)
    {

        if (job->state == JOB_RUNNING)
        {
            // For RR, consider if the job is being preempted due to quantum expiration
            // Update waiting time considering the quantum or remaining time adjustment
            if (scheme == RR)
            {
            }
            else if (scheme == PSJF)
            {
                // For PSJF, consider if the job is being preempted due to a shorter job arriving
                // Update waiting time considering the new job's running time
            }
        }
        else if (scheme == PPRI)
        {
            // PPRI特定的逻辑，例如考虑到达的新作业是否有更高的优先级
        }
        // Update common stats
        if (job->start_time == -1)
            job->start_time = current_time; // First run
        if (job->state == JOB_FINISHED)
        {
            job->completion_time = current_time;
            job->turnaround_time = job->completion_time - job->arrival_time;
            // Calculate final waiting time, if not already updated
        }
    }
    else
    {
        // Handle non-preemptive strategies
        if (job->state == JOB_RUNNING && job->start_time == -1)
        {
            job->start_time = current_time; // First run
            job->response_time = current_time - job->arrival_time;
        }
        if (job->state == JOB_FINISHED)
        {
            job->completion_time = current_time;
            job->turnaround_time = job->completion_time - job->arrival_time;
            job->waiting_time = job->turnaround_time - job->running_time;
        }
    }
    // Adjust based on the job's current state and the specific requirements of your scheduler
}

job_t *create_job(int job_number, int time, int running_time, int priority)
{
    job_t *new_job = (job_t *)malloc(sizeof(job_t));
    if (new_job == NULL)
    {
        // Handle memory allocation failure
        return NULL;
    }

    // Initialize the fields of the job
    new_job->job_number = job_number;
    new_job->arrival_time = time;
    new_job->time_left = running_time;
    new_job->priority = priority;
    new_job->state = JOB_WAITING;         // The job is initially waiting
    new_job->response_time = 0;           // Or initialize as -1, depending on how you handle unstarted jobs
    new_job->waiting_time = 0;            // Initialize waiting time
    new_job->start_time = -1;             // The job has not started yet
    new_job->completion_time = 0;         // The job has not completed yet
    new_job->turnaround_time = 0;         // Initialize turnaround time
    new_job->running_time = running_time; // Initialize the total running time of the job
    new_job->next = NULL;                 // Pointer to the next job

    return new_job;
}

void mark_job_as_finished(int job_number, int completion_time)
{
    // Iterate through the job queue to find the specified job
    job_t *job = job_queue_head;
    while (job != NULL)
    {
        if (job->job_number == job_number)
        {
            // Update job state to finished
            job->state = JOB_FINISHED;
            // Record the job completion time
            job->completion_time = completion_time;
            // Calculate turnaround time (completion time - arrival time)
            job->turnaround_time = completion_time - job->arrival_time;
            // Optionally, calculate waiting time (turnaround time - running time)
            job->waiting_time = job->turnaround_time - job->running_time;
            // Add other updates here as needed upon job completion

            // Update metrics
            total_jobs += 1;
            total_response_time += job->response_time;
            total_turnaround_time += job->turnaround_time;
            total_waiting_time += job->waiting_time;

            break; // Exit loop once the job is found and updated
        }
        job = job->next; // Move to the next job in the queue
    }
}

int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
    // initialize the job
    job_t *new_job = create_job(job_number, time, running_time, priority);
    if (new_job == NULL)
    {
        // Handle job creation failure
        return -1;
    }

    // add the job to the queue
    insert_job_into_queue(new_job);

    // if it is preemptive, determine the next job to run.
    if (scheme == RR)
    {
        if (current_job)
        {
            current_job->state = JOB_WAITING; // Preempt the current job
        }
        job_t *next_job = determine_next_job_to_run();
        next_job->state = JOB_RUNNING;
        current_job = next_job;
        return current_job->job_number;
        // if the next job is not the current job, then preempt the current job and run the next job.
        // if (next_job && next_job->job_number != current_job->job_number)
        // {

        //     // update the next job's state
        //     next_job->state = JOB_RUNNING;
        //     // update the current job
        //     current_job = next_job;
        // }
    }
    else if (scheme == PSJF || scheme == PPRI)
    {
        // Simply stop the current job and find the new shortest/priority job. Run the new shortest/priority job.
        if (current_job)
        {
            current_job->state = JOB_WAITING; // Preempt the current job
        }
        job_t *next_job = determine_next_job_to_run();
        next_job->state = JOB_RUNNING;
        current_job = next_job;
        return current_job->job_number;
    }

    // for non-preemptive, we will run the job when the current job is finished.
    return current_job ? current_job->job_number : new_job->job_number;
}

int scheduler_job_finished(int job_number, int time)
{
    // Mark the job as finished and update its stats
    mark_job_as_finished(job_number, time);

    // Determine the next job to run
    job_t *next_job = determine_next_job_to_run();

    if (next_job)
    {
        // If there is a next job to run, update its state and set it as the current job
        current_job = next_job;
        current_job->state = JOB_RUNNING;
        return next_job->job_number;
    }
    else
    {
        // If there are no more jobs to run, set the current job to NULL
        current_job = NULL;
        return -1;
    }
}

int scheduler_quantum_expired(int time)
{
    return -1;
}

double scheduler_average_waiting_time()
{
    if (total_jobs == 0)
    {
        return 0;
    }
    return (double)total_waiting_time / total_jobs;
}

double scheduler_average_turnaround_time()
{
    if (total_jobs == 0)
    {
        return 0;
    }
    return (double)total_turnaround_time / total_jobs;
}

double scheduler_average_response_time()
{
    if (total_jobs == 0)
    {
        return 0;
    }
    return (double)total_response_time / total_jobs;
}

void scheduler_clean_up()
{
    job_t *temp = job_queue_head;
    while (temp != NULL)
    {
        job_t *next = temp->next;
        free(temp);
        temp = next;
    }
    job_queue_head = NULL;
    current_job = NULL;
    total_waiting_time = 0;
    total_turnaround_time = 0;
    total_response_time = 0;
    total_jobs = 0;
}

void scheduler_show_queue()
{
}