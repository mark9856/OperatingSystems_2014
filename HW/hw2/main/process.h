#ifndef Process_h_
#define Process_h_
#include <string>
#include <queue>

class Process {
    public:
        // CONSTRUCTOR
        Process(std::string t);
        void SetPriority(int p);
        void Aging();
        std::string type_;

    private:
        std::string type;
        int priority;           // Priority of process
        int turn_around_time;   // Turnaround time
        int total_wait_time;    // Total wait time
        int CPU_burst;
        int IO_burst;

};

//int getCPUBoundNO(std::queue<Process> processes);

#endif
