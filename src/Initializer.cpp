/* 
 * File:   Initializer.cpp
 * Author: es3ce
 * 
 * Created on September 23, 2014, 10:54 AM
 */

#include "Initializer.h"

#include <limits>
#include <set>
#include <map>
#include <queue>
#include <list>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include "DataStructures.h"
#include "ARserver.h"
#include "readcall.h"
#include "callgenerator/CallGenerator.h"
#include "randgen/randgen.h"

#define TIMETAG 1
#define BGPTAG 2
#define CALLTAG 3
#define MRAI 5

using namespace std;

Initializer::Initializer() {
    
    
    readSimulationParam("InputParameters//Simulation-related-input-params");
    
    readARsystemParams();
    
    //TODO: readNumberOfASes())
    readASnumber();
    
    //A data structure to keep delay
    DelayStruc Delays(num_of_ASes);
    
    //read the delays of different Inter AS links
    readDelayFile(Delays);
    
    //define a vector to keep the AR servers of all the ASs
    vector<ARserver> ARSERVER_vector;
    
    //store number of ASes of all ASes in this array
    int nodeNum[num_of_ASes];   
    
    //Create the required modules for each AS (ARServer which includes: Scheduler, ARBGP, IPCE and EPCE) for each AS
    for (int AS_ID= 1; AS_ID <= num_of_ASes; AS_ID++) {
        
        //TODO: what are the required input values of ARServer class
        //make an ARServer instance for each AS
        ARserver ARServer = ARserver(AS_ID, AR_TimeWindow_size, 60,topology_path);
        
        
        //save nodeNume in the array
        nodeNum[AS_ID-1]=ARServer.numOfNodes;
        
       //Add the AR server to the related vector
        ARSERVER_vector.push_back(ARServer);
    }

   
    
    //create vector of call generators
    vector<CallGenerator> CallGenerator_vector;
    for (int i = 1; i <= num_of_ASes; i++) {
        CallGenerator callGen = CallGenerator(i);
        callGen.readNodeVector(nodeNum);
        //TODO: In future every AS should have its own call-gen parameters
        callGen.readCommonFile(call_gen_path);
        CallGenerator_vector.push_back(callGen);
    }
    
    //TODO: call generator does not produce different calls for diff prob matrices.
    
    //Priority Queue to keep ARBGP msgs in order (Priority Queue needs comparateor function)
    priority_queue<ARBGP_Node, vector<ARBGP_Node>, MyComparatorARBGP> ARBGP_Q;
    //Priority Queue to keep Create reservation Request call from neighbor AR servers (EPCE) 
    priority_queue<ARSchedule_Node, vector<ARSchedule_Node>, MyComparatorARSchedule> ARSchedule_Q;
    //Priority Queue to save Create reservation Request from call generator within the domain of that AR server
    priority_queue<Call_Node, vector<Call_Node>, MyComparatorCALL> CALL_Q;
    
    
    //generate the first round of calls
    //for each AS, execute the call generator to generate a request
    for (int i = 0; i < CallGenerator_vector.size(); i++) {
        Call_Node tmp;
        //TODO: it should be changed. generateCall should receive the arrival rate and produce arrival time within it

        //generate a call according to input parameters
        CallGenerator_vector[i].generateCall(0,AR_TimeWindow_size,lead_time,single_TimeSlot_size,1);
        //save the produced request parameters in Queue
        tmp.arrival_time = CallGenerator_vector[i].arrival_time;
        tmp.from_AS = i + 1;
        tmp.from_node = CallGenerator_vector[i].source_node;
        tmp.to_AS = CallGenerator_vector[i].dest_AS;
        tmp.to_node = CallGenerator_vector[i].dest_node;
        tmp.duration = CallGenerator_vector[i].Duration;
        tmp.capacity = CallGenerator_vector[i].Capacity;
        //also save the AR options in the queue
        for (int j = 0; j < CallGenerator_vector[i].ARvec.size(); j++)
            tmp.AR_vec.push_back(CallGenerator_vector[i].ARvec[j]);
        //push the request in Call Queue
        CALL_Q.push(tmp);
    }

    //the arrival time of earliest Routing request call
    double ReqCall_earliest_time = 0;
    //get the execution time of earliest routing request from Call queue
    ReqCall_earliest_time = CALL_Q.top().arrival_time;
  
    //the arrival time of sending BGP Update
    double BGP_update_time = 0;
    double current_time = 0;
    
    //main loop
    //until the request time is within simulation time continue
    while (current_time <= simulation_time) {
        
        //Variables for saving the earliest time of ARBGP calls and ARSchedule
        double ARBGP_earliest_time = DBL_MAX;
        double ARSchedule_earliest_time = DBL_MAX;
        //variable to show which time of call should be executed now
        int case_flag;
        
        //TODO: the names in Graph Elements should be changed
        //get the execution time of earliest call in ARBGP Queue
        if (!ARBGP_Q.empty()) {
            ARBGP_earliest_time = ARBGP_Q.top().do_update_time;
        }
        
        //get the execution time of earliest call in ARSchedule Queue
        if(!ARSchedule_Q.empty())
        {
            ARSchedule_earliest_time = ARSchedule_Q.top().do_decision_time;
        }
       //Check if ARBGP time is the smallest one
        if (ARBGP_earliest_time <= ReqCall_earliest_time && ARBGP_earliest_time <= ARSchedule_earliest_time && ARBGP_earliest_time <= BGP_update_time)
            case_flag = 1;
        //check if ARSchedule time is the smallest
        else if (ARSchedule_earliest_time <= ReqCall_earliest_time && ARSchedule_earliest_time <= ARBGP_earliest_time && ARSchedule_earliest_time <= BGP_update_time)
            case_flag = 2;
        //check if routing Request Call is the smallest
        else if (ReqCall_earliest_time <= ARBGP_earliest_time && ReqCall_earliest_time <= ARSchedule_earliest_time && ReqCall_earliest_time <= BGP_update_time)
            case_flag = 3;
        else
            //else the smallest in BGP update message
            case_flag = 4;

        switch (case_flag) {
            case 1:
                //do ARBGP update
                
                //update the current time to the current executing call
                current_time = ARBGP_Q.top().do_update_time;
                
                //Action: print the ARBGP call
                cout << "do BGP update at:" << ARBGP_Q.top().do_update_time << endl;
                //TODO: Study delailed about what this function does.
                ARSERVER_vector[ARBGP_Q.top().to_AS - 1].AR_BGP.recvUpdate(ARBGP_Q.top().from_AS, ARBGP_Q.top().NLRI_vector);
                //pop the executed call
                ARBGP_Q.pop();

                break;
            case 2:
            {
                //do AR Schedule Call
                //update the current time to the current executing call
                current_time = ARSchedule_Q.top().do_decision_time;
             
                if(ARSchedule_Q.top().from_AS == ARSchedule_Q.top().to_AS)
                {
                    //intra call
                    //can be reserved
                    if(ARSERVER_vector[ARSchedule_Q.top().from_AS -1].IPCE_module.findPathAndReserv(ARSchedule_Q.top().from_node,ARSchedule_Q.top().to_node,ARSchedule_Q.top().capacity,ARSchedule_Q.top().duration,ARSchedule_Q.top().AR_vector[ARSchedule_Q.top().AR_time]))
                    {
                        cout<<"One reservation made!"<<endl;
                        ARSchedule_Q.pop();
                    }
                    else
                    {
                        if(ARSchedule_Q.top().AR_time < ARSchedule_Q.top().AR_vector.size()-1)
                        {
                            ARSchedule_Node tmp(ARSchedule_Q.top().from_AS,ARSchedule_Q.top().from_node,ARSchedule_Q.top().to_AS,ARSchedule_Q.top().to_node,ARSchedule_Q.top().send_time,ARSchedule_Q.top().total_delay,ARSchedule_Q.top().capacity,ARSchedule_Q.top().duration,ARSchedule_Q.top().AR_time+1,ARSchedule_Q.top().AS_path,ARSchedule_Q.top().AR_vector);
                            ARSchedule_Q.pop();
                            ARSchedule_Q.push(tmp);
                        }
                        else
                            ARSchedule_Q.pop();
                    }
                }
                else
                {
                    //inter call
                    cout<<"inter AS calls not tested!"<<endl;
                }
                break;
            }
            case 3:
            {
                //accept a call
                //cout << "process call:" << ReqCall_earliest_time << endl;
                /*
                call_count++;
                if (call_count >= READCALL.linecount) {
                    //the end
                    for (int i = 0; i < ARSERVER_vector.size(); i++) {
                        int as = i + 1;
                        cout << "AS" << as << ":" << endl;

                        for (std::map<Node, vector<NLRI> >::iterator iter = ARSERVER_vector[i].AR_BGP.RIB.begin(); iter != ARSERVER_vector[i].AR_BGP.RIB.end(); ++iter) {
                            int asnum = iter->first.as_num;
                            int node_num = iter->first.vertex_num;
                            cout << asnum << " " << node_num << ":";
                            for (int i = 0; i < iter->second.size(); i++) {
                                cout << iter->second[i].toString() << " ";
                            }
                            cout << endl;

                        }

                    }
                    return 1;
                }
                ReqCall_earliest_time = READCALL.callarrive[call_count];
                current_time = ReqCall_earliest_time;
                 */
                current_time = CALL_Q.top().arrival_time;
                cout << "process a call:" << CALL_Q.top().arrival_time << " " << CALL_Q.top().capacity << " " << CALL_Q.top().duration << " ";
                for (int i = 0; i < CALL_Q.top().AR_vec.size(); i++) {
                    cout << CALL_Q.top().AR_vec[i] << " ";
                }
                cout << CALL_Q.top().from_AS << " " << CALL_Q.top().from_node << " " << CALL_Q.top().to_AS << " " << CALL_Q.top().to_node << endl;
                //push this call into the ARSchedule_Q
                ARSchedule_Node Sche_Node;
                Sche_Node.from_AS = CALL_Q.top().from_AS;
                Sche_Node.to_AS = CALL_Q.top().to_AS;
                Sche_Node.send_time = CALL_Q.top().arrival_time;
                Sche_Node.total_delay = 0;
                Sche_Node.do_decision_time = CALL_Q.top().arrival_time;
                Sche_Node.from_node = CALL_Q.top().from_node;
                Sche_Node.to_node = CALL_Q.top().to_node;
                Sche_Node.capacity = CALL_Q.top().capacity;
                Sche_Node.duration = CALL_Q.top().duration;
                for(int i = 0; i < CALL_Q.top().AR_vec.size(); i++)
                {
                    Sche_Node.AR_vector.push_back(CALL_Q.top().AR_vec[i]);
                }
                ARSchedule_Q.push(Sche_Node);
                Call_Node tmp;
                
                CallGenerator_vector[CALL_Q.top().from_AS-1].generateCall(CALL_Q.top().arrival_time,AR_TimeWindow_size,lead_time,single_TimeSlot_size,1);
                tmp.arrival_time = CallGenerator_vector[CALL_Q.top().from_AS-1].arrival_time;
                tmp.from_AS = CALL_Q.top().from_AS;
                tmp.from_node = CallGenerator_vector[CALL_Q.top().from_AS-1].source_node;
                tmp.to_AS = CallGenerator_vector[CALL_Q.top().from_AS-1].dest_AS;
                tmp.to_node = CallGenerator_vector[CALL_Q.top().from_AS-1].dest_node;
                tmp.duration = CallGenerator_vector[CALL_Q.top().from_AS-1].Duration;
                tmp.capacity = CallGenerator_vector[CALL_Q.top().from_AS-1].Capacity;
                for (int j = 0; j < CallGenerator_vector[CALL_Q.top().from_AS-1].ARvec.size(); j++)
                    tmp.AR_vec.push_back(CallGenerator_vector[CALL_Q.top().from_AS-1].ARvec[j]);
                CALL_Q.pop();
                CALL_Q.push(tmp);
                ReqCall_earliest_time = CALL_Q.top().arrival_time;
                break;
            }
            case 4:
                //send BGP updates
                current_time = BGP_update_time;
                BGP_update_time += MRAI;
                if(current_time == 0)
                for (int i = 0; i < ARSERVER_vector.size(); i++) {
                    ARSERVER_vector[i].AR_BGP.constructUpdate(0);
                }
                for (int i = 0; i < ARSERVER_vector.size(); i++) {
                    ARSERVER_vector[i].AR_BGP.sendUpdate(current_time, ARBGP_Q, Delays,AR_TimeWindow_size,ARSERVER_vector[i].IPCE_module );
                }
                break;
        }

    }
    
    
}


void Initializer::readSimulationParam(string path)
{
    
    ifstream in(path.c_str());
    
    int index=1;
    
    if (!in.is_open())
        cout << "Can not open Simulation Parameter file";
    
    string line;
    
    //read lines of the file
    while (getline(in, line))
    {
        istringstream iss(line, istringstream::in);
        
        //check to ignor empty and commented lines
        if (!line.length())
            continue;

        if (line[0] == '/' && line[1]=='/') // Ignore the line starts with #
            continue;
        
        //check which line of the file is read to save it in the proper variable
        if(index==1)
        {
             iss >> simulation_time;
             index++;
        }else if(index==2)
        {
            iss >> numOfRun;
            index++;
        }else if (index==3)
        {
            iss >> topology_path;
            index++;
        }else if (index==4)
        {
            iss >> call_gen_path;
            index++;
        }else if (index==5)
        {
            iss >> AR_param_path;
        }
    }
    //Test if the file read works properly
    //cout << simulation_time << endl << numOfRun << endl << topology_path << endl << call_gen_path << endl << AR_param_path;
	
}

void Initializer::readARsystemParams()
{
     
    ifstream in(AR_param_path.c_str());
    
    int index=1;  
    if (!in.is_open())
        cout << "Can not open AR System Input Parameter file";
    
    string line;
    
    //read lines of the file
    while (getline(in, line))
    {
        istringstream iss(line, istringstream::in);
        
        //check to ignore empty and commented lines
        if (!line.length())
            continue;

        if (line[0] == '/' && line[1]=='/') // Ignore the line starts with //
            continue;
        
        //check which line of the file is read to save it in the proper variable
        if(index==1)
        {
             iss >> single_TimeSlot_size;
             index++;
        }else if(index==2)
        {
            iss >> AR_TimeWindow_size;
            index++;
        }else if (index==3)
        {
            iss >> lead_time;
            index++;
        }
    }
        
    //Test if the file read works properly
    //cout << single_TimeSlot_size << endl << AR_TimeWindow_size << endl << lead_time ;
	
    
}

void Initializer::readASnumber()
{
    stringstream inter_AS_toplogy_filename;
    inter_AS_toplogy_filename << topology_path << "Inter-AS-topology";
    string ASfile = inter_AS_toplogy_filename.str();
    ifstream in(ASfile.c_str());
    
    if (!in.is_open())
        cout << "Can not open Inter AS topology info file";
    
    string line;
    getline(in, line);
    //check to ignore empty and commented lines
    while (!line.length())
        getline(in, line);
    while (line[0] == '/' && line[1]=='/') // Ignore the line starts with //
        getline(in, line);
    
    istringstream iss(line, istringstream::in);
    iss >> num_of_ASes;
   
    //Test if the number of ASs read correctly
    //cout << "number of ASes in this topology: " << num_of_ASes;
}

void Initializer::readDelayFile(DelayStruc Delays)
{
     int f_AS, t_AS;
    double delay;
    
    //read delay file which contain the delays between different ARservers
    string delay_file = topology_path+"Inter-AS-server-delay";
    ifstream def(delay_file.c_str());
    
    string line;
    
    while(getline(def, line))
    {
        istringstream iss(line, istringstream::in);
        
        //check to ignore empty and commented lines
        if (!line.length())
            continue;

        if (line[0] == '/' && line[1]=='/') // Ignore the line starts with //
            continue;
    
        iss >>f_AS;
        iss >> t_AS;
        iss >> delay;
    
        Delays.addDelay(f_AS, t_AS, delay);
        
        //Test if read the correct values from the file
        //cout << f_AS << " " << t_AS << " " << delay <<endl;
    }
    
}