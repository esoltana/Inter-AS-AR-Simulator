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
#include "randgen/randgen.h"

#include <cstdlib>
#include <iomanip>

#define TIMETAG 1
#define BGPTAG 2
#define CALLTAG 3
#define MRAI 5

using namespace std;

Initializer::Initializer(double arrRate, int simulationTime, string filename) {
    
    
    readSimulationParam("InputParameters//Simulation-related-input-params");
    
    readARsystemParams();
    
   
    readNumberOfASes();
    
    //TODO: should be changed for multipl ASes
    num_of_ASes=1;
    
    int nodeNum[num_of_ASes];

    
    //A data structure to keep delay
    DelayStruc Delays(num_of_ASes);
    
    //should be uncommented for multiple ASes
    //read the delays of different Inter AS links
    //readDelayFile(Delays);
    
    //these lines should be removed for multiple ASes
    //make an ARServer instance for each AS
    ARserver ARServer = ARserver(1, AR_TimeWindow_size, lead_time,single_TimeSlot_size,topology_path);
        
        
    //save nodeNume in the array
    nodeNum[0]=ARServer.numOfNodes;
        
       //Add the AR server to the related vector
    ARSERVER_vector.push_back(ARServer);
    
    //Create the required modules for each AS (ARServer which includes: Scheduler, ARBGP, IPCE and EPCE) for each AS
    /*for (int AS_ID= 1; AS_ID <= num_of_ASes; AS_ID++) {
        
        //TODO: lead_time value is not correct, why multiply by 60
        //make an ARServer instance for each AS
        ARserver ARServer = ARserver(AS_ID, AR_TimeWindow_size, lead_time,single_TimeSlot_size,topology_path);
        
        
        //save nodeNume in the array
        nodeNum[AS_ID-1]=ARServer.numOfNodes;
        
       //Add the AR server to the related vector
        ARSERVER_vector.push_back(ARServer);
    }
     */

   
    simulateMsgPassing(nodeNum, arrRate, simulationTime,filename);
    
    
    
}

void Initializer::simulateMsgPassing(int nodeNum[],double arrRate, int simulationTime,string filename)
{
    //create vector of call generators
    for (int i = 1; i <= num_of_ASes; i++) {
        CallGenerator callGen = CallGenerator(i,AR_TimeWindow_size,lead_time,single_TimeSlot_size);
        callGen.readNodeVector(nodeNum, num_of_ASes);
        //TODO: In future every AS should have its own call-gen parameters
        callGen.readCommonFile(call_gen_path);
        //TODO: is added for simulation should be changed
        callGen.arrival_rate=arrRate;
        CallGenerator_vector.push_back(callGen);
    }
    
    //generate the first round of calls  
    generateFirstRoundCalls();
  
    //the arrival time of earliest reservation request call
    double ReqCall_earliest_time = 0;
    //get the execution time of earliest reservation request from Call queue
    ReqCall_earliest_time = GeneratedCALL_Q.top().arrival_instant_in_sec;
  
    //the arrival time of sending BGP Update
    double BGP_update_time = DBL_MAX;
    //keep the current time

    //keep the current time
    double current_time = 0;
    
    int result=0;
    
    ofstream myfile;
    //myfile.open("Output-files/1-USST-singleLink-oneOption.txt", ios::app);
    myfile.open(filename.c_str(), ios::app);
    
    
    simulation_time=simulationTime;
                     
    //main loop
    //until the request time is within simulation time continue
    
    double callNumberUSST=0, callNumberEST=0, callNumber=0, blockedUSST=0, blockedEST=0, pathLengthEST=0, pathLengthUSST=0, firstAR=0, secondAR=0, thirdAR=0, selectedOption=0, meanWaitingEST=0;
    while (current_time <= simulation_time) {
        
        //Variables for saving the earliest time of ARBGP calls and ARSchedule
        double ARBGP_earliest_time = DBL_MAX;
        double ARSchedule_earliest_time = DBL_MAX;
        //variable to show which time of call should be executed now
        int case_flag;
        

        //get the execution time of earliest call in ARBGP Queue
        if (!ARBGP_Q.empty()) {
            ARBGP_earliest_time = ARBGP_Q.top().do_update_time;
        }
        //get the execution time of earliest call in ARSchedule Queue
        if(!Inter_AS_call_Q.empty())
        {
            ARSchedule_earliest_time = Inter_AS_call_Q.top().arrival_instance_in_sec;
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
                ARSERVER_vector[ARBGP_Q.top().to_AS - 1].actionARBGPreceive(ARBGP_Q.top().from_AS, ARBGP_Q.top().NLRI_vector);
                //pop the executed call
                ARBGP_Q.pop();

                break;
            case 2:
            {
                //do Inter AS Call
                //TODO: should be completed for EPCE calls to be accepted 
                //update the current time to the current executing call
                current_time = Inter_AS_call_Q.top().arrival_instance_in_sec;
                
                //cout << "process an Inter AS call: " ;
                
                Inter_AS_call_Q.pop();
                break;
            }
            case 3:
            {
                //accept a generated call
                
                current_time = GeneratedCALL_Q.top().arrival_instant_in_sec;
                /*cout << "process a call: arrival_instant_in_sec:" << GeneratedCALL_Q.top().arrival_instant_in_sec << " arrival_instant_in_TS:" << GeneratedCALL_Q.top().arrival_instant_in_TS << "  capacity:" << GeneratedCALL_Q.top().capacity << " " << GeneratedCALL_Q.top().cap_return << " duration:" << GeneratedCALL_Q.top().duration << "  AR-options:";
                
                for (int i = 0; i < GeneratedCALL_Q.top().AR_vec.size(); i++) {
                    cout << GeneratedCALL_Q.top().AR_vec[i] << " ";
                }
                cout << " fromAS:" << GeneratedCALL_Q.top().from_AS << "  fromNode:" << GeneratedCALL_Q.top().from_node << "  ToAS:" << GeneratedCALL_Q.top().to_AS << "  toNode:" << GeneratedCALL_Q.top().to_node << endl <<endl;
                */
                
                //check if it is an Inter AS or Intra AS call
                 if(GeneratedCALL_Q.top().from_AS == GeneratedCALL_Q.top().to_AS)
                {
                    //call IPCE module
                    result=ARSERVER_vector[GeneratedCALL_Q.top().to_AS - 1].executeIntraCall(GeneratedCALL_Q.top());
                    callNumber++;
                    
                    if(result==0)
                    {
                        if(GeneratedCALL_Q.top().isUSST==1)
                        {
                            blockedUSST++;
                            callNumberUSST++;
                        }
                        else if(GeneratedCALL_Q.top().isUSST==2)
                        {
                            blockedEST++;
                            callNumberEST++;
                        }
                    }
                    else
                    {
                        if(GeneratedCALL_Q.top().isUSST==1)
                        {
                            callNumberUSST++;
                            pathLengthUSST+= ARSERVER_vector[GeneratedCALL_Q.top().to_AS - 1].IPCE_module.pathLength;
                            selectedOption=ARSERVER_vector[GeneratedCALL_Q.top().to_AS - 1].IPCE_module.selectedOptionIndex;
                        
                            if(selectedOption==0)
                                firstAR++;
                            else if(selectedOption==1)
                                secondAR++;
                            else if(selectedOption==2)
                                thirdAR++;
                        }else if (GeneratedCALL_Q.top().isUSST==2)
                        {
                            callNumberEST++;
                            pathLengthEST+= ARSERVER_vector[GeneratedCALL_Q.top().to_AS - 1].IPCE_module.pathLength;
                            selectedOption=ARSERVER_vector[GeneratedCALL_Q.top().to_AS - 1].IPCE_module.selectedOptionIndex+GeneratedCALL_Q.top().arrival_instant_in_TS;
                            meanWaitingEST+=(selectedOption-GeneratedCALL_Q.top().AR_vec[0]);
                        }
                        
                    }
                    
                    
                    
                     //writing into file for code verification
                    //myfile << left << setw(10) << GeneratedCALL_Q.top().arrival_instant_in_sec << setw(8) << GeneratedCALL_Q.top().arrival_instant_in_TS << setw(8) << result << setw(8) << GeneratedCALL_Q.top().isUSST << setw(5) << GeneratedCALL_Q.top().from_node << setw(6) << GeneratedCALL_Q.top().to_node << setw(5) << GeneratedCALL_Q.top().duration << setw(6) << GeneratedCALL_Q.top().capacity;
                    myfile << GeneratedCALL_Q.top().arrival_instant_in_sec << " " << GeneratedCALL_Q.top().arrival_instant_in_TS << " " << result << " " << GeneratedCALL_Q.top().isUSST << " " << GeneratedCALL_Q.top().from_node << " " << GeneratedCALL_Q.top().to_node << " " << GeneratedCALL_Q.top().duration << " " << GeneratedCALL_Q.top().capacity<< " ";
                    int f=1;
                    for (int j = 0; j < GeneratedCALL_Q.top().AR_vec.size(); j++)
                    {
                        if(f){
                        myfile << GeneratedCALL_Q.top().AR_vec[j];
                        f=0;
                        }else
                            myfile << "," <<GeneratedCALL_Q.top().AR_vec[j];
                    }
                            
                    
                    myfile << " " << ARSERVER_vector[GeneratedCALL_Q.top().to_AS - 1].IPCE_module.selectedOptionIndex << " " << ARSERVER_vector[GeneratedCALL_Q.top().to_AS - 1].IPCE_module.pathLength <<" " ;
                    int first=1;
                    for (int j = 0; j < ARSERVER_vector[GeneratedCALL_Q.top().to_AS - 1].IPCE_module.pathvector.size(); j++)
                    {
                        if(first)
                        {
                            myfile << ARSERVER_vector[GeneratedCALL_Q.top().to_AS - 1].IPCE_module.pathvector[j];
                            first=0;
                        }
                        else
                            myfile << "-" << ARSERVER_vector[GeneratedCALL_Q.top().to_AS - 1].IPCE_module.pathvector[j];
                    }
                    
                    myfile <<"\n";
                    
                 }else{
                     //call EPCE module and generate Inter_AS_call
                    
                    //TODO: the send time and total time should be computed by EPCE module in ARSERVER and new call is generated.
                    //push this call into the Inter_AS_Call_Queue
                    Inter_AS_Call_Node InterASCall_Node;
                    InterASCall_Node.from_AS = GeneratedCALL_Q.top().from_AS;
                    InterASCall_Node.to_AS = GeneratedCALL_Q.top().to_AS;
                    //InterASCall_Node.send_time = 
                    //InterASCall_Node.total_delay =
                    //InterASCall_Node.do_decision_time = 
                    InterASCall_Node.from_node = GeneratedCALL_Q.top().from_node;
                    InterASCall_Node.to_node = GeneratedCALL_Q.top().to_node;
                    InterASCall_Node.capacity = GeneratedCALL_Q.top().capacity;
                    InterASCall_Node.duration = GeneratedCALL_Q.top().duration;
                    for(int i = 0; i < GeneratedCALL_Q.top().AR_vec.size(); i++)
                    {
                        InterASCall_Node.AR_vector.push_back(GeneratedCALL_Q.top().AR_vec[i]);
                    }
                    Inter_AS_call_Q.push(InterASCall_Node);
                 }
               
                //Generate a new call
                Call_Node tmp;
                
                CallGenerator_vector[GeneratedCALL_Q.top().from_AS-1].generateCall(GeneratedCALL_Q.top().arrival_instant_in_sec);
                tmp.isUSST=CallGenerator_vector[GeneratedCALL_Q.top().from_AS-1].isUSST;
                tmp.arrival_instant_in_sec = CallGenerator_vector[GeneratedCALL_Q.top().from_AS-1].arrival_time;
                tmp.arrival_instant_in_TS = CallGenerator_vector[GeneratedCALL_Q.top().from_AS-1].arrival_timeslot;
                tmp.from_AS = GeneratedCALL_Q.top().from_AS;
                tmp.from_node = CallGenerator_vector[GeneratedCALL_Q.top().from_AS-1].source_node;
                tmp.to_AS = CallGenerator_vector[GeneratedCALL_Q.top().from_AS-1].dest_AS;
                tmp.to_node = CallGenerator_vector[GeneratedCALL_Q.top().from_AS-1].dest_node;
                tmp.duration = CallGenerator_vector[GeneratedCALL_Q.top().from_AS-1].Duration;
                tmp.capacity = CallGenerator_vector[GeneratedCALL_Q.top().from_AS-1].Capacity;
                tmp.cap_return=CallGenerator_vector[GeneratedCALL_Q.top().from_AS-1].ESTcap_ack;
                for (int j = 0; j < CallGenerator_vector[GeneratedCALL_Q.top().from_AS-1].ARvec.size(); j++)
                    tmp.AR_vec.push_back(CallGenerator_vector[GeneratedCALL_Q.top().from_AS-1].ARvec[j]);
                GeneratedCALL_Q.pop();
                GeneratedCALL_Q.push(tmp);
                ReqCall_earliest_time = GeneratedCALL_Q.top().arrival_instant_in_sec;
                
                break;
            }
            case 4:
                //send BGP updates
                //TODO: it does not work proparely, because MRAI is 5 and in each round the turn is given to BGP update
                current_time = BGP_update_time;
                BGP_update_time += MRAI;
                if(current_time == 0)
                for (int i = 0; i < ARSERVER_vector.size(); i++) {
                    ARSERVER_vector[i].AR_BGP_module.constructUpdate(0);
                }
                for (int i = 0; i < ARSERVER_vector.size(); i++) {
                    //ARSERVER_vector[i].AR_BGP_module.sendUpdate(current_time, ARBGP_Q, Delays,AR_TimeWindow_size,ARSERVER_vector[i].IPCE_module );
                }
            break;
        }

    }
    myfile.close();
   
    double successNumUSST=callNumberUSST-blockedUSST;
    double successNumEST=callNumberEST-blockedEST;
    double avgpathLengthUSST=pathLengthUSST/successNumUSST;
    double avgpathLengthEST=pathLengthEST/successNumEST;
    double meanWait=meanWaitingEST/successNumEST;
    double max_link_Util=0;
    CBP= blockedUSST/callNumberUSST;
    //should define max link utlization.
    //double firstPerc=firstAR/successNum*100;
    //double secondPerc=secondAR/successNum*100;
    //double thirdPerc=thirdAR/successNum*100;
  
}

void Initializer::generateFirstRoundCalls()
{
    //for each AS, execute the call generator to generate a request
    
    for (int i = 0; i < CallGenerator_vector.size(); i++) {
    
        Call_Node tmp;
       
        //generate a call according to input parameters
        CallGenerator_vector[i].generateCall(0);
        //save the produced request parameters in Queue
        tmp.isUSST=CallGenerator_vector[i].isUSST;
        tmp.arrival_instant_in_sec = CallGenerator_vector[i].arrival_time;
        tmp.arrival_instant_in_TS = CallGenerator_vector[i].arrival_timeslot;
        tmp.from_AS = i + 1;
        tmp.from_node = CallGenerator_vector[i].source_node;
        tmp.to_AS = CallGenerator_vector[i].dest_AS;
        tmp.to_node = CallGenerator_vector[i].dest_node;
        tmp.duration = CallGenerator_vector[i].Duration;
        tmp.capacity = CallGenerator_vector[i].Capacity;
        tmp.cap_return=CallGenerator_vector[i].ESTcap_ack;
        //also save the AR options in the queue
        for (int j = 0; j < CallGenerator_vector[i].ARvec.size(); j++)
            tmp.AR_vec.push_back(CallGenerator_vector[i].ARvec[j]);
        //push the request in Call Queue
        GeneratedCALL_Q.push(tmp);
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
            //TODO: may be should divided by timeslot to keep the number of time slots 
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

void Initializer::readNumberOfASes()
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
    //cout << "number of ASes in this topology: " << num_of_ASes << endl;
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