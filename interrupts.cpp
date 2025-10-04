/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 * @author Tony Yao
 * @author Wenxuan Han 101256669 
 */

#include "interrupts.hpp"


int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);
    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/
    int current_time = 0;
    int context_save_time = 10;
    int each_activity_within_ISR = 40; 
    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        if (activity == ""){
            continue;
        }

        if (activity == "CPU") {
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", CPU Burst\n";
            current_time += duration_intr;
        }
        else if (activity == "SYSCALL")
        {
            auto [new_execution, updated_time] = intr_boilerplate(current_time, duration_intr, context_save_time, vectors);
            execution += new_execution; 
            current_time = updated_time;

            if (duration_intr < 0 || duration_intr >= delays.size())
            {
                continue;
            } 
            
            int amount_activity_ISR = 0; 
            execution += std::to_string(current_time) + ", " + std::to_string(each_activity_within_ISR) + ", SYSCALL: run the ISR (device driver)\n"; 
            current_time += each_activity_within_ISR; 
            amount_activity_ISR++; 

            execution += std::to_string(current_time) + ", " + std::to_string(each_activity_within_ISR) + ", transfer data from device to memory\n"; 
            current_time += each_activity_within_ISR; 
            amount_activity_ISR++; 

            int remain_activity_time_within_ISR; 
            remain_activity_time_within_ISR = delays.at(duration_intr) - (amount_activity_ISR * each_activity_within_ISR); 

            if (remain_activity_time_within_ISR < 0)
            {
                continue;
            }
            
            execution += std::to_string(current_time) + ", " + std::to_string(remain_activity_time_within_ISR) + ", check for errors\n"; 
            current_time += remain_activity_time_within_ISR;

        }
        else if (activity == "END_IO") {

            auto [new_execution, updated_time] = intr_boilerplate(current_time, duration_intr, context_save_time, vectors);
            execution += new_execution;
            current_time = updated_time; 

            if (duration_intr < 0 || duration_intr >= delays.size())
            {
                continue;
            }

            int amount_activity = 0; 

            execution += std::to_string(current_time) + ", " + std::to_string(each_activity_within_ISR) + ", END_IO: run the ISR(device driver)\n"; 
            current_time += each_activity_within_ISR; 
            amount_activity++;

            int device_number_delay_time = delays.at(duration_intr); 
            int remain_time_within_ISR = device_number_delay_time - (amount_activity * each_activity_within_ISR);

            if (remain_activity_time_within_ISR < 0)
            {
                continue;
            }
            
            execution += std::to_string(current_time) + ", " + std::to_string(remain_time_within_ISR) + ", check device status\n"; 
            current_time += remain_time_within_ISR; 
            
        }
        else { 
             std::cerr << "error is happened here so nothing to do unless OS let u to do work" << std::endl; 
        }


        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
