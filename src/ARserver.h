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
	ARserver(int AS_num, int reservationWindowSize, int extension,string topology_path);
	void constructUpdate();
        void readIntraTopology(string topology_path);
        void readInterLinks(string topology_path);
        
        TOPOLOGY topology;
	int numOfNodes;
        
        ARBGP AR_BGP;
        IPCE IPCE_module;
        
private:
        int AS_ID;
	int ARWindowSize, ARExtension;
        //timeslots should be added.
};


#endif	/* ARSERVER_H */

