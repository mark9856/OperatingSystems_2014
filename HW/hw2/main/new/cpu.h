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
    void continueCurrentProcess(std::list<Process>& io_list,  int & time_elapse, int tcs);
    Process preempt(Process p, int tcs);
    void increaseRunTime();
    Process getRunningProcess() {return process_in_execution;}
    
private:
    int CPUID;
    std::string status;  //status includes: vacant, busy, csIn, csOut etc.// enum status {vacant, busy, csIn, csOut}//
    int run_time;
    int idle_time;
    Process process_in_execution;
    
};

#endif

