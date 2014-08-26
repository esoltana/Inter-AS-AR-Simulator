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
#include "IPCE.h"
#include "Graph.h"
#include "readcall.h"
#include "ARserver.h" 
using namespace std;


ARserver::ARserver(int as_num, int reservationWindowSize, int extension)
{
	AS_num = as_num;
	AR_BGP.AS_num = as_num;
	ARWindowSize = reservationWindowSize;
	ARExtension = extension;
	AR_BGP.ARWindowSize= reservationWindowSize;
	AR_BGP.ARExtension = extension;
	AR_BGP.ARBGPinit();
	IPCE_module.windowSize = reservationWindowSize;
        IPCE_module.extSize = extension;
}

void ARserver::handleCall(vector<int> ARvec, int band, int duration, int sourceAS, int sourceNode, int destAS, int destNode)
{
    /*
	//double arr_time, capacity;
	//int capacity, duration, source_node, dest_AS, dest_node;
	string ARtime;
	vector<int> ARvec;
	//ss >> arr_time >> ARtime >> capacity >> duration >> source_node >> dest_AS >> dest_node;
	
	stringstream ars(ARtime);
	int tmpAR;
	while( getline(ars,tmpAR,'$'))
	ARvec.push_back(tmpAR);
	//if this is an intra-AS call
	if(dest_AS == AS_num)
	{
		
	}
	else
	{
		
	}
     * */
}



