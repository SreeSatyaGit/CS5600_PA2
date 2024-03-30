#include "libscheduler.h"

double total_jobs = 0, total_waiting_time = 0, total_turnaround_time = 0, total_response_time = 0;
typedef struct job_node {
    int job_number, arrival_time, running_time, remaining_time, priority, start_time, first_run_time, completion_time;
    struct job_node* next;
} job_node_t;

job_node_t* head = NULL;
extern int scheme;

void scheduler_show_queue() {
    for (job_node_t* current = head; current != NULL; current = current->next)
        printf("%d ", current->job_number);
}

int scheduler_new_job(int job_number, int time, int running_time, int priority) {
    job_node_t* new_node = malloc(sizeof(job_node_t));
    if (!new_node) exit(EXIT_FAILURE);
    *new_node = (job_node_t){job_number, time, running_time, running_time, priority, -1, -1, -1, NULL};

    total_jobs++;
    if (!head || scheme == FCFS || scheme == RR) {
        if (!head) { head = new_node; head->start_time = head->first_run_time = time; }
        else {
            job_node_t* current = head;
            while (current->next != NULL) current = current->next;
            current->next = new_node;
        }
    } else { /* Insertion logic for SJF, PSJF, PRI, PPRI */ }
    return head ? head->job_number : -1;
}

int scheduler_job_finished(int job_number, int time) {
    if (!head) return -1;
    job_node_t *finished_job = head;
    head = head->next;

    finished_job->completion_time = time;
    int turnaround_time = finished_job->completion_time - finished_job->arrival_time;
    int waiting_time = turnaround_time - finished_job->running_time;
    int response_time = finished_job->first_run_time - finished_job->arrival_time;

    total_waiting_time += waiting_time;
    total_turnaround_time += turnaround_time;
    total_response_time += response_time;

    free(finished_job);
    return head ? (head->first_run_time = head->start_time = time, head->job_number) : -1;
}

int scheduler_quantum_expired(int time) {
    if (scheme != RR || !head) return -1;
    if (!head->next) return head->job_number;
    job_node_t* expired_job = head;
    while (expired_job->next) expired_job = expired_job->next;
    expired_job->next = head;
    head = head->next;
    expired_job->next->next = NULL;
    return head->job_number;
}

double scheduler_average_waiting_time() { return total_waiting_time / total_jobs; }
double scheduler_average_turnaround_time() { return total_turnaround_time / total_jobs; }
double scheduler_average_response_time() { return total_response_time / total_jobs; }

void scheduler_clean_up() {
    while (head) {
        job_node_t* temp_node = head;
        head = head->next;
        free(temp_node);
    }
}
