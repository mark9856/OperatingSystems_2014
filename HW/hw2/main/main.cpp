/*
  To successfully compile, I added a dummy "process.h" which need to be implemented
  I generated processes as a vector<Process>.
  Sisi,
    Please let me know if you need a queue<Process> or other data structures
    instead of a vector to implement your algorithm functions.
    You may need to output the algorithm results to a text file. If you do, I'll
    declare an output stream in main() and pass it as one of the arguments for your
    algorithm implementations
  Huey,
    For now I just treat all processes as objects of a single Process class.
    I differentiate cpu or io bound processes by constructor argument.
    Please let me know if you need two classes of Process(like CPUProcess and IOProcess)
  Let me know if anything is wrong in the following codes or I need to add anything to it
  At least it can compile in my laptop...
*/
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <vector>
#include "process.h"

//Time is in unit "ms"
//global variable with default values
float time_elapse = 0.0;
std::string algorithm = "SJF";
float timeslice = 100;
int num_processes = 12;
int num_CPU = 4;

//function prototypes
bool valid_input(int argc, char* const argv[]);
bool algorithm_exists(std::string const & algorithm);
std::vector<Process> generate_processes();
void do_algorithm(std::vector<Process>& processes);
//these following four functions are yet to be implemented
//Sisi will try to do some of these parts
void SJF(std::vector<Process>& processes);
void SRF(std::vector<Process>& processes);
void Round_Robin(std::vector<Process>& processes);
void Preemptive_Priority(std::vector<Process>& processes);


int main(int argc, char* argv[]) {
    /*
    1 <= argc <=
    argv[1]:type of algorithm("SJF","SRT","RR","PP")
            default value is "SJF"
    argv[2]:if argv[1] is "RR":
                timeslice, default value is 100
            if argv[1] is not "RR"
                number of processes, default value is 12
    argv[3]:number of processes if argv[1] is "RR"
            default value is 12
            number of CPUs if argv[1] is not "RR"
            default value is 4
     argv[4]:number of CPUs if argv[1] is "RR"
            default value is 4
    */
    //check command line configuration and setup global variables
    if(!valid_input(argc, argv)) return 1;
    //generate a vector of processes(80% cpu bound and 20% i/o bound)
    std::vector<Process> processes = generate_processes();
    do_algorithm(processes);

    return 0;
}

bool algorithm_exists(std::string const & algorithm) {
    if (algorithm != "SJF" && algorithm != "SRF"
        && algorithm != "RR" && algorithm != "PP") {
        std::cerr << "\nERROR: Scheduling algorithm \"" << algorithm
        << "\" is not defined. Please choose a valid algorithm name "
        << "from the following:\n  "
        << "SJF: Shortest-Job-First with no preemtion\n  "
        << "SRT: Shorted-Job-First with preemption "
        << "(Shortest-Remaining-Time-First)\n  "
        << "RR: Round-Robin with preemtion via configurable time slice\n  "
        << "PP: Preemptive Priority with random priority levels 0-4\n\n";
        return 0;
    } else return 1;
}

bool valid_input(int argc, char* const argv[]) {
    bool success = 0;
    switch (argc) {
        case 1://no command line argument is provided
            std::cout << "\nRunning default scheduling algorithm \"SJF\"...\n";
            std::cout << "Using default number of processes: 12...\n";
            std::cout << "Using default number of CPUs: 4...\n\n";
            success = 1;
            break;
        case 2://algorithm is specified
            algorithm = std::string(argv[1]);
            if(!algorithm_exists(algorithm)) return 0;
            std::cout << "\nRunning scheduling algorithm \"" << algorithm
            <<"\"...\n";
            if (algorithm == "RR") {
                std::cout << "Using default timeslice: 100ms...\n";
            }
            std::cout << "Using default number of processes: 12...\n";
            std::cout << "Using default number of CPUs: 4...\n\n";
            success = 1;
            break;
        case 3://timeslice/number of processes are specified
            algorithm = std::string(argv[1]);
            if(!algorithm_exists(algorithm)) return 0;
            std::cout << "\nRunning scheduling algorithm: \"" << algorithm
            << "\"...\n";
            if (algorithm == "RR") {
                timeslice = atof(argv[2]);
                std::cout << "Timeslice is "<<timeslice<<"ms...\n";
                std::cout << "Using default number of processes: 12...\n";
                std::cout << "Using default number of CPUs: 4...\n\n";
            } else {
                num_processes = atoi(argv[2]);
                std::cout << "Number of processes is "<< num_processes
                << "...\n";
                std::cout << "Using default number of CPUs: 4...\n\n";
            }
            success = 1;
            break;
        case 4:
            algorithm = std::string(argv[1]);
            if(!algorithm_exists(algorithm)) return 0;
            std::cout << "\nRunning scheduling algorithm \"" << algorithm
            << "\"...\n";
            if (algorithm == "RR") {
                timeslice = atof(argv[2]);
                std::cout << "Timeslice is "<<timeslice<<"ms...\n";
                num_processes = atoi(argv[3]);
                std::cout << "Number of processes is "<< num_processes
                << "...\n";
                std::cout << "Using default number of CPUs: 4...\n\n";
            } else {
                num_processes = atoi(argv[2]);
                std::cout << "Number of processes is "<< num_processes
                << "...\n";
                num_CPU = atoi(argv[3]);
                std::cout << "There are "<<num_CPU<<" CPUs working...\n\n";
            }
            success = 1;
            break;
        case 5:
            algorithm = std::string(argv[1]);
            if(!algorithm_exists(algorithm)) return 0;
            std::cout << "\nRunning scheduling algorithm \"" << algorithm
            << "\"...\n";
            if (algorithm == "RR") {
                timeslice = atof(argv[2]);
                std::cout << "Timeslice is "<<timeslice<<"ms...\n";
                num_processes = atoi(argv[3]);
                std::cout << "Number of processes is "<< num_processes
                << "...\n";
                num_CPU = atoi(argv[4]);
                std::cout << "There are "<<num_CPU<<" CPUs working...\n\n";
                success = 1;
            } else {
                std::cerr << "ERROR: Too many arguments!\n\n";
                success = 0;
            }
            break;
        default:
            std::cerr << "ERROR: Too many arguments!\n";
            success = 0;
            break;
    }
    return success;
}

std::vector<Process> generate_processes() {
    //80% are CPU bound, 20% are I/O bound
    int cpu_bound = num_processes * 0.8;
    int io_bound = num_processes - cpu_bound;
    int generated_process = 0;
    std::vector<Process> processes;
    while (generated_process < num_processes) {
        int cpu_or_io = rand() % 2;//1 for cpu, 0 for i/o
        if(cpu_or_io == 1 && cpu_bound > 0) {
            Process p("CPU");
            processes.push_back(p);
            cpu_bound--;
        } else if(cpu_or_io == 0 && io_bound > 0) {
            Process p("IO");
            processes.push_back(p);
            io_bound--;
        } else continue;
        generated_process++;
    }
    return processes;
}

void do_algorithm(std::vector<Process>& processes) {
    if (algorithm == "SJF") SJF(processes);
    else if (algorithm == "SRF") SRF(processes);
    else if (algorithm == "RR") Round_Robin(processes);
    else Preemptive_Priority(processes);
    //algorithm type checked already
}

void SJF(std::vector<Process>& processes) {
    //yet to be implemented
}
void SRF(std::vector<Process>& processes) {
    //yet to be implemented
}
void Round_Robin(std::vector<Process>& processes) {
    //yet to be implemented
}
void Preemptive_Priority(std::vector<Process>& processes) {
    //yet to be implemented
}
