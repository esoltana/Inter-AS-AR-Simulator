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
#include "GraphElements.h"
#include "ARserver.h" 
using namespace std;


ARserver::ARserver(int AS_num, string topology_path)
{
        AS_ID=AS_num;
        readIntraTopology(topology_path);
        readInterLinks(topology_path);
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
            in >> node_u;
            in >> node_v;
            in >> link_weight;
            in >> link_band;

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

void ARserver::printTopology()
{
    Intra_Link link; 
    Inter_AS_Links interl;
    cout << "Intra Links within AS" << AS_ID << " topology:" << endl;
    for(int i=0; i < topology.Intra_Links_table.size(); i++)
    {
        link=topology.Intra_Links_table[i];
        cout << link.start_node << " --> " << link.end_node << "    b: " << link.bandwidth << "    w:"<< link.weight << endl;
    }
    
    cout << endl << "Inter Links from AS" << AS_ID << " to other ASes:" << endl;
    for(int i=0; i < topology.InterASLinks_table.size(); i++)
    {
        interl=topology.InterASLinks_table[i];
        cout << "ASes " << interl.start_AS << ":" << interl.end_AS << " boundaryNodes: " << interl.start_node << " --> " << interl.end_node << "    b: " << interl.bandwidth << "    w:"<< interl.weight << endl;
    }
}
