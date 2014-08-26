/* 
 * File:   ARserver.h
 * Author: leo
 *
 * Created on June 27, 2014, 12:16 AM
 */

#ifndef ARSERVER_H
#define	ARSERVER_H
#include "ARBGP.h"
#include "IPCE.h"
using namespace std;

class ARserver
{
public:
	ARserver(int as_num, int reservationWindowSize, int extension);
	void constructUpdate();
        void handleCall(vector<int> ARvec, int band, int duration, int sourceAS, int sourceNode, int destAS, int destNode);
        GRAPH the_graph;
	ARBGP AR_BGP;
	IPCE IPCE_module;
	//EPCE EPCE_module;
	
private:
	int AS_num;
	int ARWindowSize, ARExtension;
};


#endif	/* ARSERVER_H */

