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
    } 

    else if (scheme == SJF) {
		// Insert the new job in sorted order based on running time
        // Special handling for inserting immediately after the head
		if (head->next != NULL) {
			if (new_node->running_time < head->next->running_time) { 
					new_node->next = head->next;
					head->next = new_node;
					return head->job_number;
			}
			else {
				// Find the correct position for the new job
				job_node_t* current = head->next;
				job_node_t* prev = head; 
				while (current != NULL && current->running_time <= new_node->running_time) {
					prev = current; 
					current = current->next;
				}
				new_node->next = current;
				prev->next = new_node;
				return head->job_number;
			}
		}
		// If the head is the only node, simply add the new node after it
		head->next = new_node;
		return head->job_number;
	}

	// For Preemptive Shortest Job First (PSJF) scheduling
	else if (scheme == PSJF) {
		// Adjust the remaining time of the current head job
        if (head != NULL) {
            head->remaining_time -= (time - head->start_time);
        }

		// Insert the new job based on remaining running time
		job_node_t* current = head;
		job_node_t* prev = NULL;
		
		while (current!= NULL && current->remaining_time <= new_node->running_time){
			prev = current;
			current = current->next;
		}
		
		new_node->next = current;
		
		if (prev != NULL){
			prev->next = new_node;
		}
		else{
			// Handle the case where the new job becomes the head
			if(new_node->first_run_time == -1){
				new_node->first_run_time = time;
			}
			// Adjust the first run time of the next job, if necessary
			if(new_node->first_run_time == new_node->next->first_run_time){
				new_node->next->first_run_time = -1;
			} 
			head = new_node;
		}
		return head->job_number;
	}

	else if (scheme == PRI) {
		if (head->next != NULL) {
			if (new_node->priority <= head->next->priority) { 
				new_node->next = head->next;
				head->next = new_node;
				return head->job_number;
			}
			else {
				job_node_t* current = head->next;
				job_node_t* prev = head; 
				while (current != NULL && current->priority <= new_node->priority) {
					prev = current; 
					current = current->next;
				}
				new_node->next = current;
				prev->next = new_node;
				return head->job_number;
			}
		}
		head->next = new_node;
		return head->job_number;
	}

	 // For Preemptive Priority (PPRI) scheduling
	else if (scheme == PPRI) {
		job_node_t* current = head;
        job_node_t* prev = NULL;
        while (current != NULL && current->priority <= new_node->priority) {
            prev = current;
            current = current->next;
        }
        new_node->next = current;
        if (prev != NULL){
            prev->next = new_node;
		} else{
			// Handle the case where the new job becomes the head
			if(new_node->first_run_time == -1){
				new_node->first_run_time = time;
			}
			 // Adjust the first run time of the next job, if necessary
			if(new_node->first_run_time == new_node->next->first_run_time){
				new_node->next->first_run_time = -1;
			} 
			head = new_node;
		}
		return head->job_number;
	}
	



    else { fprintf(stderr, "Unknown scheduling algorithm\n");
		free(new_node);
		return -1; }
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
