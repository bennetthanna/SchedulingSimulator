#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>

int remaining_tasks = 0, throughput = 0, wait_time = 0, turnaround_time = 0, elapsed_time = 0, simulation_time, time_slice;

struct Process {
	int PID;
	int arrival_time;
	int burst_time;
	int times_scheduled;
};

std::vector<Process*>processes;

// sorts by arrival time in descending order
bool compare_by_arrival(const Process *a, const Process *b) {
    return a->arrival_time < b->arrival_time;
}

// sorts by arrival time and then burst time in descending order
bool compare_by_arrival_then_burst(const Process *a, const Process *b) {
    if (a->arrival_time != b->arrival_time) {
    	return a->arrival_time < b->arrival_time;
    }
    return a->burst_time < b->burst_time;
}

// sorts by burst time in descending order
bool compare_by_burst(const Process *a, const Process *b) {
	return a->burst_time < b->burst_time;
}

/* prints statistics to stderr
 * only processes that terminated are included in the averages
 */
void print_stats() {
	fprintf(stderr, "Throughput: %i\n", throughput);
	fprintf(stderr, "Average wait time: %f\n", ((float)wait_time/(float)throughput));
	fprintf(stderr, "Average turnaround time: %f\n",((float)turnaround_time/(float)throughput));
	fprintf(stderr, "Remaining tasks: %i\n", remaining_tasks);
}

/* sorts the processes by arrival time
 * schedules process in arriving order
 */
void first_come_first_serve(std::vector<Process*>processes) {
	std::sort(processes.begin(), processes.end(), compare_by_arrival);
	remaining_tasks = processes.size();
	std::cout << "=================================" << std::endl;
	for (int i = 0; i < processes.size(); ++i) {
		// if the next process to be scheduled hasn't arrived yet then increment time
		while (elapsed_time < processes[i]->arrival_time) {
			elapsed_time += 1;
		}
		// check elapsed time against simultation time
		if (elapsed_time <= simulation_time) {
			std::cout << elapsed_time << ": scheduling PID " << processes[i]->PID << " : CPU = " << processes[i]->burst_time << std::endl;
		} else {
			std::cout << "OUT OF TIME!\n";
			break;
		}
		// check to make sure it can finish it's burst cycle
		if ((elapsed_time + processes[i]->burst_time) > simulation_time) {
			std::cout << "OUT OF TIME!" << std::endl;
			break;
		}
		// only add to stats if the process was able to terminate
		wait_time += (elapsed_time - processes[i]->arrival_time);
		elapsed_time += processes[i]->burst_time;
		turnaround_time += (elapsed_time - processes[i]->arrival_time);
		std::cout << elapsed_time << " : PID " << processes[i]->PID << " terminated" << std::endl;
		throughput++;
		remaining_tasks--;
	}
	std::cout << "=================================" << std::endl;
}

/* sorts the processes by arrival time then burst time
 * schedules the first arrived process with the shortest burst time
 * then continuously resorts the processes based on which processes have arrived compared to elapsed time
 * until there are no processes left
 */
void shortest_job_first(std::vector<Process*>processes) {
	std::vector<Process*>arrived_processes;
	std::sort(processes.begin(), processes.end(), compare_by_arrival_then_burst);
	remaining_tasks = processes.size();
	std::cout << "=================================" << std::endl;
	while (elapsed_time < processes[0]->arrival_time) {
		elapsed_time += 1;
	}
	if (elapsed_time <= simulation_time) {
		std::cout << elapsed_time << ": scheduling PID " << processes[0]->PID << " : CPU = " << processes[0]->burst_time << std::endl;
	}
	if ((elapsed_time + processes[0]->burst_time) > simulation_time) {
		std::cout << "OUT OF TIME!" << std::endl;
	} else {
		wait_time += (elapsed_time - processes[0]->arrival_time);
		elapsed_time += processes[0]->burst_time;
		std::cout << elapsed_time << " : PID " << processes[0]->PID << " terminated" << std::endl;
		turnaround_time += (elapsed_time - processes[0]->arrival_time);
		throughput++;
		remaining_tasks--;
		processes.erase(processes.begin());
		// continuously resort new vector of arrived processes based solely on burst time
		while (processes.size() > 0) {
			arrived_processes.clear();
			for (int i = 0; i < processes.size(); ++i) {
				if (processes[i]->arrival_time <= elapsed_time) {
					arrived_processes.push_back(processes[i]);
				}
			}
			std::sort(arrived_processes.begin(), arrived_processes.end(), compare_by_burst);
			while (elapsed_time < arrived_processes[0]->arrival_time) {
				elapsed_time += 1;
			}
			if (elapsed_time <= simulation_time) {
				std::cout << elapsed_time << ": scheduling PID " << arrived_processes[0]->PID << " : CPU = " << arrived_processes[0]->burst_time << std::endl;
			} else {
				std::cout << "OUT OF TIME!\n";
				break;
			}
			if ((elapsed_time + arrived_processes[0]->burst_time) > simulation_time) {
				std::cout << "OUT OF TIME!" << std::endl;
				break;
			}
			wait_time += (elapsed_time - arrived_processes[0]->arrival_time);
			elapsed_time += arrived_processes[0]->burst_time;
			turnaround_time += (elapsed_time - arrived_processes[0]->arrival_time);
			std::cout << elapsed_time << " : PID " << arrived_processes[0]->PID << " terminated" << std::endl;
			throughput++;
			remaining_tasks--;
			for (int i = 0; i < processes.size(); ++i) {
				// find the matching arrived process in the vector of all processes and erase
				if (processes[i]->PID == arrived_processes[0]->PID) {
					processes.erase(processes.begin() + i);
				}
			}
		}
	}
	std::cout << "=================================" << std::endl;
}

/* sorts the processes by arrival time
 * schedules each process for the given time slice
 * if the burst time is less than the time slice then the process terminates and is erased from the vector
 * once the iterator reaches the end of the vector it starts back at the beginning
 * until there are no processes left
 */
void round_robin(std::vector<Process*>processes) {
	std::sort(processes.begin(), processes.end(), compare_by_arrival);
	remaining_tasks = processes.size();
	std::cout << "=================================" << std::endl;
	std::vector<Process*>::iterator it;
	for (it = processes.begin(); it != processes.end(); ) {
		while (elapsed_time < processes[0]->arrival_time) {
			elapsed_time += 1;
		}
		if (elapsed_time <= simulation_time) {
			std::cout << elapsed_time << ": scheduling PID " << (*it)->PID << " : CPU = " << (*it)->burst_time << std::endl;
		}
		if (time_slice > (*it)->burst_time) {
			if ((elapsed_time + (*it)->burst_time) > simulation_time) {
				std::cout << "OUT OF TIME!" << std::endl;
				break;
			}
		} else {
			if ((elapsed_time + time_slice) > simulation_time) {
				std::cout << "OUT OF TIME!" << std::endl;
				break;
			}
		}
		if ((*it)->burst_time <= time_slice) {
			// only want to calculate wait time once on last time it's scheduled
			wait_time += (elapsed_time - (*it)->arrival_time - ((*it)->times_scheduled * time_slice));
			elapsed_time += (*it)->burst_time;
			std::cout << elapsed_time << " : PID " << (*it)->PID << " terminated" << std::endl;
			turnaround_time += (elapsed_time - (*it)->arrival_time);
			throughput++;
			remaining_tasks--;
			it = processes.erase(it);
		} else {
			(*it)->times_scheduled++;
			elapsed_time += time_slice;
			(*it)->burst_time -= time_slice;
			if ((*it)->burst_time != 0) {
				std::cout << elapsed_time << " : suspending PID " << (*it)->PID << " : CPU = " << (*it)->burst_time << std::endl;
				it++;
			}	
		}
		// loop back around to beginning if there are still processes to be scheduled
		if (it == processes.end() && processes.size() > 0) {
			it = processes.begin();
		}
	}
	std::cout << "=================================" << std::endl;
}

int main(int argc, char **argv) {

	Process *process;
	int PID, arrival_time, burst_time; 
	std::string algorithm;

	// check number of command line arguments
	if (argc != 3 && argc != 4) {
		fprintf(stderr, "usage: ./hw3 simulation_time algorithm [time_slice]\n");
		exit(1);
	}

	// check for correct algorithm
	if (strcmp(argv[2], "FCFS") != 0 && strcmp(argv[2], "SJF") != 0 && strcmp(argv[2], "RR") != 0) {
		fprintf(stderr, "algorithm: FCFS, SJF, RR\n");
		exit(1);
	}

	// check that a time slice argument is given for round robin
	if (strcmp(argv[2], "RR") == 0 && argc != 4) {
		fprintf(stderr, "RR must be given a 3 arguments\nusage: ./hw3 simulation_time algorithm [time_slice]\n");
		exit(1);
	}

	// check that FCFS and SJF are given 2 arguments
	if ((strcmp(argv[2], "FCFS") == 0 || strcmp(argv[2], "SJF") == 0) && argc != 3) {
		fprintf(stderr, "FCFS and SJF only take 2 arguments\nusage: ./hw3 simulation_time algorithm\n");
		exit(1);
	}

	// check time is greater than 0
	if (atoi(argv[1]) <= 0) {
		fprintf(stderr, "Simulation time must be greater than 0\n");
		exit(1);
	}


	simulation_time = atoi(argv[1]);
	algorithm = argv[2];
	if (argc == 4) {
		if (atoi(argv[3]) <= 0) {
			fprintf(stderr, "Time slice must be greater than 0\n");
			exit(1);
		} else {
			time_slice = atoi(argv[3]);
		}
	}

	// continuously get process information from user
	while (std::cin >> PID >> arrival_time >> burst_time) {
		process = new Process();
		process->PID = PID;
		process->arrival_time = arrival_time;
		process->burst_time = burst_time;
		processes.push_back(process);
	}

	// call the correct function based on chosen algorithm
	if (algorithm == "FCFS") {
		first_come_first_serve(processes);
		print_stats();
	} else if (algorithm == "SJF") {
		shortest_job_first(processes);
		print_stats();
	} else if (algorithm == "RR") {
		round_robin(processes);
		print_stats();
	}

	// free the memory used by the processes
	for (int i = 0; i < processes.size(); ++i) {
		delete(processes[i]);
	}
}