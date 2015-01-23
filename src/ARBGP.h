/* 
 * File:   ARBGP.h
 * Author: leo
 *
 * Created on June 27, 2014, 12:17 AM
 */

#ifndef ARBGP_H
#define	ARBGP_H

#include "DataStructures.h"
#include <string>
#include <list>
#include <queue>
#include "IPCE.h"

using namespace std;

class ARBGP
{
public:
	int AS_num;
	int num_vertices;
	int ARWindowSize, ARExtension;
	ARBGP();
	ARBGP(int asnum, int num_of_vertices);
	void ARBGPinit();
	void addNeighbor(int as_num, int border_router);
	void sendUpdate(double send_time, priority_queue<ARBGP_Node, vector<ARBGP_Node>, MyComparatorARBGP>& ARBGP_Q, DelayStruc DFile, int recvwindow, IPCE& IPCE_M);
	void recvUpdate(int incomingAS, vector<NLRI> NLRI_vector);
	void addMCNAccess(int as_num, int egress_ingress, int node1, int node2);
	void getMCNList();
	void getMCNProcessNumber(int offset);
	void constructUpdate(int flag);
	bool checkNLRI(NLRI n1, NLRI n2);
        bool sameNLRI(NLRI n1, NLRI n2);
	void parseUpdateMessage(NLRI new_NLRI);
	vector<int> MCN_IDs;
	vector<int> MCN_process_num;
	string update_message;
	map <Node,vector<NLRI>,NodeCompare > RIB;
        map <int,vector<NLRI> > RIB_out;
        double threshold_capacity;
	map <int,MCN> MCNlist;
private:

	//the data structure that stores all the neighbor ASes
	
	map <int,AdjRibsOut> AdjRibsOutList;
	map <Node,vector<NLRI> > RIB_in;
	
	vector< vector<int> > BRlist;
	
};
#endif	/* ARBGP_H */

