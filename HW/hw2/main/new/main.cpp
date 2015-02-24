#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <vector>
#include <queue>
#include <list>
#include <map>
#include "process.h"
#include "cpu.h"

//Time is in unit "ms"
//global variable with default values
int time_elapse = 0;
int tcs = 2;//context switching time
std::string algorithm = "SJF";
int timeslice = 100;
int num_processes = 12;
int num_CPU = 4;
std::vector<CPU> CPUs;

//function prototypes
bool valid_input(int argc, char* const argv[]);
bool algorithm_exists(std::string const & algorithm);
void create_cpus();
std::queue<Process> generate_processes();
void do_algorithm(std::queue<Process>& ready_queue, std::list<Process>& io_list);
//these following four functions are yet to be implemented
//Sisi will try to do some of these parts
void SJF(std::queue<Process>& ready_queue, std::list<Process>& io_list);
void SRF(std::queue<Process>& ready_queue, std::list<Process>& io_list);
void Round_Robin(std::queue<Process>& ready_queue, std::list<Process>& io_list);
void Preemptive_Priority(std::queue<Process>& ready_queue, std::list<Process>& io_list);
void order_by_priority(std::queue<Process>& ready_queue);
void order_by_remaining(std::queue<Process> & ready_queue);
void do_io(std::list<Process>& io_list);//run processes in io list for 1ms
void increase_waiting_time_with_aging(std::queue<Process>& ready_queue);
void increase_waiting_time(std::queue<Process>& ready_queue);
bool any_io_process_finishes(std::list<Process>& io_list, std::vector<Process> & processes_exiting_io);
bool sort_queue_by_priority(Process& p1, Process& p2);
bool sort_queue_by_remaining(Process& p1, Process& p2);


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
    //create cup objects
    create_cpus();
    //generate a vector of processes(80% cpu bound and 20% i/o bound)
    std::queue<Process> ready_queue = generate_processes();
    //print initial ready_queue
    /*
    std::queue<Process> tmp_queue = ready_queue;
    while(!tmp_queue.empty()) {
        tmp_queue.front().print();
        tmp_queue.pop();
    }
    */
    std::list<Process> io_list;
    do_algorithm(ready_queue,io_list);

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

void create_cpus() {
    for (int i = 0; i < num_CPU; i++) {
        CPU one_cpu(i);
        CPUs.push_back(one_cpu);
    }
}

std::queue<Process> generate_processes() {
    //20% are CPU bound, 80% are I/O bound
    int io_bound = num_processes * 0.8;
    int cpu_bound = num_processes - io_bound;
    int generated_process = 0;
    std::queue<Process> processes;
    while (generated_process < num_processes) {
        int cpu_or_io = rand() % 2;//1 for cpu, 0 for i/o
        if(cpu_or_io == 1 && cpu_bound > 0) {
            Process p("CPU",generated_process);
            processes.push(p);
            cpu_bound--;
        } else if(cpu_or_io == 0 && io_bound > 0) {
            Process p("IO",generated_process);
            processes.push(p);
            io_bound--;
        } else continue;
        generated_process++;
    }
    return processes;
}

void do_algorithm(std::queue<Process>& ready_queue, std::list<Process>& io_list) {
    if (algorithm == "SJF") SJF(ready_queue, io_list);
    else if (algorithm == "SRF") SRF(ready_queue, io_list);
    else if (algorithm == "RR") Round_Robin(ready_queue, io_list);
    else Preemptive_Priority(ready_queue, io_list);
    //algorithm type checked already
}

void SJF(std::queue<Process>& ready_queue, std::list<Process>& io_list) {
    //yet to be implemented
}
void SRF(std::queue<Process>& ready_queue, std::list<Process>& io_list) {
    std::vector<Process> processes_exiting_io;
    std::vector<Process> running_queue;
    //initially ordered in terms of Remaining time
    //order_by_priority(ready_queue);
    order_by_remaining(ready_queue);
    std::queue<Process> tmp_queue = ready_queue;
    // print out the entry processes
    while(!tmp_queue.empty()) {
        tmp_queue.front().print();
        tmp_queue.pop();
    }
    // running the processes until no CPUBound Process left
    while (getCPUBoundNO(ready_queue,io_list, running_queue) > 0) {
        //process.h non member function
        //run processes in CPUs
        for (int i = 0; i < num_CPU; i++) {
            if (CPUs[i].getStatus()=="idle") {
                if(!ready_queue.empty()) {
                    CPUs[i].loadNewProcess(ready_queue.front());
                    ready_queue.pop();
                }
            } else if (CPUs[i].getStatus() == "ready_to_run_new_process") {
                //if cpu is ready to run loaded process
                //CPUs[i].runNewProcess();
                CPUs[i].continueCurrentProcess(io_list, time_elapse, tcs);
                std::cout<<"[Time "<<time_elapse<<"] "
                <<CPUs[i].getRunningProcess().getType() <<"("
                <<CPUs[i].getRunningProcess().getID()<<")"
                <<" begins CPU burst\n";
            } else if (CPUs[i].getStatus() == "preempt_remove_process"){
                //CPUs[i].increaseRunTime();
                CPUs[i].setStatus("preempt_move_in_process");
            } else if (CPUs[i].getStatus() == "preempt_move_in_process"){
                //CPUs[i].increaseRunTime();
                CPUs[i].setStatus("ready_to_run_new_process");
            } else if(CPUs[i].getStatus()=="running"){
                //if running process has a lower priority than
                //the newly arrived process
                CPUs[i].continueCurrentProcess(io_list, time_elapse, tcs);
            }
        }
        //update running queue
        running_queue.clear();
        for (int i = 0; i < num_CPU; i++) {
            running_queue.push_back(CPUs[i].getRunningProcess());
        }
        do_io(io_list);//run processes in io list for 1ms
        //increase waiting time of all processes in ready_queue
        //increase_waiting_time_with_aging(ready_queue);
        increase_waiting_time(ready_queue);
        //increase time_elapse after run 1ms cpu processes and io process
        time_elapse++;
        //if any io process finishes, delete it from io list
        //and move it to process_exiting_io
        if(any_io_process_finishes(io_list, processes_exiting_io)) {
            //there can be multiple processes finishing io at the
            //same time. We need to rank them by priority and
            //check if do preemption at each of the CPU. The processes
            //that finished io but cannot preempt any CPUs will go to the
            //ready queue
            //check if running processes need to be preempted by the process_exiting_io
            //std::sort(processes_exiting_io.begin(),processes_exiting_io.end(),sort_queue_by_priority);
            std::sort(processes_exiting_io.begin(),processes_exiting_io.end(),sort_queue_by_remaining);
            int num_processes_exiting_io = processes_exiting_io.size();
            for (int j = 0; j < num_processes_exiting_io;j++) {
                for (int i = 0; i < num_CPU; i++) {
                    //if (CPUs[i].getRunningProcess().getPriority() > processes_exiting_io[j].getPriority() && CPUs[i].getStatus()!="idle") {
                    if (CPUs[i].getRunningProcess().getRemainingCPUBurstTime() > processes_exiting_io[j].getRemainingCPUBurstTime() && CPUs[i].getStatus()!="idle") {
                        Process preempted_process = CPUs[i].preempt(processes_exiting_io[j], tcs);
                        processes_exiting_io[j] = preempted_process;
                        break;
                    }
                }
            }
            //std::sort(processes_exiting_io.begin(),processes_exiting_io.end(),sort_queue_by_priority);
            std::sort(processes_exiting_io.begin(),processes_exiting_io.end(),sort_queue_by_remaining);
            for (int i = 0; i < processes_exiting_io.size(); i++) {
                ready_queue.push(processes_exiting_io[i]);
            }
            processes_exiting_io.clear();
        }
    }
}


void Round_Robin(std::queue<Process>& ready_queue, std::list<Process>& io_list) {
    //yet to be implemented
}
void Preemptive_Priority(std::queue<Process>& ready_queue,
                         std::list<Process>& io_list) {
    std::vector<Process> processes_exiting_io;
    std::vector<Process> running_queue;
    //initially ordered in terms of priority
    order_by_priority(ready_queue);
    std::queue<Process> tmp_queue = ready_queue;
    // print out the entry processes
    while(!tmp_queue.empty()) {
        tmp_queue.front().print();
        tmp_queue.pop();
    }
    // running the processes until no CPUBound Process left
    while (getCPUBoundNO(ready_queue,io_list, running_queue) > 0) {
        //process.h non member function
        //run processes in CPUs
        for (int i = 0; i < num_CPU; i++) {
            if (CPUs[i].getStatus()=="idle") {
                if(!ready_queue.empty()) {
                    CPUs[i].loadNewProcess(ready_queue.front());
                    ready_queue.pop();
                }
            } else if (CPUs[i].getStatus() == "ready_to_run_new_process") {
                //if cpu is ready to run loaded process
                //CPUs[i].runNewProcess();
                CPUs[i].continueCurrentProcess(io_list, time_elapse, tcs);
                std::cout<<"[Time "<<time_elapse<<"] "
                <<CPUs[i].getRunningProcess().getType() <<"("
                <<CPUs[i].getRunningProcess().getID()<<")"
                <<" begins CPU burst\n";
            } else if (CPUs[i].getStatus() == "preempt_remove_process"){
                //CPUs[i].increaseRunTime();
                CPUs[i].setStatus("preempt_move_in_process");
            } else if (CPUs[i].getStatus() == "preempt_move_in_process"){
                //CPUs[i].increaseRunTime();
                CPUs[i].setStatus("ready_to_run_new_process");
            } else if(CPUs[i].getStatus()=="running"){
                //if running process has a lower priority than
                //the newly arrived process
                CPUs[i].continueCurrentProcess(io_list, time_elapse, tcs);
            }
        }
        //update running queue
        running_queue.clear();
        for (int i = 0; i < num_CPU; i++) {
            running_queue.push_back(CPUs[i].getRunningProcess());
        }
        do_io(io_list);//run processes in io list for 1ms
        //increase waiting time of all processes in ready_queue
        increase_waiting_time_with_aging(ready_queue);
        //increase time_elapse after run 1ms cpu processes and io process
        time_elapse++;
        //if any io process finishes, delete it from io list
        //and move it to process_exiting_io
        if(any_io_process_finishes(io_list, processes_exiting_io)) {
            //there can be multiple processes finishing io at the
            //same time. We need to rank them by priority and
            //check if do preemption at each of the CPU. The processes
            //that finished io but cannot preempt any CPUs will go to the
            //ready queue
            //check if running processes need to be preempted by the process_exiting_io
            std::sort(processes_exiting_io.begin(),processes_exiting_io.end(),sort_queue_by_priority);
            int num_processes_exiting_io = processes_exiting_io.size();
            for (int j = 0; j < num_processes_exiting_io;j++) {
                for (int i = 0; i < num_CPU; i++) {
                    if (CPUs[i].getRunningProcess().getPriority() > processes_exiting_io[j].getPriority() && CPUs[i].getStatus()!="idle") {
                        Process preempted_process = CPUs[i].preempt(processes_exiting_io[j], tcs);
                        processes_exiting_io[j] = preempted_process;
                        break;
                    }
                }
            }
            std::sort(processes_exiting_io.begin(),processes_exiting_io.end(),sort_queue_by_priority);
            for (int i = 0; i < processes_exiting_io.size(); i++) {
                ready_queue.push(processes_exiting_io[i]);
            }
            processes_exiting_io.clear();
        }
    }
}

void order_by_priority(std::queue<Process>& ready_queue) {
    std::vector<Process> ordered_queue;
    int N = ready_queue.size();
    for (int i = 0; i < N; i++) {
        ordered_queue.push_back(ready_queue.front());
        ready_queue.pop();
    }
    std::sort(ordered_queue.begin(),ordered_queue.end(),sort_queue_by_priority);
    for (int i = 0; i < ordered_queue.size(); i++) {
        ready_queue.push(ordered_queue[i]);
    }
}

void order_by_remaining(std::queue<Process> & ready_queue){
    std::vector<Process> ordered_queue;
    int N = ready_queue.size();
    for(int i = 0; i < N; i++){
        ordered_queue.push_back(ready_queue.front());
        ready_queue.pop();
    }
    std::sort(ordered_queue.begin(),ordered_queue.end(), sort_queue_by_remaining);
    for(int i = 0; i < ordered_queue.size(); i ++){
        ready_queue.push(ordered_queue[i]);
    }
}

bool sort_queue_by_priority(Process& p1, Process& p2) {
    return (p1.getPriority() < p2.getPriority());
}

bool sort_queue_by_remaining(Process& p1, Process& p2){
    return (p1.getRemainingCPUBurstTime() < p2.getRemainingCPUBurstTime());
}

void do_io(std::list<Process>& io_list) {//run processes in io list for 1ms
    std::list<Process>::iterator itr = io_list.begin();
    for (; itr != io_list.end(); itr++) {
        if (itr->getRemainingIOBurstTime()!=0) {
            itr->increaseIORunningTime();
        }
    }
}

void increase_waiting_time_with_aging(std::queue<Process>& ready_queue) {
    std::queue<Process> tmp_queue;
    while(!ready_queue.empty()) {
        tmp_queue.push(ready_queue.front());
        ready_queue.pop();
    }
    while(!tmp_queue.empty()) {
        tmp_queue.front().increaseWaitingTime();
        if(tmp_queue.front().getWaitingTime() % 1200 == 0) {
            tmp_queue.front().Aging();
        }
        ready_queue.push(tmp_queue.front());
        tmp_queue.pop();
    }
    
}

void increase_waiting_time(std::queue<Process>& ready_queue){
    std::queue<Process> tmp_queue;
    while(!ready_queue.empty()) {
        tmp_queue.push(ready_queue.front());
        ready_queue.pop();
    }
    while(!tmp_queue.empty()) {
        tmp_queue.front().increaseWaitingTime();
        ready_queue.push(tmp_queue.front());
        tmp_queue.pop();
    }
}

bool any_io_process_finishes(std::list<Process>& io_list, std::vector<Process> & processes_exiting_io) {
    std::list<Process>::iterator itr = io_list.begin();
    for (; itr != io_list.end();) {
        if(itr->reachIOBurst()) {
            itr->resetIORunningTime();
            processes_exiting_io.push_back(*itr);
            itr = io_list.erase(itr);
        } else {
            itr++;
        }
    }
    return (processes_exiting_io.size()!=0);
}
