#include <limits>
#include <set>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include "DataStructures.h"
#include "IPCE.h"
#include "readcall.h"
#include "ARserver.h" 
using namespace std;


ARserver::ARserver(int AS_num, int AR_TimeWindow_size, int lead_time, int single_TimeSlot_size, string topology_path)
{
    ARWindowSize = AR_TimeWindow_size;
    ARExtension = lead_time;
    timeSlotSize=single_TimeSlot_size;
    AS_ID=AS_num;

    readIntraTopology(topology_path);
    readInterLinks(topology_path);
    
    //TODO: should modify them if needed. is not complete yet
    AR_BGP_module= ARBGP();
    initializeARBGP();
    
    //creat an instance of IPCE module
    IPCE_module=IPCE(ARWindowSize,ARExtension,numOfNodes);
    //IPCE module reads intra network topology and save it in a new format to use in find path function
    IPCE_module.readTopology(topology.Intra_Links_table);

}

void ARserver::initializeARBGP()
{
    //TODO: didn't understand why we need to have toplogy in this format again
    for (int i = 0; i < topology.InterASLinks_table.size(); i++) {
        //outgoing links
        if (topology.InterASLinks_table[i].start_AS == AS_ID) {
            AR_BGP_module.addMCNAccess(topology.InterASLinks_table[i].end_AS, 1, topology.InterASLinks_table[i].start_node, topology.InterASLinks_table[i].end_node);
        } else {
            AR_BGP_module.addMCNAccess(topology.InterASLinks_table[i].start_AS, 2, topology.InterASLinks_table[i].start_node, topology.InterASLinks_table[i].end_node);
        }
    }
    //TODO: Should completely study it in ARBGP module - why we need it
    AR_BGP_module.getMCNList();
}


void ARserver::readIntraTopology(string topology_path)
{
    int node_u, node_v, link_weight, link_band, start_AS, end_AS;
    
    int index=1;
    string line;
    
    //define the AS name which is AS+number
    stringstream ASPath;
    ASPath << topology_path << "//AS" << AS_ID << "//AS" << AS_ID;
    
    string asfile = ASPath.str();
    //read AS+Number file to find the number of vertices and the topology of AS network
    ifstream in(asfile.c_str());

    if (!in.is_open())
        cout << "Can not open Intra AS topology info file";
    
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
             iss >> AS_ID;
             index++;
        }else if(index==2)
        {
            iss >> numOfNodes;
            index++;
        }else if (index==3)
        {
         //read the intra network topology(u,v,b,w) from AS file and define the graph object in ARserver
            iss >> node_u;
            iss >> node_v;
            iss >> link_weight;
            iss >> link_band;

            Intra_Link tmp;
            tmp.start_node = node_u;
            tmp.end_node = node_v;
            tmp.bandwidth = link_band;
            tmp.weight=link_weight;
            topology.Intra_Links_table.push_back(tmp);
         
        }
    }

}

void ARserver::readInterLinks(string topology_path)
{
    string start_AS_node;
    string end_AS_node;
    int node_u, node_v, link_weight, link_band, start_AS, end_AS;
    int index=1;
    string line;
    
    //define the AS name which is AS+number
    stringstream path;
    path << topology_path << "//Inter-AS-topology";
    
    string asfile = path.str();
    //read AS+Number file to find the number of vertices and the topology of AS network
    ifstream in(asfile.c_str());

    if (!in.is_open())
        cout << "Can not open Inter AS topology info file";
    
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
            //the line which is related to number of ASs and should path it.
             index++;
        }else if(index==2)
        {

            //Read the Inter AS links
            iss >> start_AS_node;
            //read the information of one link
            int tmpindex = start_AS_node.find(":");
            start_AS = atoi(start_AS_node.substr(0, tmpindex).c_str());
            node_u = atoi(start_AS_node.substr((tmpindex + 1), start_AS_node.length() - tmpindex - 1).c_str());
            
            iss >> end_AS_node;
            tmpindex = end_AS_node.find(":");
            end_AS = atoi(end_AS_node.substr(0, tmpindex).c_str());
            node_v = atoi(end_AS_node.substr((tmpindex + 1), end_AS_node.length() - tmpindex - 1).c_str());
            iss >> link_weight;
            iss >> link_band;

            if(start_AS==AS_ID or end_AS==AS_ID)
            {
                Inter_AS_Links inter_link_entry;
                inter_link_entry.start_AS = start_AS;
                inter_link_entry.end_AS = end_AS;
                inter_link_entry.start_node = node_u;
                inter_link_entry.end_node = node_v;
                inter_link_entry.weight= link_weight;
                //if want to change the strategy of assigning bandwidth to a predefined mode:like all inter_domain links are 
                //64Gbps, please modify here
                inter_link_entry.bandwidth = link_band;
                // inter_link_entry.theLink.constructTable(windowSize,windowExt);
                topology.InterASLinks_table.push_back(inter_link_entry);
            }
        }
    }
}

void ARserver::actionARBGPreceive(int from_AS,vector<NLRI> NLRI_vector)
{
    //TODO: Study details about what this function does.
                AR_BGP_module.recvUpdate(from_AS, NLRI_vector);
}


ARSchedule_Node ARserver::actionSchedulerReceive(ARSchedule_Node ARSchNode)
{
    ARSchedule_Node tmp;
    cout << "  schedule in AR server from: " << ARSchNode.from_AS << " to: " << ARSchNode.to_AS << endl;
    
    if(ARSchNode.from_AS == ARSchNode.to_AS)
    {
        //intra call
        //can be reserved
        //TODO: Wrong AR_time Does not get any value, there should be a loop between different AR options. 
        //cout << "AR_time: " <<ARSchNode.AR_time;
        if(IPCE_module.findPathAndReserv(ARSchNode.from_node,ARSchNode.to_node,ARSchNode.capacity,ARSchNode.duration,ARSchNode.AR_vector))
        {
            cout<<"One reservation made!"<<endl<<endl;
        }
        else
        {
            cout << "no reservation" <<endl << endl ;
            
            /*
            if(ARSchNode.AR_time < ARSchNode.AR_vector.size()-1)
            {
                tmp=new ARSchedule_Node(ARSchNode.from_AS,ARSchNode.from_node,ARSchNode.to_AS,ARSchNode.to_node,ARSchNode.send_time,ARSchNode.total_delay,ARSchNode.capacity,ARSchNode.duration,ARSchNode.AR_time+1,ARSchNode.AS_path,ARSchNode.AR_vector);
                return tmp;
            }
             */


        }
    }
    else
    {
        //inter call
        //EPCE should be called
        cout<<"inter AS calls not tested!"<<endl;
    }
    return tmp;
}


