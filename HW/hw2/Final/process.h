#ifndef Process_h_
#define Process_h_
#include <string>
#include <queue>
#include <list>

class Process {
public:
    // CONSTRUCTOR
    Process();
    Process(std::string t, int ID);
    Process& operator= (const Process & p);
    std::string getType();
    void setPriority(int p);
    int getPriority() {return priority;}
    void setFinishTime(int f) {finish_time=f;}
    void Aging();
    void print();
    void setID(int i);
    int cpuBurst() {return CPU_burst;}
    int ioBurst() {return IO_burst;}
    void increaseCPURunningTime() {cpu_running_time++;run_time++;total_cpu_run_time++;}
    void increaseIORunningTime() {io_running_time++;run_time++;}
    void increaseWaitingTime() {total_wait_time++;}
    void decreaseRemainingCPUBurst() {remaining_cpu_burst--;}
    void increaseTotalCPURunTime() {total_cpu_run_time++;}
    bool reachCPUBurst() {return cpu_running_time == CPU_burst;}
    bool reachIOBurst() {return io_running_time == IO_burst;}
    void resetCPURunningTime();
    void resetIORunningTime() {io_running_time=0;}
    int getRemainingCPUBurst() {return remaining_cpu_burst;}
    int getRemainingCPUBurstTime() {return CPU_burst - cpu_running_time;}
    int getRemainingIOBurstTime() {return IO_burst - io_running_time;}
    int getWaitingTime() {return finish_time - total_cpu_run_time;}
    int getTurnaroundTime() {return finish_time - 0;}
    int getTotalCPURunTime() {return total_cpu_run_time;}
    int getFinishTime() {return finish_time;}
    int getID() {return process_id;}
    int getRunTime() {return run_time;}
    void reset();
    
    
private:
    std::string type;
    int process_id;
    int priority;// Priority of process
    int total_wait_time;    // Total wait time
    int total_cpu_run_time;
    int run_time;
    int CPU_burst;
    int IO_burst;
    int remaining_cpu_burst;
    int cpu_running_time;
    int io_running_time;
    int finish_time;
    
};

// return the number of CPU bound processes.
int getCPUBoundNO(std::queue<Process> ready_queue, std::list<Process>& io_list, std::vector<Process> &running_queue);
#endif
