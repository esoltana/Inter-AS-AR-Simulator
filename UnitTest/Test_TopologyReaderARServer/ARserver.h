/* 
 * File:   ARserver.h
 * Author: leo
 *
 * Created on June 27, 2014, 12:16 AM
 */

#ifndef ARSERVER_H
#define	ARSERVER_H
#include "GraphElements.h"
using namespace std;

class ARserver
{
public:
	ARserver(int AS_num, string topology_path);
	
        void readIntraTopology(string topology_path);
        void readInterLinks(string topology_path);
        void printTopology();
        TOPOLOGY topology;
	int numOfNodes;
        
       
        
private:
        int AS_ID;
	
};


#endif	/* ARSERVER_H */

