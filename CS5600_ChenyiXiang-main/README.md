# CS5600: Process Scheduling Simulator

## Author Information
- **Name**: Chenyi Xiang, Zongyu Wu, Satya Nandivada Bharadwaja
- **Date**: 03/16/2024
- **Course**: CS5600

## Project Overview
This project is a part of the CS5600 course, focusing on understanding and implementing process scheduling within operating systems. Our team has developed a library to handle basic scheduling tasks using a discrete event simulator provided by the course instructors. This simulator mimics time, job arrivals, and job executions, allowing us to focus on the scheduling algorithm without directly interacting with an actual operating system.

The project supports various scheduling schemes, including:
- First-Come, First-Served (FCFS)
- Shortest Job First (SJF)
- Preemptive Shortest Job First (PSJF)
- Priority (PRI)
- Preemptive Priority (PPRI)
- Round Robin (RR)

Our implementation aims to efficiently manage job scheduling based on these policies, focusing on minimizing waiting times, turnaround times, and response times.

## Compilation and Execution Instructions
To compile the scheduler library and the discrete event simulator, use the following commands in the terminal:

```bash
make clean
make


To run the simulator with a specific scheduling scheme and a job schedule, use the command format below:

./simulator <scheme> <job_file>
Here, <scheme> refers to one of the scheduling schemes (FCFS, SJF, PSJF, PRI, PPRI, RR#), and <job_file> is the path to a CSV file containing the job list. Example CSV files can be found in the examples/ directory.
