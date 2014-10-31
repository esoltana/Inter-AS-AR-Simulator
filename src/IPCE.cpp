/* 
 * File:   IPCE.cpp
 * Author: leo
 * revised: Elahe
 * Created on June 27, 2014, 12:22 AM
 */

#include "IPCE.h"
#include <vector>
#include <string>
#include "intradijkstra.h"
//28 performance evaluations  MM1 Queuy system
// available bandwidth in timeslot (t k , t k + τ ) on link l ∈ Λ where t k ≤ t ≤ (t k + τ ), 1 ≤ k ≤ K
IPCE::IPCE(){}

IPCE::IPCE(int reservationWindowSize, int extension, int numNode) {
    windowSize = reservationWindowSize;
    extSize = extension;
    num_nodes=numNode;
}
//TODO: Add comments.
//TODO: Ask Zhe why he produces a new data structure instead of adding a reservation window to previous intra link struct
//TODO: why not considering weight of the link?????

void IPCE::readTopology(vector<Intra_Link> intra_links) {
    //for each entry of intra_links table
    for (int i = 0; i < intra_links.size(); i++) {
        int node1 = intra_links[i].start_node;
        int node2 = intra_links[i].end_node;
        //create an index based on start and end nodes
        int index = node1 * 1000 + node2;
        NodeTable tmp;
        tmp.setBand(intra_links[i].bandwidth);
        tmp.constructResTable(windowSize, extSize);
        intraASLinksAR[index] = tmp;
    }
}

//ask Shawn to show Steve's code
bool IPCE::findPathAndReserv(int source_node, int dest_node, double capacity, int duration, vector<int> ARvec) {
    cout << "find path in IPCE " << endl;
    intradijkstra G;
    
    for (int i = 0; i < ARvec.size(); i++) {
        
        G.read(source_node, dest_node, num_nodes, intraASLinksAR, ARvec[i], ARvec[i] + duration, capacity);
        
        G.calculateDistance();
        
        
        //check if any path is found
        //no path between these two nodes
        if (G.flag == 0)
            continue;
        else {
            G.output();
            reserveCall(G.pathvector, ARvec[i], ARvec[i] + duration, capacity);
            return true;
        }
    }
    return false;

}

//TODO: use the notations from the paper as much as possible
//TODO: available bandwidth table instead of reservation table
//TODO: Capacity for the transmission rate of a link bps
//TODO: "Rate" for rate requested for the call bps
//TODO: delete rate functions
bool IPCE::findsinglePathR(int source_vertex, int dest_vertex, double capacity_rate, int duration, int AR_time) {

    intradijkstra G;
    if (source_vertex == 0 || dest_vertex == 0)
        return true;
    // should have a conversion between current time and rvwindow time
    G.readR(source_vertex, dest_vertex, num_nodes, intraASLinksAR, AR_time, AR_time + duration, capacity_rate);
    //G.recreate(ARvec[i], intraASLinksAR, intraASLinksAR.size(), ARvec[i] + duration, capacity);
    G.calculateDistance();
    //no path between these two nodes
    if (G.flag != 0)
        return true;
    return false;

}

bool IPCE::findsinglePathAndReserv(int source_node, int dest_node, double capacity, int duration, int AR_time) {
    cout << "find path in IPCE " << endl;
    intradijkstra G;
        // should have a conversion between current time and rwindow time
        G.read(source_node, dest_node, num_nodes, intraASLinksAR, AR_time, AR_time + duration, capacity);

        G.calculateDistance();
        G.output();
        
        
        //check if any path is found
        //no path between these two nodes
        if (G.flag == 0)
        {
            cout<<"should be a false!"<<endl;
            return false;
        }
        else {
            

            reserveCall(G.pathvector, AR_time, AR_time + duration, capacity);
            return true;
        }
    return false;

}


//TODO: check it to work correctly
bool IPCE::reserveCall(vector<int> pathVector, int start_time, int end_time, double capacity) {

    for (int i = 0; i < pathVector.size() - 1; i++) {

        for (int j = start_time; j <= end_time; j++) {
            int vertex1 = pathVector[i];
            int vertex2 = pathVector[i+1];

            intraASLinksAR[vertex1 * 1000 + vertex2].reservationTable[j] -= capacity;
            intraASLinksAR[vertex2 * 1000 + vertex1].reservationTable[j] -= capacity;
        }
    }

}




