/* 
 * File:   IPCE.cpp
 * Author: leo
 * 
 * Created on June 27, 2014, 12:22 AM
 */

#include "IPCE.h"
#include <vector>
#include <string>
#include "intradijkstra.h"

IPCE::IPCE() {
}

IPCE::IPCE(int reservationWindowSize, int extension) {
    windowSize = reservationWindowSize;
    extSize = extension;
}

void IPCE::readTopology(vector<Intra_Link> intra_edges) {
    for (int i = 0; i < intra_edges.size(); i++) {
        int node1 = intra_edges[i].start_node;
        int node2 = intra_edges[i].end_node;
        int index = node1 * 1000 + node2;
        NodeTable tmp;
        tmp.getBand(intra_edges[i].bandwidth);
        tmp.constructTable(windowSize, extSize);
        intraASLinksAR[index] = tmp;

    }
}

bool IPCE::findPath(int source_vertex, int dest_vertex, double capacity, int duration, vector<int> ARvec) {

    intradijkstra G;
    for (int i = 0; i < ARvec.size(); i++) {
        // should have a conversion between current time and rvwindow time
        G.read(source_vertex, dest_vertex, num_vertices, intraASLinksAR, ARvec[i], ARvec[i] + duration, capacity);
        //G.recreate(ARvec[i], intraASLinksAR, intraASLinksAR.size(), ARvec[i] + duration, capacity);
        G.calculateDistance();
        //no path between these two nodes
        if (G.flag == 0)
            continue;
        else {
            G.output();
            //reserveCall(G.pathvector, ARvec[i], ARvec[i] + duration, capacity);
            return true;
        }
    }
    return false;

}

bool IPCE::findPathR(int source_vertex, int dest_vertex, double capacity_rate, int duration, int AR_time) {

    intradijkstra G;
    if (source_vertex == 0 || dest_vertex == 0)
        return true;
    // should have a conversion between current time and rvwindow time
    G.readR(source_vertex, dest_vertex, num_vertices, intraASLinksAR, AR_time, AR_time + duration, capacity_rate);
    //G.recreate(ARvec[i], intraASLinksAR, intraASLinksAR.size(), ARvec[i] + duration, capacity);
    G.calculateDistance();
    //no path between these two nodes
    if (G.flag != 0)
        return true;
    return false;

}

bool IPCE::findPathAndReserv(int source_vertex, int dest_vertex, double capacity, int duration, int AR_time) {

    intradijkstra G;
        // should have a conversion between current time and rvwindow time
        G.read(source_vertex, dest_vertex, num_vertices, intraASLinksAR, AR_time, AR_time + duration, capacity);
        //G.recreate(ARvec[i], intraASLinksAR, intraASLinksAR.size(), ARvec[i] + duration, capacity);
        G.calculateDistance();
        G.output();
        //cout<<"print flag:"<<G.flag<<endl;
        //no path between these two nodes
        if (G.flag == 0)
        {
            //cout<<"should be a false!"<<endl;
            return false;
        }
        else {
            
            //cout<<"do the reservation when flag is "<< G.flag <<endl;
            reserveCall(G.pathvector, AR_time, AR_time + duration, capacity);
            return true;
        }
    return false;

}

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




