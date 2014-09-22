/* 
 * File:   main.cpp
 * Author: leo
 *
 * Created on June 26, 2014, 8:56 PM
 */

//Revised by Elahe on September 7, 2014

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
#include "GraphElements.h"
#include "ARserver.h"
#include "readcall.h"
#include "CallGenerator.h"
#include "randgen.h"

#define TIMETAG 1
#define BGPTAG 2
#define CALLTAG 3
#define MRAI 5

using namespace std;

int main(int argc, char* argv[]) {
  
	
    //temp variable used for showing the path of files
    string path;
      
    /*simulation related parameters */
    //use this time to continue receiving new calls
    double simulation_time;
    int num_of_ASs;
    path="inputParameters/Simulation-related-input-params";
    ifstream ifs(path.c_str());
    ifs >> num_of_ASs;
    ifs >> simulation_time;
    
	
     /*AR System related parameters*/
    // Duration of timeslot in second
    int single_slot_time;
    // length of AR window(seconds)
    int AR_whole_time;
     //Lead time in timeslots
    int lead_time;
    path="inputParameters/AR-system-input-params";
    ifstream ifss(path.c_str());
    ifss >> single_slot_time;
    ifss >> AR_whole_time;
    ifss >> lead_time;
    
    
    //read delay file which contain the delays between different ARservers
    string delay_file = "inputParameters/AS-Topology-Info/delay-Inter-AS";
    ifstream def(delay_file.c_str());
    int f_AS, t_AS;
    double delay;
    def >> num_of_ASs;
    //create the object that contains the delay parameters
    DelayFile Delays(num_of_ASs);
    while (def >> f_AS && def >> t_AS) {
        def >> delay;
        Delays.addDelay(f_AS, t_AS, delay);
    }
    
    //define a vector to keep the AR servers of all the ASs
    vector<ARserver> ARSERVER_vector;
    //store nodenum of all ASes in this array
    int nodeNum[num_of_ASs];
    
    //read the related Inter AS links in the next for loop from this file
    path="inputParameters/AR-Topology-Info/Inter-AS-Topology";
    
    
    //Create the required modules for each AS (ARServer which includes: Scheduler, ARBGP, IPCE and EPCE) for each AS
    for (int as_num = 1; as_num <= num_of_ASs; as_num++) {

        ifstream interIfs(path.c_str());
        
        //make an ARServer instance for each AS
        ARserver ARSERVER = ARserver(as_num, AR_whole_time, 60);
        int tmpasnum, num_vertices, node_u, node_v, edge_weight, edge_band, start_AS, end_AS;
        string start_AS_vertex;
        string end_AS_vertex;
              
        //define the AS name which is AS+number
        stringstream asname;
        asname << "inputParameters/AS-Topology-Info/AS" << as_num << "/AS" << as_num;
        string asfile = asname.str();
        
        //read AS+Number file to find the number of vertices and the topology of AS network
        ifstream inf(asfile.c_str());
        inf >> tmpasnum;
        inf >> num_vertices;
        
        //save nodeNume in the array
        nodeNum[as_num-1]=num_vertices;
        
        //TODO: Can have num_vertices in ARSEERVEr not to define it twice. 
        ARSERVER.AR_BGP.num_vertices = num_vertices;
        ARSERVER.IPCE_module.num_vertices = num_vertices;
        
        //read the intra network topology(u,v,b,w) from AS file and define the graph object in ARserver
        while (inf >> node_u) {
            if (node_u == -1) {
                break;
            }
            inf >> node_v;
            inf >> edge_weight;
            inf >> edge_band;
          
            Edge tmp;
            tmp.u = node_u;
            tmp.v = node_v;
            tmp.bandwidth = edge_band;
            tmp.weight=edge_weight;
            
            ARSERVER.the_graph.intra_edges.push_back(tmp);
        }
        
        //Read the Inter AS links
        while (interIfs >> start_AS_vertex) {
            //read the information of one link
            int tmpindex = start_AS_vertex.find(":");
            start_AS = atoi(start_AS_vertex.substr(0, tmpindex).c_str());
            node_u = atoi(start_AS_vertex.substr((tmpindex + 1), start_AS_vertex.length() - tmpindex - 1).c_str());
            inf >> end_AS_vertex;
            tmpindex = end_AS_vertex.find(":");
            end_AS = atoi(end_AS_vertex.substr(0, tmpindex).c_str());
            node_v = atoi(end_AS_vertex.substr((tmpindex + 1), end_AS_vertex.length() - tmpindex - 1).c_str());
            inf >> edge_weight;
            inf >> edge_band;
            
            if(start_AS==as_num or end_AS==as_num)
            {
            Inter_AS_Links inter_link_entry;
            inter_link_entry.from_AS = start_AS;
            inter_link_entry.to_AS = end_AS;
            inter_link_entry.start_vertex = node_u;
            inter_link_entry.end_vertex = node_v;
            inter_link_entry.weight= edge_weight;
            //if want to change the strategy of assigning bandwidth to a predefined mode:like all inter_domain links are 
            //64Gbps, please modify here
            inter_link_entry.band = edge_band;
            // inter_link_entry.theLink.constructTable(windowSize,windowExt);
            ARSERVER.the_graph.InterASLinks_table.push_back(inter_link_entry);
            }
        }
        
        for (int i = 0; i < ARSERVER.the_graph.InterASLinks_table.size(); i++) {
            //outgoing links
            if (ARSERVER.the_graph.InterASLinks_table[i].from_AS == as_num) {
                ARSERVER.AR_BGP.addMCNAccess(ARSERVER.the_graph.InterASLinks_table[i].to_AS, 1, ARSERVER.the_graph.InterASLinks_table[i].start_vertex, ARSERVER.the_graph.InterASLinks_table[i].end_vertex);
            } else {
                ARSERVER.AR_BGP.addMCNAccess(ARSERVER.the_graph.InterASLinks_table[i].from_AS, 2, ARSERVER.the_graph.InterASLinks_table[i].start_vertex, ARSERVER.the_graph.InterASLinks_table[i].end_vertex);
            }
        }
        //TODO: Should completely study it in ARBGP module
        ARSERVER.AR_BGP.getMCNList();
   
       //Add the AR server to the related vector
        ARSERVER_vector.push_back(ARSERVER);
    }

    //Priority Queue to keep ARBGP calls/msgs in order (Priority Queue needs comparateor function)
    priority_queue<ARBGP_Node, vector<ARBGP_Node>, MyComparatorARBGP> ARBGP_Q;
    //Priority Queue to keep ARSchedule calls
    priority_queue<ARSchedule_Node, vector<ARSchedule_Node>, MyComparatorARSchedule> ARSchedule_Q;
    //Priority Queue to save Routing Request calls
    priority_queue<Call_Node, vector<Call_Node>, MyComparatorCALL> CALL_Q;
    
    //TODO: Better to use graph class instead of creating a new topology instance in IPCE
    //All the IPCE module ASs read intra network topology
    for (int j = 0; j < ARSERVER_vector.size(); j++)
        ARSERVER_vector[j].IPCE_module.readTopology(ARSERVER_vector[j].the_graph.intra_edges);


    //create vector of call generators
    vector<CallGenerator> CallGenerator_vector;
    for (int i = 1; i <= num_of_ASs; i++) {
        CallGenerator callGen = CallGenerator(i);
        callGen.readNodeVector(nodeNum);
        //TODO: In future every AS should have its own call-gen parameters
        callGen.readCommonFile("InputParameters/call-gen-input-params");
        callGen.readprobMatrix("InputParameters/src_dst_prob_matrix");
        CallGenerator_vector.push_back(callGen);
    }
    
   
    //generate the first round of calls
    //for each AS, execute the call generator to generate a request
    for (int i = 0; i < CallGenerator_vector.size(); i++) {
        Call_Node tmp;
        //TODO: it should be changed. generateCall should receive the arrival rate and produce arrival time within it

        //generate a call according to input parameters
        CallGenerator_vector[i].generateCall(0,AR_whole_time,lead_time,single_slot_time,1);
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
                
                CallGenerator_vector[CALL_Q.top().from_AS-1].generateCall(CALL_Q.top().arrival_time,AR_whole_time,lead_time,single_slot_time,1);
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
                    ARSERVER_vector[i].AR_BGP.sendUpdate(current_time, ARBGP_Q, Delays,AR_whole_time,ARSERVER_vector[i].IPCE_module );
                }
                break;
        }

    }
    

    return 0;
}

