/* 
 * File:   main.cpp
 * Author: leo
 *
 * Created on June 26, 2014, 8:56 PM
 */

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
    //num of AS. Could be a input parameter 
    /*
    if (argc != 2) // argc should be 2 for correct execution
        // We print argv[0] assuming it is the program name
        cout << "usage: " << argv[0] << " number_of_simulation\n";
     */
    //int num_of_simulation = atoi(argv[1]);
    int num_of_simulation = 1;
    int num_of_AS;
    int as_num;
    double dt = 0.01;
    int call_count = 0;
    double tmpt = 0;
    double last_update_time = 0;
    double simulation_time = 10;
    double next_active_time = 0;
    double arrival_rate = 50;
    // each time slots of AR window equals this amount of time(seconds)
    int single_slot_time = 60;
    // length of AR window(seconds)
    int AR_whole_time = 24 * 60 * 60 / single_slot_time;
    int lead_time = 60/single_slot_time*3;
    //the arrival time of previous call and the arrival time of current call

    int prev_call = 0;
    double curr_call = 0;
    int curr_update = 0;
    //the time to send the BGP update 
    double BGP_update_send = 0;
    //the AS number is the process number
    //there are three types of processes: 1) ARserver; 2) call generator 3) additional call flows
    //read in the mpi file in inputdata folder, the first parameter specifies the number of ASes, the second parameter 
    //specifies the number of additional call flows
    //ARserver processes
    int count_t = 0;
    vector<ARserver> ARSERVER_vector;


    ///////read in files that contain the delays between different ASes
    string delay_file = "DelayFile";
    ifstream def(delay_file.c_str());
    int f_AS, t_AS;
    double delay;
    def >> num_of_AS;
    //create the object that contains the delay parameters
    DelayFile Delays(num_of_AS);
    while (def >> f_AS && def >> t_AS) {
        def >> delay;
        Delays.addDelay(f_AS, t_AS, delay);
    }
    
    //store nodenum of all ASes in this array
    int nodeNum[num_of_AS];
    
    //Create the required modules (ARServer which includes: Scheduler, ARBGP, IPCE and EPCE) for each AS
    for (as_num = 1; as_num <= num_of_AS; as_num++) {
        //make an ARServer instance for each AS
        ARserver ARSERVER = ARserver(as_num, AR_whole_time, 60);
        int tmpasnum, num_vertices, node_u, node_v, edge_weight, edge_band, start_AS, end_AS;
        string start_AS_vertex;
        string end_AS_vertex;
        
        //delete
        vector<Edge> intra_edges;
        vector<Inter_AS_Links> InterASLinks_table;
        //
        
        //define the AS name which is AS+number
        stringstream asname;
        asname << "inputdata/AS" << as_num << "/AS" << as_num;
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
        while (inf >> node_u && node_u != 0) {
            if (node_u == -1) {
                break;
            }
            inf >> node_v;
            //TODO: weight is not assigned to graph
            inf >> edge_weight;
            inf >> edge_band;
            Edge tmp;
            tmp.u = node_u;
            tmp.v = node_v;
            tmp.b = edge_band;
            ARSERVER.the_graph.intra_edges.push_back(tmp);
        }
        while (inf >> start_AS_vertex) {
            int tmpindex = start_AS_vertex.find(":");
            start_AS = atoi(start_AS_vertex.substr(0, tmpindex).c_str());
            node_u = atoi(start_AS_vertex.substr((tmpindex + 1), start_AS_vertex.length() - tmpindex - 1).c_str());
            inf >> end_AS_vertex;
            tmpindex = end_AS_vertex.find(":");
            end_AS = atoi(end_AS_vertex.substr(0, tmpindex).c_str());
            node_v = atoi(end_AS_vertex.substr((tmpindex + 1), end_AS_vertex.length() - tmpindex - 1).c_str());
            inf >> edge_weight;
            inf >> edge_band;
            Inter_AS_Links inter_link_entry;
            inter_link_entry.from_AS = start_AS;
            inter_link_entry.to_AS = end_AS;
            inter_link_entry.start_vertex = node_u;
            inter_link_entry.end_vertex = node_v;
            //if want to change the strategy of assigning bandwidth to a predefined mode:like all inter_domain links are 
            //64Gbps, please modify here
            inter_link_entry.band = edge_band;
            // inter_link_entry.theLink.constructTable(windowSize,windowExt);
            ARSERVER.the_graph.InterASLinks_table.push_back(inter_link_entry);
        }
        for (int i = 0; i < ARSERVER.the_graph.InterASLinks_table.size(); i++) {
            //outgoing links
            if (ARSERVER.the_graph.InterASLinks_table[i].from_AS == as_num) {
                ARSERVER.AR_BGP.addMCNAccess(ARSERVER.the_graph.InterASLinks_table[i].to_AS, 1, ARSERVER.the_graph.InterASLinks_table[i].start_vertex, ARSERVER.the_graph.InterASLinks_table[i].end_vertex);
            } else {
                ARSERVER.AR_BGP.addMCNAccess(ARSERVER.the_graph.InterASLinks_table[i].from_AS, 2, ARSERVER.the_graph.InterASLinks_table[i].start_vertex, ARSERVER.the_graph.InterASLinks_table[i].end_vertex);
            }
        }
        ARSERVER.AR_BGP.getMCNList();
        //AS_num starts from 1 while the index of AS_vector start from 0
       // ARSERVER.AR_BGP.getMCNProcessNumber(-1);
        ARSERVER_vector.push_back(ARSERVER);
    }

    //test MCN_IDs
    for (int j = 0; j < ARSERVER_vector.size(); j++) {
        for (int i = 0; i < ARSERVER_vector[j].AR_BGP.MCN_IDs.size(); i++)
            cout << ARSERVER_vector[j].AR_BGP.MCN_IDs[i] << " ";
        cout << endl;
    }


    /*
    int mcount = ARSERVER.AR_BGP.MCN_IDs.size();
    while(mcount>0)
    {
            MPI_Recv(&recvmess,200,MPI_CHAR,MPI_ANY_SOURCE,BGPTAG,MPI_COMM_WORLD,&status);
            mcount--;
            string recvd(recvmess);
            cout<< "AS "<<as_num<<" received: "<<recvd<<endl;
    }
     */
    //print RIB

    

    //read in calls(one giant call file)
    //readcall READCALL;
    stringstream filename;
   // filename << "callfile" << num_of_simulation;
    //string callfile = filename.str();
    //READCALL.init(callfile.c_str());
    //set up BAn
    //READCALL.ReadCall(3);
    //set up two queues(BGP update and AR) as well as next call time(curr_call)
    //ARBGP_queue ARBGP_Q;
    priority_queue<ARBGP_Node, vector<ARBGP_Node>, MyComparatorARBGP> ARBGP_Q;
    priority_queue<ARSchedule_Node, vector<ARSchedule_Node>, MyComparatorARSchedule> ARSchedule_Q;
   // ARSchedule_queue ARSchedule_Q;
   // curr_call = READCALL.callarrive[call_count];
    for (int j = 0; j < ARSERVER_vector.size(); j++)
    ARSERVER_vector[j].IPCE_module.readTopology(ARSERVER_vector[j].the_graph.intra_edges);
    //vector<int> arvec;
   // arvec.push_back(2);
   // arvec.push_back(3);
   // arvec.push_back(4);
    //ARSERVER_vector[0].IPCE_module.findPath(2, 4, 5, 5, arvec);


    //create vector of call generators
    vector<CallGenerator> CallGenerator_vector;
    for (int i = 1; i <= num_of_AS; i++) {
        CallGenerator callGen = CallGenerator(i);
        //callGen.readNodeVector("common/nodenum");
        callGen.readNodeVector(nodeNum);
        callGen.readCommonFile("common/common_parameter");
        callGen.readMatrix("common/src_dst_prob_matrix");
        CallGenerator_vector.push_back(callGen);
    }
    priority_queue<Call_Node, vector<Call_Node>, MyComparatorCALL> CALL_Q;
    //generate the first round of calls
    for (int i = 0; i < CallGenerator_vector.size(); i++) {
        Call_Node tmp;
        double arrival_time = expon(arrival_rate);
        CallGenerator_vector[i].generateCall(arrival_time,AR_whole_time,lead_time,single_slot_time,1);
        tmp.arrival_time = arrival_time;
        tmp.from_AS = i + 1;
        tmp.from_node = CallGenerator_vector[i].source_node;
        tmp.to_AS = CallGenerator_vector[i].dest_AS;
        tmp.to_node = CallGenerator_vector[i].dest_node;
        tmp.duration = CallGenerator_vector[i].Duration;
        tmp.capacity = CallGenerator_vector[i].Capacity;
        for (int j = 0; j < CallGenerator_vector[i].ARvec.size(); j++)
            tmp.AR_vec.push_back(CallGenerator_vector[i].ARvec[j]);
        CALL_Q.push(tmp);
    }

    curr_call = CALL_Q.top().arrival_time;
    /*
    for(int i = 0; i < ARSERVER_vector.size(); i++)
    {
        int t = i+1;
        cout<<"Inter Links for AS "<<t<<":"<<endl;
        
        for(map<int,MCN>::iterator iter = ARSERVER_vector[i].AR_BGP.MCNlist.begin(); iter != ARSERVER_vector[i].AR_BGP.MCNlist.end(); iter++)
        {
            cout<<"To AS "<<iter->first<<endl;
            for(int k = 0; k < iter->second.egress_links.size(); k++)
                cout<<iter->second.egress_links[k].first<<" "<< iter->second.egress_links[k].second<<endl;
        }
       
        for(int j = 0; j < ARSERVER_vector[i].the_graph.InterASLinks_table.size(); j++)
        {
            cout<<"From AS "<<ARSERVER_vector[i].the_graph.InterASLinks_table[j].from_AS<<"To AS "<<ARSERVER_vector[i].the_graph.InterASLinks_table[j].to_AS<<endl;
            cout<<ARSERVER_vector[i].the_graph.InterASLinks_table[j].start_vertex<<" "<<ARSERVER_vector[i].the_graph.InterASLinks_table[j].end_vertex<<endl;
        }
    }
    */
    
    
    
    
    //main loop
    while (next_active_time <= simulation_time) {
        // cout << "next:" << next_active_time << endl;
        // cout<<ARBGP_Q.top().do_update_time<<endl;
        //case_flag = 1: ARBGP
        //case_flag = 2: ARschedule
        //case_flag = 3: process call
        double ARBGP_earliest_time = DBL_MAX;
        double ARSchedule_earliest_time = DBL_MAX;
        if (!ARBGP_Q.empty()) {
            ARBGP_earliest_time = ARBGP_Q.top().do_update_time;
            //cout<<"thetop:"<<ARBGP_Q.top().do_update_time<<endl;
        }
        if(!ARSchedule_Q.empty())
        {
            ARSchedule_earliest_time = ARSchedule_Q.top().do_decision_time;
        }
        //cout<<"BGP time:"<<ARBGP_earliest_time<<" "<<curr_call<<" "<<ARSchedule_Q.earliest_time<<" "<<BGP_update_send<<endl;
        int case_flag;
        if (ARBGP_earliest_time <= curr_call && ARBGP_earliest_time <= ARSchedule_earliest_time && ARBGP_earliest_time <= BGP_update_send)
            case_flag = 1;
        else if (ARSchedule_earliest_time <= curr_call && ARSchedule_earliest_time <= ARBGP_earliest_time && ARSchedule_earliest_time <= BGP_update_send)
            case_flag = 2;
        else if (curr_call <= ARBGP_earliest_time && curr_call <= ARSchedule_earliest_time && curr_call <= BGP_update_send)
            case_flag = 3;
        else
            case_flag = 4;

        switch (case_flag) {
            case 1:
                //do ARBGP update

                next_active_time = ARBGP_Q.top().do_update_time;
                // ARBGP_Node * BGP_message;
                // BGP_message = ARBGP_Q.top();
                cout << "do BGP update at:" << ARBGP_Q.top().do_update_time << endl;


                //  cout << "to" << ARBGP_Q.top().to_AS << endl;
                ARSERVER_vector[ARBGP_Q.top().to_AS - 1].AR_BGP.recvUpdate(ARBGP_Q.top().from_AS, ARBGP_Q.top().NLRI_vector);
                //  cout << "do update!" << endl;
                //for (int i = 0; i < ARBGP_Q.top().NLRI_vector.size(); i++)
                //     cout << ARBGP_Q.top().NLRI_vector[i].toString() << endl;

                ARBGP_Q.pop();

                break;
            case 2:
            {
                //do AR
                next_active_time = ARSchedule_Q.top().do_decision_time;
                //ARSchedule_Node * AR_message;
                //AR_message = ARSchedule_Q.popNode();
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
                //cout << "process call:" << curr_call << endl;
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
                curr_call = READCALL.callarrive[call_count];
                next_active_time = curr_call;
                 */
                next_active_time = CALL_Q.top().arrival_time;
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
                double arrival_time = expon(arrival_rate) + CALL_Q.top().arrival_time;
                CallGenerator_vector[CALL_Q.top().from_AS-1].generateCall(arrival_time,AR_whole_time,lead_time,single_slot_time,1);
                tmp.arrival_time = arrival_time;
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
                curr_call = CALL_Q.top().arrival_time;
                break;
            }
            case 4:
                //send BGP updates
                next_active_time = BGP_update_send;
                BGP_update_send += MRAI;
                if(next_active_time == 0)
                for (int i = 0; i < ARSERVER_vector.size(); i++) {
                    ARSERVER_vector[i].AR_BGP.constructUpdate(0);
                }
                for (int i = 0; i < ARSERVER_vector.size(); i++) {
                    ARSERVER_vector[i].AR_BGP.sendUpdate(next_active_time, ARBGP_Q, Delays,AR_whole_time,ARSERVER_vector[i].IPCE_module );
                }
                break;
        }

    }


    for (int i = 0; i < ARSERVER_vector.size(); i++) {
        int as = i + 1;
        cout << "AS" << as << ":" << endl;
        cout<<"reservation window:"<<endl;
        for(map<int, EdgeTable>::iterator iter = ARSERVER_vector[i].IPCE_module.intraASLinksAR.begin(); iter != ARSERVER_vector[i].IPCE_module.intraASLinksAR.end(); ++iter)
        {
            cout<<"from "<<iter->first/1000 <<" to "<<iter->first%1000<<":";
            iter->second.printTable();
        }
/*
        for (std::map<Node, vector<NLRI> >::iterator iter = ARSERVER_vector[i].AR_BGP.RIB.begin(); iter != ARSERVER_vector[i].AR_BGP.RIB.end(); ++iter) {
            int asnum = iter->first.as_num;
            int node_num = iter->first.vertex_num;
            cout << asnum << " " << node_num << ":";
            for (int i = 0; i < iter->second.size(); i++) {
                cout << iter->second[i].toString() << " "<<endl;
            }
            cout << endl;

        }
*/
    }

    return 0;
}

