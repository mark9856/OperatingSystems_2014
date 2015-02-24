#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <queue>
#include "process.h"

Process::Process() {
    process_id = 0;
    type = "Undetermined";
    total_wait_time = 0;
    cpu_running_time = 0;
    io_running_time = 0;
    total_cpu_run_time = 0;
    run_time=0;
    priority = 65535;
    finish_time = 0;
}

Process::Process(std::string t, int ID){
    process_id = ID;
    type = t;
    total_wait_time = 0;
    cpu_running_time = 0;
    io_running_time = 0;
    run_time=0;
    total_cpu_run_time = 0;
    priority = rand() % 5;
    finish_time = 0;
    if(t == "IO"){
        CPU_burst = 20 + rand() % 180;
        IO_burst = 1000 + rand() % 3500;
    }
    else if(t == "CPU"){
        CPU_burst = 200 + rand() % 2800;
        IO_burst = 1200 + rand() % 2000;
        remaining_cpu_burst = 8;
    }
}

void
Process::reset() {
    process_id = 0;
    type = "Undetermined";
    total_wait_time = 0;
    cpu_running_time = 0;
    io_running_time = 0;
    total_cpu_run_time = 0;
    run_time=0;
    remaining_cpu_burst = 0;
    priority = 65535;
    finish_time = 0;
}

Process& Process::operator= (const Process & p){
    this -> process_id = p.process_id;
    this -> type = p.type;
    this -> total_wait_time = p.total_wait_time;
    this -> cpu_running_time = p.cpu_running_time;
    this -> io_running_time = p.io_running_time;
    this -> run_time = p.run_time;
    this -> priority = p.priority;
    this -> CPU_burst = p. CPU_burst;
    this -> IO_burst = p.IO_burst;
    this -> remaining_cpu_burst = p.remaining_cpu_burst;
    this -> total_cpu_run_time = p.total_cpu_run_time;
    return *this;
}

std::string Process::getType(){
    return type;
}

void Process::setPriority(int p){
    if(p < 0 || p > 4){
        std::cerr << "priority should be larger than 0 and smaller than 5" << std::endl;
    }
    else {
        priority = p;
    }
    return;
}

void Process::Aging(){
    if(priority > 0){
        priority--;
    }
}

void Process::print(){
    std::cout << type <<"("<<process_id<<")" << " Priority: "<< priority <<"  CPU_burst: " << CPU_burst << " IO_burst: " << IO_burst << std::endl;
}


void Process::setID(int i){
    if(i < 1){
        std::cerr << "id should be larger than 0" << std::endl;
    }
    else{
        process_id = i;
    }
    return;
}


void
Process::resetCPURunningTime() {
    cpu_running_time = 0;
}

int getCPUBoundNO(std::queue<Process> ready_queue, std::list<Process>& io_list, std::vector<Process>& running_queue){
    int count = 0;
    int id = 0;
    Process p;
    while(!ready_queue.empty()){
        if(ready_queue.front().getType() == "CPU"){
            count ++;
            id = ready_queue.front().getID();
            p = ready_queue.front();
        }
        ready_queue.pop();
    }
    std::list<Process>::iterator itr = io_list.begin();
    for(; itr != io_list.end(); itr++){
        if(itr->getType() == "CPU"){
            id = itr->getID();
            p = *itr;
            count ++;
        }
    }
    for (int i = 0; i < running_queue.size(); i++) {
        if(running_queue[i].getType()=="CPU") {
            id = running_queue[i].getID();
            p = running_queue[i];
            count++;
        }
    }
    return count;
}
