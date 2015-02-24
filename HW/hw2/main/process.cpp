#include <stdio.h>
#include <string>
#include <queue>

#include "process.h"
Process::Process(std::string t){
    type = t;
    turn_around_time = 0;
    total_wait_time = 0;
    priority = rand%5;
    if(t == "CPU"){
        CPU_burst = 20 + rand%180;
        IO_burst = 1000 + rand%3500;
    }
    else if(t == "IO"){
        CPU_burst = 200 + rand%2800;
        IO_burst = 1200 + rand%2000;
    }
}

void Process::SetPriority(int p){
    if(p < 0 || p > 4){
        std::cerr << "priority should be larger than 0 and smaller than 5" << std::endl;
    }
    else {
        priority = p;
    }
    return;
}

void Process::Aging(){
    if(priority <= 0){
        std::cerr << "priority is lower than 1" << std::endl;
    }
    else{
        priority --;
    }
    return;
}
