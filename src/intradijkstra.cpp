/* 
 * File:   intradijkstra.cpp
 * Author: leo
 * 
 * Created on March 24, 2013, 2:42 AM
 */

#include "intradijkstra.h"
#include<iostream>
//#include "p.h"

#define INFINITY 999

/*
 * Description:  This class does an intra-domain shortest path algorithm with available bandwidth constrains 
 */


/*
 * initialize several vectors used by the algorithm
 */
void intradijkstra::initialize() {
    for (int i = 0; i < numOfNodes; i++) {
        mark[i] = false;
        predecessor[i] = -1;
        distance[i] = INFINITY;
    }
    distance[source] = 0;
}


int intradijkstra::checkAvailability( map<int, linkAvailableBandwithTable>& intraLinks, int startTimeSlot, int endTimeSlot, double capacity)
{
    int f=0;
    for (int j = 0; j < pathvector.size()-1; j++) {
        
        for (int k = startTimeSlot; k < endTimeSlot; k++) {

            if (intraLinks[pathvector[j]*1000+pathvector[j+1]].availableBandwidthTable[k] < capacity) {

                f=1;
                break;
            }

        }
    }
    return f;
}



void intradijkstra::checkDirectionUSST()
{
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            if(adjMatrix[i][j] == INFINITY)
                adjMatrix[j][i]=INFINITY;
        }
    }
}

void intradijkstra::checkDirectionEST()
{
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            if(adjMatrix[i][j] == INFINITY)
                adjMatrix[j][i]=INFINITY;
            else if(adjMatrix[i][j] == 100)
            {
                if(adjMatrix[j][i]<100)
                    adjMatrix[i][j] = adjMatrix[j][i];
                else
                    adjMatrix[i][j] = INFINITY;
            }
        }
    }
}
/*
 * Input: starting: the starting node number; ending: the ending node number; edgedata: the reference of a map of EdgeTable, please see DataStructures.h to understand the data structure of edgedata object, 
 * generally speaking this is a data structure for each edge in the intra-domain topology with advance reservation window integrated; start: the starting time slot; end: the ending time slot; capacity: the requested capacity(Gbps)
 * 
 * Job description: This function reads in the edge data structure and fills a 2D matrix. The row(from) and column(to) number of the matrix correspond to the two endpoints of an edge
 * The value of each element in the matrix equals to the available bandwidth and for those pairs which are not directly connected, the value equals to INFINITY. If the available bandwidth
 * on certain edges(links) is less than capacity, then the value will also be set to INFINITY.
 * The matrix is used for computing a valid shortest path by using Dijkstra algorithm
 */
void intradijkstra::readForShortest(int starting, int ending, int numNode, map<int, linkAvailableBandwithTable>& intraLinks, int startTimeSlot, int endTimeSlot, double capacity) {
    //TODO: the size of the matrix should be equal to number of nodes, not a fix value
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            adjMatrix[i][j] = INFINITY;
        }
    }
    numOfNodes = numNode;
    for (std::map<int, linkAvailableBandwithTable>::iterator iter = intraLinks.begin(); iter != intraLinks.end(); ++iter) {
        int from_node = iter->first / 1000;
        int to_node = iter->first % 1000;
       

        w=iter->second.weight;
        adjMatrix[from_node-1][to_node-1] = w;
        
    }
        
    source = starting-1;
    dest = ending-1;
   
}

void intradijkstra::read(int starting, int ending, int numNode, map<int, linkAvailableBandwithTable>& intraLinks, int startTimeSlot, int endTimeSlot, double capacity) {
    //TODO: the size of the matrix should be equal to number of nodes, not a fix value
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            adjMatrix[i][j] = INFINITY;
        }
    }
    numOfNodes = numNode;
    for (std::map<int, linkAvailableBandwithTable>::iterator iter = intraLinks.begin(); iter != intraLinks.end(); ++iter) {
        int from_node = iter->first / 1000;
        int to_node = iter->first % 1000;

        w=iter->second.weight;

        for (int k = startTimeSlot; k < endTimeSlot; k++) {
            if (iter->second.availableBandwidthTable[k] < capacity) {
                w = INFINITY;
                break;
            }
        }
        adjMatrix[from_node-1][to_node-1] = w;
    }
    source = starting-1;
    dest = ending-1;
}


void intradijkstra::readEST(int starting, int ending, int numNode, map<int, linkAvailableBandwithTable>& intraLinks, int startTimeSlot, int endTimeSlot, double capacity, double cap_return) {
    //TODO: the size of the matrix should be equal to number of nodes, not a fix value
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            adjMatrix[i][j] = INFINITY;
        }
    }
    numOfNodes = numNode;
    for (std::map<int, linkAvailableBandwithTable>::iterator iter = intraLinks.begin(); iter != intraLinks.end(); ++iter) {
        int from_node = iter->first / 1000;
        int to_node = iter->first % 1000;

        w=iter->second.weight;

        for (int k = startTimeSlot; k < endTimeSlot; k++) {
            if (iter->second.availableBandwidthTable[k] < cap_return) {
                    w = INFINITY;
                    break;
            }
        }
        if(w!=INFINITY)  
        {
            for (int k = startTimeSlot; k < endTimeSlot; k++) {
               if (iter->second.availableBandwidthTable[k] < capacity) {
                    w = 100;
                    break;
                }
            }
        }
        adjMatrix[from_node-1][to_node-1] = w;
    }
    source = starting-1;
    dest = ending-1;
}


/*
 * called by other inner functions
 */
int intradijkstra::getClosestUnmarkedNode() {
    int minDistance = INFINITY;
    int closestUnmarkedNode;
    for (int i = 0; i < numOfNodes; i++) {
        if ((!mark[i]) && (minDistance >= distance[i])) {
            minDistance = distance[i];
            closestUnmarkedNode = i;
        }
    }
    //cout << "source: " << source << " closest: " << closestUnmarkedNode << " minDistance: "<< minDistance << endl;
    return closestUnmarkedNode;
}

/*
 * compute the shortest path
 */
void intradijkstra::calculateDistance() {

    initialize();
    int minDistance = INFINITY;
    int closestUnmarkedNode;
    int count = 0;
    while (count < numOfNodes) {
        closestUnmarkedNode = getClosestUnmarkedNode();
        mark[closestUnmarkedNode] = true;
        for (int i = 0; i < numOfNodes ; i++) {
            
            if ((!mark[i]) && (adjMatrix[closestUnmarkedNode][i] > 0)) {
               
                if (distance[i] > distance[closestUnmarkedNode] + adjMatrix[closestUnmarkedNode][i]) {
                    distance[i] = distance[closestUnmarkedNode] + adjMatrix[closestUnmarkedNode][i];
                    predecessor[i] = closestUnmarkedNode;
                }
            }
        }
        count++;
    }
    
    pathvector.clear();
    constructPath(dest);
            
}

/*
 * called by output()
 */
void intradijkstra::constructPath(int node) {
    
    if (node == source) {
        
        pathvector.push_back(node);

        flag = 1;
    } else if (predecessor[node] == -1) {
        //cout << "No path from " << source+1 << "to" << node+1 << endl;
        flag = 0;
    } else {

        constructPath(predecessor[node]);
        
        pathvector.push_back(node);
    }
}

/*
 * output the shortest path. If no valid path is found, then print 'no path' information(by printPaht() function)
 */
void intradijkstra::output() {

    for (int j = 0; j < pathvector.size(); j++)
        pathvector[j]+=1;
    /*cout << "path:";
    for (int j = 0; j < pathvector.size(); j++)
        cout << pathvector[j] << "--";
    cout << "->distance:" << distance[dest] << endl;
     */ 
}


