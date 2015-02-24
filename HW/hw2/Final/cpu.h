#ifndef CPU_h_
#define CPU_h_
#include <queue>
#include <string>
#include "process.h"

class CPU {
public:
    CPU(int id);
    bool isIdle() {return (status=="idle");}
    void setStatus(std::string s) {status=s;}
    std::string getStatus() {return status;}
    void loadNewProcess(Process p);
    void runNewProcess();
    void continueCurrentProcess(std::list<Process>& io_list,
                                std::vector<Process> & finished_processes,
                                int time_elapse);
    Process preempt(Process p);
    void increaseRunTime();
    Process getRunningProcess() {return process_in_execution;}
    int getRunTime() {return run_time;}
    
private:
    int CPUID;
    std::string status;
    int run_time;
    Process process_in_execution;
    
};

// print out process CPU burst completion
//[time 5299ms] CPU-bound process ID 7 CPU burst done
//(turnaround time 5299ms, total wait time 3666ms)
void printCompletion(Process & p, int & time_elapse);
// print out process termination
//[time 7989ms] CPU-bound process ID 5 terminated
//(avg turnaround time 587ms, avg total wait time 155ms)
void printTermination(Process & p, int & time_elapse);

#endif
