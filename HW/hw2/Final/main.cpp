/*
 modification:
 1. Successfully set finish time for all cpu_bound and io_bound processes,
    so the turnaround time should be correct now.
 2. Simulation had been done.
 */

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <vector>
#include <queue>
#include <list>
#include <map>
#include <iomanip>
#include "process.h"
#include "cpu.h"

//Time is in unit "ms"
//global variable with default values
int time_elapse = 0;
std::string algorithm = "SJF";
int timeslice = 100;
int num_processes = 12;
int num_CPU = 4;
std::vector<CPU> CPUs;
std::vector<Process> finished_processes;

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
void order_by_remaining(std::queue<Process>& ready_queue);
void do_io(std::list<Process>& io_list);//run processes in io list for 1ms
void increase_waiting_time_with_aging(std::queue<Process>& ready_queue);
void increase_waiting_time(std::queue<Process>& ready_queue);
bool any_io_process_finishes(std::list<Process>& io_list, std::vector<Process> & processes_exiting_io);
bool sort_queue_by_priority(Process& p1, Process& p2);
bool sort_queue_by_remaining(Process& p1, Process& p2); 
void move_all_remaining_processes_to_finished_processes (std::queue<Process> &ready_queue,
                                                         std::list<Process> &io_list,
                                                         std::vector<Process> &finished_processes);
//print out sentence of process entry
//[time 0ms] Interactive process ID 1 entered ready queue (requires 188ms CPU time; priority 2)
//[time 0ms] CPU-bound process ID 2 entered ready queue (requires 2340ms CPU time; priority 3)
void printEntry(Process & p);
// print out context switch
//[time 4279ms] Context switch (swapping out process ID 4 for process ID 17)
void printSwitch(Process & exist, Process & replace);
// print out aging event
//[time 6100ms] Increased priority of CPU-bound process ID 12 to 2 due to aging
void printAging(Process & p);

//Analysis of turnaround times,total wait times, Average CPU utilization overall (across all CPUs)
//Average CPU utilization per each process
void simulation_analysis();


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
    std::list<Process> io_list;
    do_algorithm(ready_queue,io_list);
    simulation_analysis();
    
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
            printEntry(p);
            cpu_bound--;
        } else if(cpu_or_io == 0 && io_bound > 0) {
            Process p("IO",generated_process);
            processes.push(p);
            printEntry(p);
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
    //yet to be implemented
    std::vector<Process> processes_exiting_io;
    std::vector<Process> running_queue;
    //initially ordered in terms of priority
    //order_by_priority(ready_queue);
    order_by_remaining(ready_queue);
    std::queue<Process> tmp_queue = ready_queue;
    while (getCPUBoundNO(ready_queue, io_list, running_queue) > 0) {
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
                CPUs[i].continueCurrentProcess(io_list, finished_processes, time_elapse);
            } else if (CPUs[i].getStatus() == "preempt_remove_process"){
                CPUs[i].setStatus("preempt_move_in_process");
            } else if (CPUs[i].getStatus() == "preempt_move_in_process"){
                CPUs[i].setStatus("ready_to_run_new_process");
            } else if(CPUs[i].getStatus()=="running"){
                //if running process has a lower priority than
                //the newly arrived process
                CPUs[i].continueCurrentProcess(io_list, finished_processes,time_elapse);
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
                    if (CPUs[i].getRunningProcess().getRemainingCPUBurstTime() > processes_exiting_io[j].getRemainingCPUBurstTime() && CPUs[i].getStatus()!="idle") {
                        Process preempted_process = CPUs[i].preempt(processes_exiting_io[j]);
                        printSwitch(preempted_process, processes_exiting_io[j]);
                        processes_exiting_io[j] = preempted_process;
                        break;
                    }
                }
            }
            std::sort(processes_exiting_io.begin(),processes_exiting_io.end(),sort_queue_by_remaining);
            for (int i = 0; i < processes_exiting_io.size(); i++) {
                ready_queue.push(processes_exiting_io[i]);
                printEntry(processes_exiting_io[i]);
            }
            processes_exiting_io.clear();
        }
    }
    //move every io processes to the finished_processes vector
    
    move_all_remaining_processes_to_finished_processes(ready_queue, io_list, finished_processes);
    std::cout<<finished_processes.size()<<"\n";
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
    while (getCPUBoundNO(ready_queue, io_list, running_queue) > 0) {
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
                CPUs[i].continueCurrentProcess(io_list, finished_processes, time_elapse);
            } else if (CPUs[i].getStatus() == "preempt_remove_process"){
                CPUs[i].setStatus("preempt_move_in_process");
            } else if (CPUs[i].getStatus() == "preempt_move_in_process"){
                CPUs[i].setStatus("ready_to_run_new_process");
            } else if(CPUs[i].getStatus()=="running"){
                //if running process has a lower priority than
                //the newly arrived process
                CPUs[i].continueCurrentProcess(io_list, finished_processes,time_elapse);
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
                    if (CPUs[i].getRunningProcess().getPriority() > processes_exiting_io[j].getPriority() &&
                        CPUs[i].getStatus()!="idle") {
                        Process preempted_process = CPUs[i].preempt(processes_exiting_io[j]);
                        printSwitch(preempted_process, processes_exiting_io[j]);
                        processes_exiting_io[j] = preempted_process;
                        break;
                    }
                }
            }
            std::sort(processes_exiting_io.begin(),processes_exiting_io.end(),sort_queue_by_priority);
            for (int i = 0; i < processes_exiting_io.size(); i++) {
                ready_queue.push(processes_exiting_io[i]);
                printEntry(processes_exiting_io[i]);
            }
            processes_exiting_io.clear();
        }
    }
    //move every io processes to the finished_processes vector
    
    move_all_remaining_processes_to_finished_processes(ready_queue, io_list, finished_processes);
    std::cout<<finished_processes.size()<<"\n";
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

void order_by_remaining(std::queue<Process>& ready_queue) {
    std::vector<Process> ordered_queue;
    int N = ready_queue.size();
    for (int i = 0; i < N; i++) {
        ordered_queue.push_back(ready_queue.front());
        ready_queue.pop();
    }
    std::sort(ordered_queue.begin(),ordered_queue.end(),sort_queue_by_remaining);
    for (int i = 0; i < ordered_queue.size(); i++) {
        ready_queue.push(ordered_queue[i]);
    }
}

bool sort_queue_by_priority(Process& p1, Process& p2) {
    return (p1.getPriority() < p2.getPriority());
}

bool sort_queue_by_remaining(Process& p1, Process& p2) {
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
            printAging(tmp_queue.front());
        }
        ready_queue.push(tmp_queue.front());
        tmp_queue.pop();
    }
    
}

void increase_waiting_time(std::queue<Process>& ready_queue) {
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

void move_all_remaining_processes_to_finished_processes (std::queue<Process> &ready_queue,
                                                         std::list<Process> &io_list,
                                                         std::vector<Process> &finished_processes) {
    //move processes in CPUs to finished_processes
    for(int i = 0; i < num_CPU; i++) {
        if(CPUs[i].getRunningProcess().getType()=="IO") {
            CPUs[i].getRunningProcess().setFinishTime(time_elapse);
            finished_processes.push_back(CPUs[i].getRunningProcess());
        }
    }
    //move processes in ready_queue to finished_processes
    while(!ready_queue.empty()) {
        ready_queue.front().setFinishTime(time_elapse);
        finished_processes.push_back(ready_queue.front());
        ready_queue.pop();
    }
    //move processes in io_list to finished_processes
    std::list<Process>::iterator itr = io_list.begin();
    for(;itr!=io_list.end();itr++) {
        itr->setFinishTime(time_elapse);
        finished_processes.push_back(*itr);
    }
}

//printing
//print out sentence of process entry
void printEntry(Process & p){
    if(p.getType() == "IO"){
        std::cout << "[time " << time_elapse << "ms] Interactive process ID " << p.getID()
        << " entered ready queue (requires " << p.cpuBurst() << "ms CPU time";
    }
    else {
        std::cout << "[time " << time_elapse << "ms] CPU-bound process ID " << p.getID()
        << " entered ready queue (requires " << p.cpuBurst() << "ms CPU time";
    }
    if(algorithm == "PP"){
        std::cout << "; priority " << p.getPriority() << ")" << std::endl;
    }
    else{
        std::cout << ")" << std::endl;
    }
    return;
}
// print out context switch
void printSwitch(Process & exist, Process & replace){
    std::cout << "[time " << time_elapse << "ms] Context switch (swapping out process ID "
    << exist.getID() << " for process ID " << replace.getID() << ")" << std::endl;
    return;
}

// print out aging event
void printAging(Process & p){
    std::cout << "[time " << time_elapse << "ms] Increased priority of ";
    if(p.getType() == "IO"){
        std::cout << "Interactive ";
    }
    else{
        std::cout << "CPU-bound ";
    }
    std::cout << " process ID " << p.getID() << " to " << p.getPriority() << "due to aging"
    << std::endl;
}

// the Analysis of the simulation
void simulation_analysis() {
    //yet to be implemented
 
    int Turnaround_min = 99999;
    int Turnaround_max = 0;
    int Turnaround_total = 0;
    int Turnaround_avg = 0;

    int Total_wait_min = 99999;
    int Total_wait_max = 0;
    int Total_wait_total = 0;
    int Total_wait_avg = 0;
    // given values to different variables
    for(int i = 0; i < finished_processes.size(); i ++){
        if(Turnaround_min > finished_processes[i].getTurnaroundTime()){
            Turnaround_min = finished_processes[i].getTurnaroundTime();
        }
        if(Turnaround_max < finished_processes[i].getTurnaroundTime()){
            Turnaround_max = finished_processes[i].getTurnaroundTime();
        }
        Turnaround_total += finished_processes[i].getTurnaroundTime();

        if(Total_wait_min > finished_processes[i].getWaitingTime()){
            Total_wait_min = finished_processes[i].getWaitingTime();
        }
        if(Total_wait_max < finished_processes[i].getWaitingTime()){
            Total_wait_max = finished_processes[i].getWaitingTime();
        }
        Total_wait_total += finished_processes[i].getWaitingTime();
        std::cout << finished_processes[i].getFinishTime() << std::endl;
    }
    // get Average time
    Turnaround_avg = (int)(Turnaround_total/finished_processes.size());
    Total_wait_avg = (int)(Total_wait_total/finished_processes.size());

    // CPU utilization percentage
    float utilization = 0;
    for(int i = 0; i < CPUs.size(); i ++){
        utilization += 100 * (float)CPUs[i].getRunTime()/(float)time_elapse;
    }
    float avg_cpu = utilization/(float)CPUs.size();



    // print out Analysis
    std::cout << std::setprecision(5) << "Turnaround time: min " << Turnaround_min 
        << " ms; avg " << Turnaround_avg << " ms; max " << Turnaround_max << " ms " << std::endl 
        << "Total wait time: min " << Total_wait_min << " ms; avg "
        << Total_wait_avg << " ms; max " << Total_wait_max << " ms " << std::endl 
        << "Average CPU utilization: " << avg_cpu << "%" << std::endl
        << "Average CPU utilization per process: " << std::endl;

    // print out Analysis of 
    for(int i = 0; i < finished_processes.size(); i ++ ){
        std::cout << std::setprecision(5) << "process ID " << finished_processes[i].getID() << " : " 
            << (float)(100 * (float)finished_processes[i].getTotalCPURunTime()/
            (float)finished_processes[i].getTurnaroundTime()) << "%" << finished_processes[i].getTurnaroundTime() << std::endl;
    }

    return;


}


