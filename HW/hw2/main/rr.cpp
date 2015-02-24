#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <queue>
#include "process.h"

float time_elapse = 0.0;
std::string algorithm = "SJF";
float timeslice = 100;
int num_processes = 12;
int num_CPU = 4;

std::queue<Process> generate_processes() {
    //80% are CPU bound, 20% are I/O bound
    int cpu_bound = num_processes * 0.8;
    int io_bound = num_processes - cpu_bound;
    int generated_process = 0;
    std::queue<Process> processes;
    
    //Process p("CPU");
    //std::cout << cpu_bound << " " << io_bound << std::endl;
    while (generated_process < num_processes) {
        int cpu_or_io = rand() % 2;//1 for cpu, 0 for i/o
        //std::cout << cpu_or_io << std::endl;
        if(cpu_or_io == 1 && cpu_bound > 0) {
            /*
            Process p("CPU");
            processes.push(p);
            cpu_bound--;
            */
        } else if(cpu_or_io == 0 && io_bound > 0) {
            Process p("IO");
            /*
            processes.push(p);
            io_bound--;
            */
        } else continue;
        generated_process++;
    }
    return processes;
}

int main(){
    Process p("CPU");
    return 0;
}

