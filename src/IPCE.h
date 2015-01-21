/* 
 * File:   IPCE.h
 * Author: leo
 *
 * Created on June 27, 2014, 12:22 AM
 */

#ifndef IPCE_H
#define	IPCE_H

#include "DataStructures.h" 
#include <map>

using namespace std;

class IPCE {
private:

public:
    int windowSizeTimeslot, ARleadtime;
    int num_nodes;
    map<int, linkAvailableBandwithTable> intraASLinksAR;
    IPCE();
    IPCE(int reservationWindowSize, int extension, int numNode);
    void readTopology(vector<Intra_Link> intra_links);
    bool findPathAndReserv(int source_vertex, int dest_vertex, double capacity, int duration, vector<int> ARvec);
    bool findsinglePathR(int source_vertex, int dest_vertex, double capacity_rate, int duration, int AR_time);
    bool USSTfindPathShortestEarliestAndReserv(int source_vertex, int dest_vertex, double capacity, int duration, vector<int> ARvec);
    bool USSTfindPathMulShortestPossibleAndReserv(int source_node, int dest_node, double capacity, int duration, vector<int> ARvec);
    bool USSTfindPathPossibleShortEarliestAndReserv(int source_node, int dest_node, double capacity, int duration, vector<int> ARvec);
    bool ESTfindPathPossibleShortEarliestAndReserv(int source_node, int dest_node, double capacity, int duration, vector<int> ARvec, double cap_return);
    bool reserveCallUSST(vector<int> pathVector, int start_time, int end_time, double capacity);
    bool reserveCallEST(vector<int> pathVector, int start_time, int end_time, double capacity, double cap_return);
    int selectedOptionIndex;
    int pathLength;
    void slideWindow(vector<Intra_Link> intra_links);
    vector <int> pathvector;
};


#endif	/* IPCE_H */

