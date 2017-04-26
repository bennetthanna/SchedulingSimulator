#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <queue>
#include <list>

struct Process {
	int PID;
	int arrival_time;
	int burst_time;
};

int main(int argc, char **argv) {

	std::queue<Process*>processes;
	Process *process;
	int PID, arrival_time, burst_time;

	if (argc != 3 && argc != 4) {
		fprintf(stderr, "usage: ./hw3 simulation_time algorithm [time_slice]\n");
		exit(1);
	} 

	if (strcmp(argv[2], "FCFS") != 0 && strcmp(argv[2], "SJF") != 0 && strcmp(argv[2], "RR") != 0) {
		fprintf(stderr, "algorithm: FCFS, SJF, RR\n");
		exit(1);
	}

	while (std::cin >> PID >> arrival_time >> burst_time) {
		process = new Process();
		process->PID = PID;
		process->arrival_time = arrival_time;
		process->burst_time = burst_time;
		processes.push(process);
	}
}