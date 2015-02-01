/* 
 * File:   Initializer.h
 * Author: es3ce
 *
 * Created on September 23, 2014, 10:54 AM
 */
#include <string>
#include "DataStructures.h"
#include "ARserver.h"
#include "callgenerator/CallGenerator.h"


using namespace std;

class Initializer {
public:
    Initializer(double arrRate, int simulationTime, string filename);
    void readSimulationParam(string path);
    void readARsystemParams();
    void readNumberOfASes();
    void readDelayFile(DelayStruc Delays);
    void simulateMsgPassing(int nodeNum[], double arrRate, int simulationTime, string filename);
    
    void generateFirstRoundCalls();
    
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
    double single_TimeSlot_size;
    // length of AR window(seconds)
    double AR_TimeWindow_size;
    
     //Lead time in timeslots
    int lead_time;
    double CBP;
/////////////Simulation related Variables    
     //define a vector to keep the AR servers of all the ASs
    vector<ARserver> ARSERVER_vector;
    //store number of ASes of all ASes in this array
   
    vector<CallGenerator> CallGenerator_vector;
    //Priority Queue to keep ARBGP update received msgs in order (Priority Queue needs comparateor function)
    priority_queue<ARBGP_Node, vector<ARBGP_Node>, MyComparatorARBGP> ARBGP_Q;
    //Priority Queue to keep Create reservation Request call from neighbor AR servers (EPCE) 
    priority_queue<Inter_AS_Call_Node, vector<Inter_AS_Call_Node>, MyComparatorInter_AS_Call> Inter_AS_call_Q;
    //Priority Queue to save Create reservation Request from call generator within the domain of that AR server
    priority_queue<Call_Node, vector<Call_Node>, MyComparatorCALL> GeneratedCALL_Q;
////////////////////////////    
    
private:

};

