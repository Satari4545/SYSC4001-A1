/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include<interrupts.hpp>

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/
    
    int time = 0;
    int context_time = 10;

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        
        // Skip if activity is not understood
        if (activity == "null" || duration_intr == -1) {
            continue;
        }

        // if activity is CPU 
        if (activity == "CPU") {
            execution += std::to_string(time) + ", " + std::to_string(duration_intr) + ", CPU Burst\n";
            time += duration_intr;
        }
        // if activity is SYSCALL
        else if (activity == "SYSCALL") {
            int device_id = duration_intr;
            
            // Use helper function for interrupt overhead
            auto [interrupt_steps, updated_time] = intr_boilerplate(time, device_id, context_time, vectors);
            execution += interrupt_steps;
            time = updated_time;

            // Run the device driver
            execution += std::to_string(time) + ", " + std::to_string(40) + 
            ", SYSCALL: run the ISR (device driver)\n";
            time += 40;

            // Transfer data between device and memory
            execution += std::to_string(time) + ", " + std::to_string(40) + 
            ", transfer data from device to memory\n";
            time += 40;
            
            // Check for errors 
            int remaining_time = delays[device_id] - 80;
            execution += std::to_string(time) + ", " + std::to_string(remaining_time) + 
            ", check for errors\n";
            time += remaining_time;

            // IRET to return from interrupt
            execution += std::to_string(time) + ", " + std::to_string(1) + ", IRET\n";
            time += 1;

        }
        // if activity is END_IO
        else if (activity == "END_IO") {
            int device_id = duration_intr;
            
            // Use helper function for interrupt overhead
            auto [interrupt_steps, updated_time] = intr_boilerplate(time, device_id, context_time, vectors);
            execution += interrupt_steps;
            time = updated_time;

            // Run the device driver
            execution += std::to_string(time) + ", " + std::to_string(40) + 
            ", ENDIO: run the ISR (device driver)\n";
            time += 40;

            // Check device status 
            int remaining_time = delays[device_id] - 40;
            execution += std::to_string(time) + ", " + std::to_string(remaining_time) + 
            ", check device status\n";
            time += remaining_time;

            // IRET to return from interrupt
            execution += std::to_string(time) + ", " + std::to_string(1) + ", IRET\n";
            time += 1;
        }
        
        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
