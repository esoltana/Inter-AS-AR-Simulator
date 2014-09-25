/* 
 * File:   Initializer.h
 * Author: es3ce
 *
 * Created on September 23, 2014, 10:54 AM
 */
#include <string>
#include "DataStructures.h"
using namespace std;

class Initializer {
public:
    Initializer();
    void readSimulationParam(string path);
    void readARsystemParams();
    void readASnumber();
    void readDelayFile(DelayStruc Delays);
    
    int num_of_ASes;
    
    /*Simulation related parameters*/
    //use simulation time to continue receiving new calls from that time
    int simulation_time;
    // number of runs
    int numOfRun;
    //the file path of topology input parameters
    string topology_path;
    //the file path of call generation input parameters
    string call_gen_path;
    //the file path of AR system input parameters
    string AR_param_path;
    
    
    /*AR System related parameters*/
    // Duration of timeslot in second
    int single_TimeSlot_size;
    // length of AR window(seconds)
    int AR_TimeWindow_size;
     //Lead time in timeslots
    int lead_time;
    
    
private:

};

