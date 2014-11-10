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
	ARserver(int AS_num, int AR_TimeWindow_size, int lead_time, int single_TimeSlot_size, string topology_path);
        
        void readIntraTopology(string topology_path);
        void readInterLinks(string topology_path);
        
        void actionARBGPreceive(int from_AS,vector<NLRI> NLRI_vector);
        
        int executeIntraCall(Call_Node IntraCallNode);
        
        void initializeARBGP();
        
        TOPOLOGY topology;
	int numOfNodes;
        
        ARBGP AR_BGP_module;
        IPCE IPCE_module;
        
private:
        int AS_ID;
	int ARWindow_timeSlot, ARleadtime;
        int timeSlotSize;
};


#endif	/* ARSERVER_H */

