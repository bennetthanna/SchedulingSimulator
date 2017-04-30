#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>

int remaining_tasks = 0, throughput = 0, wait_time = 0, turnaround_time = 0;

struct Process {
	int PID;
	int arrival_time;
	int burst_time;
	int times_scheduled;
};

bool compare_by_arrival(const Process *a, const Process *b) {
    return a->arrival_time < b->arrival_time;
}

bool compare_by_arrival_then_burst(const Process *a, const Process *b) {
    if (a->arrival_time != b->arrival_time) {
    	return a->arrival_time < b->arrival_time;
    }
    return a->burst_time < b->burst_time;
}

bool compare_by_burst(const Process *a, const Process *b) {
	return a->burst_time < b->burst_time;
}

void print_stats() {
	std::cout << "Throughput: " << throughput << std::endl;
	std::cout << "Average wait time: " << (float)wait_time/(float)throughput << std::endl;
	std::cout << "Average turnaround time: " << (float)turnaround_time/(float)throughput << std::endl;
	std::cout << "Remaining tasks: " << remaining_tasks << std::endl;
}

int main(int argc, char **argv) {

	std::vector<Process*>processes;
	Process *process;
	int PID, arrival_time, burst_time, simulation_time, time_slice; 
	int time = 0;
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

		std::sort(processes.begin(), processes.end(), compare_by_arrival);
		remaining_tasks = processes.size();

		std::cout << "=================================" << std::endl;
		for (int i = 0; i < processes.size(); ++i) {
			while (time < processes[i]->arrival_time) {
				time += 1;
			}
			if ((time + processes[i]->burst_time) > simulation_time) {
				std::cout << "OUT OF TIME!" << std::endl;
				break;
			}
			wait_time += (time - processes[i]->arrival_time);
			std::cout << time << ": scheduling PID " << processes[i]->PID << " : CPU = " << processes[i]->burst_time << std::endl;
			time += processes[i]->burst_time;
			turnaround_time += (time - processes[i]->arrival_time);
			std::cout << time << " : PID " << processes[i]->PID << " terminated" << std::endl;
			throughput++;
			remaining_tasks--;
		}

		std::cout << "=================================" << std::endl;
		print_stats();

	} else if (algorithm == "SJF") {
		std::vector<Process*>arrived_processes;
		std::sort(processes.begin(), processes.end(), compare_by_arrival_then_burst);
		remaining_tasks = processes.size();

		std::cout << "=================================" << std::endl;
		while (time < processes[0]->arrival_time) {
			time += 1;
		}
		if ((time + processes[0]->burst_time) > simulation_time) {
			std::cout << "OUT OF TIME!" << std::endl;
		} else {
			wait_time += (time - processes[0]->arrival_time);
			std::cout << time << ": scheduling PID " << processes[0]->PID << " : CPU = " << processes[0]->burst_time << std::endl;
			time += processes[0]->burst_time;
			turnaround_time += (time - processes[0]->arrival_time);
			std::cout << time << " : PID " << processes[0]->PID << " terminated" << std::endl;
			throughput++;
			remaining_tasks--;
			processes.erase(processes.begin());

			while (processes.size() > 0) {
				arrived_processes.clear();
				for (int i = 0; i < processes.size(); ++i) {
					if (processes[i]->arrival_time <= time) {
						arrived_processes.push_back(processes[i]);
					}
				}
				std::sort(arrived_processes.begin(), arrived_processes.end(), compare_by_burst);
				while (time < arrived_processes[0]->arrival_time) {
					time += 1;
				}
				if ((time + arrived_processes[0]->burst_time) > simulation_time) {
					std::cout << "OUT OF TIME!" << std::endl;
					break;
				}
				wait_time += (time - arrived_processes[0]->arrival_time);
				std::cout << time << ": scheduling PID " << arrived_processes[0]->PID << " : CPU = " << arrived_processes[0]->burst_time << std::endl;
				for (int i = 0; i < processes.size(); ++i) {
					if (processes[i]->PID == arrived_processes[0]->PID) {
						time += processes[i]->burst_time;
						turnaround_time += (time - processes[i]->arrival_time);
						std::cout << time << " : PID " << processes[i]->PID << " terminated" << std::endl;
						throughput++;
						remaining_tasks--;
						processes.erase(processes.begin() + i);
					}
				}
			}
		}
		std::cout << "=================================" << std::endl;
		print_stats();

	} else if (algorithm == "RR") {
		std::sort(processes.begin(), processes.end(), compare_by_arrival);
		remaining_tasks = processes.size();
		std::cout << "=================================" << std::endl;
		std::vector<Process*>::iterator it;
		for (it = processes.begin(); it != processes.end(); ) {
			while (time < processes[0]->arrival_time) {
				time += 1;
			}
			if (time_slice > (*it)->burst_time) {
				if ((time + (*it)->burst_time) > simulation_time) {
					std::cout << "OUT OF TIME!" << std::endl;
					break;
				}
			} else {
				if ((time + time_slice) > simulation_time) {
					std::cout << "OUT OF TIME!" << std::endl;
					break;
				}
			}
			if ((*it)->burst_time <= time_slice) {
				wait_time += (time - (*it)->arrival_time - ((*it)->times_scheduled * time_slice));
			}
			std::cout << time << ": scheduling PID " << (*it)->PID << " : CPU = " << (*it)->burst_time << std::endl;
			(*it)->times_scheduled++;
			if ((*it)->burst_time < time_slice) {
				time += (*it)->burst_time;
				std::cout << time << " : PID " << (*it)->PID << " terminated" << std::endl;
				turnaround_time += (time - (*it)->arrival_time);
				throughput++;
				remaining_tasks--;
				it = processes.erase(it);
			} else {
				time += time_slice;
				(*it)->burst_time -= time_slice;
				if ((*it)->burst_time != 0) {
					std::cout << time << " : suspending PID " << (*it)->PID << " : CPU = " << (*it)->burst_time << std::endl;
					if ((*(it++))->arrival_time < time) {
						it = processes.begin();
					} else {
						it++;
					}
				} else {
					std::cout << time << " : PID " << (*it)->PID << " terminated" << std::endl;
					turnaround_time += (time - (*it)->arrival_time);
					throughput++;
					remaining_tasks--;
					it = processes.erase(it);
				}	
			}
			if (it == processes.end() && processes.size() > 0) {
				it = processes.begin();
			}
		}
		std::cout << "=================================" << std::endl;
		print_stats();
	}

	for (int i = 0; i < processes.size(); ++i) {
		delete(processes[i]);
	}
}