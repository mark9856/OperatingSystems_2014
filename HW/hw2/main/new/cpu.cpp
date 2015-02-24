#include <iostream>
#include "cpu.h"

CPU::CPU(int id){
    CPUID = id;
    status = "idle";
    run_time = 0;
    idle_time = 0;
}

void CPU::loadNewProcess(Process p){
    // status should not be idle
    if(status != "idle"){
        return;
    }
    status = "ready_to_run_new_process";
    process_in_execution = p;  // set up process_in_execution equals to p
    //process_in_execution.increaseCPURunningTime();
    //time_elapse += tcs/2;//for move in the process
    //run_time += tcs/2;//for move in the process
    //time_elapse ++;//run for 1ms
    //run_time ++;//run for 1ms
    std::cout<<"Loading process "<<p.getID()<<"\n";
    return;
}

void CPU::runNewProcess(){
    // status should not be idle
    if(status != "ready_to_run_new_process"){
        return;
    }
    status = "running";
    //process_in_execution = p;  // set up process_in_execution equals to p
    process_in_execution.increaseCPURunningTime();
    //time_elapse += tcs/2;//for move in the process
    //run_time += tcs/2;//for move in the process
    //time_elapse ++;//run for 1ms
    run_time ++;//run for 1ms
    return;
}

void
CPU::continueCurrentProcess(std::list<Process>& io_list,  int & time_elapse, int tcs) {
    if (!process_in_execution.reachCPUBurst() ) {
        //if process in execution is not done with cpu burst
        process_in_execution.increaseCPURunningTime();//continue execution
        //std::cout<<"    !"<<process_in_execution.getID()<<"! ~"
        //         <<process_in_execution.getRemainingIOBurstTime()<<"~ *"
        //         <<process_in_execution.getRemainingCPUBurst()<<"*\n";

        status="running";
        run_time++;
    } else {
        //else if process in execution finishes cpu burst
        std::cout<<"[Time "<<time_elapse<<"] "
                 <<process_in_execution.getType()<<"("
                 <<process_in_execution.getID()
                 <<") CPU burst done\n";
        if(process_in_execution.getType()=="CPU") {
            //if it’s a cpu bound process
            process_in_execution.decreaseRemainingCPUBurst();
            std::cout<<"    "
                     <<process_in_execution.getType()<<"("
                     <<process_in_execution.getID()
                     <<") "<<process_in_execution.getRemainingCPUBurst()
                     <<"\n";
            if(process_in_execution.getRemainingCPUBurst()<=0) {
                //if cpu bound process finishes all 8 burst
                process_in_execution.reset();
                status="idle";
                //time_elapse += tcs/2;//for remove the process
                //run_time += tcs/2;//for remove the process
            } else {
                process_in_execution.resetCPURunningTime();
                process_in_execution.resetIORunningTime();
                io_list.push_back(process_in_execution);
                status="idle";
                //run_time += tcs/2;//for remove the process
            }
            status="idle";
        } else {
            //if it’s cpu bound and not finishes all bursts, or
            //it’s io bound process, go to I/O list
            process_in_execution.resetCPURunningTime();
            process_in_execution.resetIORunningTime();
            io_list.push_back(process_in_execution);
            status="idle";
            //run_time += tcs/2;
        }
        process_in_execution.reset();
    }
    return;
}

void
CPU::increaseRunTime() {
    //process_in_execution.increaseCPURunningTime();
    run_time++;
}

Process
CPU::preempt(Process p, int tcs) {
    Process tmp_p = process_in_execution;
    process_in_execution = p;
    //push the preempted process at the front of the queue
    status="preempt_remove_process";
    return tmp_p;
    //run_time += tcs;
}
