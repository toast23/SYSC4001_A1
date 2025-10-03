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

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        if (activity == ""){
            continue;
        }

        if (activity == "CPU") {
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", CPU execution\n";
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
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", SYSCALL: run the ISR\n"; 
            current_time += delays.at(duration_intr); 
            execution += std::to_string(current_time) + ", " + std::to_string(1) + ", IRET\n"; 
            current_time++; 
        }
        else if (activity == "END_IO") {
            execution += std::to_string(current_time) + ", " + std::to_string(1) + ", Check priority of interrput\n";
            current_time++; 
            execution += std::to_string(current_time) + ", " + std::to_string(1) + ", check if masked\n";
            current_time++; 

            auto [new_execution, updated_time] = intr_boilerplate(current_time, duration_intr, context_save_time, vectors);
            execution += new_execution;
            current_time = updated_time; 

            if (duration_intr < 0 || duration_intr >= delays.size())
            {
                continue;
            }

            int device_number_delay_time = delays.at(duration_intr); 
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", END_IO: run the ISR\n"; 
            current_time += device_number_delay_time; 
            execution += std::to_string(current_time) + ", " + std::to_string(1) + ", IRET\n"; 
            current_time++; 
            
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
