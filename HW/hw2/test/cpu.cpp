#include <iostream>
#include "cpu.h"

CPU::CPU(int id){
    CPUID = id;
    status = "idle";
    run_time = 0;
}

//load the the process at the head of the queue to the CPU,
//but don't run just for now. Run it at next 1ms.
// O(1)
void CPU::loadNewProcess(Process p){
    // status should not be idle
    if(status != "idle"){
        return;
    }
    status = "ready_to_run_new_process";
    process_in_execution = p;  // set up process_in_execution equals to p
    return;
}

//If a process is loaded in the previous 1ms, run it now.
void CPU::runNewProcess(){
    // status should not be idle
    if(status != "ready_to_run_new_process"){
        return;
    }
    status = "running";
    //process_in_execution = p;  // set up process_in_execution equals to p
    process_in_execution.increaseCPURunningTime();
    run_time ++;//run for 1ms
    return;
}

//If there is a process running at the CPU, continue running it if
//it doens't reach CPU burst time.
// O(1)
void
CPU::continueCurrentProcess(std::list<Process>& io_list,
                            std::vector<Process> & finished_processes,
                            int time_elapse) {
    if (!process_in_execution.reachCPUBurst()) {
        //if process in execution is not done with cpu burst
        process_in_execution.increaseCPURunningTime();//continue execution
        status="running";
        run_time++;
    } else {
        //else if process in execution finishes cpu burst
        printCompletion(process_in_execution, time_elapse);
        if(process_in_execution.getType()=="CPU") {
            //if it’s a cpu bound process
            // O(1)
            process_in_execution.decreaseRemainingCPUBurst();
            if(process_in_execution.getRemainingCPUBurst()==0) {
                //if cpu bound process finishes all 8 burst
                process_in_execution.setFinishTime(time_elapse);
                finished_processes.push_back(process_in_execution);
                printTermination(process_in_execution, time_elapse);
                process_in_execution.reset();
            } else {
                process_in_execution.resetCPURunningTime();
                process_in_execution.resetIORunningTime();
                io_list.push_back(process_in_execution);
            }
        } else {
            //if it’s cpu bound and not finishes all bursts, or
            //it’s io bound process, go to I/O list
            process_in_execution.resetCPURunningTime();
            process_in_execution.resetIORunningTime();
            io_list.push_back(process_in_execution);
        }
        status="idle";
        process_in_execution.reset();
    }
    return;
}

void
CPU::increaseRunTime() {
    run_time++;
}

//Change the current running process to be the newly preempting process.
Process
CPU::preempt(Process p) {
    Process tmp_p = process_in_execution;
    process_in_execution = p;
    //push the preempted process at the front of the queue
    status="preempt_remove_process";
    return tmp_p;
}

// print out process CPU burst completion
void printCompletion(Process & p, int & time_elapse){
    std::cout << "[time " << time_elapse << "ms] ";
    if(p.getType() == "IO"){
        std::cout << "Interactive ";
    }
    else{
        std::cout << "CPU-bound ";
    }
    std::cout << "process ID " << p.getID() << " CPU burst done ( turnaround time "
    << p.getTurnaroundTime() << "ms, total wait time " << p.getWaitingTime() << ")"
    << std::endl;
}

// print out process termination
void printTermination(Process & p, int & time_elapse){
    std::cout << "[time " << time_elapse << "ms] CPU-bound process ID " << p.getID()
    << " terminated ( avg turnaround time " << p.getTurnaroundTime()/8
    << " ms, avg total wait time " << p.getWaitingTime()/8 << " ms) " << std::endl;
}



