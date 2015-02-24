void SRF(std::queue<Process>& ready_queue, std::list<Process>& io_list) {
    std::vector<Process> processes_exiting_io;
    std::vector<Process> running_queue;
    //initially ordered in terms of priority
    order_by_remaining(ready_queue);
    std::queue<Process> tmp_queue = ready_queue;
    // print out the entry processes
    while(!tmp_queue.empty()) {
        tmp_queue.front().print();
        tmp_queue.pop();
    }
    // running the processes until no CPUBound Process left
    while (getCPUBoundNO(ready_queue,io_list, running_queue) > 0) {
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
                //CPUs[i].runNewProcess();
                CPUs[i].continueCurrentProcess(io_list, time_elapse, tcs);
            } else if (CPUs[i].getStatus() == "preempt_remove_process"){
                //CPUs[i].increaseRunTime();
                CPUs[i].setStatus("preempt_move_in_process");
            } else if (CPUs[i].getStatus() == "preempt_move_in_process"){
                //CPUs[i].increaseRunTime();
                CPUs[i].setStatus("ready_to_run_new_process");
            } else if(CPUs[i].getStatus()=="running"){
                //if running process has a lower priority than
                //the newly arrived process
                CPUs[i].continueCurrentProcess(io_list, time_elapse, tcs);
            }
        }
        //update running queue
        running_queue.clear();
        for (int i = 0; i < num_CPU; i++) {
            running_queue.push_back(CPUs[i].getRunningProcess());
        }
        do_io(io_list);//run processes in io list for 1ms
        //increase waiting time of all processes in ready_queue
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
            std::sort(processes_exiting_io.begin(),processes_exiting_io.end(),sort_queue_by_priority);
            int num_processes_exiting_io = processes_exiting_io.size();
            for (int j = 0; j < num_processes_exiting_io;j++) {
                for (int i = 0; i < num_CPU; i++) {
                    if (CPUs[i].getRunningProcess().getPriority() > processes_exiting_io[j].getPriority() &&
                        CPUs[i].getStatus()!="idle") {
                        Process preempted_process = CPUs[i].preempt(processes_exiting_io[j], tcs);
                        processes_exiting_io[j] = preempted_process;
                        break;
                    }
                }
            }
            std::sort(processes_exiting_io.begin(),processes_exiting_io.end(),sort_queue_by_priority);
            for (int i = 0; i < processes_exiting_io.size(); i++) {
                ready_queue.push(processes_exiting_io[i]);
            }
            processes_exiting_io.clear();
        }
    }
}
   

