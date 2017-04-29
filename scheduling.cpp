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
	// sort by arrival time then burst time
	// keep a counter of elapsed time
	// compare all items in vectors arrival time to see if they are <= elapsed time
	// if so, put into new vector, sort by burst time, schedule the first process
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
bool compare_by_arrival_then_burst(const Process *a, const Process *b) {
    if (a->arrival_time != b->arrival_time) {
    	return a->arrival_time < b->arrival_time;
    }
    return a->burst_time < b->burst_time;
}

bool compare_by_burst(const Process *a, const Process *b) {
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

	} else if (algorithm == "SJF") {
		std::vector<Process*>arrived_processes;
		std::sort(processes.begin(), processes.end(), compare_by_arrival_then_burst);

		for (int i = 0; i < processes.size(); ++i) {
			std::cout << "PID: " << processes[i]->PID << " Arrival: " << processes[i]->arrival_time << " Burst: " << processes[i]->burst_time << std::endl;
		}
		while (time < processes[0]->arrival_time) {
			time += 1;
		}
		std::cout << time << ": scheduling PID " << processes[0]->PID << " : CPU = " << processes[0]->burst_time << std::endl;
		time += processes[0]->burst_time;
		processes.erase(processes.begin());
		for (int i = 0; i < processes.size(); ++i) {
			std::cout << "PID: " << processes[i]->PID << " Arrival: " << processes[i]->arrival_time << " Burst: " << processes[i]->burst_time << std::endl;
		}
		while (processes.size() > 0) {
			arrived_processes.clear();
			for (int i = 0; i < processes.size(); ++i) {
				if (processes[i]->arrival_time <= time) {
					arrived_processes.push_back(processes[i]);
				}
			}
			std::sort(arrived_processes.begin(), arrived_processes.end(), compare_by_burst);
			std::cout << "Sorted\n";
			for (int i = 0; i < arrived_processes.size(); ++i) {
				std::cout << "PID: " << arrived_processes[i]->PID << " Arrival: " << arrived_processes[i]->arrival_time << " Burst: " << arrived_processes[i]->burst_time << std::endl;
			}
			std::cout << time << ": scheduling PID " << arrived_processes[0]->PID << " : CPU = " << arrived_processes[0]->burst_time << std::endl;
			for (int i = 0; i < processes.size(); ++i) {
				if (processes[i]->PID == arrived_processes[0]->PID) {
					time += processes[i]->burst_time;
					processes.erase(processes.begin() + i);
				}
			}
			std::cout << "New processes vector\n";
			for (int i = 0; i < processes.size(); ++i) {
				std::cout << "PID: " << processes[i]->PID << " Arrival: " << processes[i]->arrival_time << " Burst: " << processes[i]->burst_time << std::endl;
			}
		}
	} else if (algorithm == "RR") {
		std::sort(processes.begin(), processes.end(), compare_by_arrival_time);

		for (int i = 0; i < processes.size(); ++i) {
			std::cout << "PID: " << processes[i]->PID << " Arrival: " << processes[i]->arrival_time << " Burst: " << processes[i]->burst_time << std::endl;
		}

		std::vector<Process*>::iterator it;
		for (it = processes.begin(); it != processes.end(); ) {
			std::cout << time << ": scheduling PID " << (*it)->PID << " : CPU = " << (*it)->burst_time << std::endl;
			if ((*it)->burst_time < time_slice) {
				time += (*it)->burst_time;
				std::cout << time << " : PID " << (*it)->PID << " terminated" << std::endl;
				it = processes.erase(it);
			} else {
				time += time_slice;
				(*it)->burst_time -= time_slice;
				if ((*it)->burst_time != 0) {
					std::cout << time << " : suspending PID " << (*it)->PID << " : CPU = " << (*it)->burst_time << std::endl;
					it++;
				} else {
					std::cout << time << " : PID " << (*it)->PID << " terminated" << std::endl;
					it = processes.erase(it);
				}	
			}
			if (it == processes.end() && processes.size() > 0) {
				it = processes.begin();
			}
		}
	}
}