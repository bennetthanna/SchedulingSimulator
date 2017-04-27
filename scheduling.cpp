#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <queue>
#include <list>
#include <vector>
#include <algorithm>


//push it back into a vector
//based on the algorithm then sort the vector
//FCFS - sort by arrival time
//SJF - sort by burst time
//RR
//http://stackoverflow.com/questions/4892680/sorting-a-vector-of-structs

struct Process {
	int PID;
	int arrival_time;
	int burst_time;
};

bool compare_by_arrival_time(const Process *a, const Process *b) {
    return a->arrival_time < b->arrival_time;
}

//sorts on arrival time then sorts on burst time
bool compare_by_burst_time(const Process *a, const Process *b) {
    if (a->arrival_time != b->arrival_time) {
    	return a->arrival_time < b->arrival_time;
    }
    return a->burst_time < b->burst_time;
}

int main(int argc, char **argv) {

	std::vector<Process*>processes;
	Process *process;
	int PID, arrival_time, burst_time, simulation_time, time_slice, remaining_tasks; 
	int time = 0, throughput = 0, wait_time = 0, turnaround_time = 0;
	std::string algorithm;

	if (argc != 3 && argc != 4) {
		fprintf(stderr, "usage: ./hw3 simulation_time algorithm [time_slice]\n");
		exit(1);
	}

	if (strcmp(argv[2], "FCFS") != 0 && strcmp(argv[2], "SJF") != 0 && strcmp(argv[2], "RR") != 0) {
		fprintf(stderr, "algorithm: FCFS, SJF, RR\n");
		exit(1);
	}

	simulation_time = atoi(argv[1]);
	algorithm = argv[2];
	if (argc == 4) {
		time_slice = atoi(argv[3]);
	}

	while (std::cin >> PID >> arrival_time >> burst_time) {
		process = new Process();
		process->PID = PID;
		process->arrival_time = arrival_time;
		process->burst_time = burst_time;
		processes.push_back(process);
	}

	if (algorithm == "FCFS") {

		std::sort(processes.begin(), processes.end(), compare_by_arrival_time);

		for (int i = 0; i < processes.size(); ++i) {
			std::cout << "PID: " << processes[i]->PID << " Arrival: " << processes[i]->arrival_time << " Burst: " << processes[i]->burst_time << std::endl;
		}

		remaining_tasks = processes.size();

		std::cout << "=================================" << std::endl;
		for (int i = 0; i < processes.size(); ++i) {
			while (time < processes[i]->arrival_time) {
				time += 1;
			}
			wait_time += time;
			std::cout << time << ": scheduling PID " << processes[i]->PID << " : CPU = " << processes[i]->burst_time << std::endl;
			time += processes[i]->burst_time;
			turnaround_time += time;
			std::cout << time << " : PID " << processes[i]->PID << " terminated" << std::endl;
			throughput++;
			remaining_tasks--;
		}

		std::cout << "=================================" << std::endl;
		std::cout << "Throughput: " << throughput << std::endl;
		std::cout << "Average wait time: " << (float)wait_time/(float)throughput << std::endl;
		std::cout << "Average turnaround time: " << (float)turnaround_time/(float)throughput << std::endl;
		std::cout << "Remaining tasks: " << remaining_tasks << std::endl;
	}
}